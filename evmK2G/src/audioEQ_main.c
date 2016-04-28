/*
 * audioSample_main.c
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

/** \file   audioSample_main.c
 *
 *  \brief    sample application for demonstration of audio driver usage
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
#include <string.h>
#include <xdc/runtime/Log.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/io/GIO.h>
#include <ti/sysbios/BIOS.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <mcasp_drv.h>
#include <ti/sysbios/io/IOM.h>
#include <Aic31.h>

#include <ti/drv/uart/UART.h>
#include <ti/drv/uart/UART_stdio.h>

#define SW_I2C
#include <Audio_evmInit.h>
#include <ti/csl/cslr_device.h>
#include <ti/csl/csl_bootcfg.h>
#include <board.h>
#include <ti/csl/hw_types.h>
extern Board_STATUS Board_init(Board_initCfg);

//#include <Audio.h>
#include <ti/sdo/edma3/drv/edma3_drv.h>
#include <ti/csl/csl_edma3.h>
#include <ti/csl/csl_bootcfg.h>
#include <audioEQ_biquad.h>
#include <math.h>
/* ========================================================================== */
/*                           MACRO DEFINTIONS                                 */
/* ========================================================================== */
BIQUAD_T filter;
BIQUAD_T *low_LShelf_Filter = &filter;
BIQUAD_T filter1;
BIQUAD_T *high_HShelf_Filter = &filter1;
BIQUAD_T filter2;
#ifdef BANDPASS
BIQUAD_T *mid_BPass_Filter = &filter2;
#else
BIQUAD_T *mid_HShelf_Filter = &filter2;
BIQUAD_T filter3;
BIQUAD_T *mid_LShelf_Filter = &filter3;
#endif


/* Handle to the EDMA driver instance                                         */
EDMA3_DRV_Handle hEdma_0;
EDMA3_DRV_Handle hEdma_1;

/* Extern definitions */
extern void configureAudio(void);
extern void McaspDevice_init(void);

/* Filter default configuration*/
double low_Fc  = 250.0;
double high_Fc = 1000.0;
double mid_Fc = 600.0; // Only used for bandpass filter
double sampleRate = 44100.0;
double defaultGain = 12.0;
double Qfactor = 0.707;

//Control Flags
int controlFilterUpdate;
int filterSwitch;

int i2a(char *s, int n);
char* itoa(int num, char *buff);

/* ========================================================================== */
/*                           FUNCTION DEFINITIONS                             */
/* ========================================================================== */

/**
 *  \brief  Void main(Void)
 *
 *   Main function of the sample application. This function enables
 *   the mcasp instance in the power sleep controller and also
 *   enables the pinmux for the mcasp 1 instance.
 *
 *  \param  None
 *  \return None
 */

Void main(Void)
{
    Board_STATUS stat = BOARD_SOK;
    Board_initCfg arg ;

	Biquad_initParams(low_LShelf_Filter);	// create a Biquad, low_LShelf_Filter;
	Biquad_setBiquad(low_LShelf_Filter, bq_type_lowshelf, low_Fc/sampleRate , Qfactor, defaultGain);

	Biquad_initParams(high_HShelf_Filter);	// create a Biquad, low_LShelf_Filter;
	Biquad_setBiquad(high_HShelf_Filter, bq_type_highshelf, high_Fc/sampleRate , Qfactor, defaultGain);

#ifdef BANDPASS
	Biquad_initParams(mid_BPass_Filter);	// create a Biquad, low_LShelf_Filter;
	Biquad_setBiquad(mid_BPass_Filter, bq_type_bandpass, mid_Fc/sampleRate , Qfactor, defaultGain);
#else
	Biquad_initParams(mid_HShelf_Filter);	// create a Biquad, low_LShelf_Filter;
	Biquad_setBiquad(mid_HShelf_Filter, bq_type_highshelf, low_Fc/sampleRate , Qfactor, defaultGain);
	Biquad_initParams(mid_LShelf_Filter);	// create a Biquad, low_LShelf_Filter;
	Biquad_setBiquad(mid_LShelf_Filter, bq_type_lowshelf, high_Fc/sampleRate , Qfactor, (-1.0*defaultGain));
#endif
	controlFilterUpdate = 0;
	filterSwitch = 1;

	configureAudio();
	arg = BOARD_INIT_UART_STDIO;
	stat = Board_init(arg);
	
    McaspDevice_init();

	Aic31_init();

    Log_info0("\r\nAudio Sample Main\n");

    BIOS_start();

    return;
}

/*
 * Mcasp init function called when creating the driver.
 */
