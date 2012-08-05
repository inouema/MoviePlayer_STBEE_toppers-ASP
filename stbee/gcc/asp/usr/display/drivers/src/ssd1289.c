/********************************************************************************/
/*!
	@file			ssd1289.c
	@author         Nemui Trinomius (http://nemuisan.blog.bai.ne.jp)
    @version        5.00
    @date           2011.12.23
	@brief          Based on Chan's MCI_OLED@LPC23xx-demo thanks!				@n
					It can drive KFM597A01-1A TFT module.

    @section HISTORY
		2010.10.01	V1.00	Stable Release.
		2010.12.31	V2.00	Cleanup SourceCode.
		2011.03.10	V3.00	C++ Ready.
		2011.10.25	V4.00	Added DMA TransactionSupport.
		2011.12.23	V5.00	Optimize Some Codes.
		
    @section LICENSE
		BSD License. See Copyright.txt
*/
/********************************************************************************/

/* Includes ------------------------------------------------------------------*/

#ifdef USE_TOPPERS
#include <sil.h>
#endif
#include "SSD1289.h"


/* check header file version for fool proof */
#if __SSD1289_H != 0x0500
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
#if 1 /* AAAAA */
	return;
#else
	ms += ticktime;
	while (ticktime < ms);
#endif
}
#endif


/**************************************************************************/
/*! 
    Display Module Reset Routine.
*/
/**************************************************************************/
inline void SSD1289_reset(void)
{
#if defined USE_TOPPERS
	sil_orw((void*)(GPIOD_BASE + 0x0c),   (1 << 2));
	sil_dly_nse(1000);  // 1us
	sil_andw((void*)(GPIOD_BASE + 0x0c), ~(1 << 2));
	sil_dly_nse(10000000); // 10ms
	sil_orw((void*)(GPIOD_BASE + 0x0c),   (1 << 2));
	//sil_dly_nse(50000000); // 50ms
#else
	SSD1289_RES_SET();							/* RES=H, RD=H, WR=H   		*/
	SSD1289_RD_SET();
	SSD1289_WR_SET();
	_delay_ms(1);								/* wait 1ms     			*/
	SSD1289_RES_CLR();							/* RES=L, CS=L   			*/
	SSD1289_CS_CLR();
	_delay_ms(10);								/* wait 10ms     			*/
	SSD1289_RES_SET();						  	/* RES=H					*/
	_delay_ms(50);				    			/* wait 50ms     			*/
#endif
}

/**************************************************************************/
/*! 
    Write LCD Command.
*/
/**************************************************************************/
inline void SSD1289_wr_cmd(uint8_t cmd)
{
	SSD1289_DC_CLR();							/* DC=L						*/

#if defined(GPIO_ACCESS_8BIT) | defined(BUS_ACCESS_8BIT)
	SSD1289_CMD = 0;
	SSD1289_WR();
#endif

#if defined USE_TOPPERS
	//sil_wrw_mem((uint32_t *)LCD_CMD_MODE_ADDR, (uint32_t)cmd);
	SSD1289_CMD = cmd; /* cmd */
#else
	sil_wrh_mem((void *)LCD_CMD_MODE_ADDR, (uint16_t)cmd);
#endif

	SSD1289_WR();								/* WR=L->H					*/
	SSD1289_DC_SET();							/* DC=H						*/
}

/**************************************************************************/
/*! 
    Write LCD Data and GRAM.
*/
/**************************************************************************/
inline void SSD1289_wr_dat(uint16_t dat)
{
#if defined USE_TOPPERS
	//sil_wrw_mem((uint32_t *)LCD_CMD_MODE_ADDR, (uint32_t)dat);
	SSD1289_DATA = dat;							/* 16bit data 				*/
#elif defined(GPIO_ACCESS_8BIT) | defined(BUS_ACCESS_8BIT)
	SSD1289_DATA = (uint8_t)(dat>>8);			/* upper 8bit data			*/
	SSD1289_WR();								/* WR=L->H					*/
	SSD1289_DATA = (uint8_t)dat;				/* lower 8bit data			*/
#else
	sil_wrh_mem((void *)LCD_CMD_MODE_ADDR, (uint16_t)dat);
#endif

	SSD1289_WR();								/* WR=L->H					*/
}

/**************************************************************************/
/*! 
    Write LCD Block Data.
*/
/**************************************************************************/
inline void SSD1289_wr_block(uint8_t *p, unsigned int cnt)
{

#ifdef  USE_DISPLAY_DMA_TRANSFER
   DMA_TRANSACTION(p, cnt);
#else
	int n;

	n = cnt % 4;
	cnt /= 4;

	while (cnt--) {
		/* avoid -Wsequence-point's warning */
		SSD1289_wr_dat(*(p+1)|*(p)<<8);
		p++;p++;
		SSD1289_wr_dat(*(p+1)|*(p)<<8);
		p++;p++;
	}
#endif

}

