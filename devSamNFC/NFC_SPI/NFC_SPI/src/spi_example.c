/**
 * \file
 *
 * \brief Serial Peripheral Interface (SPI) example for SAM.
 *
 * Copyright (c) 2011-2013 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

/**
 * \mainpage SPI Example
 *
 * \par Purpose
 *
 * This example uses Serial Peripheral Interface (SPI) of one EK board in
 * slave mode to communicate with another EK board's SPI in master mode.
 *
 * \par Requirements
 *
 * This package can be used with two SAM evaluation kits boards.
 * Please connect the SPI pins from one board to another.
 * \copydoc spi_example_pin_defs
 *
 * \par Descriptions
 *
 * This example shows control of the SPI, and how to configure and
 * transfer data with SPI. By default, example runs in SPI slave mode,
 * waiting SPI slave & UART inputs.
 *
 * The code can be roughly broken down as follows:
 * <ul>
 * <li> 't' will start SPI transfer test.
 * <ol>
 * <li>Configure SPI as master, and set up SPI clock.
 * <li>Send 4-byte CMD_TEST to indicate the start of test.
 * <li>Send several 64-byte blocks, and after transmitting the next block, the
 * content of the last block is returned and checked.
 * <li>Send CMD_STATUS command and wait for the status reports from slave.
 * <li>Send CMD_END command to indicate the end of test.
 * </ol>
 * <li>Setup SPI clock for master.
 * </ul>
 *
 * \par Usage
 *
 * -# Compile the application.
 * -# Connect the UART port of the evaluation board to the computer and open
 * it in a terminal.
 *    - Settings: 115200 bauds, 8 bits, 1 stop bit, no parity, no flow control.
 * -# Download the program into the evaluation board and run it.
 * -# Upon startup, the application will output the following line on the
 *    terminal:
 *    \code
 *     -- Spi Example  --
 *     -- xxxxxx-xx
 *     -- Compiled: xxx xx xxxx xx:xx:xx --
 *    \endcode
 * -# The following traces detail operations on the SPI example, displaying
 *    success or error messages depending on the results of the commands.
 *
 */

#include "asf.h"
#include "stdio_serial.h"
#include "conf_board.h"
#include "conf_clock.h"
#include "conf_spi_example.h"
#include "devSpiCntrl.h"
#include "pn532Cntrl.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

#define STRING_EOL    "\r"
#define STRING_HEADER "--Spi Example --\r\n" \
		"-- "BOARD_NAME" --\r\n" \
		"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL



/**
 * \brief Start SPI transfer test.
 */
static void spi_master_go(void)
{
	//do stuff	
	while(1)
	{
		uint32_t id = 0;
		readPassiveTargetID(PN532_MIFARE_ISO14443A);//todo may need to change
		if(!id)
		{
			printf("\n\rMIFARE ID =%08x", id);	
		}
		else
		{
			puts("\n\rNo ID");	
			
		}
	}
	
	
}
	//uint32_t cmd;
	//uint32_t block;
	//uint32_t i;
//
	///* Configure SPI as master, set up SPI clock. */
	//spi_master_initialize();
//
	///*
	 //* Send CMD_TEST to indicate the start of test, and device shall return
	 //* RC_RDY.
	 //*/
	//while (1) {
		//cmd = CMD_TEST;
		//puts("-> Master sending CMD_TEST... \r");
		//spi_master_transfer(&cmd, sizeof(cmd));
		//if (cmd == RC_RDY) {
			//puts("   <- Slave response RC_SYN, RC_RDY \r");
			//break;
		//}
		//if (cmd != RC_SYN) {
			//printf("-E- Response unexpected: 0x%x \n\r", (unsigned)cmd);
			//return;
		//}
	//}
	///* Send CMD_DATA with 4 blocks (64 bytes per page). */
	//puts("-> Master sending CMD_DATA... \r");
	//cmd = CMD_DATA | MAX_DATA_BLOCK_NUMBER;
	//
	////load command to the device
	//
	//spi_master_transfer(&cmd, sizeof(cmd));
	//puts("                                <---- Slave response RC_RDY \r");
	//for (block = 0; block < MAX_DATA_BLOCK_NUMBER; block++) {
		//for (i = 0; i < COMM_BUFFER_SIZE; i++) {
			//gs_uc_spi_buffer[i] = block;
		//}
		//printf("-> Master sending block %u ... \n\r", (unsigned)block);
		//spi_master_transfer(gs_uc_spi_buffer, COMM_BUFFER_SIZE);
		//if (block) {
			//for (i = 0; i < COMM_BUFFER_SIZE; i++) {
				//if (gs_uc_spi_buffer[i] != (block - 1)) {
					//break;
				//}
			//}
			//if (i < COMM_BUFFER_SIZE) {
				//printf("-E- block %u contains unexpected data \n\r",
					//(unsigned)block);
			//} else {
				//printf("   <- Slave response last block %x \n\r",
					//(unsigned)(block - 1));
			//}
		//}
	//}
//
	//for (i = 0; i < MAX_RETRY; i++) {
		//cmd = CMD_STATUS;
		//puts("-> Master sending CMD_STATUS... \r");
		//spi_master_transfer(&cmd, sizeof(cmd));
		//if (cmd == RC_RDY) {
			//puts("   <- Slave response RC_RDY \r");
			//break;
		//}
	//}
	//if (i >= MAX_RETRY) {
		//puts("   <- Slave no response \r");
		//return;
	//}
//
	//puts("   <- Slave reports status...\r");
	//printf("-I- Received  %u commands:",
		//(unsigned)gs_spi_status.ul_total_command_number);
//
	//for (i = 0; i < gs_spi_status.ul_total_command_number; i++) {
		//printf(" 0x%08x", (unsigned)gs_spi_status.ul_cmd_list[i]);
	//}
	//printf(" \n\r-I- Received  %lu data blocks \n\r",
			//(unsigned long)gs_spi_status.ul_total_block_number);
//
	//for (i = 0; i < MAX_RETRY; i++) {
		//puts("-> Master sending CMD_END... \r");
		//cmd = CMD_END;
		//spi_master_transfer(&cmd, sizeof(cmd));
//
		//if (cmd == RC_SYN) {
			//puts("   <- Slave response RC_SYN \r");
			//break;
		//}
	//}
//
	//if (i >= MAX_RETRY) {
		//puts("   <- Slave no response \r");
	//}
//
	//puts("\r");
	//puts("SPI transfer test finished! \r");
//}

/**
 *  \brief Configure the Console UART.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
#ifdef CONF_UART_CHAR_LENGTH
		.charlength = CONF_UART_CHAR_LENGTH,
#endif
		.paritytype = CONF_UART_PARITY,
#ifdef CONF_UART_STOP_BITS
		.stopbits = CONF_UART_STOP_BITS,
#endif
	};

	/* Configure console UART. */
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

/**
 * \brief Application entry point for SPI example.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	/* Initialize the SAM system. */
	sysclk_init();
	board_init();

	/* Initialize the console UART. */
	configure_console();

	/* Output example information. */
	puts(STRING_HEADER);

	//initializes spi and the library for this
	initPN532();
	
	uint32_t versionData = getFirmwareVersion();
	if(!versionData)
	{
		puts("ERROR NO NFC DEVICE");
	}
	else
	{
		
		printf("VersionData %08x", versionData);
	}
	
	//configure with the SAMconfig thingy
	SAMConfig();
	
	
	spi_master_go();

}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond
