/********************************************************************************/
/*!
	@file			display_if_basis.c
	@author         Nemui Trinomius (http://nemuisan.blog.bai.ne.jp)
    @version        5.00
    @date           2011.11.05
	@brief          Interface of Display Device Basics for STM32 uC.			@n
					"MCU Depend Layer"

    @section HISTORY
		2010.07.02	V1.00	Restart Here.
		2010.10.01	V2.00	Changed CTRL-Port Contol Procedure.
		2010.12.31	V3.00	Changed Some.
		2011.03.10	V4.00	C++ Ready.
		2011.11.05	V5.00	Improved Portability.
		
    @section LICENSE
		BSD License. See Copyright.txt
*/
/********************************************************************************/

/* Includes ------------------------------------------------------------------*/
//#include "stm32f10x_conf.h" /* AAAAA */
#include "stm32f.h"
#include "display_if_basis.h"


/* Defines -------------------------------------------------------------------*/
#define SPI_BaudPrescale SPI_BaudRatePrescaler_2

/* Variables -----------------------------------------------------------------*/
#if defined(USE_HARDWARE_SPI)
SPI_InitTypeDef SPI_InitStructure;
SPI_TypeDef* LCD_SPI;
#endif

#ifdef USE_DISPLAY_DMA_TRANSFER
static DMA_InitTypeDef  LCD_DMA_InitStructure;
#endif

/* Constants -----------------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/
#if !defined(USE_SOFTWARE_SPI) && !defined(USE_HARDWARE_SPI)
/**************************************************************************/
/*! 
    Display Driver Lowest Layer Settings.
*/
/**************************************************************************/
static void FSMC_Conf(void)
{
#if 1
	/* b12:WREN b5.4:MMD(16bits) b0:MDKEN */
	sil_wrw_mem((uint32_t *)FSMC_BCR1, (uint32_t)0x00001011);
	/* b15..8:DATA setup time   b7..4:ADDR hold time   b3..0:ADDR setup time */
	sil_wrw_mem((uint32_t *)FSMC_BTR1, (uint32_t)0x00000e1e);

	/*
	 * Write Timing Register 設定不要
	 * Extendmodeの時しか有効にならないので
	 */
#else
	FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef p,r;
	/*-- FSMC Configuration ------------------------------------------------------*/
	/* FSMC_Bank1_NORSRAMx configuration */
	/* Read Condition */
	r.FSMC_AddressSetupTime = 30;
	r.FSMC_AddressHoldTime  = 0;
	r.FSMC_DataSetupTime    = 30;
	r.FSMC_BusTurnAroundDuration = 0x00;
	r.FSMC_CLKDivision = 0x00;
	r.FSMC_DataLatency = 0x00;
	r.FSMC_AccessMode  = FSMC_AccessMode_A;

	/* Write Condition */
	p.FSMC_AddressSetupTime = 6;
	p.FSMC_AddressHoldTime  = 1;
	p.FSMC_DataSetupTime    = 9;
	p.FSMC_BusTurnAroundDuration = 0x00;
	p.FSMC_CLKDivision = 0x00;
	p.FSMC_DataLatency = 0x00;
	p.FSMC_AccessMode = FSMC_AccessMode_A;


	/* Color LCD configuration ------------------------------------ */
	FSMC_NORSRAMInitStructure.FSMC_Bank               = FSMC_Bank1_NORSRAM1;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux     = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType         = FSMC_MemoryType_SRAM;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth    = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode    = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode           = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive   = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation     = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal         = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode       = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst         = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &r;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

	/* Enable FSMC Bank1_NOR Bank */
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
#endif
}

