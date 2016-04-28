/*
 * audioSample_io.c
 *
 * This file contains the test / demo code to demonstrate the Audio component
 * driver functionality on SYS/BIOS 6.
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

/** \file     audioSample_io.c
 *
 *  \brief    sample application for demostration of audio playing
 *
 *  This file contains the implementation of the sample appliation for the
 *  demonstration of audio playing through the audio interface layer.
 *
 *             (C) Copyright 2009, Texas Instruments, Inc
 */

/* ========================================================================== */
/*                            INCLUDE FILES                                   */
/* ========================================================================== */

#include <xdc/std.h>
#include <ti/sysbios/io/IOM.h>
#include <xdc/runtime/Memory.h>
#include <ti/sysbios/heaps/HeapMem.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <mcasp_drv.h>

#include <ti/sdo/edma3/drv/edma3_drv.h>
#include <include/McaspLocal.h>
#include "ICodec.h"
#include "stdio.h"
#include "string.h"
#include <audioEQ_biquad.h>
//#define DSP_MODE

/* ========================================================================== */
/*                          IMPORTED VARIABLES                                */
/* ========================================================================== */

EDMA3_DRV_Handle edma3init(unsigned int edma3Id, EDMA3_DRV_Result *);
extern EDMA3_DRV_Handle hEdma_0;
extern EDMA3_DRV_Handle hEdma_1;
extern HeapMem_Handle myHeap;
extern BIQUAD_T filter;
extern BIQUAD_T *low_LShelf_Filter;
extern BIQUAD_T filter1;
extern BIQUAD_T *high_HShelf_Filter;
extern BIQUAD_T filter2;
#ifdef BANDPASS
extern BIQUAD_T *mid_BPass_Filter;
#else
extern BIQUAD_T *mid_HShelf_Filter;
extern BIQUAD_T filter3;
extern BIQUAD_T *mid_LShelf_Filter;
#endif

extern int controlFilterUpdate;
extern int filterSwitch;
/* ========================================================================== */
/*                          MACRO DEFINITIONS                                 */
/* ========================================================================== */

/*
 * Buffers placed in external memory are aligned on a 128 bytes boundary.
 * In addition, the buffer should be of a size multiple of 128 bytes for
 * the cache work optimally on the C6x.
 */
#define BUFLEN                  1024         /* number of samples in the frame */
#define BUFALIGN                128 /* alignment of buffer for use of L2 cache */


/** Number of serializers configured for record */
#define RX_NUM_SERIALIZER       (1u)
#define TX_NUM_SERIALIZER       (1u)

#define BUFSIZE                 (BUFLEN * 4)	// 4 bytes per word

#define NUM_BUFS                2   /* Num Bufs to be issued and reclaimed */
#define NUM_BANDS              3   /* Number of frequency Bands */
/* Function prototype */
static Void createStreams();
static Void prime();

Ptr rxbuf[NUM_BUFS];
Ptr txbuf[NUM_BUFS];
Ptr scratch[NUM_BANDS];

int gblErrFlag=0;
void GblErr(int arg)
{
	gblErrFlag=1;
}


