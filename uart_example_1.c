/**
  @file
  @brief
  UART Example 1

  This example program will echo back the input from UART0 onto UART0 in a
  polled function.
*/
/*
 * ============================================================================
 * History
 * =======
 * 30 Oct 2014 : Created
 *
 * Copyright (C) Bridgetek Pte Ltd
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd 
 * ("Bridgetek") subject to the licence terms set out
 * http://brtchip.com/BRTSourceCodeLicenseAgreement/ ("the Licence Terms").
 * You must read the Licence Terms before downloading or using the Software.
 * By installing or using the Software you agree to the Licence Terms. If you
 * do not agree to the Licence Terms then do not download or use the Software.
 *
 * Without prejudice to the Licence Terms, here is a summary of some of the key
 * terms of the Licence Terms (and in the event of any conflict between this
 * summary and the Licence Terms then the text of the Licence Terms will
 * prevail).
 *
 * The Software is provided "as is".
 * There are no warranties (or similar) in relation to the quality of the
 * Software. You use it at your own risk.
 * The Software should not be used in, or for, any medical device, system or
 * appliance. There are exclusions of Bridgetek liability for certain types of loss
 * such as: special loss or damage; incidental loss or damage; indirect or
 * consequential loss or damage; loss of income; loss of business; loss of
 * profits; loss of revenue; loss of contracts; business interruption; loss of
 * the use of money or anticipated savings; loss of information; loss of
 * opportunity; loss of goodwill or reputation; and/or loss of, damage to or
 * corruption of data.
 * There is a monetary cap on Bridgetek's liability.
 * The Software may have subsequently been amended by another user and then
 * distributed by that other user ("Adapted Software").  If so that user may
 * have additional licence terms that apply to those amendments. However, Bridgetek
 * has no liability in relation to those amendments.
 * ============================================================================
 */

#include <stdint.h>
#include <string.h>
#include "ft900.h"

void setup(void);
void loop(void);


void myputc(void* p, char c)
{
    uart_write((ft900_uart_regs_t*)p, (uint8_t)c);
}

int main(void)
{
    setup();
    for(;;)
    {
        loop();
    }
    return 0;
}

void setup()
{
    /* Enable the UART Device... */
    sys_enable(sys_device_uart0);
    /* Set UART0 GPIO functions to UART0_TXD and UART0_RXD... */
#if defined(__FT900__)
    gpio_function(48, pad_uart0_txd); /* UART0 TXD */
    gpio_function(49, pad_uart0_rxd); /* UART0 RXD */
#elif defined(__FT930__)
    gpio_function(23, pad_uart0_txd); /* UART0 TXD */
    gpio_function(22, pad_uart0_rxd); /* UART0 RXD */
#endif
    uart_open(UART0,                    /* Device */
              1,                        /* Prescaler = 1 */
              UART_DIVIDER_19200_BAUD,  /* Divider = 1302 */
              uart_data_bits_8,         /* No. Data Bits */
              uart_parity_none,         /* Parity */
              uart_stop_bits_1);        /* No. Stop Bits */

    /* Print out a welcome message... */
    uart_puts(UART0,
        "\x1B[2J" /* ANSI/VT100 - Clear the Screen */
        "\x1B[H"  /* ANSI/VT100 - Move Cursor to Home */
        "Copyright (C) Bridgetek Pte Ltd \r\n"
        "--------------------------------------------------------------------- \r\n"
        "Welcome to UART Example 1... \r\n"
        "\r\n"
        "issue commands on uart as below: \r\n"
        " \"w 100 23\", means <write mode> <RAM address to write> <value to write> \r\n"
    	" \"r 100\"   , means <read mode> <RAM address to read> \r\n"
        "--------------------------------------------------------------------- \r\n"
        );
    /* Enable tfp_printf() functionality... */
    init_printf(UART0,myputc);
}

void loop()
{

    //commands can be "w 100 23"; // "<write mode> <RAM address to write> <value to write>"
    //commands can be "r 100";  // "<read mode> <RAM address to read>
    char ip[100]=""; // to store the command from the user
    char mode;
    int iaddress;
    int ivalue;

   	char saddress[9];
	char svalue[6];
	int i=2;
	int j=0;
	int k=0;
// read the command from UART
	{
		int x = 0;
		uart_read(UART0, &ip[0]);
		for(; ip[x]!=0x0D; x++)
		{
			uart_write(UART0, ip[x]);
			uart_read(UART0, &ip[x+1]);
		}
		ip[x] = '\0';
	}


// deciphering the command
    mode = ip[0]; //extracting 'mode'

    if ((mode =='w') || (mode =='r'))
    {
		for(;(ip[i] !=' ') && (ip[i]!='\0');i++,j++)
		{
			 saddress[j] = ip[i]; //extracting 'address' component
		}

		saddress[j] = '\0';
		iaddress = atoi(saddress); //converting string address to integer address
		i++; //move to next component

	    unsigned int *memaddr = iaddress;
		if (mode =='w')
		{
			for (;ip[i]!='\0';i++,k++)
			{
				svalue[k] = ip[i]; // extracting 'value' component
			}

			svalue[k] = '\0';
			ivalue =  atoi(svalue); //converting string value to integer value

			tfp_printf("\n");
		    tfp_printf("mode = %c, iaddress = %d, ivalue = %d \n\n", mode,iaddress,ivalue);
			*memaddr = ivalue;
			tfp_printf("after write: %d\n", *memaddr);

		}
		else
		{
			tfp_printf("\n");
			tfp_printf("mode = %c, iaddress = %d\n", mode,iaddress);
		    tfp_printf("after read: %d\n", *memaddr);
		}

    }
    else
    {
    	//error
    	tfp_printf("error please try again!!!\r\n");
    }
}