/**************************************************************************/
/*! 
    Set Rectangle.
*/
/**************************************************************************/
inline void SSD1289_rect(uint32_t x, uint32_t width, uint32_t y, uint32_t height)
{

	SSD1289_wr_cmd(0x45);				/* Horizontal RAM Start ADDR */
	SSD1289_wr_dat(OFS_RAW + y);
	SSD1289_wr_cmd(0x46);				/* Horizontal RAM End ADDR */
	SSD1289_wr_dat(OFS_RAW + height);

	SSD1289_wr_cmd(0x44);				/* Vertical Start,End ADDR */
	SSD1289_wr_dat(((OFS_COL + width)<<8)|(OFS_COL + x));

	SSD1289_wr_cmd(0x4F);				/* GRAM Vertical/Horizontal ADDR Set(AD0~AD7) */
	SSD1289_wr_dat(OFS_RAW + y);
	SSD1289_wr_cmd(0x4E);				/* GRAM Vertical/Horizontal ADDR Set(AD8~AD16) */
	SSD1289_wr_dat(OFS_COL + x);

	SSD1289_wr_cmd(0x22);				/* Write Data to GRAM */

}

/**************************************************************************/
/*! 
    Clear Display.
*/
/**************************************************************************/
inline void SSD1289_clear(void)
{
	volatile uint32_t n;

	SSD1289_rect(0,MAX_X-1,0,MAX_Y-1);
	n = (uint32_t)(MAX_X) * (MAX_Y);

	do {
		SSD1289_wr_dat(COL_BLACK);
	} while (--n);

}


/**************************************************************************/
/*! 
    Read LCD Register.
*/
/**************************************************************************/
inline uint16_t SSD1289_rd_cmd(uint8_t cmd)
{
	uint16_t val;

#if defined(GPIO_ACCESS_8BIT) | defined(BUS_ACCESS_8BIT)
	uint16_t temp;
#endif

	SSD1289_wr_cmd(cmd);
	SSD1289_WR_SET();

#if defined(GPIO_ACCESS_8BIT) | defined(BUS_ACCESS_8BIT)
    ReadLCDData(temp);
#endif

#if defined USE_TOPPERS
	// AAAAA val = (uint16_t)sil_rew_mem((uint32_t *)LCD_DATA_MODE_ADDR);
	ReadLCDData(val);
#else
	val = sil_reh_mem((void *)LCD_DATA_MODE_ADDR);
#endif



#if defined(GPIO_ACCESS_8BIT) | defined(BUS_ACCESS_8BIT)
	val &= 0x00FF;
	val |= temp<<8;
#endif

	return val;
}


