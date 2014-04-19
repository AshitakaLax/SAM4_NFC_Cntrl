/*
 * pn532Cntrl.c
 *
 * Created: 4/19/2014 4:03:51 PM
 *  Author: Levi Balling
 */ 

#include "pn532Cntrl.h"
#include "stdio_serial.h"
#include <string.h>


uint8_t pn532ack[] = {
  0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00};
uint8_t pn532response_firmwarevers[] = {
  0x00, 0xFF, 0x06, 0xFA, 0xD5, 0x03};

#define PN532_PACK_BUFF_SIZE 64
uint8_t pn532_packetbuffer[PN532_PACK_BUFF_SIZE];


/**************
* private prototypes
**************/
 uint8_t _cs;//Chip seclect pin for PN532 is optional.
// SPIClass pn532_SPI;

 void write(uint8_t _data);
 uint8_t readByte(void);//reads a single byte
 uint8_t readSpiStatus(void);
 bool checkSpiAck();
 void read(uint8_t* buff, uint8_t n);//reads n bytes, and puts them in buff
 void writeCommand(uint8_t* cmd, uint8_t cmdlen);


/**************
* Public items
**************/

// Initialize the PN532 chip
// currently the cs does nothing
void initPN532(uint8_t cs)
{
	//todo set the CS to be an output and high
}

void begin(void)
{
	//initializes the spi interface
	
	//Spi interface should be in mode 0(data rate)
	//LSB order
	//frequency is 1MHZ Opt 1
	//CS to Low
	//wait for it
	//sync up with the board with a dummy response
	sendCommandCheckAck(pn532_packetbuffer, 1);
}

//not sure what this is for
bool SAMConfig(void)
{
	
	pn532_packetbuffer[0] = PN532_SAMCONFIGURATION;
	pn532_packetbuffer[1] = 0x01; // normal mode;
	pn532_packetbuffer[2] = 0x14; // timeout 50ms * 20 = 1 second
	pn532_packetbuffer[3] = 0x01; // use IRQ pin!

	if (! sendCommandCheckAck(pn532_packetbuffer, 4))
	return false;

	// read data packet
	read(pn532_packetbuffer, 8);

	return  (pn532_packetbuffer[5] == 0x15);
}

uint32_t getFirmwareVersion(void)
{
	uint32_t response;

	pn532_packetbuffer[0] = PN532_FIRMWAREVERSION;

	if (!sendCommandCheckAck(pn532_packetbuffer, 1))
		return 0;

	//Read data packet
	read(pn532_packetbuffer, 12);
	
	//Check some basic stuff
	//compares that is a valid response
	if (0 != strncmp((char *)pn532_packetbuffer, (char *)pn532response_firmwarevers, 6))
	{
		return 0;
	}

	response = pn532_packetbuffer[6];
	response <<= 8;
	response |= pn532_packetbuffer[7];
	response <<= 8;
	response |= pn532_packetbuffer[8];
	response <<= 8;
	response |= pn532_packetbuffer[9];

	return response;
	
}

uint32_t readPassiveTargetID(uint8_t cardbaudrate)
{
	
	
}

