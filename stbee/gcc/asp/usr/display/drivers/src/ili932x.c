/********************************************************************************/
/*!
	@file			ili932x.c
	@author         Nemui Trinomius (http://nemuisan.blog.bai.ne.jp)
    @version        7.00
    @date           2012.04.20
	@brief          Based on Chan's MCI_OLED@LPC23xx-demo thanks!				@n
					Available TFT-LCM are listed below.							@n
					 -YHY024006A				(ILI9325)	8/16bit mode.		@n
					 -EGO028Q02-F05				(ILI9325)	8/16bit mode.		@n
					 -WBX280V009				(ILI9325)	8/16bit mode.		@n
					 -V320P243282WSI7TB   		(ILI9320)	16bit mode.			@n
					 -PH240320T-030-XP1Q  		(R61505U)	8/16bit mode.		@n
					 -TFT-GG1N4198UTSW-W-TP-E  	(LGDP4531)	8/16bit,spi mode.	@n
					 -MR028-9325-51P-TP-A  		(ILI9328)	8/16bit,spi mode.	@n
					 -MR024-9325-51P-TP-B  		(ILI9325C)	8/16bit,spi mode.	@n
					 -OPFPCT0634-V0				(ILI9320)	8/16bit mode.		@n
					 -TS8026Y					(ST7781)	8bit mode.			@n
					 -TFT1P3204-E				(R61505W)	8/16bit mode.		@n
					 -KD032C-2-TP 				(ILI9325C)	8/16bit,spi mode.   @n
					 -TFT1P2477-E				(R61505V)	8bit mode.

    @section HISTORY
		2010.03.01	V1.00	Stable Release.
		2010.10.01	V2.00	Changed CTRL-Port Contol Procedure.
		2010.12.31	V3.00	Added ILI9328 device.
							Added 3-Wire Serial Support.
		2011.03.10	V4.00	C++ Ready.
		2011.10.25	V5.00	Added OPFPCT0634-V0 Consideration.
							Added DMA TransactionSupport.
		2012.01.18	V6.00	Added ST7781 device.
		2012.04.20	V7.00	Added R61505W & R61505V device.

    @section LICENSE
		BSD License. See Copyright.txt
*/
/********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "ili932x.h"
/* check header file version for fool proof */
#if __ILI932X_H != 0x0700
#error "header file version is not correspond!"
#endif

/* Defines -------------------------------------------------------------------*/
/* If U use OPFPCT0634-V0 Module,Uncomment below define */
/*#define MODULE_OPFPCT0634*/

/* Variables -----------------------------------------------------------------*/

/* Constants -----------------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/

/**************************************************************************/
/*! 
    Abstract Layer Delay Settings.
*/
/**************************************************************************/
#ifndef __SYSTICK_H
volatile uint32_t ticktime;
static inline void _delay_ms(uint32_t ms)
{
	ms += ticktime;
	while (ticktime < ms);
}
#endif

/**************************************************************************/
/*! 
    Display Module Reset Routine.
*/
/**************************************************************************/
inline void ILI932x_reset(void)
{
#ifdef USE_ILI932x_TFT
	ILI932x_RES_SET();							/* RES=H, RD=H, WR=H   		*/
	ILI932x_RD_SET();
	ILI932x_WR_SET();
	_delay_ms(10);								/* wait 10ms     			*/

	ILI932x_RES_CLR();							/* RES=L, CS=L   			*/
	ILI932x_CS_CLR();

#elif  USE_ILI932x_SPI_TFT
	ILI932x_RES_SET();							/* RES=H, CS=H				*/
	ILI932x_CS_SET();
	ILI932x_SCK_SET();							/* SPI MODE3     			*/
	_delay_ms(10);								/* wait 10ms     			*/

	ILI932x_RES_CLR();							/* RES=L		   			*/

#endif

	_delay_ms(50);								/* wait 50ms     			*/
	ILI932x_RES_SET();						  	/* RES=H					*/
	_delay_ms(50);				    			/* wait 50ms     			*/
}

/* Select SPI or Parallel in MAKEFILE */
#ifdef USE_ILI932x_TFT
/**************************************************************************/
/*! 
    Write LCD Command.
*/
/**************************************************************************/
inline void ILI932x_wr_cmd(uint16_t cmd)
{
	ILI932x_DC_CLR();							/* DC=L						*/

#if defined(GPIO_ACCESS_8BIT) | defined(BUS_ACCESS_8BIT)
	ILI932x_CMD = 0;
	ILI932x_WR();
#endif

	ILI932x_CMD = cmd;							/* cmd(8bit_Low or 16bit)	*/
	ILI932x_WR();								/* WR=L->H					*/

	ILI932x_DC_SET();							/* DC=H						*/
}

/**************************************************************************/
/*! 
    Write LCD Data and GRAM.
*/
/**************************************************************************/
inline void ILI932x_wr_dat(uint16_t dat)
{
#if defined(GPIO_ACCESS_8BIT) | defined(BUS_ACCESS_8BIT)
	ILI932x_DATA = (uint8_t)(dat>>8);			/* upper 8bit data			*/
	ILI932x_WR();								/* WR=L->H					*/
#endif

	ILI932x_DATA = dat;							/* data(8bit_Low or 16bit)	*/
	ILI932x_WR();								/* WR=L->H					*/
}

/**************************************************************************/
/*! 
    Write LCD Block Data.
*/
/**************************************************************************/
inline void ILI932x_wr_block(uint8_t *p, unsigned int cnt)
{

#ifdef  USE_DISPLAY_DMA_TRANSFER
   DMA_TRANSACTION(p, cnt);
#else

	cnt /= 4;
	
	while (cnt--) {
		/* avoid -Wsequence-point's warning */
		ILI932x_wr_dat(*(p+1)|*(p)<<8);
		p++;p++;
		ILI932x_wr_dat(*(p+1)|*(p)<<8);
		p++;p++;
	}
#endif

}

/**************************************************************************/
/*! 
    Read LCD Register.
*/
/**************************************************************************/
inline uint16_t ILI932x_rd_cmd(uint16_t cmd)
{
	uint16_t val;

#if defined(GPIO_ACCESS_8BIT) | defined(BUS_ACCESS_8BIT)
	uint16_t temp;
#endif

	ILI932x_wr_cmd(cmd);
	ILI932x_WR_SET();

#if defined(GPIO_ACCESS_8BIT) | defined(BUS_ACCESS_8BIT)
    ReadLCDData(temp);
#endif

    ReadLCDData(val);

#if defined(GPIO_ACCESS_8BIT) | defined(BUS_ACCESS_8BIT)
	val &= 0x00FF;
	val |= temp<<8;
#endif

	return val;
}


#elif USE_ILI932x_SPI_TFT
/**************************************************************************/
/*! 
    Write LCD Command.
*/
/**************************************************************************/
inline void ILI932x_wr_cmd(uint16_t cmd)
{
	DISPLAY_ASSART_CS();						/* CS=L		     */
	
	SendSPI(START_WR_CMD);
	SendSPI16(cmd);

	DISPLAY_NEGATE_CS();						/* CS=H		     */
}

/**************************************************************************/
/*! 
    Write LCD Data.
*/
/**************************************************************************/
inline void ILI932x_wr_dat(uint16_t dat)
{	
	DISPLAY_ASSART_CS();						/* CS=L		     */
	
	SendSPI(START_WR_DATA);
	SendSPI16(dat);

	DISPLAY_NEGATE_CS();						/* CS=H		     */
}

/**************************************************************************/
/*! 
    Write LCD Block Data.
*/
/**************************************************************************/
inline void ILI932x_wr_block(uint8_t *p,unsigned int cnt)
{

	DISPLAY_ASSART_CS();						/* CS=L		     */
	SendSPI(START_WR_DATA);

#ifdef  USE_DISPLAY_DMA_TRANSFER
   DMA_TRANSACTION(p, cnt );
#else

	cnt /= 4;

	while (cnt--) {
		/* avoid -Wsequence-point's warning */
		SendSPI16(((*(p+1))|(*(p)<<8)));
		p++;p++;
		SendSPI16(((*(p+1))|(*(p)<<8)));
		p++;p++;
	}
#endif

	DISPLAY_NEGATE_CS();						/* CS=H		     */
}

/**************************************************************************/
/*! 
    Read LCD Register.
*/
/**************************************************************************/
inline  uint16_t ILI932x_rd_cmd(uint16_t cmd)
{
	uint16_t val;
	uint8_t temp;

	DISPLAY_ASSART_CS();						/* CS=L		     */
	
	SendSPI(START_WR_CMD);
	SendSPI16(cmd);
	
	DISPLAY_NEGATE_CS();						/* CS=H		     */
	

	DISPLAY_ASSART_CS();						/* CS=L		     */
	
	SendSPI(START_RD_DATA);
	temp = RecvSPI();							/* Dummy Read */
	temp = RecvSPI();							/* Upper Read */
	val  = RecvSPI();							/* Lower Read */

	val &= 0x00FF;
	val |= (uint16_t)temp<<8;
	
	DISPLAY_NEGATE_CS();						/* CS=H		     */

	return val;
}
#endif


/**************************************************************************/
/*! 
    Set Rectangle.
*/
/**************************************************************************/
inline void ILI932x_rect(uint32_t x, uint32_t width, uint32_t y, uint32_t height)
{

	ILI932x_wr_cmd(0x0050);				/* Horizontal RAM Start ADDR */
	ILI932x_wr_dat(OFS_COL + x);
	ILI932x_wr_cmd(0x0051);				/* Horizontal RAM End ADDR */
	ILI932x_wr_dat(OFS_COL + width);
	ILI932x_wr_cmd(0x0052);				/* Vertical RAM Start ADDR */
	ILI932x_wr_dat(OFS_RAW + y);
	ILI932x_wr_cmd(0x0053);				/* Vertical End ADDR */
	ILI932x_wr_dat(OFS_RAW + height);

	ILI932x_wr_cmd(0x0020);				/* GRAM Vertical/Horizontal ADDR Set(AD0~AD7) */
	ILI932x_wr_dat(OFS_RAW + y);
	ILI932x_wr_cmd(0x0021);				/* GRAM Vertical/Horizontal ADDR Set(AD8~AD16) */
	ILI932x_wr_dat(OFS_COL + x);

	ILI932x_wr_cmd(0x0022);			/* Write Data to GRAM */

}

/**************************************************************************/
/*! 
    Clear Display.
*/
/**************************************************************************/
inline void ILI932x_clear(void)
{
	volatile uint32_t n;

	ILI932x_rect(0,MAX_X-1,0,MAX_Y-1);
	n = (uint32_t)(MAX_X) * (MAX_Y);

	do {
		ILI932x_wr_dat(COL_BLACK);
	} while (--n);

}


/**************************************************************************/
/*! 
    TFT-LCD Module Initialize.
*/
/**************************************************************************/
void ILI932x_init(void)
{
	uint16_t devicetype;
	
	Display_IoInit_If();

	ILI932x_reset();

	/* Check Device Code */
	devicetype = ILI932x_rd_cmd(0x0000);  			/* Confirm Vaild LCD Controller */

	if(devicetype == 0x9325)
	{
		/* Initialize ILI9325 & ILI9325C */
		ILI932x_wr_cmd(0x00E3);						/* Set the internal vcore voltage */ 
		ILI932x_wr_dat(0x3008);
		ILI932x_wr_cmd(0x00E7);						/* Set the internal vcore voltage */ 
		ILI932x_wr_dat(0x0012);
		ILI932x_wr_cmd(0x00EF);						/* Set the internal vcore voltage */ 
		ILI932x_wr_dat(0x1231);

		ILI932x_wr_cmd(0x0001);						/* Set SS and SM bit */ 
		ILI932x_wr_dat(0x0100);

		ILI932x_wr_cmd(0x0002);						/* Set 1 line inversion */ 
		ILI932x_wr_dat(0x0700);

		ILI932x_wr_cmd(0x0003);						/* Set GRAM write direction and BGR=1 */ 
		ILI932x_wr_dat((1<<12)|(1<<9)|(1<<7)|(1<<5)|(1<<4));
		/* ILI932x_wr_dat(0x1030); */				/* original */

		ILI932x_wr_cmd(0x0004);						/* Scalling Control */ 
		ILI932x_wr_dat(0x0000);
		
		ILI932x_wr_cmd(0x0008);						/* Display Control 2(0x0207) */ 
		ILI932x_wr_dat(0x0207);						/* Set the back porch and front porch */

		ILI932x_wr_cmd(0x0009);						/* Display Control 3(0x0000) */ 
		ILI932x_wr_dat(0x0000);						/* Set non-display area refresh cycle ISC[3:0] */

		ILI932x_wr_cmd(0x000a);						/* Frame Cycle Control 3(0x0000) */ 
		ILI932x_wr_dat(0x0000);						/* FMARK function */

		ILI932x_wr_cmd(0x000c);						/* Extern Display Interface Control 1(0x0000) */ 
		ILI932x_wr_dat(0x0000);						/* RGB interface setting */

		ILI932x_wr_cmd(0x000d);						/* Frame Maker Position */ 
		ILI932x_wr_dat(0x0000);

		ILI932x_wr_cmd(0x000f);						/* Extern Display Interface Control 2 */ 
		ILI932x_wr_dat(0x0000);						/* RGB interface polarity */
		
		/* ----------- Power ON Sequence ---------- */
		ILI932x_wr_cmd(0x0010);						/* Power Control 1(0x16b0) */ 
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0011);						/* Power Control 2(0x0001) */ 
		ILI932x_wr_dat(0x0007);
		ILI932x_wr_cmd(0x0012);						/* Power Control 3(0x0138) */ 
		ILI932x_wr_dat(0x0000);						/* VREG1OUT voltage */
		ILI932x_wr_cmd(0x0013);						/* Power Control 4 */ 
		ILI932x_wr_dat(0x0000);						/* VDV[4:0] for VCOM amplitude */
		_delay_ms(200);								/* Dis-charge capacitor power voltage */

		ILI932x_wr_cmd(0x0010);						/* Power Control 1(0x16b0) */ 
		ILI932x_wr_dat(0x1490);

		ILI932x_wr_cmd(0x0011);						/* Power Control 2(0x0001) */ 
		ILI932x_wr_dat(0x0221);						/* R11h=0x0227 at VCI=2.8V */
		_delay_ms(50);
		
		ILI932x_wr_cmd(0x0012);						/* Power Control 3(0x0138) */
		ILI932x_wr_dat(0x001c);						/* External reference voltage= Vci */
		_delay_ms(50);

		ILI932x_wr_cmd(0x0013);						/* Power Control 4 */
		ILI932x_wr_dat(0x0a00);						/* R13=0F00 when R12=009E;VDV[4:0] for VCOM amplitude */

		ILI932x_wr_cmd(0x0029);						/* Power Control 7 */ 
		ILI932x_wr_dat(0x000f);						/* R29=0019 when R12=009E;VCM[5:0] for VCOMH 0012  */

		ILI932x_wr_cmd(0x002b);						/* Frame Rate and Color Control */ 
		ILI932x_wr_dat(0x000d);						/* Frame Rate = 128Hz */
		_delay_ms(50);

		/* ----------- Adjust the Gamma Curve ---------- */
		ILI932x_wr_cmd(0x0030);	
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0031);	
		ILI932x_wr_dat(0x0203);
		ILI932x_wr_cmd(0x0032);	
		ILI932x_wr_dat(0x0001);
		ILI932x_wr_cmd(0x0035);	
		ILI932x_wr_dat(0x0205);
		ILI932x_wr_cmd(0x0036);	
		ILI932x_wr_dat(0x030c);
		ILI932x_wr_cmd(0x0037);	
		ILI932x_wr_dat(0x0607);
		ILI932x_wr_cmd(0x0038);	
		ILI932x_wr_dat(0x0405);
		ILI932x_wr_cmd(0x0039);	
		ILI932x_wr_dat(0x0707);
		ILI932x_wr_cmd(0x003c);	
		ILI932x_wr_dat(0x0502);
		ILI932x_wr_cmd(0x003d);	
		ILI932x_wr_dat(0x1008);

		/* ------------------ Set GRAM area --------------- */
		ILI932x_wr_cmd(0x0060);						/* Gate Scan Line */ 
		ILI932x_wr_dat(0xa700);
		ILI932x_wr_cmd(0x0061);						/* Driver Output Control */ 
		ILI932x_wr_dat(0x0001);
		ILI932x_wr_cmd(0x006a);						/* Set scrolling line */ 
		ILI932x_wr_dat(0x0000);
		
		/* -------------- Partial Display Control --------- */
		ILI932x_wr_cmd(0x0080);						/* Display Position? Partial Display 1 */ 
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0081);						/* RAM Address Start? Partial Display 1 */ 
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0082);						/* RAM Address End-Partial Display 1 */ 
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0083);						/* Display Position? Partial Display 2 */ 
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0084);						/* RAM Address Start? Partial Display 2 */ 
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0085);						/* RAM Address End-Partial Display 2 */ 
		ILI932x_wr_dat(0x0000);
		
		/* -------------- Panel Control ------------------- */
		ILI932x_wr_cmd(0x0090);						/* Frame Cycle Control(0x0013) */ 
		ILI932x_wr_dat(0x0010);
		ILI932x_wr_cmd(0x0092);						/* Panel Interface Control 2 */ 
		ILI932x_wr_dat(0x0600);
		ILI932x_wr_cmd(0x0093);						/* Panel Interface Control 3 */ 
		ILI932x_wr_dat(0x0003);
		ILI932x_wr_cmd(0x0095);						/* Frame Cycle Control(0x0110) */ 
		ILI932x_wr_dat(0x0110);
		ILI932x_wr_cmd(0x0097);						/* Frame Cycle Control? */ 
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0098);						/* Frame Cycle Control */ 
		ILI932x_wr_dat(0x0000);


		ILI932x_wr_cmd(0x0007);						/* Display On */ 
		ILI932x_wr_dat((1<<8)|(1<<5)|(1<<4)|(1<<1)|(1<<0));
	}

	else if(devicetype == 0x9320)
	{
		/* Initialize ILI9320 */
		ILI932x_wr_cmd(0x00E5);						/* Set the Vcore voltage and this setting is must.*/ 
		ILI932x_wr_dat(0x8000);
		ILI932x_wr_cmd(0x0000);						/* Start Oscillation */ 
		ILI932x_wr_dat(0x0001); 
		_delay_ms(15);

		ILI932x_wr_cmd(0x0001);						/* Driver Output Control */ 
		ILI932x_wr_dat(0x0100);
		ILI932x_wr_cmd(0x0002);						/* Driver Waveform Control Set 1 line inversion */ 
		ILI932x_wr_dat(0x0700);
		ILI932x_wr_cmd(0x0003);						/* Entry Mode Set  Set GRAM write direction and BGR=1 */ 
		ILI932x_wr_dat((1<<12)|(1<<7)|(1<<5)|(1<<4));
		/* ILI932x_wr_dat((1<<15)|(1<<7)|(1<<4)|(1<<5)|(1<<12)); */ /* 262k colour */
		/* ILI932x_wr_dat(0x1018); */				/* original */

		ILI932x_wr_cmd(0x0004);						/* Clear Resizing Control */ 
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0008);						/* Display Control 2(0x0207) set the back porch and front porch */ 
		ILI932x_wr_dat(0x0202);
		ILI932x_wr_cmd(0x0009);						/* Display Control 3(0x0000) set non-display area refresh cycle ISC[3:0] */ 
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x000a);						/* Frame Cycle Control 3(0x0000) */ 
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x000c);						/* Extern Display Interface Control 1(0x0000) */ 
		ILI932x_wr_dat((1<<0));
		ILI932x_wr_cmd(0x000d);						/* Frame Maker Position */ 
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x000f);						/* Extern Display Interface Control 2 */ 
		ILI932x_wr_dat(0x0000);	

		/* ----------- Power ON Sequence ---------- */
		ILI932x_wr_cmd(0x0010);						/* Power Control 1 */ 
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0011);						/* Power Control 2 */ 
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0012);						/* Power Control 3 */ 
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0013);						/* Power Control 4 */ 
		ILI932x_wr_dat(0x0000);
		_delay_ms(200);								/* Discharge */

		ILI932x_wr_cmd(0x0010);						/* Power Control 1(0x16b0) */ 
		ILI932x_wr_dat((1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));
		ILI932x_wr_cmd(0x0011);						/* Power Control 2(0x0001) */ 
		ILI932x_wr_dat(0x0007);
		_delay_ms(50);

		ILI932x_wr_cmd(0x0012);						/* Power Control 3(0x0138) */ 
		ILI932x_wr_dat((1<<8)|(1<<4)|(0<<0));

		_delay_ms(50);
		ILI932x_wr_cmd(0x0013);						/* Power Control 4 */ 
		ILI932x_wr_dat(0x0b00);
		ILI932x_wr_cmd(0x0029);						/* Power Control 7 */ 
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x002b);						/* Power Control VCM[4:0] for VCOMH */ 
		ILI932x_wr_dat((1<<4));
		_delay_ms(50);
		/* ILI932x_wr_dat((1<<15)|(1<<14)|(1<<4)); */ /* 262k colour + 16M dither */

		/* ----------- Adjust the Gamma Curve ---------- */
