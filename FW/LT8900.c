//#include "config.h"
#include "lt8900.h"

uint8_t ActChannel = DEFAULT_CHANNEL;

void LT8900_Init(void) {

  //delay_init(16);
  GPIO_Init(LT8900_PKT_GPIO_PORT, LT8900_PKT_PIN, GPIO_MODE_IN_FL_NO_IT);
  GPIO_Init(LT8900_NCS_GPIO_PORT, LT8900_NCS_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_WriteHigh(LT8900_NCS_GPIO_PORT,(GPIO_Pin_TypeDef) LT8900_NCS_PIN);                        //deselect lt8900

  SPI_DeInit();
  /* Initialize SPI in Slave mode  */
  SPI_Init(SPI_FIRSTBIT_MSB, SPI_PRESC, SPI_MODE_MASTER, SPI_CLOCKPOLARITY_LOW,
           SPI_CLOCKPHASE_2EDGE, SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT,(uint8_t)0x07);
  SPI_Cmd(ENABLE);
}

void LT8900_InitRegisters(void) {

	LT8900_writeRegister(0, 0x6fe0);
  LT8900_writeRegister(1, 0x5681);
  LT8900_writeRegister(2, 0x6617);
  LT8900_writeRegister(4, 0x9cc9);    //why does this differ from powerup (5447)
  LT8900_writeRegister(5, 0x6637);    //why does this differ from powerup (f000)
  LT8900_writeRegister(7, 0x004c);    //ch76
  LT8900_writeRegister(8, 0x6c90);    //power (default 71af) UNDOCUMENTED
  LT8900_writeRegister(9, 0x1840);		//Tx Power Level Max AAAA xBBB Bxxx xxxx A:PA current Control, B:Amplifier Gain
  //setCurrentControl(4, 0);     // power & gain.

  LT8900_writeRegister(10, 0x7ffd);   //bit 0: XTAL OSC enable
  LT8900_writeRegister(11, 0x0008);   //bit 8: Power down RSSI (0=  RSSI operates normal)
  LT8900_writeRegister(12, 0x0000);
  LT8900_writeRegister(13, 0x48bd);   //(default 4855)

  LT8900_writeRegister(22, 0x00ff);
  LT8900_writeRegister(23, 0x8005);  //bit 2: Calibrate VCO before each Rx/Tx enable
  LT8900_writeRegister(24, 0x0067);
  LT8900_writeRegister(25, 0x1659);
  LT8900_writeRegister(26, 0x19e0);
  LT8900_writeRegister(27, 0x1300);  //bits 5:0, Crystal Frequency adjust
  LT8900_writeRegister(28, 0x1800);

  //fedcba9876543210
  LT8900_writeRegister(32, 0x4800);  //AAABBCCCDDEEFFFG  A preamble length, B, syncword length, c trailer length, d packet type
  //                  E FEC_type, F BRCLK_SEL, G reserved
  //0x5000 = 0101 0000 0000 0000 = preamble 010 (3 bytes), B 10 (48 bits)

  LT8900_writeRegister(33, 0x3fc7);
  LT8900_writeRegister(34, 0x2000);  //
  LT8900_writeRegister(35, 0x0300);  //POWER mode,  bit 8/9 on = retransmit = 3x (default)

  //setSyncWord(0x03805a5a03800380);
    LT8900_writeRegister(36, 0x050A);
    LT8900_writeRegister(37, 0x0000);
    LT8900_writeRegister(38, 0x0000);
    LT8900_writeRegister(39, 0x55AA);

  LT8900_writeRegister(40, 0x4403);  //max allowed error bits = 0 (01 = 0 error bits)
  LT8900_writeRegister(41, 0xB000);
 // writeRegister(R_PACKETCONFIG,PACKETCONFIG_CRC_ON | PACKETCONFIG_PACK_LEN_ENABLE | PACKETCONFIG_FW_TERM_TX);

  LT8900_writeRegister(42, 0xfdb0);
  LT8900_writeRegister(43, 0x000f);
	#ifdef _USE_LT8920
	LT8900_writeRegister(44,0x0100);		//set datarate to 1M
	LT8900_writeRegister(45,0x0080);		//Option: 0x0152 or 0x0080 for 1M, 0x0552 for 250k,125k and 62.5k
	#endif
  LT8900_setDataRate(1);

  //15:8, 01: 1Mbps 04: 250Kbps 08: 125Kbps 10: 62.5Kbps

  LT8900_writeRegister(R_FIFO, 0x0000);  //TXRX_FIFO_REG (FIFO queue)

  LT8900_writeRegister(R_FIFO_CONTROL, 0x8080); //Fifo Rx/Tx queue reset

  _delay_ms(2);
  LT8900_writeRegister(R_CHANNEL, _BV(CHANNEL_TX_BIT));  //set TX mode.  (TX = bit 8, RX = bit 7, so RX would be 0x0080)
  _delay_ms(2);
  LT8900_writeRegister(R_CHANNEL, ActChannel);  // Frequency = 2402 + channel
}

void LT8900_startListening(void) {
  LT8900_writeRegister(R_CHANNEL, ActChannel & CHANNEL_MASK);   //turn off rx/tx
  _delay_ms(1);
  LT8900_writeRegister(R_FIFO_CONTROL, 0x8080);  //flush rx
  LT8900_writeRegister(R_CHANNEL,  (ActChannel & CHANNEL_MASK) | _BV(CHANNEL_RX_BIT));   //enable RX
  //_delay_ms(5);
}

bool LT8900_available(void) {
  //read the PKT_FLAG state; this can also be done with a hard wire.

  if (GPIO_ReadInputPin(LT8900_PKT_GPIO_PORT, (GPIO_Pin_TypeDef) LT8900_PKT_PIN) != 0)
  {
    return TRUE;
  }

  return FALSE;
}

bool LT8900_available_R(void)
{
  uint16_t status_val;
  status_val = LT8900_readRegister(R_STATUS);
  if (bitRead(status_val,STATUS_PKT_FLAG) == TRUE)
    {
        return TRUE;
    }
  else
  return FALSE;
}

void LT8900_softReset(void)
{
  uint16_t status;
  status = LT8900_readRegister(R_POWER_MODE);
  LT8900_writeRegister(R_POWER_MODE,status | POWER_DOWN_ENABLE);
  _delay_ms(20);
  LT8900_writeRegister(R_POWER_MODE,status & POWER_DOWN_DISABLE);
  _delay_ms(20);
}

int LT8900_read(uint8_t *buffer, uint8_t maxBuffer) {
  uint16_t status_val,data;
	uint8_t pos =0, packetSize;
  status_val = LT8900_readRegister(R_STATUS);
  if (bitRead(status_val, STATUS_CRC_BIT) == FALSE)
  {
    //CRC ok

    data = LT8900_readRegister(R_FIFO);
    packetSize = data >> 8;
    if(maxBuffer < packetSize+1)
    {
			LT8900_writeRegister(R_FIFO_CONTROL, 0x8080);
      //BUFFER TOO SMALL
        return -2;
    }


    buffer[pos++] = (data & 0xFF);
    while (pos < packetSize)
    {
      data = LT8900_readRegister(R_FIFO);
      buffer[pos++] = data >> 8;
      buffer[pos++] = data & 0xFF;
    }

    //LT8900_startListening();
    return packetSize;
  }
  else
  {
    //CRC error
    data = LT8900_readRegister(R_FIFO);
    LT8900_writeRegister(R_FIFO_CONTROL, 0x8080);
    return -1;
  }
}

bool LT8900_sendPacket(uint8_t *data, uint8_t packetSize)
{
	uint8_t pos = 0;
  if (packetSize < 1 || packetSize > 255)
  {
    return FALSE;
  }

  LT8900_writeRegister(R_CHANNEL, 0x0000);
  LT8900_writeRegister(R_FIFO_CONTROL, 0x8000);  //flush tx
  _delay_us(10);
  //packets are sent in 16bit words, and the first word will be the packet size.
  //start spitting out words until we are done.


  LT8900_writeRegister2(R_FIFO, packetSize, data[pos++]);
  while (pos < packetSize)
  {
    uint8_t msb = data[pos++];
    uint8_t lsb = data[pos++];

    LT8900_writeRegister2(R_FIFO, msb, lsb);
    
  }
	_delay_us(10);
  LT8900_writeRegister(R_CHANNEL,  (ActChannel & CHANNEL_MASK) | _BV(CHANNEL_TX_BIT));   //enable TX
  _delay_us(10);
  //Wait until the packet is sent.
  while (GPIO_ReadInputPin(LT8900_PKT_GPIO_PORT, (GPIO_Pin_TypeDef) LT8900_PKT_PIN) == 0)
  {
      //do nothing.
  }

  return TRUE;
}

void LT8900_setChannel(uint8_t channel){
  ActChannel = channel;
  LT8900_writeRegister(R_CHANNEL,  (channel & CHANNEL_MASK));
}

bool LT8900_setDataRate(uint8_t rate) {
  uint16_t newValue;

  switch (rate)
  {
    case 1:
      newValue = DATARATE_1MBPS;
      break;
    case 2:
      newValue = DATARATE_250KBPS;
      break;
    case 3:
      newValue = DATARATE_125KBPS;
      break;
    case 4:
      newValue = DATARATE_62KBPS;
      break;
    default:
      return FALSE;
  }

  LT8900_writeRegister(R_DATARATE, newValue);

  //verify
  if (LT8900_readRegister(R_DATARATE) == newValue) return TRUE;
    else return FALSE;
}

uint16_t LT8900_readRegister(uint8_t reg) {
  uint8_t high,low;
  //Chip select
  GPIO_WriteLow(LT8900_NCS_GPIO_PORT, (GPIO_Pin_TypeDef)LT8900_NCS_PIN);

  //Send address and read command
  //while (SPI_GetFlagStatus(SPI_FLAG_TXE)== RESET);
  while(!(SPI->SR & (uint8_t)SPI_FLAG_TXE));
  SPI_SendData((REGISTER_READ | (REGISTER_MASK & reg)));
  while (SPI->SR & (uint8_t)SPI_FLAG_BSY);
  while(!(SPI->SR & (uint8_t)SPI_FLAG_RXNE));
  //while (SPI_GetFlagStatus(SPI_FLAG_BSY)== SET);
  //while (SPI_GetFlagStatus(SPI_FLAG_RXNE)== RESET);
  //_delay_us(1);
  SPI_ReceiveData();

  //Get data
  //while (SPI_GetFlagStatus(SPI_FLAG_TXE)== RESET);
  while(!(SPI->SR & (uint8_t)SPI_FLAG_TXE));
  SPI_SendData(0x00);
  while (SPI->SR & (uint8_t)SPI_FLAG_BSY);
  while(!(SPI->SR & (uint8_t)SPI_FLAG_RXNE));
  //while (SPI_GetFlagStatus(SPI_FLAG_BSY)== SET);
  //while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET);
  high = SPI_ReceiveData();

  //while (SPI_GetFlagStatus(SPI_FLAG_TXE)== RESET);
  while(!(SPI->SR & (uint8_t)SPI_FLAG_TXE));
  SPI_SendData(0x00);
  while (SPI->SR & (uint8_t)SPI_FLAG_BSY);
  while(!(SPI->SR & (uint8_t)SPI_FLAG_RXNE));
  //while (SPI_GetFlagStatus(SPI_FLAG_BSY)== SET);
  //while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET);
  low = SPI_ReceiveData();

  //Chip select
  GPIO_WriteHigh(LT8900_NCS_GPIO_PORT, (GPIO_Pin_TypeDef)LT8900_NCS_PIN);

  return (high * 256 + low);
}

void LT8900_writeRegister(uint8_t reg, uint16_t data) {
  uint8_t high = data / 256;
  uint8_t low = data % 256;

  LT8900_writeRegister2(reg, high, low);
}

void LT8900_writeRegister2(uint8_t reg, uint8_t high, uint8_t low) {
  //Chip select
  GPIO_WriteLow(LT8900_NCS_GPIO_PORT, (GPIO_Pin_TypeDef)LT8900_NCS_PIN);

  //Send address and write command
  while (SPI_GetFlagStatus(SPI_FLAG_TXE)== RESET);
  SPI_SendData(REGISTER_WRITE | (REGISTER_MASK & reg));
  while (SPI_GetFlagStatus(SPI_FLAG_BSY)== SET);
  while (SPI_GetFlagStatus(SPI_FLAG_RXNE)== RESET);
  SPI_ReceiveData();

  //Send data

    while (SPI_GetFlagStatus(SPI_FLAG_TXE)== RESET);
    SPI_SendData(high);
    while (SPI_GetFlagStatus(SPI_FLAG_BSY)== SET);
    while (SPI_GetFlagStatus(SPI_FLAG_RXNE)== RESET);
    SPI_ReceiveData();

    while (SPI_GetFlagStatus(SPI_FLAG_TXE)== RESET);
    SPI_SendData(low);
    while (SPI_GetFlagStatus(SPI_FLAG_BSY)== SET);
    while (SPI_GetFlagStatus(SPI_FLAG_RXNE)== RESET);
    SPI_ReceiveData();

  //Chip select
  GPIO_WriteHigh(LT8900_NCS_GPIO_PORT, (GPIO_Pin_TypeDef)LT8900_NCS_PIN);
}