/**************************************************************************/
/*! 
    Display Driver Lowest Layer Settings.
*/
/**************************************************************************/
static void GPIO_Conf(void)
{
#ifdef USE_TOPPERS
	/* PORT-D */
	set_cr_mode(GPIOD_BASE, 15, MODE_OUTPUT_50MHZ); set_cr_cnf (GPIOD_BASE, 15, CNF_OUT_AF_PP);
	set_cr_mode(GPIOD_BASE, 14, MODE_OUTPUT_50MHZ); set_cr_cnf (GPIOD_BASE, 14, CNF_OUT_AF_PP);
	set_cr_mode(GPIOD_BASE, 13, MODE_INPUT);        set_cr_cnf (GPIOD_BASE, 13, CNF_IN_FLOATING);
	set_cr_mode(GPIOD_BASE, 12, MODE_INPUT);        set_cr_cnf (GPIOD_BASE, 12, CNF_IN_FLOATING);
	set_cr_mode(GPIOD_BASE, 11, MODE_OUTPUT_50MHZ); set_cr_cnf (GPIOD_BASE, 11, CNF_OUT_AF_PP);
	set_cr_mode(GPIOD_BASE, 10, MODE_OUTPUT_50MHZ); set_cr_cnf (GPIOD_BASE, 10, CNF_OUT_AF_PP);
	set_cr_mode(GPIOD_BASE,  9, MODE_OUTPUT_50MHZ); set_cr_cnf (GPIOD_BASE,  9, CNF_OUT_AF_PP);
	set_cr_mode(GPIOD_BASE,  8, MODE_OUTPUT_50MHZ); set_cr_cnf (GPIOD_BASE,  8, CNF_OUT_AF_PP);

	set_cr_mode(GPIOD_BASE,  7, MODE_OUTPUT_50MHZ); set_cr_cnf (GPIOD_BASE,  7, CNF_OUT_AF_PP);
	set_cr_mode(GPIOD_BASE,  6, MODE_INPUT);        set_cr_cnf (GPIOD_BASE,  6, CNF_IN_FLOATING);
	set_cr_mode(GPIOD_BASE,  5, MODE_OUTPUT_50MHZ); set_cr_cnf (GPIOD_BASE,  5, CNF_OUT_AF_PP);
	set_cr_mode(GPIOD_BASE,  4, MODE_OUTPUT_50MHZ); set_cr_cnf (GPIOD_BASE,  4, CNF_OUT_AF_PP);
	set_cr_mode(GPIOD_BASE,  3, MODE_INPUT);        set_cr_cnf (GPIOD_BASE,  3, CNF_IN_FLOATING);
	set_cr_mode(GPIOD_BASE,  2, MODE_OUTPUT_50MHZ); set_cr_cnf (GPIOD_BASE,  2, CNF_OUT_GP_PP);
	set_cr_mode(GPIOD_BASE,  1, MODE_OUTPUT_50MHZ); set_cr_cnf (GPIOD_BASE,  1, CNF_OUT_AF_PP);
	set_cr_mode(GPIOD_BASE,  0, MODE_OUTPUT_50MHZ); set_cr_cnf (GPIOD_BASE,  0, CNF_OUT_AF_PP);

	/* PORT-E */
	set_cr_mode(GPIOE_BASE, 15, MODE_OUTPUT_50MHZ); set_cr_cnf (GPIOE_BASE, 15, CNF_OUT_AF_PP);
	set_cr_mode(GPIOE_BASE, 14, MODE_OUTPUT_50MHZ); set_cr_cnf (GPIOE_BASE, 14, CNF_OUT_AF_PP);
	set_cr_mode(GPIOE_BASE, 13, MODE_OUTPUT_50MHZ); set_cr_cnf (GPIOE_BASE, 13, CNF_OUT_AF_PP);
	set_cr_mode(GPIOE_BASE, 12, MODE_OUTPUT_50MHZ); set_cr_cnf (GPIOE_BASE, 12, CNF_OUT_AF_PP);
	set_cr_mode(GPIOE_BASE, 11, MODE_OUTPUT_50MHZ); set_cr_cnf (GPIOE_BASE, 11, CNF_OUT_AF_PP);
	set_cr_mode(GPIOE_BASE, 10, MODE_OUTPUT_50MHZ); set_cr_cnf (GPIOE_BASE, 10, CNF_OUT_AF_PP);
	set_cr_mode(GPIOE_BASE,  9, MODE_OUTPUT_50MHZ); set_cr_cnf (GPIOE_BASE,  9, CNF_OUT_AF_PP);
	set_cr_mode(GPIOE_BASE,  8, MODE_OUTPUT_50MHZ); set_cr_cnf (GPIOE_BASE,  8, CNF_OUT_AF_PP);

	set_cr_mode(GPIOE_BASE,  7, MODE_OUTPUT_50MHZ); set_cr_cnf (GPIOE_BASE,  7, CNF_OUT_AF_PP);
	set_cr_mode(GPIOE_BASE,  6, MODE_INPUT);        set_cr_cnf (GPIOE_BASE,  6, CNF_IN_FLOATING);
	set_cr_mode(GPIOE_BASE,  5, MODE_INPUT);        set_cr_cnf (GPIOE_BASE,  5, CNF_IN_FLOATING);
	set_cr_mode(GPIOE_BASE,  4, MODE_INPUT);        set_cr_cnf (GPIOE_BASE,  4, CNF_IN_FLOATING);
	set_cr_mode(GPIOE_BASE,  3, MODE_INPUT);        set_cr_cnf (GPIOE_BASE,  3, CNF_IN_FLOATING);
	set_cr_mode(GPIOE_BASE,  2, MODE_INPUT);        set_cr_cnf (GPIOE_BASE,  2, CNF_IN_FLOATING);
	set_cr_mode(GPIOE_BASE,  1, MODE_INPUT);        set_cr_cnf (GPIOE_BASE,  1, CNF_IN_FLOATING);
	set_cr_mode(GPIOE_BASE,  0, MODE_INPUT);        set_cr_cnf (GPIOE_BASE,  0, CNF_IN_FLOATING);

#else
	GPIO_InitTypeDef GPIO_InitStructure;
	/* Enable FSMC clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
	/* Enable FSMC Data/Address GPIO clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);
	/*-- GPIO Configuration ------------------------------------------------------*/
	/* SRAM Data lines,  NOE and NWE configuration */
	/*
	 * PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9),
	 * PE.13(D10), PE.14(D11), PE.15(D12)
	 */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                                 GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
                                 GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

  /* PD.00(D2), PD.01(D3), PD.04(RD), PD.5(WR), PD.7(CS), PD.8(D13), PD.9(D14),
     PD.10(D15), PD.11(RS) PD.14(D0) PD.15(D1) */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 |
                                 GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
                                 GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* SRAM Address lines (D/C) configuration */
	/* CS configuration */
	/* RESET (GPIO) */
#endif
}