Mcasp_HwSetupData mcasp2RcvSetup = {
        /* .rmask    = */ 0xFFFFFFFF, /* All the data bits are to be used     */
#if defined (DSP_MODE)
        /* .rfmt     = */ 0x000080f0,
#else /* I2S MODE*/
    /* .rfmt     = */ 0x000180F0,
#endif								  /* 0/1 bit delay from framsync
                                       * MSB first
                                       * No extra bit padding
                                       * Padding bit (ignore)
                                       * slot Size is 32
                                       * Reads from DMA port
                                       * NO rotation
                                       */
#if defined (MCASP_MASTER)
	#if defined (DSP_MODE)
    /* .afsrctl  = */ 0x00000002,           /* burst mode,
                                             * Frame sync is one bit
                                             * internally generated frame sync
                                             * Rising edge is start of frame
                                             */

	#else /* I2S MODE*/
          /* .afsrctl  = */ 0x00000113,     /* I2S mode,
                                             * Frame sync is one word
                                             * Internally generated frame sync
                                             * Falling edge is start of frame
                                             */
	#endif
#else
	#if defined (DSP_MODE)
    /* .afsrctl  = */ 0x00000000,           /* burst mode,
                                             * Frame sync is one bit
                                             * Externally generated frame sync
                                             * Rising edge is start of frame
                                             */

	#else /* I2S MODE*/
          /* .afsrctl  = */ 0x00000111,     /* I2S mode,
                                             * Frame sync is one word
                                             * Externally generated frame sync
                                             * Falling edge is start of frame
                                             */
	#endif
#endif

#if defined (DSP_MODE)
/* .rtdm     = */ 0x00000001,           /* slot 1 is active (DSP)
                                         *              */

#else /* I2S MODE*/
      /* .rtdm     = */ 0x00000003,     /* 2 slots are active (I2S)
                                         *            */
#endif
        /* .rintctl  = */ 0x00000003, /* sync error and overrun error         */
        /* .rstat    = */ 0x000001FF, /* reset any existing status bits       */
        /* .revtctl  = */ 0x00000000, /* DMA request is enabled or disabled   */
        {
#if defined (MCASP_MASTER)
		#if defined (DSP_MODE)
			/* .aclkrctl  = */ 0x00000027,
			/* .ahclkrctl = */ 0x00008027,
		#else /* I2S MODE*/
			/* .aclkrctl  = */ 0x000000A7,	/* Div (8), Internal Source, rising edge */
			/* .ahclkrctl = */ 0x00008013,	/* Div (20), Internal AUX_CLK Source */
		#endif
#else
		#if defined (DSP_MODE)
			/* .aclkrctl  = */ 0x00000000,
		#else /* I2S MODE*/
			/* .aclkrctl  = */ 0x00000080,	/* External Source, rising edge */
		#endif
			/* .ahclkrctl = */ 0x00000000,	/* Don't Care */
#endif
             /* .rclkchk   = */ 0x00000000
        }
} ;

Mcasp_HwSetupData mcasp2XmtSetup = {
        /* .xmask    = */ 0xFFFFFFFF, /* All the data bits are to be used     */
		#if defined (DSP_MODE)
			/* .xfmt     = */ 0x000080F0,
		#else /* I2S MODE*/
			/* .xfmt     = */ 0x000180F0,
		#endif 						  /*
                                       * 0/1 bit delay from framsync
                                       * MSB first
                                       * No extra bit padding
                                       * Padding bit (ignore)
                                       * slot Size is 32
                                       * Reads from DMA port
                                       * 0-bit rotation
                                       */
#if defined (MCASP_MASTER)
		#if defined (DSP_MODE)
		/* .afsxctl  = */ 0x00000002,       /* burst mode,
											 * Frame sync is one bit
											 * Internally generated frame sync
											 * Rising edge is start of frame
											 */
		/* .xtdm     = */ 0x00000001,       /* slot 1 is active (DSP) */
		#else /*I2S MODE*/
			  /* .afsxctl  = */ 0x00000113, /* I2S mode,
											 * Frame sync is one word
											 * internally generated frame sync
											 * Falling edge is start of frame
											 */
			  /* .xtdm     = */ 0x00000003, /* 2 slots are active (I2S) */
		#endif
#else
		#if defined (DSP_MODE)
		/* .afsxctl  = */ 0x00000000,       /* burst mode,
											 * Frame sync is one bit
											 * Rising edge is start of frame
											 * externally generated frame sync
											 */
		/* .xtdm     = */ 0x00000001,       /* slot 1 is active (DSP) */
		#else /*I2S MODE*/
			  /* .afsxctl  = */ 0x00000111, /* I2S mode,
											 * Frame sync is one word
											 * Externally generated frame sync
											 * Falling edge is start of frame
											 */
			  /* .xtdm     = */ 0x00000003, /* 2 slots are active (I2S) */
		#endif
#endif
        /* .xintctl  = */ 0x00000007, /* sync error,overrun error,clK error   */
        /* .xstat    = */ 0x000001FF, /* reset any existing status bits       */
        /* .xevtctl  = */ 0x00000000, /* DMA request is enabled or disabled   */
        {
#if defined (MCASP_MASTER)
        #if defined (DSP_MODE)
			/* .aclkxctl  = */ 0x00000027,
			/* .ahclkxctl = */ 0x00008027,

		#else /* I2S MODE*/
			/* .aclkxctl  = */ 0x000000A7, /* Div (8), Internal Source, SYNC, Falling edge */
			/* .ahclkxctl = */ 0x00008013, /* Div (20), Internal AUX_CLK Source */
		#endif
#else
		#if defined (DSP_MODE)
			/* .aclkxctl  = */ 0x00000000,	/* External Source, SYNC */

		#else /* I2S MODE*/
			/* .aclkxctl  = */ 0x00000080, /* External Source, SYNC, Falling edge */
		#endif
#endif
             /* .xclkchk   = */ 0x00000000
        },

};

