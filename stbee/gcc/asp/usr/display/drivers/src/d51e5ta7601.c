/********************************************************************************/
/*!
	@file			d51e5ta7601.c
	@author         Nemui Trinomius (http://nemuisan.blog.bai.ne.jp)
    @version        1.00
    @date           2012.06.23
	@brief          Based on Chan's MCI_OLED@LPC23xx-demo thanks!				@n
					Available TFT-LCM are listed below.							@n
					It can drive TFT1P4705 TFT module.(8/16bit)

    @section HISTORY
		2012.06.23	V1.00	Stable Release.

    @section LICENSE
		BSD License. See Copyright.txt
*/
/********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "d51e5ta7601.h"
/* check header file version for fool proof */
#if __D51E5TA7601_H != 0x0100
#error "header file version is not correspond!"
#endif

/* Defines -------------------------------------------------------------------*/

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
inline void D51E5TA7601_reset(void)
{
	D51E5TA7601_RES_SET();						/* RES=H, RD=H, WR=H   		*/
	D51E5TA7601_RD_SET();
	D51E5TA7601_WR_SET();
	_delay_ms(10);								/* wait 10ms     			*/

	D51E5TA7601_RES_CLR();						/* RES=L, CS=L   			*/
	D51E5TA7601_CS_CLR();

	_delay_ms(50);								/* wait 50ms     			*/
	D51E5TA7601_RES_SET();						/* RES=H					*/
	_delay_ms(50);				    			/* wait 50ms     			*/
}

/**************************************************************************/
/*! 
    Write LCD Command.
*/
/**************************************************************************/
inline void D51E5TA7601_wr_cmd(uint16_t cmd)
{
	D51E5TA7601_DC_CLR();							/* DC=L						*/

#if defined(GPIO_ACCESS_8BIT) | defined(BUS_ACCESS_8BIT)
	D51E5TA7601_CMD = 0;
	D51E5TA7601_WR();
#endif

	D51E5TA7601_CMD = cmd;							/* cmd(8bit_Low or 16bit)	*/
	D51E5TA7601_WR();								/* WR=L->H					*/

	D51E5TA7601_DC_SET();							/* DC=H						*/
}

/**************************************************************************/
/*! 
    Write LCD Data and GRAM.
*/
/**************************************************************************/
inline void D51E5TA7601_wr_dat(uint16_t dat)
{
#if defined(GPIO_ACCESS_8BIT) | defined(BUS_ACCESS_8BIT)
	D51E5TA7601_DATA = (uint8_t)(dat>>8);			/* upper 8bit data			*/
	D51E5TA7601_WR();								/* WR=L->H					*/
#endif

	D51E5TA7601_DATA = dat;							/* data(8bit_Low or 16bit)	*/
	D51E5TA7601_WR();								/* WR=L->H					*/
}

/**************************************************************************/
/*! 
    Write LCD Block Data.
*/
/**************************************************************************/
inline void D51E5TA7601_wr_block(uint8_t *p, unsigned int cnt)
{

#ifdef  USE_DISPLAY_DMA_TRANSFER
   DMA_TRANSACTION(p, cnt);
#else

	cnt /= 4;
	
	while (cnt--) {
		/* avoid -Wsequence-point's warning */
		D51E5TA7601_wr_dat(*(p+1)|*(p)<<8);
		p++;p++;
		D51E5TA7601_wr_dat(*(p+1)|*(p)<<8);
		p++;p++;
	}
#endif

}