/**************************************************************************/
/*! 
    TFT-LCD Module Initialize.
*/
/**************************************************************************/
void SSD1289_init (void)
{
	uint16_t devicetype;

	Display_IoInit_If();

	SSD1289_reset();

	/* Check Device Code */
	devicetype = SSD1289_rd_cmd(0x0000);  			/* Confirm Vaild LCD Controller */
	if(devicetype == 0x8989)
	{
		SSD1289_wr_cmd(0x07);
		SSD1289_wr_dat(0x0021);
		SSD1289_wr_cmd(0x00);
		SSD1289_wr_dat(0x0001);
		SSD1289_wr_cmd(0x07);
		SSD1289_wr_dat(0x0023);
		SSD1289_wr_cmd(0x10);
		SSD1289_wr_dat(0x0000);
		sil_dly_nse(30000000); // 30ms

		SSD1289_wr_cmd(0x07);
		SSD1289_wr_dat(0x0033);
		SSD1289_wr_cmd(0x11);
		SSD1289_wr_dat(0x6070);
		SSD1289_wr_cmd(0x02);
		SSD1289_wr_dat(0x0500);
		SSD1289_wr_cmd(0x03);
		SSD1289_wr_dat(0x0804);
		SSD1289_wr_cmd(0x0C);
		SSD1289_wr_dat(0x0000);
		SSD1289_wr_cmd(0x0D);
		SSD1289_wr_dat(0x0808);

		SSD1289_wr_cmd(0x0E);
		SSD1289_wr_dat(0x2900);
		SSD1289_wr_cmd(0x1E);
		SSD1289_wr_dat(0x00B8);
		SSD1289_wr_cmd(0x01);
		SSD1289_wr_dat(0x2B3F);
		SSD1289_wr_cmd(0x10);
		SSD1289_wr_dat(0x0000);

		SSD1289_wr_cmd(0x05);
		SSD1289_wr_dat(0x0000);
		SSD1289_wr_cmd(0x06);
		SSD1289_wr_dat(0x0000);
		SSD1289_wr_cmd(0x16);
		SSD1289_wr_dat(0xEF1C);
		SSD1289_wr_cmd(0x17);
		SSD1289_wr_dat(0x0003);
		SSD1289_wr_cmd(0x07);
		SSD1289_wr_dat(0x0233);

		SSD1289_wr_cmd(0x0B);
		SSD1289_wr_dat(0x0000|(3<<6));
		SSD1289_wr_cmd(0x0F);
		SSD1289_wr_dat(0x0000);
		SSD1289_wr_cmd(0x41);
		SSD1289_wr_dat(0x0000);
		SSD1289_wr_cmd(0x42);
		SSD1289_wr_dat(0x0000);
		SSD1289_wr_cmd(0x48);
		SSD1289_wr_dat(0x0000);

		SSD1289_wr_cmd(0x49);
		SSD1289_wr_dat(0x013F);
		SSD1289_wr_cmd(0x4A);
		SSD1289_wr_dat(0x0000);
		SSD1289_wr_cmd(0x4B);
		SSD1289_wr_dat(0x0000);
		SSD1289_wr_cmd(0x44);
		SSD1289_wr_dat(0xEF00);
		SSD1289_wr_cmd(0x45);
		SSD1289_wr_dat(0x0000);

		SSD1289_wr_cmd(0x46);
		SSD1289_wr_dat(0x013F);
		SSD1289_wr_cmd(0x30);
		SSD1289_wr_dat(0x0707);
		SSD1289_wr_cmd(0x31);
		SSD1289_wr_dat(0x0204);
		SSD1289_wr_cmd(0x32);
		SSD1289_wr_dat(0x0204);
		SSD1289_wr_cmd(0x33);
		SSD1289_wr_dat(0x0502);

		SSD1289_wr_cmd(0x34);
		SSD1289_wr_dat(0x0507);
		SSD1289_wr_cmd(0x35);
		SSD1289_wr_dat(0x0204);
		SSD1289_wr_cmd(0x36);
		SSD1289_wr_dat(0x0204);
		SSD1289_wr_cmd(0x37);
		SSD1289_wr_dat(0x0502);
		SSD1289_wr_cmd(0x3A);
		SSD1289_wr_dat(0x0302);

		SSD1289_wr_cmd(0x3B);
		SSD1289_wr_dat(0x0302);
		SSD1289_wr_cmd(0x23);
		SSD1289_wr_dat(0x0000);
		SSD1289_wr_cmd(0x24);
		SSD1289_wr_dat(0x0000);
		SSD1289_wr_cmd(0x4F);
		SSD1289_wr_dat(0x8000);
		SSD1289_wr_cmd(0x4E);
		SSD1289_wr_dat(0x0000);

		SSD1289_wr_cmd(0x34);
		SSD1289_wr_dat(0xAA55);
	}

	else { for(;;);} /* Invalid Device Code!! */

	SSD1289_clear();


#if 0	/* test code RED */
	volatile uint32_t n;

	SSD1289_rect(0,MAX_X-1,0,MAX_Y-1);
	n = (MAX_X) * (MAX_Y);

	do {
		SSD1289_wr_dat(COL_RED);
	} while (--n);

	sil_dly_nse(30000000); // 30ms
	//	_delay_ms(500);
	for(;;);
#endif

}


/**************************************************************************/
/*! 
    Draw Windows 24bitBMP File.
*/
/**************************************************************************/
int SSD1289_draw_bmp(const uint8_t* ptr){

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
	SSD1289_clear();

    /* Limit to MAX_Y */
	m = (bh <= MAX_Y) ? biofs : biofs + (bh - MAX_Y) * iw;

	/* Set First data offset */  
	i = m % 512;

	/* Set BMP's Bottom-Left point */
	m = ye;

    /* Limit MAX_X */
	w = (bw > MAX_X) ? MAX_X : bw;

	do {
		SSD1289_rect(xs, xe, m, m);
		n = 0;
		p = (uint8_t*)ptr + i;
		do {
			n++;
			/* 65k colour access */
			d = *p++ >> 3;
			d |= (*p++ >> 2) << 5;
			d |= (*p++ >> 3) << 11;
			SSD1289_wr_dat(d);

		} while (n < w);
		i += iw;

	} while (m-- > ys);

	return 1;
}