/* McAsp channel parameters                                  */
Mcasp_ChanParams  mcasp2_chanparam[2]=
{
    {
        0x0001,                    /* number of serialisers      */
        {Mcasp_SerializerNum_1, }, /* serialiser index           */
        &mcasp2RcvSetup,
        TRUE,
        Mcasp_OpMode_TDM,          /* Mode (TDM/DIT)             */
        Mcasp_WordLength_32,
        NULL,
        0,
        NULL,
        GblErr,
#if defined (DSP_MODE)
        1,
		Mcasp_BufferFormat_1SER_1SLOT,
#else      /* I2S MODE*/
        2, /* number of TDM channels      */
		Mcasp_BufferFormat_1SER_MULTISLOT_INTERLEAVED,
#endif
        TRUE,
        TRUE
    },
    {
        0x0001,                   /* number of serialisers       */
        {Mcasp_SerializerNum_0,},
        &mcasp2XmtSetup,
        TRUE,
        Mcasp_OpMode_TDM,
        Mcasp_WordLength_32,      /* word width                  */
        NULL,
        0,
        NULL,
        GblErr,
#if defined (DSP_MODE)
        1,
		Mcasp_BufferFormat_1SER_1SLOT,
#else      /* I2S MODE*/
        2, /* number of TDM channels      */
		Mcasp_BufferFormat_1SER_MULTISLOT_INTERLEAVED,
#endif
        TRUE,
        TRUE
    }
};

/*
 * ======== createStreams ========
 */

#include <Aic31.h>

Ptr  hAicDev;
Ptr  hAicChannel;

ICodec_ChannelConfig AIC31_config =
{
		44100,  /* sampling rate for codec */
		90,  /* gain (%) for codec      */
		0x00,
		0x00
};

/* McASP Device handles */
Ptr  hMcaspDev2;

/* McASP Device parameters */
Mcasp_Params mcasp2Params;


/* Channel Handles */
Ptr hMcasp2TxChan;
Ptr hMcasp2RxChan;

int txChanMode,rxChanMode;
int hMcaspRxChan=1,hMcaspTxChan=2;
int rxFrameIndex=1, txFrameIndex=1;
volatile int RxFlag=0,TxFlag=0;
Semaphore_Handle semR,semT;
Semaphore_Params params;

Error_Block eb;

void mcaspAppCallback(void* arg, MCASP_Packet *ioBuf)
{

	if(*(int*)arg == 0x0001)
	{
		RxFlag++;
	if(rxFrameIndex==0)
		rxFrameIndex=1;
	else
		rxFrameIndex=0;
	/* post semaphore */
	Semaphore_post(semR);
	}
	if(*(int*)arg == 0x0002)
		{
	if(txFrameIndex==0)
			txFrameIndex=1;
	else
				txFrameIndex=0;
		TxFlag++;
		/* post semaphore */
		Semaphore_post(semT);
		}

}

extern  Int aic31MdCreateChan(
                    Ptr                 *chanp,
                    Ptr                 devp,
                    String              name,
                    Int                 mode,
                    Ptr                 chanParams,
                    IOM_TiomCallback    cbFxn,
                    Ptr                 cbArg
                    );