/**************************************************************************/
/*! 
    Read LCD Register.
*/
/**************************************************************************/
inline uint16_t D51E5TA7601_rd_cmd(uint16_t cmd)
{
	uint16_t val;

#if defined(GPIO_ACCESS_8BIT) | defined(BUS_ACCESS_8BIT)
	uint16_t temp;
#endif

	D51E5TA7601_wr_cmd(cmd);
	D51E5TA7601_WR_SET();

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


/**************************************************************************/
/*! 
    Set Rectangle.
*/
/**************************************************************************/
inline void D51E5TA7601_rect(uint32_t x, uint32_t width, uint32_t y, uint32_t height)
{

	D51E5TA7601_wr_cmd(0x0044);				/* Horizontal RAM Start ADDR */
	D51E5TA7601_wr_dat(OFS_COL + x);
	D51E5TA7601_wr_cmd(0x0045);				/* Horizontal RAM End ADDR */
	D51E5TA7601_wr_dat(OFS_COL + width);
	D51E5TA7601_wr_cmd(0x0046);				/* Vertical RAM Start ADDR */
	D51E5TA7601_wr_dat(OFS_RAW + y);
	D51E5TA7601_wr_cmd(0x0047);				/* Vertical End ADDR */
	D51E5TA7601_wr_dat(OFS_RAW + height);

	D51E5TA7601_wr_cmd(0x0020);				/* GRAM Vertical/Horizontal ADDR Set(AD0~AD7) */
	D51E5TA7601_wr_dat(OFS_RAW + y);
	D51E5TA7601_wr_cmd(0x0021);				/* GRAM Vertical/Horizontal ADDR Set(AD8~AD16) */
	D51E5TA7601_wr_dat(OFS_COL + x);

	D51E5TA7601_wr_cmd(0x0022);			/* Write Data to GRAM */

}

/**************************************************************************/
/*! 
    Clear Display.
*/
/**************************************************************************/
inline void D51E5TA7601_clear(void)
{
	volatile uint32_t n;

	D51E5TA7601_rect(0,MAX_X-1,0,MAX_Y-1);
	n = (uint32_t)(MAX_X) * (MAX_Y);

	do {
		D51E5TA7601_wr_dat(COL_BLACK);
	} while (--n);

}


/**************************************************************************/
/*! 
    TFT-LCD Module Initialize.
*/
/**************************************************************************/
void D51E5TA7601_init(void)
{
	uint16_t devicetype;
	
	Display_IoInit_If();

	D51E5TA7601_reset();

	/* Check Device Code */
	devicetype = D51E5TA7601_rd_cmd(0x0000);  			/* Confirm Vaild LCD Controller */

	if(devicetype == 0x9325)
	{
		/* Initialize ILI9325 & ILI9325C */
		D51E5TA7601_wr_cmd(0x00E3);						/* Set the internal vcore voltage */ 
		D51E5TA7601_wr_dat(0x3008);
		D51E5TA7601_wr_cmd(0x00E7);						/* Set the internal vcore voltage */ 
		D51E5TA7601_wr_dat(0x0012);
		D51E5TA7601_wr_cmd(0x00EF);						/* Set the internal vcore voltage */ 
		D51E5TA7601_wr_dat(0x1231);

		D51E5TA7601_wr_cmd(0x0001);						/* Set SS and SM bit */ 
		D51E5TA7601_wr_dat(0x0100);

		D51E5TA7601_wr_cmd(0x0002);						/* Set 1 line inversion */ 
		D51E5TA7601_wr_dat(0x0700);

		D51E5TA7601_wr_cmd(0x0003);						/* Set GRAM write direction and BGR=1 */ 
		D51E5TA7601_wr_dat((1<<12)|(1<<9)|(1<<7)|(1<<5)|(1<<4));
		/* D51E5TA7601_wr_dat(0x1030); */				/* original */

		D51E5TA7601_wr_cmd(0x0004);						/* Scalling Control */ 
		D51E5TA7601_wr_dat(0x0000);
		
		D51E5TA7601_wr_cmd(0x0008);						/* Display Control 2(0x0207) */ 
		D51E5TA7601_wr_dat(0x0207);						/* Set the back porch and front porch */

		D51E5TA7601_wr_cmd(0x0009);						/* Display Control 3(0x0000) */ 
		D51E5TA7601_wr_dat(0x0000);						/* Set non-display area refresh cycle ISC[3:0] */

		D51E5TA7601_wr_cmd(0x000a);						/* Frame Cycle Control 3(0x0000) */ 
		D51E5TA7601_wr_dat(0x0000);						/* FMARK function */

		D51E5TA7601_wr_cmd(0x000c);						/* Extern Display Interface Control 1(0x0000) */ 
		D51E5TA7601_wr_dat(0x0000);						/* RGB interface setting */

		D51E5TA7601_wr_cmd(0x000d);						/* Frame Maker Position */ 
		D51E5TA7601_wr_dat(0x0000);

		D51E5TA7601_wr_cmd(0x000f);						/* Extern Display Interface Control 2 */ 
		D51E5TA7601_wr_dat(0x0000);						/* RGB interface polarity */
		
		/* ----------- Power ON Sequence ---------- */
		D51E5TA7601_wr_cmd(0x0010);						/* Power Control 1(0x16b0) */ 
		D51E5TA7601_wr_dat(0x0000);
		D51E5TA7601_wr_cmd(0x0011);						/* Power Control 2(0x0001) */ 
		D51E5TA7601_wr_dat(0x0007);
		D51E5TA7601_wr_cmd(0x0012);						/* Power Control 3(0x0138) */ 
		D51E5TA7601_wr_dat(0x0000);						/* VREG1OUT voltage */
		D51E5TA7601_wr_cmd(0x0013);						/* Power Control 4 */ 
		D51E5TA7601_wr_dat(0x0000);						/* VDV[4:0] for VCOM amplitude */
		_delay_ms(200);								/* Dis-charge capacitor power voltage */

		D51E5TA7601_wr_cmd(0x0010);						/* Power Control 1(0x16b0) */ 
		D51E5TA7601_wr_dat(0x1490);

		D51E5TA7601_wr_cmd(0x0011);						/* Power Control 2(0x0001) */ 
		D51E5TA7601_wr_dat(0x0221);						/* R11h=0x0227 at VCI=2.8V */
		_delay_ms(50);
		
		D51E5TA7601_wr_cmd(0x0012);						/* Power Control 3(0x0138) */
		D51E5TA7601_wr_dat(0x001c);						/* External reference voltage= Vci */
		_delay_ms(50);

		D51E5TA7601_wr_cmd(0x0013);						/* Power Control 4 */
		D51E5TA7601_wr_dat(0x0a00);						/* R13=0F00 when R12=009E;VDV[4:0] for VCOM amplitude */

		D51E5TA7601_wr_cmd(0x0029);						/* Power Control 7 */ 
		D51E5TA7601_wr_dat(0x000f);						/* R29=0019 when R12=009E;VCM[5:0] for VCOMH 0012  */

		D51E5TA7601_wr_cmd(0x002b);						/* Frame Rate and Color Control */ 
		D51E5TA7601_wr_dat(0x000d);						/* Frame Rate = 128Hz */
		_delay_ms(50);

		/* ----------- Adjust the Gamma Curve ---------- */
		D51E5TA7601_wr_cmd(0x0030);	
		D51E5TA7601_wr_dat(0x0000);
		D51E5TA7601_wr_cmd(0x0031);	
		D51E5TA7601_wr_dat(0x0203);
		D51E5TA7601_wr_cmd(0x0032);	
		D51E5TA7601_wr_dat(0x0001);
		D51E5TA7601_wr_cmd(0x0035);	
		D51E5TA7601_wr_dat(0x0205);
		D51E5TA7601_wr_cmd(0x0036);	
		D51E5TA7601_wr_dat(0x030c);
		D51E5TA7601_wr_cmd(0x0037);	
		D51E5TA7601_wr_dat(0x0607);
		D51E5TA7601_wr_cmd(0x0038);	
		D51E5TA7601_wr_dat(0x0405);
		D51E5TA7601_wr_cmd(0x0039);	
		D51E5TA7601_wr_dat(0x0707);
		D51E5TA7601_wr_cmd(0x003c);	
		D51E5TA7601_wr_dat(0x0502);
		D51E5TA7601_wr_cmd(0x003d);	
		D51E5TA7601_wr_dat(0x1008);

		/* ------------------ Set GRAM area --------------- */
		D51E5TA7601_wr_cmd(0x0060);						/* Gate Scan Line */ 
		D51E5TA7601_wr_dat(0xa700);
		D51E5TA7601_wr_cmd(0x0061);						/* Driver Output Control */ 
		D51E5TA7601_wr_dat(0x0001);
		D51E5TA7601_wr_cmd(0x006a);						/* Set scrolling line */ 
		D51E5TA7601_wr_dat(0x0000);
		
		/* -------------- Partial Display Control --------- */
		D51E5TA7601_wr_cmd(0x0080);						/* Display Position? Partial Display 1 */ 
		D51E5TA7601_wr_dat(0x0000);
		D51E5TA7601_wr_cmd(0x0081);						/* RAM Address Start? Partial Display 1 */ 
		D51E5TA7601_wr_dat(0x0000);
		D51E5TA7601_wr_cmd(0x0082);						/* RAM Address End-Partial Display 1 */ 
		D51E5TA7601_wr_dat(0x0000);
		D51E5TA7601_wr_cmd(0x0083);						/* Display Position? Partial Display 2 */ 
		D51E5TA7601_wr_dat(0x0000);
		D51E5TA7601_wr_cmd(0x0084);						/* RAM Address Start? Partial Display 2 */ 
		D51E5TA7601_wr_dat(0x0000);
		D51E5TA7601_wr_cmd(0x0085);						/* RAM Address End-Partial Display 2 */ 
		D51E5TA7601_wr_dat(0x0000);
		
		/* -------------- Panel Control ------------------- */
		D51E5TA7601_wr_cmd(0x0090);						/* Frame Cycle Control(0x0013) */ 
		D51E5TA7601_wr_dat(0x0010);
		D51E5TA7601_wr_cmd(0x0092);						/* Panel Interface Control 2 */ 
		D51E5TA7601_wr_dat(0x0600);
		D51E5TA7601_wr_cmd(0x0093);						/* Panel Interface Control 3 */ 
		D51E5TA7601_wr_dat(0x0003);
		D51E5TA7601_wr_cmd(0x0095);						/* Frame Cycle Control(0x0110) */ 
		D51E5TA7601_wr_dat(0x0110);
		D51E5TA7601_wr_cmd(0x0097);						/* Frame Cycle Control? */ 
		D51E5TA7601_wr_dat(0x0000);
		D51E5TA7601_wr_cmd(0x0098);						/* Frame Cycle Control */ 
		D51E5TA7601_wr_dat(0x0000);


		D51E5TA7601_wr_cmd(0x0007);						/* Display On */ 
		D51E5TA7601_wr_dat((1<<8)|(1<<5)|(1<<4)|(1<<1)|(1<<0));

	}

	else { for(;;);} /* Invalid Device Code!! */

	D51E5TA7601_clear();

#if 0	/* test code RED */
	volatile uint32_t n;

	D51E5TA7601_rect(0,MAX_X-1,0,MAX_Y-1);
	n = (uint32_t)(MAX_X) * (MAX_Y);

	do {
		D51E5TA7601_wr_dat(COL_RED);
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
int D51E5TA7601_draw_bmp(const uint8_t* ptr){

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
	D51E5TA7601_clear();

    /* Limit to MAX_Y */
	m = (bh <= MAX_Y) ? biofs : biofs + (bh - MAX_Y) * iw;

	/* Set First data offset */  
	i = m % 512;

	/* Set BMP's Bottom-Left point */
	m = ye;

    /* Limit MAX_X */
	w = (bw > MAX_X) ? MAX_X : bw;

	do {
		D51E5TA7601_rect(xs, xe, m, m);
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
			D51E5TA7601_wr_dat(d);

		} while (n < w);
		i += iw;

	} while (m-- > ys);

	return 1;
}

/* End Of File ---------------------------------------------------------------*/