uint32_t authenticateBlock(	uint8_t cardnumber /*1 or 2*/, uint32_t cid /*Card NUID*/, uint8_t blockaddress /*0 to 63*/,
uint8_t authtype /*Either KEY_A or KEY_B */, uint8_t* keys)
{
	 pn532_packetbuffer[0] = PN532_INDATAEXCHANGE;
	 pn532_packetbuffer[1] = cardnumber;  // either card 1 or 2 (tested for card 1)
	 if(authtype == KEY_A)
	 {
		 pn532_packetbuffer[2] = PN532_AUTH_WITH_KEYA;
	 }
	 else
	 {
		 pn532_packetbuffer[2] = PN532_AUTH_WITH_KEYB;
	 }
	 pn532_packetbuffer[3] = blockaddress; //This address can be 0-63 for MIFARE 1K card

	 pn532_packetbuffer[4] = keys[0];
	 pn532_packetbuffer[5] = keys[1];
	 pn532_packetbuffer[6] = keys[2];
	 pn532_packetbuffer[7] = keys[3];
	 pn532_packetbuffer[8] = keys[4];
	 pn532_packetbuffer[9] = keys[5];

	 pn532_packetbuffer[10] = ((cid >> 24) & 0xFF);
	 pn532_packetbuffer[11] = ((cid >> 16) & 0xFF);
	 pn532_packetbuffer[12] = ((cid >> 8) & 0xFF);
	 pn532_packetbuffer[13] = ((cid >> 0) & 0xFF);

	 if (! sendCommandCheckAck(pn532_packetbuffer, 14))
	 return false;

	 // read data packet
	 read(pn532_packetbuffer, 2+6);

	 #ifdef PN532DEBUG
	 for(int iter=0;iter<14;iter++)
	 {
		 puts(pn532_packetbuffer[iter], HEX);
		 puts(" ");
	 }
	 // check some basic stuff

	 puts("AUTH");
	 for(uint8_t i=0;i<2+6;i++)
	 {
		 puts(pn532_packetbuffer[i], HEX);
		 puts(" ");
	 }
	 #endif

	 if((pn532_packetbuffer[6] == 0x41) && (pn532_packetbuffer[7] == 0x00))
	 {
		 return true;
	 }
	 else
	 {
		 return false;
	 }
}


/****************************************************************************/
/*Function: Read a block(16 uint8_ts) from the tag and stores in the parameter.*/
/*Parameter:-uint8_t cardnumber,can be 1 or 2;                              */
/*          -blockaddress,range from 0 to 63;                               */
/*	    -uint8_t* block,will save 16uint8_ts that read from tag.				*/
/*Return:   bool         					      						*/
bool readMemoryBlock(uint8_t cardnumber /*1 or 2*/,uint8_t blockaddress /*0 to 63*/, uint8_t * block)
{

	pn532_packetbuffer[0] = PN532_INDATAEXCHANGE;
	pn532_packetbuffer[1] = cardnumber;  // either card 1 or 2 (tested for card 1)
	pn532_packetbuffer[2] = PN532_MIFARE_READ;
	pn532_packetbuffer[3] = blockaddress; //This address can be 0-63 for MIFARE 1K card

	if (! sendCommandCheckAck(pn532_packetbuffer, 4))
	return false;

	// read data packet
	read(pn532_packetbuffer, 18+6);
	// check some basic stuff
	#ifdef PN532DEBUG
	puts("READ");
	#endif
	for(uint8_t i=8;i<18+6;i++)
	{
		block[i-8] = pn532_packetbuffer[i];
		#ifdef PN532DEBUG
		puts(pn532_packetbuffer[i], HEX);
		puts(" ");
		#endif
	}
	if((pn532_packetbuffer[6] == 0x41) && (pn532_packetbuffer[7] == 0x00))
	{
		return true; //read successful
	}
	else
	{
		return false;
	}	
}


/****************************************************************************/
/*Function: Write a block(16 uint8_ts) to the tag.                             */
/*Parameter:-uint8_t cardnumber,can be 1 or 2;                              */
/*          -blockaddress,range from 0 to 63;                               */
/*	    -uint8_t* block,saves 16uint8_ts that will write to the tag.			*/
/*Return:  bool															*/
/*Note:Donot write to Sector Trailer Block unless you know what you are doing.*/
bool writeMemoryBlock(uint8_t cardnumber /*1 or 2*/,uint8_t blockaddress /*0 to 63*/, uint8_t * block)
{
	
	pn532_packetbuffer[0] = PN532_INDATAEXCHANGE;
	pn532_packetbuffer[1] = cardnumber;  // either card 1 or 2 (tested for card 1)
	pn532_packetbuffer[2] = PN532_MIFARE_WRITE;
	pn532_packetbuffer[3] = blockaddress;

	for(uint8_t uint8_t=0; uint8_t <16; uint8_t++)
	{
		pn532_packetbuffer[4+uint8_t] = block[uint8_t];
	}

	if (! sendCommandCheckAck(pn532_packetbuffer, 20))
	return false;
	// read data packet
	read(pn532_packetbuffer, 2+6);

	#ifdef PN532DEBUG
	// check some basic stuff
	puts("WRITE");
	for(uint8_t i=0;i<2+6;i++)
	{
		puts(pn532_packetbuffer[i], HEX);
		puts(" ");
	}
	#endif

	if((pn532_packetbuffer[6] == 0x41) && (pn532_packetbuffer[7] == 0x00))
	{
		return true; //write successful
	}
	else
	{
		return false;
	}
	
}