#ifdef MODULE_OPFPCT0634
		ILI932x_wr_cmd(0x0030);
		ILI932x_wr_dat(0x0001);
		ILI932x_wr_cmd(0x0031);
		ILI932x_wr_dat(0x0606);
		ILI932x_wr_cmd(0x0032);
		ILI932x_wr_dat(0x0304);
		ILI932x_wr_cmd(0x0033);
		ILI932x_wr_dat(0x0202);
		ILI932x_wr_cmd(0x0034);
		ILI932x_wr_dat(0x0202);
		ILI932x_wr_cmd(0x0035);
		ILI932x_wr_dat(0x0103);
		ILI932x_wr_cmd(0x0036);
		ILI932x_wr_dat(0x011d);
		ILI932x_wr_cmd(0x0037);
		ILI932x_wr_dat(0x0404);
		ILI932x_wr_cmd(0x0038);
		ILI932x_wr_dat(0x0404);
		ILI932x_wr_cmd(0x0039);
		ILI932x_wr_dat(0x0404);
		ILI932x_wr_cmd(0x003C);
		ILI932x_wr_dat(0x0700);
		ILI932x_wr_cmd(0x003D);
		ILI932x_wr_dat(0x0A1F);
#else
		ILI932x_wr_cmd(0x0030);
		ILI932x_wr_dat(0x0007);
		ILI932x_wr_cmd(0x0031);
		ILI932x_wr_dat(0x0203);
		ILI932x_wr_cmd(0x0032);
		ILI932x_wr_dat(0x0001);
		ILI932x_wr_cmd(0x0035);
		ILI932x_wr_dat(0x0007);
		ILI932x_wr_cmd(0x0036);
		ILI932x_wr_dat(0x0407);
		ILI932x_wr_cmd(0x0037);
		ILI932x_wr_dat(0x0607);
		ILI932x_wr_cmd(0x0038);
		ILI932x_wr_dat(0x0106);
		ILI932x_wr_cmd(0x0039);
		ILI932x_wr_dat(0x0007);
		ILI932x_wr_cmd(0x003C);
		ILI932x_wr_dat(0x0007);
		ILI932x_wr_cmd(0x003D);
		ILI932x_wr_dat(0x001E);
#endif
		
		/* ------------------ Set GRAM area --------------- */
		ILI932x_wr_cmd(0x0060);						/* Driver Output Control */ 
		ILI932x_wr_dat(0x2700);
		ILI932x_wr_cmd(0x0061);						/* Driver Output Control */ 
#ifdef MODULE_OPFPCT0634
		ILI932x_wr_dat(0x0000);
#else
		ILI932x_wr_dat(0x0001);