/**************************************************************************/
/*! 
    Display Driver Lowest Layer Settings.
*/
/**************************************************************************/
void Display_IoInit_If()
{
	GPIO_Conf();
	FSMC_Conf();
}



#ifdef USE_DISPLAY_DMA_TRANSFER
/**************************************************************************/
/*! 
    Display Driver Lowest Layer Settings.
*/
/**************************************************************************/
void lcd_dma_transfer(const uint8_t *buff,unsigned int btr)
{

	/* Check if the DMA Stream is disabled before enabling it. */
	while (DMA_GetCmdStatus(FSMCLCD_DMA_STREAM_TX) != DISABLE){ ; }
 
	/* Configure DMA Stream */
	LCD_DMA_InitStructure.DMA_PeripheralBaseAddr 	= (uint32_t)buff;					/* M2M Source Address 		*/
	LCD_DMA_InitStructure.DMA_BufferSize 			= btr;
	DMA_Init(FSMCLCD_DMA_STREAM_TX, &LCD_DMA_InitStructure);
 
	/* Clear DMA_FSMC TransferComplete Flag */
 	DMA_ClearFlag(FSMCLCD_DMA_STREAM_TX,FSMCLCD_DMA_FLAG_TX_TC);

	/* Enable FSMC_DMA */
	DMA_Cmd(FSMCLCD_DMA_STREAM_TX, ENABLE);

	/* Wait until FSMC_DMA_TX Complete */
	while (DMA_GetFlagStatus(FSMCLCD_DMA_STREAM_TX,FSMCLCD_DMA_FLAG_TX_TC) == RESET) { ; }

	/* Disable FSMC_DMA */
	DMA_Cmd(FSMCLCD_DMA_STREAM_TX, DISABLE);

}
#endif