/**********************************************************************/
/*Function: Configure the NFC shield as initiator in the peer to peer */
/*	      commnunication and only the initiator set the baud rate.*/
/*Parameter:-uint8_t baudrate,Any number from 0-2. 0 for 106kbps or 
 	    1 for 201kbps or 2 for 424kbps but 106kps is not supported yet;*/
/*Return: bool,ture = the shield finds the target and is configured */
/*	as initiator successfully.                      	       */
uint32_t configurePeerAsInitiator(uint8_t baudrate /* Any number from 0-2. 0 for 106kbps or 1 for 201kbps or 2 for 424kbps */) //106kps is not supported
{
		
		pn532_packetbuffer[0] = PN532_INJUMPFORDEP;
		pn532_packetbuffer[1] = 0x01; //Active Mode
		pn532_packetbuffer[2] = baudrate;// Use 1 or 2. //0 i.e 106kps is not supported yet
		pn532_packetbuffer[3] = 0x01; //Indicates Optional Payload is present

		//Polling request payload
		pn532_packetbuffer[4] = 0x00;
		pn532_packetbuffer[5] = 0xFF;
		pn532_packetbuffer[6] = 0xFF;
		pn532_packetbuffer[7] = 0x00;
		pn532_packetbuffer[8] = 0x00;

		if (!sendCommandCheckAck(pn532_packetbuffer, 9))
		return false;

		// read data packet
		read(pn532_packetbuffer, 19+6);

		#ifdef PN532DEBUG
			// check the response
			puts("\nPEER_INITIATOR");
			for(uint8_t i=0;i<19+6;i++)
			{
				puts(pn532_packetbuffer[i], HEX);
				puts(" ");
			}
		#endif

		return (pn532_packetbuffer[7] == 0x00); //No error

	
}

uint32_t configurePeerAsTarget()
{
	  uint8_t pbuffer[38] =      {
		  PN532_TGINITASTARGET,
		  0x00,
		  0x08, 0x00, //SENS_RES
		  0x12, 0x34, 0x56, //NFCID1
		  0x40, //SEL_RES
		  0x01, 0xFE, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, // POL_RES
		  0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
		  0xFF, 0xFF,
		  0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, //NFCID3t: Change this to desired value
		  0x00, 0x00 //Length of general and historical uint8_ts
	  };
	  for(uint8_t i = 0;i < 38;i ++)
	  {
		  pn532_packetbuffer[i] = pbuffer[i];
	  }
	  if (! sendCommandCheckAck(pn532_packetbuffer, 38))
	  return false;

	  // read data packet
	  read(pn532_packetbuffer, 18+6);

	  #ifdef PN532DEBUG
	  // check some basic stuff

	  puts("PEER_TARGET");
	  for(uint8_t i=0;i<18+6;i++)
	  {
		  puts(pn532_packetbuffer[i], HEX);
		  puts(" ");
	  }
	  #endif

	  return (pn532_packetbuffer[23] == 0x00); //No error as it received all response
	
}