#endif

		ILI932x_wr_cmd(0x006a);						/* Vertical Srcoll Control */ 
		ILI932x_wr_dat(0x0000);

		/* -------------- Partial Display Control --------- */
		ILI932x_wr_cmd(0x0080);						/* Display Position? Partial Display 1 */ 
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0081);						/* RAM Address Start? Partial Display 1 */ 
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0082);						/* RAM Address End-Partial Display 1 */ 
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0083);						/* Display Position? Partial Display 2 */ 
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0084);						/* RAM Address Start? Partial Display 2 */ 
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0085);						/* RAM Address End-Partial Display 2 */ 
		ILI932x_wr_dat(0x0000);
		
		/* -------------- Panel Control ------------------- */
		ILI932x_wr_cmd(0x0090);						/* Frame Cycle Control(0x0013) */ 
		ILI932x_wr_dat((0<<7)|(16<<0));
		ILI932x_wr_cmd(0x0092);						/* Panel Interface Control 2 */ 
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0093);						/* Panel Interface Control 3 */ 
		ILI932x_wr_dat(0x0001);
		ILI932x_wr_cmd(0x0095);						/* Frame Cycle Control(0x0110) */ 
		ILI932x_wr_dat(0x0110);
		ILI932x_wr_cmd(0x0097);						/* Frame Cycle Control? */ 
		ILI932x_wr_dat((0<<8));
		ILI932x_wr_cmd(0x0098);						/* Frame Cycle Control */ 
		ILI932x_wr_dat(0x0000);

		ILI932x_wr_cmd(0x0007);						/* Display On */ 
		ILI932x_wr_dat((1<<8)|(1<<5)|(1<<4)|(1<<1)|(1<<0));
	}

	else if(devicetype == 0x1505)
	{
		/* Initialize R61505 & R61505U */
		ILI932x_wr_cmd(0x0000);
		ILI932x_wr_dat(0x0000);
		
		ILI932x_wr_cmd(0x0007);
		ILI932x_wr_dat(0x0001);
		_delay_ms(50);  
		
		ILI932x_wr_cmd(0x0017);
		ILI932x_wr_dat(0x0001);
		_delay_ms(50);
		
		ILI932x_wr_cmd(0x0010);
		ILI932x_wr_dat(0x17b0);
		ILI932x_wr_cmd(0x0011);
		ILI932x_wr_dat(0x0007);
		ILI932x_wr_cmd(0x0012);
		ILI932x_wr_dat(0x011a);
		ILI932x_wr_cmd(0x0013);
		ILI932x_wr_dat(0x0f00);
		ILI932x_wr_cmd(0x0015);
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0029);
		ILI932x_wr_dat(0x0009);
		ILI932x_wr_cmd(0x00fd);
		ILI932x_wr_dat(0x0000);
		_delay_ms(50);
		
		ILI932x_wr_cmd(0x0012);
		ILI932x_wr_dat(0x013a);
		_delay_ms(250);
		
		ILI932x_wr_cmd(0x0001);
		ILI932x_wr_dat(0x0100);
		ILI932x_wr_cmd(0x0002);
		ILI932x_wr_dat(0x0700);

		ILI932x_wr_cmd(0x0003);						/* Entry Mode Set  Set GRAM write direction and BGR=1 */ 
		ILI932x_wr_dat((1<<12)|(1<<9)|(1<<7)|(1<<5)|(1<<4));

		ILI932x_wr_cmd(0x0008);
		ILI932x_wr_dat(0x0808);
		ILI932x_wr_cmd(0x0009);
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x000a);
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x000c);
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x000d);
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0030);
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0031);
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0032);
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0033);
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0034);
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0035);
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0036);
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0037);
		ILI932x_wr_dat(0x0707);
		ILI932x_wr_cmd(0x0038);
		ILI932x_wr_dat(0x0707);
		ILI932x_wr_cmd(0x0039);
		ILI932x_wr_dat(0x0707);
		ILI932x_wr_cmd(0x003a);
		ILI932x_wr_dat(0x0303);
		ILI932x_wr_cmd(0x003b);
		ILI932x_wr_dat(0x0303);
		ILI932x_wr_cmd(0x003c);
		ILI932x_wr_dat(0x0707);
		ILI932x_wr_cmd(0x003d);
		ILI932x_wr_dat(0x0808);
		
		ILI932x_wr_cmd(0x0060);
		ILI932x_wr_dat(0x2700);
		ILI932x_wr_cmd(0x0061);
		ILI932x_wr_dat(0x0001);
		ILI932x_wr_cmd(0x006a);
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0090);
		ILI932x_wr_dat(0x0010);
		ILI932x_wr_cmd(0x0092);
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0093);
		ILI932x_wr_dat(0x0000);
	
		ILI932x_wr_cmd(0x0007);
		ILI932x_wr_dat(0x0021);
		_delay_ms(10);

		ILI932x_wr_cmd(0x0007);
		ILI932x_wr_dat(0x0061);
		_delay_ms(250);

		ILI932x_wr_cmd(0x0007);
		ILI932x_wr_dat(0x0173);
		ILI932x_wr_cmd(0x0030);
		ILI932x_wr_dat(0x0707);
		ILI932x_wr_cmd(0x0031);
		ILI932x_wr_dat(0x0407);
		ILI932x_wr_cmd(0x0032);
		ILI932x_wr_dat(0x0203);
		ILI932x_wr_cmd(0x0033);
		ILI932x_wr_dat(0x0303);
		ILI932x_wr_cmd(0x0034);
		ILI932x_wr_dat(0x0303);
		ILI932x_wr_cmd(0x0035);
		ILI932x_wr_dat(0x0202);
		ILI932x_wr_cmd(0x0036);
		ILI932x_wr_dat(0x001f);
		ILI932x_wr_cmd(0x0037);
		ILI932x_wr_dat(0x0707);
		ILI932x_wr_cmd(0x0038);
		ILI932x_wr_dat(0x0407);
		ILI932x_wr_cmd(0x0039);
		ILI932x_wr_dat(0x0203);
		ILI932x_wr_cmd(0x003a);
		ILI932x_wr_dat(0x0303);
		ILI932x_wr_cmd(0x003b);
		ILI932x_wr_dat(0x0303);
		ILI932x_wr_cmd(0x003c);
		ILI932x_wr_dat(0x0202);
		ILI932x_wr_cmd(0x003d);
		ILI932x_wr_dat(0x001f);
		_delay_ms(20);
	}

	else if(devicetype == 0x4531 || devicetype == 0x3145) /* 0x3145 is SPIMODE work around */
	{
		/* LGDP4531 */
		/* From Nemui-San
           CAUTION!! THIS INITIALIZE CODE IS UNSTABLE !!
		   TO OBTAIN CERTAIN CONTROL, YOU MUST GET INITIALIZATION
           WITHIN 5SEC SINCE POWERED TFT-LCM's MAIN VCC !! 
		*/
		ILI932x_wr_cmd(0x0000);
		ILI932x_wr_dat(0x0001);
		_delay_ms(50);
		
		ILI932x_wr_cmd(0x0010);
		ILI932x_wr_dat(0x0628);
		ILI932x_wr_cmd(0x0012);
		ILI932x_wr_dat(0x0006);
		ILI932x_wr_cmd(0x0013);
		ILI932x_wr_dat(0x0A7F);/*ILI932x_wr_dat(0x0A32);*/
		_delay_ms(100);
		
		ILI932x_wr_cmd(0x0011);
		ILI932x_wr_dat(0x0040);
		ILI932x_wr_cmd(0x0015);
		ILI932x_wr_dat(0x0020);
		_delay_ms(40);
	
		ILI932x_wr_cmd(0x0012);
		ILI932x_wr_dat(0x001A);/*ILI932x_wr_dat(0x0016);*/
		_delay_ms(40);
		
		ILI932x_wr_cmd(0x0010);
		ILI932x_wr_dat(0x3630);/*ILI932x_wr_dat(0x5660);*/
		ILI932x_wr_cmd(0x0013); 
		ILI932x_wr_dat(0x2D4D);/*ILI932x_wr_dat(0x2A4E);*/
		_delay_ms(10);
	
		ILI932x_wr_cmd(0x0008); 
		ILI932x_wr_dat(0x0303);
		ILI932x_wr_cmd(0x0001); 
		ILI932x_wr_dat(0x0100);	
		ILI932x_wr_cmd(0x0002); 
		ILI932x_wr_dat(0x0300);
		ILI932x_wr_cmd(0x0003); 
		ILI932x_wr_dat((1<<12)|(1<<9)|(1<<7)|(1<<5)|(1<<4));
		ILI932x_wr_cmd(0x000A); 
		ILI932x_wr_dat(0x0008);/*ILI932x_wr_dat(0x0000);*/
		ILI932x_wr_cmd(0x0030); 
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0031);
		ILI932x_wr_dat(0x0404);/*ILI932x_wr_dat(0x0402);*/
		ILI932x_wr_cmd(0x0032); 
		ILI932x_wr_dat(0x0106);
		ILI932x_wr_cmd(0x0033); 
		ILI932x_wr_dat(0x0305);/*ILI932x_wr_dat(0x0700);*/
		ILI932x_wr_cmd(0x0034); 
		ILI932x_wr_dat(0x0104);
		ILI932x_wr_cmd(0x0035); 
		ILI932x_wr_dat(0x0201);/*ILI932x_wr_dat(0x0301);*/
		ILI932x_wr_cmd(0x0036); 
		ILI932x_wr_dat(0x0707);
		ILI932x_wr_cmd(0x0037); 
		ILI932x_wr_dat(0x0502);/*ILI932x_wr_dat(0x0305);*/
		ILI932x_wr_cmd(0x0038); 
		ILI932x_wr_dat(0x0208);
		ILI932x_wr_cmd(0x0039); 
		ILI932x_wr_dat(0x0F0B);
		ILI932x_wr_cmd(0x0041); 
		ILI932x_wr_dat(0x0002);
		ILI932x_wr_cmd(0x0060); 
		ILI932x_wr_dat(0x2700);
		ILI932x_wr_cmd(0x0061); 
		ILI932x_wr_dat(0x0001);
		ILI932x_wr_cmd(0x0090);
		ILI932x_wr_dat(0x011d);/*ILI932x_wr_dat(0x0119);*/
		ILI932x_wr_cmd(0x0093); 
		ILI932x_wr_dat(0x0001);/*ILI932x_wr_dat(0x0004);*/
		ILI932x_wr_cmd(0x0092); 
		ILI932x_wr_dat(0x010A);
		
		ILI932x_wr_cmd(0x0007); 
		ILI932x_wr_dat(0x0001);
		ILI932x_wr_cmd(0x0007); 
		ILI932x_wr_dat(0x0021);
		ILI932x_wr_cmd(0x0007); 
		ILI932x_wr_dat(0x0023);
		ILI932x_wr_cmd(0x0007); 
		ILI932x_wr_dat(0x0033);
		ILI932x_wr_cmd(0x0007); 
		ILI932x_wr_dat(0x0133);
		_delay_ms(20);

	}

	else if(devicetype == 0x9328)
	{
		/* Initialize ILI9328 */
		ILI932x_wr_cmd(0x00E3);						/* Set the internal vcore voltage */ 
		ILI932x_wr_dat(0x3008);
		ILI932x_wr_cmd(0x00E7);						/* Set the internal vcore voltage */ 
		ILI932x_wr_dat(0x0012);
		ILI932x_wr_cmd(0x00EF);						/* Set the internal vcore voltage */ 
		ILI932x_wr_dat(0x1231);
		
		ILI932x_wr_cmd(0x0001); 					/* set SS and SM bit 							*/
		ILI932x_wr_dat(0x0100);
		ILI932x_wr_cmd(0x0002); 					/* set 1 line inversion 						*/
		ILI932x_wr_dat(0x0700);
		ILI932x_wr_cmd(0x0003);						/* Set GRAM write direction and BGR=1 */ 
		ILI932x_wr_dat((1<<12)|(1<<7)|(1<<5)|(1<<4));
		/* ILI932x_wr_dat(0x1030); */				/* original */
		ILI932x_wr_cmd(0x0004);						/* Resize register								*/
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0008); 					/* set the back porch and front porch			*/
		ILI932x_wr_dat(0x0207);
		ILI932x_wr_cmd(0x0009);					 	/* set non-display area refresh cycle ISC[3:0]	*/
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x000A); 					/* FMARK function								*/
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x000C); 					/* RGB interface setting						*/
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x000D); 					/* Frame marker Position						*/
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x000F); 					/* RGB interface polarity						*/
		ILI932x_wr_dat(0x0000);

		/* Power On sequence */
		ILI932x_wr_cmd(0x0010); 					/* SAP, BT[3:0], AP, DSTB, SLP, STB				*/
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0011);	 					/* DC1[2:0], DC0[2:0], VC[2:0]					*/
		ILI932x_wr_dat(0x0007);
		ILI932x_wr_cmd(0x0012); 					/* VREG1OUT voltage								*/
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0013); 					/* VDV[4:0] for VCOM amplitude					*/
		ILI932x_wr_dat(0x0000);
		_delay_ms(200); 							/* Dis-charge capacitor power voltage			*/
		ILI932x_wr_cmd(0x0010);	 					/* SAP, BT[3:0], AP, DSTB, SLP, STB				*/
		ILI932x_wr_dat(0x1290);
		ILI932x_wr_cmd(0x0011); 					/* DC1[2:0], DC0[2:0], VC[2:0]					*/
		ILI932x_wr_dat(0x0227);
		_delay_ms(50);      
		ILI932x_wr_cmd(0x0012); 					/* Internal reference voltage= Vci;				*/
		ILI932x_wr_dat(0x001A);
		_delay_ms(50);      
		ILI932x_wr_cmd(0x0013); 					/* Set VDV[4:0] for VCOM amplitude				*/
		ILI932x_wr_dat(0x1800);
		ILI932x_wr_cmd(0x0029); 					/* Set VCM[5:0] for VCOMH						*/
		ILI932x_wr_dat(0x0028);
		ILI932x_wr_cmd(0x002B); 					/* Set Frame Rate								*/
		ILI932x_wr_dat(0x000C);
		_delay_ms(50);      

		/* Adjust the Gamma Curve */
		ILI932x_wr_cmd(0x0030);
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0031);
		ILI932x_wr_dat(0x0305);
		ILI932x_wr_cmd(0x0032);
		ILI932x_wr_dat(0x0003);
		ILI932x_wr_cmd(0x0035);
		ILI932x_wr_dat(0x0304);
		ILI932x_wr_cmd(0x0036);
		ILI932x_wr_dat(0x000F);
		ILI932x_wr_cmd(0x0037);
		ILI932x_wr_dat(0x0407);
		ILI932x_wr_cmd(0x0038);
		ILI932x_wr_dat(0x0204);
		ILI932x_wr_cmd(0x0039);
		ILI932x_wr_dat(0x0707);
		ILI932x_wr_cmd(0x003C);
		ILI932x_wr_dat(0x0403);
		ILI932x_wr_cmd(0x003D);
		ILI932x_wr_dat(0x1604);

		/* Set GRAM area */
		ILI932x_wr_cmd(0x0060); 					/* Gate Scan Line								*/
		ILI932x_wr_dat(0xA700);
		ILI932x_wr_cmd(0x0061); 					/* NDL,VLE, REV									*/
		ILI932x_wr_dat(0x0001);
		ILI932x_wr_cmd(0x006A); 					/* set scrolling line							*/
		ILI932x_wr_dat(0x0000);

		/* Partial Display Control */
		ILI932x_wr_cmd(0x0080);
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0081);
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0082);
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0083);
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0084);
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0085);
		ILI932x_wr_dat(0x0000);

		/* Panel Control */
		ILI932x_wr_cmd(0x0090);
		ILI932x_wr_dat(0x0010);
		ILI932x_wr_cmd(0x0092);
		ILI932x_wr_dat(0x0600);
		ILI932x_wr_cmd(0x0007);						/* Display On */ 
		ILI932x_wr_dat((1<<8)|(1<<5)|(1<<4)|(1<<1)|(1<<0));

	}

	else if(devicetype == 0x7783)
	{
		/* Initialize ST7781 */
		ILI932x_wr_cmd(0x00FF);
		ILI932x_wr_dat(0x0001);
		ILI932x_wr_cmd(0x00F3);
		ILI932x_wr_dat(0x0008);
		ILI932x_wr_cmd(0x00F3);

		/* Display control setting */
		ILI932x_wr_cmd(0x0001);				/* output direct */
		ILI932x_wr_dat(0x0100);
		ILI932x_wr_cmd(0x0002);				/* line inversion */
		ILI932x_wr_dat(0x0700);
		ILI932x_wr_cmd(0x0003);				/* entry mode (65K,write ram direction ,BGR) */
		ILI932x_wr_dat((1<<12)|(1<<7)|(1<<5)|(1<<4));//ILI932x_wr_dat(0x1030);
		ILI932x_wr_cmd(0x0008);				/* Front porch=3,Back porch=2 */
		ILI932x_wr_dat(0x0302);
		ILI932x_wr_cmd(0x0009);				/* scan cycle */
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x000A);				/* Fmark On */
		ILI932x_wr_dat(0x0000);

		/* initial power status */
		ILI932x_wr_cmd(0x0010);				/* power control1 */
		ILI932x_wr_dat(0x0790);
		ILI932x_wr_cmd(0x0011);				/* power control2 */ 
		ILI932x_wr_dat(0x0005);
		ILI932x_wr_cmd(0x0012);				/* power control3 */
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0013);				/* power control4 */
		ILI932x_wr_dat(0x0000);
		_delay_ms(100);       				/* 100ms */

		/* Power supply startup 1 setting */
		ILI932x_wr_cmd(0x0010);				/* power control1 */
		ILI932x_wr_dat(0x12b0);
		_delay_ms(50);        				/* 50ms */
		ILI932x_wr_cmd(0x0011);				/* power control2 */
		ILI932x_wr_dat(0x0007);
		_delay_ms(50);            			/* 50ms */ 
		ILI932x_wr_cmd(0x0012);				/* power control3 */
		ILI932x_wr_dat(0x008c);
		ILI932x_wr_cmd(0x0013);				/* power control4 */
		ILI932x_wr_dat(0x1700);
		ILI932x_wr_cmd(0x0029);				/* Vcomh setting */
		ILI932x_wr_dat(0x0022);
		_delay_ms(50);            			/*50ms */

		/* default gamma (CABC OFF) */
		ILI932x_wr_cmd(0x0030);
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0031);
		ILI932x_wr_dat(0x0505);
		ILI932x_wr_cmd(0x0032);
		ILI932x_wr_dat(0x0205);
		ILI932x_wr_cmd(0x0035);
		ILI932x_wr_dat(0x0206);
		ILI932x_wr_cmd(0x0036);
		ILI932x_wr_dat(0x0408);
		ILI932x_wr_cmd(0x0037);
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0038);
		ILI932x_wr_dat(0x0504);
		ILI932x_wr_cmd(0x0039);
		ILI932x_wr_dat(0x0206);
		ILI932x_wr_cmd(0x003c);
		ILI932x_wr_dat(0x0206);
		ILI932x_wr_cmd(0x003d);
		ILI932x_wr_dat(0x0408);
	
		ILI932x_wr_cmd(0x0060);				/* Gate scan control */
		ILI932x_wr_dat(0xa700);
		ILI932x_wr_cmd(0x0061);				/* Non-display Area setting */
		ILI932x_wr_dat(0x0001);
		ILI932x_wr_cmd(0x0090);				/* RTNI setting */
		ILI932x_wr_dat(0x0033);
		ILI932x_wr_cmd(0x0007);				/* display on */
		ILI932x_wr_dat(0x0133);
		_delay_ms(50);

	}
	
	else if((devicetype == 0xB505) || (devicetype == 0xC505))
	{
		/* Initialize R61505W and R61505V */
		_delay_ms(10);
		ILI932x_wr_cmd(0x0000);
		ILI932x_wr_cmd(0x0000);
		ILI932x_wr_cmd(0x0000);
		ILI932x_wr_cmd(0x0000);
		ILI932x_wr_cmd(0x00A4);
		ILI932x_wr_dat(0x0001);
		_delay_ms(10);

		ILI932x_wr_cmd(0x0060);			/* NL */
		ILI932x_wr_dat(0x2700);
		ILI932x_wr_cmd(0x0008);			/* FP & BP */
		ILI932x_wr_dat(0x0808);
		ILI932x_wr_cmd(0x0030);			/* Gamma settings */
		ILI932x_wr_dat(0x0214);
		ILI932x_wr_cmd(0x0031);
		ILI932x_wr_dat(0x3715);
		ILI932x_wr_cmd(0x0032);
		ILI932x_wr_dat(0x0604);
		ILI932x_wr_cmd(0x0033);
		ILI932x_wr_dat(0x0E16);
		ILI932x_wr_cmd(0x0034);
		ILI932x_wr_dat(0x2211);
		ILI932x_wr_cmd(0x0035);
		ILI932x_wr_dat(0x1500);
		ILI932x_wr_cmd(0x0036);
		ILI932x_wr_dat(0x8507);
		ILI932x_wr_cmd(0x0037);
		ILI932x_wr_dat(0x1407);
		ILI932x_wr_cmd(0x0038);
		ILI932x_wr_dat(0x1403);
		ILI932x_wr_cmd(0x0039);
		ILI932x_wr_dat(0x0020);
		ILI932x_wr_cmd(0x0090);			/* DIVI & RTNI */
		ILI932x_wr_dat(0x0015);
		ILI932x_wr_cmd(0x0010);			/* BT,AP */
		ILI932x_wr_dat(0x0410);
		ILI932x_wr_cmd(0x0011);			/* VC,DC0,DC1 */
		ILI932x_wr_dat(0x0237);
		ILI932x_wr_cmd(0x0029);			/* VCM1 */
		ILI932x_wr_dat(0x0046);
		ILI932x_wr_cmd(0x002A);			/* VCMSEL,VCM2 */
		ILI932x_wr_dat(0x0046);
		ILI932x_wr_cmd(0x0007);
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0012);			/* VRH,VCMR,PSON=0,PON=0 */
		ILI932x_wr_dat(0x0189);
		ILI932x_wr_cmd(0x0013);			/* VDV */
		ILI932x_wr_dat(0x1100);
		_delay_ms(150);

		ILI932x_wr_cmd(0x0012);			/* PSON=1,PON=1 */
		ILI932x_wr_dat(0x01B9);
		ILI932x_wr_cmd(0x0001);			/* Other mode settings */
		ILI932x_wr_dat(0x0100);
		ILI932x_wr_cmd(0x0002);			/* BC0=1--Line inversion */
		ILI932x_wr_dat(0x0200);
		ILI932x_wr_cmd(0x0003);
		ILI932x_wr_dat((1<<12)|(1<<9)|(1<<7)|(1<<5)|(1<<4));
		/* ILI932x_wr_dat(0x1030); */				/* original */
		ILI932x_wr_cmd(0x0009);
		ILI932x_wr_dat(0x0001);
		ILI932x_wr_cmd(0x000A);
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x000D);
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x000E);			/* VCOM equalize */
		ILI932x_wr_dat(0x0030);

		ILI932x_wr_cmd(0x0061);
		ILI932x_wr_dat(0x0001);
		ILI932x_wr_cmd(0x006A);
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0080);
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0081);
		ILI932x_wr_dat(0x0000);
		ILI932x_wr_cmd(0x0082);
		ILI932x_wr_dat(0x005F);
		ILI932x_wr_cmd(0x0092);
		ILI932x_wr_dat(0x0100);
		ILI932x_wr_cmd(0x0093);
		ILI932x_wr_dat(0x0701);
		_delay_ms(80);

		ILI932x_wr_cmd(0x0007);
		ILI932x_wr_dat(0x0100);			/* BASEE=1--Display On */

	}

	else { for(;;);} /* Invalid Device Code!! */

	ILI932x_clear();