static Void createStreams()
{
	int status;

	char remName[10]="aic";
	int mode = IOM_INPUT;
	mcasp2_chanparam[0].edmaHandle = hEdma_1;
    mcasp2_chanparam[1].edmaHandle = hEdma_1;

	/* Create McASP2 channel for Tx */
	status = mcaspCreateChan(&hMcasp2TxChan, hMcaspDev2,
							 MCASP_OUTPUT,
							 &mcasp2_chanparam[1],
							 mcaspAppCallback, &txChanMode);

	if((status != MCASP_COMPLETED) || (hMcasp2TxChan == NULL))
	{
		System_printf("mcaspCreateChan for McASP2 Tx Failed\n");
		BIOS_exit(0);
	}


	/* Create McASP2 channel for Rx */
	status = mcaspCreateChan(&hMcasp2RxChan, hMcaspDev2,
	                         MCASP_INPUT,
	                         &mcasp2_chanparam[0],
	                         mcaspAppCallback, &rxChanMode);
	if((status != MCASP_COMPLETED) || (hMcasp2RxChan == NULL))
	{
		System_printf("mcaspCreateChan for McASP2 Rx Failed\n");
		BIOS_exit(0);
	}

	status = aic31MdCreateChan(
		&hAicChannel,
		hAicDev,
		remName,
		mode,
		(Ptr)(&AIC31_config),
		mcaspAppCallback,
		&rxChanMode);

	if ((NULL == hAicChannel) &&
			(IOM_COMPLETED != status))
	{
		System_printf("AIC Create Channel Failed\n");
	}
	else
	{

	}
}

/*
 * ======== prime ========
 */
MCASP_Packet rxFrame[2];
MCASP_Packet txFrame[2];
#include <ti/sysbios/family/c64p/Hwi.h>

Hwi_Handle myHwi;
static Void prime()
{
	Error_Block  eb;
    Int32        count = 0, status;
    IHeap_Handle iheap;

    iheap = HeapMem_Handle_to_xdc_runtime_IHeap(myHeap);
    Error_init(&eb);

    /* Allocate buffers for the SIO buffer exchanges                          */
    for(count = 0; count < (NUM_BUFS ); count ++)
    {
        rxbuf[count] = Memory_calloc(iheap, BUFSIZE * RX_NUM_SERIALIZER,
        														 BUFALIGN, &eb);
        if(NULL == rxbuf[count])
        {
            System_printf("\r\nMEM_calloc failed.\n");
        }
    }

    /* Allocate buffers for the SIO buffer exchanges                          */
    for(count = 0; count < (NUM_BUFS); count ++)
    {
        txbuf[count] = Memory_calloc(iheap, BUFSIZE * TX_NUM_SERIALIZER,
        														BUFALIGN, &eb);
        if(NULL == txbuf[count])
        {
            System_printf("\r\nMEM_calloc failed.\n");
        }
    }

    /* Allocate buffers for the SIO buffer exchanges                          */
           for(count = 0; count < (NUM_BANDS); count ++)
           {
              scratch[count] = Memory_calloc(iheap, BUFSIZE * TX_NUM_SERIALIZER,
               														BUFALIGN, &eb);
               if(NULL == scratch[count])
               {
                   System_printf("\r\nMEM_calloc failed.\n");
               }
               else
            	   memset((uint8_t *)scratch[count], 0xFF, BUFSIZE);
           }


    for(count = 0; count < NUM_BUFS; count ++)
    {
        /* Issue the first & second empty buffers to the input stream         */

    	memset((uint8_t *)rxbuf[count], 0xFF, BUFSIZE);
			/* RX frame processing */
			rxFrame[count].cmd = 0;
			rxFrame[count].addr = (void*)(getGlobalAddr(rxbuf[count]));
			rxFrame[count].size = BUFSIZE;
			rxFrame[count].arg = (uint32_t) hMcasp2RxChan;
			rxFrame[count].status = 0;
			rxFrame[count].misc = 1;   /* reserved - used in callback to indicate asynch packet */

		/* Submit McASP packet for Rx */
		status = mcaspSubmitChan(hMcasp2RxChan, &rxFrame[count]);
		if((status != MCASP_PENDING))
			System_printf ("Debug: Error McASP2 RX : Prime  buffer  #%d submission FAILED\n", count);


    }

    for(count = 0; count < (NUM_BUFS); count ++)
       {

       	memset((uint8_t *)txbuf[count], 0xF0, BUFSIZE);
   			/* TX frame processing */
   			txFrame[count].cmd = 0;
   			txFrame[count].addr = (void*)(getGlobalAddr(txbuf[count]));
   			txFrame[count].size = BUFSIZE;
   			txFrame[count].arg = (uint32_t) hMcasp2TxChan;
   			txFrame[count].status = 0;
   			txFrame[count].misc = 1;   /* reserved - used in callback to indicate asynch packet */

   		/* Submit McASP packet for Tx */
   		status = mcaspSubmitChan(hMcasp2TxChan, &txFrame[count]);
   		if((status != MCASP_PENDING))
   			System_printf ("Debug: Error McASP2 TX : Prime  buffer  #%d submission FAILED\n", count);
       }

}