#if 0 /* MY code */
		SSD1289_wr_cmd(0x07);
		SSD1289_wr_dat(0x0021);
		SSD1289_wr_cmd(0x00);
		SSD1289_wr_dat(0x0001);
		SSD1289_wr_cmd(0x07);
		SSD1289_wr_dat(0x0023);
		SSD1289_wr_cmd(0x10);
		SSD1289_wr_dat(0x0000);
		sil_dly_nse(30000000);

		SSD1289_wr_cmd(0x07);
		SSD1289_wr_dat(0x0033);
		SSD1289_wr_cmd(0x11);
		SSD1289_wr_dat(0x6070);
		SSD1289_wr_cmd(0x02);
		SSD1289_wr_dat(0x0500);
		SSD1289_wr_cmd(0x03);
		SSD1289_wr_dat(0x0804);
		SSD1289_wr_cmd(0x0C);
		SSD1289_wr_dat(0x0000);
		SSD1289_wr_cmd(0x0D);
		SSD1289_wr_dat(0x0808);

		SSD1289_wr_cmd(0x0E);
		SSD1289_wr_dat(0x2900);
		SSD1289_wr_cmd(0x1E);
		SSD1289_wr_dat(0x00B8);
		SSD1289_wr_cmd(0x01);
		SSD1289_wr_dat(0x2B3F);
		SSD1289_wr_cmd(0x10);
		SSD1289_wr_dat(0x0000);

		SSD1289_wr_cmd(0x05);
		SSD1289_wr_dat(0x0000);
		SSD1289_wr_cmd(0x06);
		SSD1289_wr_dat(0x0000);
		SSD1289_wr_cmd(0x16);
		SSD1289_wr_dat(0xEF1C);
		SSD1289_wr_cmd(0x17);
		SSD1289_wr_dat(0x0003);
		SSD1289_wr_cmd(0x07);
		SSD1289_wr_dat(0x0233);

		SSD1289_wr_cmd(0x0B);
		SSD1289_wr_dat(0x0000|(3<<6));
		SSD1289_wr_cmd(0x0F);
		SSD1289_wr_dat(0x0000);
		SSD1289_wr_cmd(0x41);
		SSD1289_wr_dat(0x0000);
		SSD1289_wr_cmd(0x42);
		SSD1289_wr_dat(0x0000);
		SSD1289_wr_cmd(0x48);
		SSD1289_wr_dat(0x0000);

		SSD1289_wr_cmd(0x49);
		SSD1289_wr_dat(0x013F);
		SSD1289_wr_cmd(0x4A);
		SSD1289_wr_dat(0x0000);
		SSD1289_wr_cmd(0x4B);
		SSD1289_wr_dat(0x0000);
		SSD1289_wr_cmd(0x44);
		SSD1289_wr_dat(0xEF00);
		SSD1289_wr_cmd(0x45);
		SSD1289_wr_dat(0x0000);

		SSD1289_wr_cmd(0x46);
		SSD1289_wr_dat(0x013F);
		SSD1289_wr_cmd(0x30);
		SSD1289_wr_dat(0x0707);
		SSD1289_wr_cmd(0x31);
		SSD1289_wr_dat(0x0204);
		SSD1289_wr_cmd(0x32);
		SSD1289_wr_dat(0x0204);
		SSD1289_wr_cmd(0x33);
		SSD1289_wr_dat(0x0502);

		SSD1289_wr_cmd(0x34);
		SSD1289_wr_dat(0x0507);
		SSD1289_wr_cmd(0x35);
		SSD1289_wr_dat(0x0204);
		SSD1289_wr_cmd(0x36);
		SSD1289_wr_dat(0x0204);
		SSD1289_wr_cmd(0x37);
		SSD1289_wr_dat(0x0502);
		SSD1289_wr_cmd(0x3A);
		SSD1289_wr_dat(0x0302);

		SSD1289_wr_cmd(0x3B);
		SSD1289_wr_dat(0x0302);
		SSD1289_wr_cmd(0x23);
		SSD1289_wr_dat(0x0000);
		SSD1289_wr_cmd(0x24);
		SSD1289_wr_dat(0x0000);
		SSD1289_wr_cmd(0x4F);
		SSD1289_wr_dat(0x8000);
		SSD1289_wr_cmd(0x4E);
		SSD1289_wr_dat(0x0000);

		SSD1289_wr_cmd(0x34);
		SSD1289_wr_dat(0xAA55);
#endif