/* Task that would call the appropriate demo after reading user's choice */
Void ReadInputTask(UArg arg0, UArg arg1)
{
	int			selection;
	char Response[10];
	BIQUAD_T *selectFilter;

		UART_printf ("\n\n======================================================\n\r\n", -1);
		UART_printf (" Currently playing Audio effect Demo. \n Make sure a stereo input is connected to Line In and\n", -1);
		UART_printf (" Connect an headphone or a speaker to LineOut. ", -1);
		UART_printf ("\n\n======================================================\n\r\n", -1);
		UART_printf (" Demo currently shows 3 band equalization using shelving biquad filters\n ", -1);
		UART_printf ("* low band = 0Hz to 250Hz\n * mid band = 250Hz to 1000Hz,\n * high band = 1000Hz to 22000Hz\n ", -1);
		UART_printf (" All bands are boosted with a 12db Peak Gain\n ", -1);

	while(1)
		{
		 if (filterSwitch)
				   UART_printf ("\n\nDo you wish to keep the equalization ON ? (y or n)\r\n\r", -1);
		 else
				   UART_printf ("\n\nDo you wish turn ON the equalization? (y or n)\r\n\r", -1);

		 UART_scanFmt("%c", Response);
		 UART_printf ("", -1);


		 if (Response[0] == 'y') {
			 filterSwitch = 1;
			 UART_printf ("\n======================================================\n\r\n", -1);
			 UART_printf ("Press 0 and hit Enter to print current gain settings\r\n\r\n", -1);
			 UART_printf ("OR  \r\n\r\n", -1);
			 UART_printf ("Select the Frequency Band to adjust the gain\r\n\r\n", -1);
			 UART_printf ("*   Press 1 for Low  and hit Enter\r\n", -1);
			 UART_printf ("*   Press 2 for High  and hit Enter\r\n", -1);
			 UART_printf ("*   Press 3 for Mid and hit Enter\r\n", -1);
			 UART_printf ("All other input will not have any effect \r\n\r\n", -1);
			 UART_printf ("======================================================\n\r\n", -1);
			 UART_scanFmt("%s", Response);
			 UART_printf ("\n",-1);
			
			 selection = atoi(Response);

			 switch (selection) {
			 	 case 0:
			 		 UART_printf ("\n",-1);
			 		 UART_printf ("  Current gain settings :\n",-1);
			 		 UART_printf ("  Low: %sdb", itoa((int)low_LShelf_Filter->peakGain,Response));
			 		 UART_printf ("  Mid: %sdb", itoa((int)mid_HShelf_Filter->peakGain,Response));
			 		 UART_printf ("  High: %sdb", itoa((int)high_HShelf_Filter->peakGain,Response));
			 		 UART_printf ("\n",-1);
			 	    break;
		  	 	 case 1:
					 selectFilter= low_LShelf_Filter;
					 break;
				 case 2:
					 selectFilter= high_HShelf_Filter;
					 break;
				 case 3:
#ifdef BANDPASS
					selectFilter= mid_BPass_Filter;
#else
					selectFilter= mid_LShelf_Filter;
#endif
					 
					 break;
				 default:
					 UART_printf ("Incorrect input \r\n", -1);
			 }

#ifdef BANDPASS
			 if (selection > 0 && selection <=3){
			 			   UART_printf ("Enter the value of gain(Recommendation -100 to 20)\r\n", -1);
			 			   UART_scanFmt("%s", Response);
			 			  // UART_printf ("%s", Response);

			 			   while(!controlFilterUpdate);
			 			   selection = atoi(Response);
			 			   Biquad_setPeakGain(selectFilter, (double)selection);
			 			   UART_printf ("Filter updated\r\n", -1);

			 }
#else

			 if (selection > 0 && selection <3){
			   UART_printf ("Enter the value of gain in dB (Recommendation -20 to 20)\r\n", -1);
			   UART_scanFmt("%s", Response);

			   while(!controlFilterUpdate);
			   selection = atoi(Response);
			   Biquad_setPeakGain(selectFilter, (double)selection);
			   UART_printf ("Gain updated\r\n", -1);
			 }
			 else if (selection == 3){
	      	    UART_printf ("Enter the value of gain in dB (Recommendation -20 to 20)\r\n", -1);
				UART_scanFmt("%s", Response);

    			while(!controlFilterUpdate);
				selection = atoi(Response);
				Biquad_setPeakGain(mid_HShelf_Filter, (double)selection);
				Biquad_setPeakGain(mid_LShelf_Filter, (double)(-1*selection));
				UART_printf ("Gain updated\r\n", -1);

			 }
#endif

		 }
         else if (Response[0] == 'n'){
        	filterSwitch = 0;
        	UART_printf ("Equalization OFF\r\n", -1);
         }
         else
        	 UART_printf ("Please select 'y' or 'n' \r\n", -1);

	}
}


int i2a(char *s, int n){
    div_t qr;
    int pos;

    if(n == 0) return 0;

    qr = div(n, 10);
    pos = i2a(s, qr.quot);
    s[pos] = qr.rem + '0';
    return pos + 1;
}

char* itoa(int num, char *buff){
    char *p = buff;
    if(num < 0){
        *p++ = '-';
        num *= -1;
    } else if(num == 0)
        *p++ = '0';
    p[i2a(p, num)]='\0';
    return buff;
}

/* ========================================================================== */
/*                                END OF FILE                                 */
/* ========================================================================== */