/*
 * ======== echo ========
 * This function copies from the input SIO to the output SIO. You could
 * easily replace the copy function with a signal processing algorithm.
 */

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/hal/Cache.h>

extern EDMA3_DRV_GblConfigParams sampleEdma3GblCfgParams[3];
extern Int aic31MdBindDev(Ptr *, Int, Ptr);

int gtxFrameIndexCount=0;
int grxFrameIndexCount=0;
int itemp;
int result, pwr_status, fs_status, bck_status;

Void Audio_echo_Task()
{
    volatile Int32 i32Count, status = 0;

	hMcaspDev2  = NULL;

    /* 1. EDMA Initializations */
    EDMA3_DRV_Result edmaResult = 0;

    /* Enable all McASP events */
    sampleEdma3GblCfgParams[0].dmaChannelHwEvtMap[0] |= 0xFFFFu;


    hEdma_1 = edma3init(1, &edmaResult);

    if (edmaResult != EDMA3_DRV_SOK)
        {
            /* Report EDMA Error
             */
            System_printf("\nEDMA driver initialization FAIL\n");
        }
        else
        {
            System_printf("\nEDMA driver initialization PASS.\n");
        }

	/* 2. SEM Initializations */
    Semaphore_Params_init(&params);

	/* Create semaphores to wait for buffer reclaiming */
    semR = Semaphore_create(0, &params, &eb);
    semT = Semaphore_create(0, &params, &eb);

	/* 3. McASP Initializations */
	/* Initialize McASP Tx and Rx parameters */

	mcasp2Params = Mcasp_PARAMS;

	/* Set the Error HW interrupt number */
	mcasp2Params.hwiNumber = 8;

	/* Bind McASP2  */
	mcasp2Params.mcaspHwSetup.rx.clk.clkSetupClk = 0x23;

	status = mcaspBindDev(&hMcaspDev2, 2, &mcasp2Params);
	if((status != MCASP_COMPLETED) || (hMcaspDev2 == NULL))
	{
		System_printf("mcaspBindDev for McASP2 Failed\n");
		abort();
	}

	/* Bind AIC Codec */
    aic31MdBindDev(&hAicDev, 0, (Ptr)&Aic31_PARAMS);

    /* Call createStream function to create I/O streams                       */
    createStreams();

    /* Call prime function to do priming                                      */
    prime();


    /* Forever loop to continously receviec and transmit audio data           */
    for (i32Count = 0; i32Count >= 0; i32Count++)
    {

    	if(gblErrFlag)
    		break;

    	Semaphore_pend(semR, BIOS_WAIT_FOREVER);
    	Semaphore_pend(semT, BIOS_WAIT_FOREVER);

        /* Reclaim full buffer from the input stream                          */


    		gtxFrameIndexCount=txFrameIndex;
    		grxFrameIndexCount=rxFrameIndex;

#if 0
    		for(itemp=0;itemp<0;itemp++)
    		{
    			asm("; Comment to maintain loops through compiler optimization");
    		}
#endif
        /* Reclaim empty buffer from the output stream to be reused           */

        /* copy the receive information to the transmit buffer                */

        Cache_inv(rxbuf[grxFrameIndexCount],BUFSIZE * RX_NUM_SERIALIZER,Cache_Type_ALL, TRUE);

        if (filterSwitch == 0)
				/* copy the receive information to the transmit buffer when equalization filter are off */
                memcpy(txbuf[gtxFrameIndexCount],rxbuf[grxFrameIndexCount],BUFSIZE * RX_NUM_SERIALIZER);
        else{
        	// filter a buffer of input samples, in-place
           //Biquad_applyFilter_int(lpFilter, (int *)rxbuf[grxFrameIndexCount], (int *)txbuf[gtxFrameIndexCount], BUFLEN);
        	    controlFilterUpdate = 0;
    	        Biquad_applyFilter32(low_LShelf_Filter, (int *)rxbuf[grxFrameIndexCount], (int *)scratch[0], BUFLEN);

#ifdef BANDPASS
    	        Biquad_applyFilter32(high_HShelf_Filter, (int *)scratch[0], (int *)scratch[1], BUFLEN);
    	        Biquad_applyFilter32(mid_BPass_Filter, (int *)scratch[1], (int *)txbuf[gtxFrameIndexCount], BUFLEN);
#else
       	     	Biquad_applyFilter32(mid_HShelf_Filter, (int *)scratch[0], (int *)scratch[1], BUFLEN);
       	        Biquad_applyFilter32(mid_LShelf_Filter, (int *)scratch[1], (int *)scratch[2], BUFLEN);
       	        Biquad_applyFilter32(high_HShelf_Filter, (int *)scratch[2], (int *)txbuf[gtxFrameIndexCount], BUFLEN);
#endif

       	        controlFilterUpdate = 1;
        }
        Cache_wbInv(txbuf[gtxFrameIndexCount],BUFSIZE * TX_NUM_SERIALIZER,Cache_Type_ALL, TRUE);

        /* Issue full buffer to the output stream                             */
        /* TX frame processing */
		txFrame[gtxFrameIndexCount].cmd = 0;
		txFrame[gtxFrameIndexCount].addr = (void*)getGlobalAddr(txbuf[gtxFrameIndexCount]);
		txFrame[gtxFrameIndexCount].size = BUFSIZE;
		txFrame[gtxFrameIndexCount].arg = (uint32_t) hMcasp2TxChan;
		txFrame[gtxFrameIndexCount].status = 0;
		txFrame[gtxFrameIndexCount].misc = 1;   /* reserved - used in callback to indicate asynch packet */

		status = mcaspSubmitChan(hMcasp2TxChan, &txFrame[gtxFrameIndexCount]);
		if((status != MCASP_PENDING))
			System_printf ("Debug: Error McASP2 TX : Prime  buffer  #%d submission FAILED\n", i32Count);

		/* Issue an empty buffer to the input stream                          */

		rxFrame[grxFrameIndexCount].cmd = 0;
		rxFrame[grxFrameIndexCount].addr = (void*)getGlobalAddr(rxbuf[grxFrameIndexCount]);
		rxFrame[grxFrameIndexCount].size = BUFSIZE;
		rxFrame[grxFrameIndexCount].arg = (uint32_t) hMcasp2RxChan;
		rxFrame[grxFrameIndexCount].status = 0;
		rxFrame[grxFrameIndexCount].misc = 1;   /* reserved - used in callback to indicate asynch packet */

		status = mcaspSubmitChan(hMcasp2RxChan, &rxFrame[grxFrameIndexCount]);
		if((status != MCASP_PENDING))
			System_printf ("Debug: Error McASP2 RX :  buffer  #%d submission FAILED\n", i32Count);

}


    gblErrFlag=1;

        status = mcaspDeleteChan(hMcasp2TxChan);
        status = mcaspDeleteChan(hMcasp2RxChan);
    	status = mcaspUnBindDev(hMcaspDev2);

    	gtxFrameIndexCount=0;
    	grxFrameIndexCount=0;

		{
			IHeap_Handle iheap;

			iheap = HeapMem_Handle_to_xdc_runtime_IHeap(myHeap);
			Error_init(&eb);
			for(i32Count = 0; i32Count < (NUM_BUFS); i32Count ++)
				{
					Memory_free(iheap,rxbuf[i32Count],BUFSIZE * RX_NUM_SERIALIZER);
					Memory_free(iheap,txbuf[i32Count],BUFSIZE * TX_NUM_SERIALIZER);
				}
		}
	System_printf ("One frame of data sent\n");
    BIOS_exit(0);
}