#if 0 /* orginal code */
		SSD1289_wr_cmd(0x00);
		SSD1289_wr_dat(0x0001);   /* oscillator on */
		sil_dly_nse(10000000);
		//		_delay_ms(10); 

		SSD1289_wr_cmd(0x03);
		SSD1289_wr_dat(0xaeae);  /* //0xA2A0//0xeeee */
		SSD1289_wr_cmd(0x0c);
		SSD1289_wr_dat(0x0000);  /* 0x0002 */
		SSD1289_wr_cmd(0x0d);
		SSD1289_wr_dat(0x000F);   
		SSD1289_wr_cmd(0x0e);
		SSD1289_wr_dat(0x3200);  /* 0x3600 */
		sil_dly_nse(10000000);
		//		_delay_ms(10); 

		SSD1289_wr_cmd(0x1e);
		SSD1289_wr_dat(0x00Ad);  /* 0x00A5 */
		SSD1289_wr_cmd(0x01);
		SSD1289_wr_dat(0x2B3F); 
		SSD1289_wr_cmd(0x02);
		SSD1289_wr_dat(0x0600); 
		SSD1289_wr_cmd(0x10);
		SSD1289_wr_dat(0x0000); 
		SSD1289_wr_cmd(0x11);
		SSD1289_wr_dat(0x6830);

		/* add 20070705 */
		SSD1289_wr_cmd(0x05);
		SSD1289_wr_dat(0x0000);      
		SSD1289_wr_cmd(0x06);
		SSD1289_wr_dat(0x0000); 
		SSD1289_wr_cmd(0x07);
		SSD1289_wr_dat(0x0233); 
		/* add 20070705 */

		SSD1289_wr_cmd(0x0b);
		SSD1289_wr_dat(0x0030);  /* 0x5300 */
		SSD1289_wr_cmd(0x0f);
		SSD1289_wr_dat(0x0000); 
		sil_dly_nse(100000000);
		//		_delay_ms(100);
	
		/* add 20070705 */
		SSD1289_wr_cmd(0x41);
		SSD1289_wr_dat(0x0000); 
		SSD1289_wr_cmd(0x42);
		SSD1289_wr_dat(0x0000); 
		SSD1289_wr_cmd(0x44);
		SSD1289_wr_dat(0xef00); 
		SSD1289_wr_cmd(0x45);
		SSD1289_wr_dat(0x0000); 
		SSD1289_wr_cmd(0x46);
		SSD1289_wr_dat(0x013f); 
		SSD1289_wr_cmd(0x48);
		SSD1289_wr_dat(0x0000); 
		SSD1289_wr_cmd(0x49);
		SSD1289_wr_dat(0x013f); 
		SSD1289_wr_cmd(0x4a);
		SSD1289_wr_dat(0x0000); 
		SSD1289_wr_cmd(0x4b);
		SSD1289_wr_dat(0x0000); 
		SSD1289_wr_cmd(0x4e);
		SSD1289_wr_dat(0x0000); 
		SSD1289_wr_cmd(0x4f);
		SSD1289_wr_dat(0x0000); 

		SSD1289_wr_cmd(0x34);
		SSD1289_wr_dat(COL_RED);

		/* optimize for video display */
		SSD1289_wr_cmd(0x28);
		SSD1289_wr_dat(0x0006); 
		SSD1289_wr_cmd(0x2f);
		SSD1289_wr_dat(0x12be);	/*0x12ae */
		SSD1289_wr_cmd(0x12);
		SSD1289_wr_dat(0x6ceb);	/*optimize for video */
		sil_dly_nse(10000000);
		//		_delay_ms(10); 
		SSD1289_wr_cmd(0x25);
		SSD1289_wr_dat(0xe000);	 
		/* add 20070705 */

		SSD1289_wr_cmd(0x30);
		SSD1289_wr_dat(0x0707);
		SSD1289_wr_cmd(0x31);
		SSD1289_wr_dat(0x0204);
		SSD1289_wr_cmd(0x32);
		SSD1289_wr_dat(0x0204);
		SSD1289_wr_cmd(0x33);
		SSD1289_wr_dat(0x0502);
		SSD1289_wr_cmd(0x34);
		SSD1289_wr_dat(0x0507);
		SSD1289_wr_cmd(0x35);
		SSD1289_wr_dat(0x0204);
		SSD1289_wr_cmd(0x36);
		SSD1289_wr_dat(0x0204);
		SSD1289_wr_cmd(0x37);
		SSD1289_wr_dat(0x0502);
		SSD1289_wr_cmd(0x3A);
		SSD1289_wr_dat(0x0302);
		SSD1289_wr_cmd(0x3B);
		SSD1289_wr_dat(0x0504);
		sil_dly_nse(20000000);
		//		_delay_ms(20);
#endif



/* End Of File ---------------------------------------------------------------*/