#else /* Serial LCD Control */
/**************************************************************************/
/*! 
    Display Driver Lowest Layer Settings.
*/
/**************************************************************************/
void Display_IoInit_If()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable CTRL Line GPIO Settings */
	RCC_AHB1PeriphClockCmd(DISPLAY_CLK_RES, ENABLE);
	RCC_AHB1PeriphClockCmd(DISPLAY_CLK_CS, ENABLE);
	RCC_AHB1PeriphClockCmd(DISPLAY_CLK_DC, ENABLE);

	GPIO_InitStructure.GPIO_Pin 	= CTRL_RES;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;
	GPIO_Init(DISPLAY_PORT_RES, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin 	= CTRL_CS;
	GPIO_Init(DISPLAY_PORT_CS, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin 	= CTRL_DC;
	GPIO_Init(DISPLAY_PORT_DC, &GPIO_InitStructure);
	
#if defined(USE_SOFTWARE_SPI)
	RCC_AHB1PeriphClockCmd(DISPLAY_CLK_SCK, ENABLE);
	GPIO_InitStructure.GPIO_Pin 	= CTRL_SCK;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_Init(DISPLAY_PORT_SCK, &GPIO_InitStructure);

	RCC_AHB1PeriphClockCmd(DISPLAY_CLK_SDI, ENABLE);
	GPIO_InitStructure.GPIO_Pin		= CTRL_SDI;
	GPIO_Init(DISPLAY_PORT_SDI, &GPIO_InitStructure);

	RCC_AHB1PeriphClockCmd(DISPLAY_CLK_SDO, ENABLE);
	GPIO_InitStructure.GPIO_Pin 	= CTRL_SDO;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;
	GPIO_Init(DISPLAY_PORT_SDO, &GPIO_InitStructure);

    /* I/O Initial State */
	DISPLAY_SCK_SET();
	DISPLAY_SDI_SET();
	DISPLAY_CS_SET();

#else
	DISPLAY_PERIF_CLK(ENABLE);

	/* Enable Stability of GPIO over 50MHz! */
	SYSCFG_CompensationCellCmd(ENABLE);

	/* Connect SPI pins to Aletenate Function */  
	GPIO_PinAFConfig(DISPLAY_PORT_SCK,SRC_SCK,GPIO_AF_SPIX);
	GPIO_PinAFConfig(DISPLAY_PORT_SDO,SRC_SDO,GPIO_AF_SPIX);
	GPIO_PinAFConfig(DISPLAY_PORT_SDI,SRC_SDI,GPIO_AF_SPIX);

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

	/* SPI SCK pin configuration */
	GPIO_InitStructure.GPIO_Pin = CTRL_SCK;
	GPIO_Init(DISPLAY_PORT_SCK, &GPIO_InitStructure);

	/* SPI MOSI pin configuration */
	GPIO_InitStructure.GPIO_Pin =  CTRL_SDI;
	GPIO_Init(DISPLAY_PORT_SDI, &GPIO_InitStructure);

	/* SPI MISO pin configuration */
	GPIO_InitStructure.GPIO_Pin =  CTRL_SDO;
	GPIO_Init(DISPLAY_PORT_SDO, &GPIO_InitStructure);

	/* SPI_MMC configuration */
	SPI_InitStructure.SPI_Direction	 		= SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode		 		= SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize	 		= SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL		 		= SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA		 		= SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS				= SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudPrescale;
	SPI_InitStructure.SPI_FirstBit	 		= SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial 	= 7;
	SPI_Init(DISPLAY_SPI_PORT, &SPI_InitStructure);

	SPI_CalculateCRC(DISPLAY_SPI_PORT, DISABLE);

	/* Enable SPIx  */
	SPI_Cmd(DISPLAY_SPI_PORT, ENABLE);

	/* Drain SPI */
	while (SPI_I2S_GetFlagStatus(DISPLAY_SPI_PORT, SPI_I2S_FLAG_TXE) == RESET) { ; }
	SPI_I2S_ReceiveData(DISPLAY_SPI_PORT);
	
	LCD_SPI = DISPLAY_SPI_PORT;
	
#ifdef USE_DISPLAY_DMA_TRANSFER
	/* Configure the DMA Stream */
	RCC_AHB1PeriphClockCmd(SPILCD_DMA_STREAM_CLOCK, ENABLE); 
    DMA_Cmd(SPILCD_DMA_STREAM_TX, DISABLE);
    DMA_DeInit(SPILCD_DMA_STREAM_TX);
	
	LCD_DMA_InitStructure.DMA_Channel 				= SPILCD_DMA_CHANNEL_TX;  
	LCD_DMA_InitStructure.DMA_PeripheralBaseAddr 	= (uint32_t)(&(LCD_SPI->DR));
	LCD_DMA_InitStructure.DMA_DIR		 	 		= DMA_DIR_MemoryToPeripheral;
	LCD_DMA_InitStructure.DMA_PeripheralInc 		= DMA_PeripheralInc_Disable;
	LCD_DMA_InitStructure.DMA_MemoryInc		 		= DMA_MemoryInc_Enable;
	LCD_DMA_InitStructure.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Byte;
	LCD_DMA_InitStructure.DMA_MemoryDataSize 		= DMA_MemoryDataSize_Byte;
	LCD_DMA_InitStructure.DMA_Mode 					= DMA_Mode_Normal;
	LCD_DMA_InitStructure.DMA_Priority 				= DMA_Priority_Medium;
	LCD_DMA_InitStructure.DMA_FIFOMode 				= DMA_FIFOMode_Disable;
	LCD_DMA_InitStructure.DMA_FIFOThreshold 		= DMA_FIFOThreshold_Full;
	LCD_DMA_InitStructure.DMA_MemoryBurst 			= DMA_MemoryBurst_Single;
	LCD_DMA_InitStructure.DMA_PeripheralBurst 		= DMA_PeripheralBurst_Single;
#endif

#endif


}

#if  defined(USE_HARDWARE_SPI)
/**************************************************************************/
/*! 
    SPI I/O Settings(if needed).
*/
/**************************************************************************/
inline void SendSPI(uint8_t dat)
{
	/* Loop while DR register in not empty */
	/* while (SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_TXE) == RESET) { ; } */

	/* Send byte through the SPI peripheral */
	/*SPI_I2S_SendData(LCD_SPI, dat);*/
	LCD_SPI->DR = dat;					

	/* Wait to receive a byte */
	/*while (SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_RXNE) == RESET) { ; }*/
	while (!(LCD_SPI->SR & SPI_I2S_FLAG_RXNE));

	/* Return the byte read from the SPI bus */
	/*SPI_I2S_ReceiveData(LCD_SPI);*/
	LCD_SPI->DR;
}

inline void SendSPI16(uint16_t dat)
{
	SendSPI((uint8_t)(dat>>8));

	SendSPI((uint8_t)dat);
}

inline uint8_t RecvSPI(void){

	uint8_t temper;
	
	/* Down Clock to Read SPI*/
	SPI_InitStructure.SPI_Direction	 		= SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode		 		= SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize	 		= SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL		 		= SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA		 		= SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS				= SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit	 		= SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial 	= 7;
	SPI_Init(DISPLAY_SPI_PORT, &SPI_InitStructure);

	/* Loop while DR register in not empty */
	/* while (SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_TXE) == RESET) { ; } */

	/* Send byte through the SPI_MMC peripheral */
	SPI_I2S_SendData(LCD_SPI, 0xFF);

	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_RXNE) == RESET) { ; }
	
	/* Return the byte read from the SPI bus */
	temper = SPI_I2S_ReceiveData(LCD_SPI);

	/* UP Clock to Write SPI*/
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudPrescale;
	SPI_Init(DISPLAY_SPI_PORT, &SPI_InitStructure);
	
	return temper;

}