/**********************************************************************/
/*Function: Transmit to the target and receive from the target.       */
/*Parameter:-char* dataOut,data buffer to send;                       */
/*	    -char* dataIn,data buffer to save the data receive.       */
/*Return:   bool,ture = No error                                   */
bool initiatorTxRx(char* dataOut,char* dataIn)
{
	
	pn532_packetbuffer[0] = PN532_INDATAEXCHANGE;
	pn532_packetbuffer[1] = 0x01; //Target 01

	for(uint8_t iter=(2+0);iter<(2+16);iter++)
	{
		pn532_packetbuffer[iter] = dataOut[iter-2]; //pack the data to send to target
	}

	if (! sendCommandCheckAck(pn532_packetbuffer, 18))
	return false;

	// read data packet
	read(pn532_packetbuffer, 18+6);

	#ifdef PN532_P2P_DEBUG
	// check the response
	puts("INITIATOR receive:");
	for(uint8_t i=0;i<18+6;i++)
	{
		puts(pn532_packetbuffer[i], HEX);
		puts(" ");
	}
	#endif

	for(uint8_t iter=8;iter<(8+16);iter++)
	{
		dataIn[iter-8] = pn532_packetbuffer[iter]; //data received from target
	}

	return (pn532_packetbuffer[7] == 0x00); //No error
}

/*Function: Recieve data first and then transmit data to the initiator*/
uint32_t targetTxRx(char* dataOut,char* dataIn)
{

	/* Receiving from Initiator */
	pn532_packetbuffer[0] = PN532_TGGETDATA;
	if (! sendCommandCheckAck(pn532_packetbuffer, 1))
	return false;

	// read data packet
	read(pn532_packetbuffer, 18+6);

	#ifdef PN532_P2P_DEBUG
	
	// check the response
	puts("TARGET RX:");
	for(uint8_t i=0;i<18+6;i++)
	{
		puts(pn532_packetbuffer[i], HEX);
		puts(" ");
	}
	#endif

	for(uint8_t iter=8;iter<(8+16);iter++)
	{
		dataIn[iter-8] = pn532_packetbuffer[iter]; //data received from initiator
	}

	/* Sending to Initiator */
	if(pn532_packetbuffer[7] == 0x00) //If no errors in receiving, send data.
	{
		pn532_packetbuffer[0] = PN532_TGSETDATA;
		for(uint8_t iter=(1+0);iter<(1+16);iter++)
		{
			pn532_packetbuffer[iter] = dataOut[iter-1]; //pack the data to send to target
		}

		if (! sendCommandCheckAck(pn532_packetbuffer, 17))
		return false;

		// read data packet
		read(pn532_packetbuffer, 2+6);

		#ifdef PN532_P2P_DEBUG
		
		// check the response
		puts("TARGET get response after transmitting: ");
		for(uint8_t i=0;i<2+6;i++)
		{
			puts(pn532_packetbuffer[i], HEX);
			puts(" ");
		}
		#endif

		return (pn532_packetbuffer[7] == 0x00); //No error
	}
	
	
}


/**********************************************************************/
/*Function: Send command to PN532 with SPI and check the ACK.          */
/*Parameter:-uint8_t* cmd,The pointer that saves the command code to be sent;*/
/*          -uint8_t cmd_len,The number of uint8_ts of the command;        */
/*	    -uint16_t timeout,default timeout is one second            */
/*Return:   bool,ture = send command successfully	               */
bool sendCommandCheckAck(uint8_t *cmd, uint8_t cmd_len)
{
	
	uint16_t timer = 0;
	uint16_t timeout = 1000;

	// write the command
	writeCommand(cmd, cmd_len);

	// Wait for chip to say it's ready!
	while (readSpiStatus() != PN532_SPI_READY) {
		if (timeout != 0) {
			timer+=10;
			if (timer > timeout)
			return false;
		}
		//todo add a 10 ms Delay here
		//delay(10);
	}

	// read acknowledgement
	if (!checkSpiAck()) {
		return false;
	}

	timer = 0;
	// Wait for chip to say its ready!
	while (readSpiStatus() != PN532_SPI_READY) {
		if (timeout != 0) {
			timer+=10;
			if (timer > timeout)
			return false;
		}
		//todo add a 10 ms delay
		//delay(10);
	}

	return true; // ack'd command
	
}