#if 0	/* test code RED */
	volatile uint32_t n;

	ILI932x_rect(0,MAX_X-1,0,MAX_Y-1);
	n = (uint32_t)(MAX_X) * (MAX_Y);

	do {
		ILI932x_wr_dat(COL_RED);
	} while (--n);
	
	_delay_ms(500);
	for(;;);
#endif

}


/**************************************************************************/
/*! 
    Draw Windows 24bitBMP File.
*/
/**************************************************************************/
int ILI932x_draw_bmp(const uint8_t* ptr){

	uint32_t n, m, biofs, bw, iw, bh, w;
	uint32_t xs, xe, ys, ye, i;
	uint8_t *p;
	uint16_t d;

	/* Load BitStream Address Offset  */
	biofs = LD_UINT32(ptr+10);
	/* Check Plane Count "1" */
	if (LD_UINT16(ptr+26) != 1)  return 0;
	/* Check BMP bit_counts "24(windows bitmap standard)" */
	if (LD_UINT16(ptr+28) != 24) return 0;
	/* Check BMP Compression "BI_RGB(no compresstion)"*/
	if (LD_UINT32(ptr+30) != 0)  return 0;
	/* Load BMP width */
	bw = LD_UINT32(ptr+18);
	/* Load BMP height */
	bh = LD_UINT32(ptr+22);
	/* Check BMP width under 1280px */
	if (!bw || bw > 1280 || !bh) return 0;
	
	/* Calculate Data byte count per holizontal line */
	iw = ((bw * 3) + 3) & ~3;

	/* Centering */
	if (bw > MAX_X) {
		xs = 0; xe = MAX_X-1;
	} else {
		xs = (MAX_X - bw) / 2;
		xe = (MAX_X - bw) / 2 + bw - 1;
	}
	if (bh > MAX_Y) {
		ys = 0; ye = MAX_Y-1;
	} else {
		ys = (MAX_Y - bh) / 2;
		ye = (MAX_Y - bh) / 2 + bh - 1;
	}

	/* Clear Display */
	ILI932x_clear();

    /* Limit to MAX_Y */
	m = (bh <= MAX_Y) ? biofs : biofs + (bh - MAX_Y) * iw;

	/* Set First data offset */  
	i = m % 512;

	/* Set BMP's Bottom-Left point */
	m = ye;

    /* Limit MAX_X */
	w = (bw > MAX_X) ? MAX_X : bw;

	do {
		ILI932x_rect(xs, xe, m, m);
		n = 0;
		p = (uint8_t*)ptr + i;
		do {
			n++;
			/* 65k colour access */
			d = *p++ >> 3;
			d |= (*p++ >> 2) << 5;
			d |= (*p++ >> 3) << 11;
		#if 0
			/* 262k colour access */
			d2 = *p << 14;	  		/* Blue  */
			d  = (*p++ >> 4);		/* Blue  */
			d |= ((*p++>>2)<<4);	/* Green */
			d |= ((*p++>>2)<<10);	/* Red   */
			/* 262k +16M dither colour access */
			d2 = *p++ << 8;			/* Blue  */
			d  = *p++;				/* Green */
			d |= *p++ << 8;			/* Red   */
		#endif
			ILI932x_wr_dat(d);

		} while (n < w);
		i += iw;

	} while (m-- > ys);

	return 1;
}

/* End Of File ---------------------------------------------------------------*/