/**************************************************************************/
/*! 
    @brief  Transmit/Receive Block using DMA.
	@param  -
    @retval : -
*/
/**************************************************************************/
void lcd_dma_transfer(const uint8_t *buff,unsigned int btr)
{
	/* Wait DMA Transfer result */
	while (DMA_GetCmdStatus(SPILCD_DMA_STREAM_TX) != DISABLE) { ; }

	/* shared DMA configuration values */
	LCD_DMA_InitStructure.DMA_Memory0BaseAddr 		= (uint32_t)buff;
	LCD_DMA_InitStructure.DMA_BufferSize		 	= btr;
	DMA_Init(SPILCD_DMA_STREAM_TX, &LCD_DMA_InitStructure);

	/* Enable SPI_DMA_TX */
	DMA_Cmd(SPILCD_DMA_STREAM_TX, ENABLE);
	
	/* Enable SPI_DMA TX request */
	SPI_I2S_DMACmd(LCD_SPI, SPI_I2S_DMAReq_Tx, ENABLE);

	/* Wait until SPI_DMA_TX Complete */
	while (DMA_GetFlagStatus(SPILCD_DMA_STREAM_TX,SPILCD_DMA_FLAG_TX_TC) == RESET) { ; }

	/* DISABLE SPI_DMA_TX */
	DMA_Cmd(SPILCD_DMA_STREAM_TX, DISABLE);
  	SPI_I2S_DMACmd(LCD_SPI, SPI_I2S_DMAReq_Tx, DISABLE);

	/* Clear DMA_FSMC TransferComplete Flag */
 	DMA_ClearFlag(SPILCD_DMA_STREAM_TX,SPILCD_DMA_FLAG_TX_TC);

	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_RXNE) == RESET) { ; }
}

#endif

#endif


/* End Of File ---------------------------------------------------------------*/