/**************
* Private items
**************/


/************** low level SPI ********/
/*Function:Transmit a uint8_t to PN532 through the SPI interface. */
 void write(uint8_t _data)
 {
	 //todo write data accross the spi interface
 }

/*Function:Receive a uint8_t from PN532 through the SPI interface */
 uint8_t readByte(void)
 {
	 //todo read Byte from the spi interface
	 uint8_t data_ = 0;//pn532_SPI.transfer(0);
	 return data_;
 }

 uint8_t readSpiStatus(void)
 {
	 //Set CS Low
	 //wait 2 ms
	 //Write
	 write(PN532_SPI_STATREAD);
	 uint8_t status = readByte();
	 //set CS High
	 return status;
 }

 bool checkSpiAck()
 {
	 uint8_t ackbuff[6];

	 read(ackbuff, 6);

	 return (0 == strncmp((char *)ackbuff, (char *)pn532ack, 6));
 }

 void read(uint8_t* buff, uint8_t n)
 {
	 //set CS Low
//	 digitalWrite(_cs, LOW);
	 //delay(2); wait 2 ms
	 write(PN532_SPI_DATAREAD);

	 #ifdef PN532DEBUG
	 puts("Reading: ");
	 #endif
	 for (uint8_t i=0; i < n; i ++)
	 {
		 //todo delay 1ms
		 //delay(1);
		 buff[i] = readByte();
		 #ifdef PN532DEBUG
		 puts(" 0x");
		 puts(buff[i], HEX);
		 #endif
	 }
 }

 void writeCommand(uint8_t* cmd, uint8_t cmd_len)
 {
	 uint8_t checksum;

	 cmd_len++;
	 #ifdef PN532DEBUG
	 puts("\nSending: ");
	 #endif

	//set CS Low
	// digitalWrite(_cs, LOW);
	//wait for 2 ms
	 //delay(2);     // or whatever the delay is for waking up the board
	 write(PN532_SPI_DATAWRITE);

	 checksum = PN532_PREAMBLE + PN532_PREAMBLE + PN532_STARTCODE2;
	 write(PN532_PREAMBLE);
	 write(PN532_PREAMBLE);
	 write(PN532_STARTCODE2);

	 write(cmd_len);
	 uint8_t cmdlen_1=~cmd_len + 1;
	 write(cmdlen_1);

	 write(PN532_HOSTTOPN532);
	 checksum += PN532_HOSTTOPN532;

	 #ifdef PN532DEBUG
	 puts(" 0x");
	 puts(PN532_PREAMBLE, HEX);
	 puts(" 0x");
	 puts(PN532_PREAMBLE, HEX);
	 puts(" 0x");
	 puts(PN532_STARTCODE2, HEX);
	 puts(" 0x");
	 puts(cmd_len, HEX);
	 puts(" 0x");
	 puts(cmdlen_1, HEX);
	 puts(" 0x");
	 puts(PN532_HOSTTOPN532, HEX);
	 #endif

	 for (uint8_t i=0; i<cmd_len-1; i++)
	 {
		 write(cmd[i]);
		 checksum += cmd[i];
		 #ifdef PN532DEBUG
		 puts(" 0x");
		 puts(cmd[i], HEX);
		 #endif
	 }
	 uint8_t checksum_1=~checksum;
	 write(checksum_1);
	 write(PN532_POSTAMBLE);
	 //set CS high
//	 digitalWrite(_cs, HIGH);

	 #ifdef PN532DEBUG
	 puts(" 0x");
	 puts(checksum_1, HEX);
	 puts(" 0x");
	 puts(PN532_POSTAMBLE, HEX);
	 #endif
 }
 
