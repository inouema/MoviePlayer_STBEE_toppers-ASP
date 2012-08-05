/********************************************************************************/
/*!
	@file			hx8352a.c
    @version        6.00
    @date           2011.12.23
	@brief          Based on Chan's MCI_OLED@LPC23xx-demo thanks!				@n
					It can drive QD-ET3207ABT TFT module(HX8352A)	16bit mode.

    @section HISTORY
		2010.07.10	V1.00	Stable Release.
		2010.10.01	V2.00	Changed CTRL-Port Contol Procedure.
		2010.12.31  V3.00   Cleanup SourceCode.
		2011.03.10	V4.00	C++ Ready.
		2011.10.25	V5.00	Added DMA TransactionSupport.
		2011.12.23	V6.00	Optimize Some Codes.

    @section LICENSE
		BSD License. See Copyright.txt
*/
/********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "hx8352a.h"
/* check header file version for fool proof */
#if __HX8352A_H != 0x0600
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
inline void HX8352A_reset(void)
{
	HX8352A_RES_SET();							/* RES=H, RD=H, WR=H   		*/
	HX8352A_RD_SET();
	HX8352A_WR_SET();
	_delay_ms(40);								/* wait 40ms     			*/

	HX8352A_RES_CLR();							/* RES=L, CS=L   			*/
	HX8352A_CS_CLR();
	_delay_ms(60);								/* wait 60ms     			*/
	
	HX8352A_RES_SET();						  	/* RES=H					*/
	_delay_ms(60);				    			/* wait 60ms     			*/
}

/**************************************************************************/
/*! 
    Write LCD Command.
*/
/**************************************************************************/
inline void HX8352A_wr_cmd(uint8_t cmd)
{
	HX8352A_DC_CLR();							/* DC=L						*/

	HX8352A_CMD = cmd;							/* cmd						*/
	HX8352A_WR();								/* WR=L->H					*/

	HX8352A_DC_SET();							/* DC=H						*/
}

/**************************************************************************/
/*! 
    Write LCD Data.
*/
/**************************************************************************/
inline void HX8352A_wr_dat(uint8_t dat)
{
	HX8352A_DATA = dat;							/* data						*/
	HX8352A_WR();								/* WR=L->H					*/
}

/**************************************************************************/
/*! 
    Write LCD GRAM.
*/
/**************************************************************************/
inline void HX8352A_wr_gram(uint16_t gram)
{
#if defined(GPIO_ACCESS_8BIT) | defined(BUS_ACCESS_8BIT)
	HX8352A_DATA = (uint8_t)(gram>>8);			/* upper 8bit data			*/
	HX8352A_WR();								/* WR=L->H					*/
	HX8352A_DATA = (uint8_t)gram;				/* lower 8bit data			*/
#else
	HX8352A_DATA = gram;						/* 16bit data 				*/
#endif
	HX8352A_WR();								/* WR=L->H					*/
}

/**************************************************************************/
/*! 
    Write LCD Block Data.
*/
/**************************************************************************/
inline void HX8352A_wr_block(uint8_t *p, unsigned int cnt)
{

#ifdef  USE_DISPLAY_DMA_TRANSFER
   DMA_TRANSACTION(p, cnt);
#else

	cnt /= 4;
	
	while (cnt--) {
		/* avoid -Wsequence-point's warning */
		HX8352A_wr_gram(*(p+1)|*(p)<<8);
		p++;p++;
		HX8352A_wr_gram(*(p+1)|*(p)<<8);
		p++;p++;
	}
#endif

}

/**************************************************************************/
/*! 
    Set Rectangle.
*/
/**************************************************************************/
inline void HX8352A_rect(uint32_t x, uint32_t width, uint32_t y, uint32_t height)
{

	HX8352A_wr_cmd(0x0002);				/* Horizontal RAM Start ADDR2 */
	HX8352A_wr_dat((OFS_COL + x)>>8);
	HX8352A_wr_cmd(0x0003);				/* Horizontal RAM Start ADDR1 */
	HX8352A_wr_dat(OFS_COL + x);
	HX8352A_wr_cmd(0x0004);				/* Horizontal RAM End ADDR2 */
	HX8352A_wr_dat((OFS_COL + width)>>8);
	HX8352A_wr_cmd(0x0005);				/* Horizontal RAM End ADDR1 */
	HX8352A_wr_dat(OFS_COL + width);
	HX8352A_wr_cmd(0x0006);				/* Vertical RAM Start ADDR2 */
	HX8352A_wr_dat((OFS_RAW + y)>>8);
	HX8352A_wr_cmd(0x0007);				/* Vertical RAM Start ADDR1 */
	HX8352A_wr_dat(OFS_RAW + y);
	HX8352A_wr_cmd(0x0008);				/* Vertical RAM End ADDR2 */
	HX8352A_wr_dat((OFS_RAW + height)>>8);
	HX8352A_wr_cmd(0x0009);				/* Vertical RAM End ADDR1 */
	HX8352A_wr_dat(OFS_RAW + height);

	HX8352A_wr_cmd(0x0022);				/* Write Data to GRAM */

}

/**************************************************************************/
/*! 
    Clear Display.
*/
/**************************************************************************/
inline void HX8352A_clear(void)
{
	volatile uint32_t n;

	HX8352A_rect(0,MAX_X-1,0,MAX_Y-1);
	n = (uint32_t)(MAX_X) * (MAX_Y);

	do {
		HX8352A_wr_dat(COL_BLACK);
	} while (--n);

}


/**************************************************************************/
/*! 
    Read LCD Register.
*/
/**************************************************************************/
inline uint16_t HX8352A_rd_cmd(uint16_t cmd)
{
	uint16_t val;

	HX8352A_wr_cmd(cmd);

	HX8352A_WR_SET();

    ReadLCDData(val);

	val &= 0x00FF;
	return val;
}


/**************************************************************************/
/*! 
    TFT-LCD Module Initialize.
*/
/**************************************************************************/
void HX8352A_init(void)
{
	uint16_t devicetype;
	
	Display_IoInit_If();

	HX8352A_reset();
	
	/* Check Device Code */
	devicetype = HX8352A_rd_cmd(0x0000);  			/* Confirm Vaild LCD Controller */

	if(devicetype == 0x0052)
	{
		/* Initialize HX8352A */
		HX8352A_wr_cmd(0x0083);
		HX8352A_wr_dat(0x0002); 
		HX8352A_wr_cmd(0x0085);
		HX8352A_wr_dat(0x0003); 
		HX8352A_wr_cmd(0x008B);
		HX8352A_wr_dat(0x0000); 
		HX8352A_wr_cmd(0x008C);
		HX8352A_wr_dat(0x0013); 
		HX8352A_wr_cmd(0x0091);
		HX8352A_wr_dat(0x0001); 
		HX8352A_wr_cmd(0x0083);
		HX8352A_wr_dat(0x0000); 
		_delay_ms(5);

		/* ---------------- Gamma Setting  ---------------------------- */
		HX8352A_wr_cmd(0x003E);
		HX8352A_wr_dat(0x00C4);
		HX8352A_wr_cmd(0x003F);
		HX8352A_wr_dat(0x0044);
		HX8352A_wr_cmd(0x0040);
		HX8352A_wr_dat(0x0022);
		HX8352A_wr_cmd(0x0041);
		HX8352A_wr_dat(0x0057);
		HX8352A_wr_cmd(0x0042);
		HX8352A_wr_dat(0x0003);
		HX8352A_wr_cmd(0x0043);
		HX8352A_wr_dat(0x0047);
		HX8352A_wr_cmd(0x0044);
		HX8352A_wr_dat(0x0002);
		HX8352A_wr_cmd(0x0045);
		HX8352A_wr_dat(0x0055);
		HX8352A_wr_cmd(0x0046);
		HX8352A_wr_dat(0x0006);
		HX8352A_wr_cmd(0x0047);
		HX8352A_wr_dat(0x004C);
		HX8352A_wr_cmd(0x0048);
		HX8352A_wr_dat(0x0006);
		HX8352A_wr_cmd(0x0049);
		HX8352A_wr_dat(0x008C); 

		/* ----------  Power Supply Setting  -------------------------- */   
		HX8352A_wr_cmd(0x0017);
		HX8352A_wr_dat(0x0091); 
		HX8352A_wr_cmd(0x0023);
		HX8352A_wr_dat(0x0001);
		HX8352A_wr_cmd(0x002B);
		HX8352A_wr_dat(0x00F9); 
		_delay_ms(20);       
		HX8352A_wr_cmd(0x0018);
		HX8352A_wr_dat(0x003A); 

		HX8352A_wr_cmd(0x001B);
		HX8352A_wr_dat(0x0011); 
		HX8352A_wr_cmd(0x0083);
		HX8352A_wr_dat(0x0002);
		HX8352A_wr_cmd(0x008A);
		HX8352A_wr_dat(0x0006);
		HX8352A_wr_cmd(0x0083);
		HX8352A_wr_dat(0x0000);
		HX8352A_wr_cmd(0x001A);
		HX8352A_wr_dat(0x0011); 

		HX8352A_wr_cmd(0x001C);
		HX8352A_wr_dat(0x000A); 
		HX8352A_wr_cmd(0x001F);
		HX8352A_wr_dat(0x0058); 
		_delay_ms(30);
		HX8352A_wr_cmd(0x0019);
		HX8352A_wr_dat(0x000A);  
		HX8352A_wr_cmd(0x0019);
		HX8352A_wr_dat(0x001A); 
		_delay_ms(50);
		HX8352A_wr_cmd(0x0019);
		HX8352A_wr_dat(0x0012); 
		_delay_ms(50);
		HX8352A_wr_cmd(0x001E);
		HX8352A_wr_dat(0x002E); 
		_delay_ms(100);

		/* ----------------  DUG FUNCTION ON ---------------------------- */
		HX8352A_wr_cmd(0x005A);
		HX8352A_wr_dat(0x0001);

		/* ------------------- RGB LUT ---------------------------------- */ 
		HX8352A_wr_cmd(0x005C);
		for(int i=0;i<3;i++)
		{
			HX8352A_wr_dat(0x0000);/* -----------0 */
			HX8352A_wr_dat(0x0003);
			HX8352A_wr_dat(0x000A);
			HX8352A_wr_dat(0x000F);
			HX8352A_wr_dat(0x0013);
			HX8352A_wr_dat(0x0016);/* -----------5 */
			HX8352A_wr_dat(0x0019);
			HX8352A_wr_dat(0x001C);
			HX8352A_wr_dat(0x001E);
			HX8352A_wr_dat(0x001F);
			HX8352A_wr_dat(0x0025);/* -----------10 */
			HX8352A_wr_dat(0x002A);
			HX8352A_wr_dat(0x0030);
			HX8352A_wr_dat(0x0035);
			HX8352A_wr_dat(0x0039);
			HX8352A_wr_dat(0x003D);/* -----------15 */
			HX8352A_wr_dat(0x0041);
			HX8352A_wr_dat(0x0045);
			HX8352A_wr_dat(0x0048);
			HX8352A_wr_dat(0x004C);
			HX8352A_wr_dat(0x004F);/* -----------20 */
			HX8352A_wr_dat(0x0053);
			HX8352A_wr_dat(0x0058);
			HX8352A_wr_dat(0x005D);
			HX8352A_wr_dat(0x0061);
			HX8352A_wr_dat(0x0066);/* -----------25 */
			HX8352A_wr_dat(0x006A);
			HX8352A_wr_dat(0x006E);
			HX8352A_wr_dat(0x0072);
			HX8352A_wr_dat(0x0076);
			HX8352A_wr_dat(0x007A);/* -----------30 */
			HX8352A_wr_dat(0x007E);
			HX8352A_wr_dat(0x0082);
			HX8352A_wr_dat(0x0085);
			HX8352A_wr_dat(0x0089);
			HX8352A_wr_dat(0x008D);/* -----------35 */
			HX8352A_wr_dat(0x0090);
			HX8352A_wr_dat(0x0094);
			HX8352A_wr_dat(0x0097);
			HX8352A_wr_dat(0x009A);
			HX8352A_wr_dat(0x009D);/* -----------40 */
			HX8352A_wr_dat(0x00A2);
			HX8352A_wr_dat(0x00A5);
			HX8352A_wr_dat(0x00A9);
			HX8352A_wr_dat(0x00AC);
			HX8352A_wr_dat(0x00B0);/* -----------45 */
			HX8352A_wr_dat(0x00B4);
			HX8352A_wr_dat(0x00B8);
			HX8352A_wr_dat(0x00BC);
			HX8352A_wr_dat(0x00C0);
			HX8352A_wr_dat(0x00C3);/* -----------50 */
			HX8352A_wr_dat(0x00C8);
			HX8352A_wr_dat(0x00CC);
			HX8352A_wr_dat(0x00D2);
			HX8352A_wr_dat(0x00D6);
			HX8352A_wr_dat(0x00DC);/* -----------55 */	
			HX8352A_wr_dat(0x00DF);
			HX8352A_wr_dat(0x00E2);
			HX8352A_wr_dat(0x00E5);
			HX8352A_wr_dat(0x00E8);
			HX8352A_wr_dat(0x00EC);/* -----------60 */
			HX8352A_wr_dat(0x00EF);
			HX8352A_wr_dat(0x00F4);
			HX8352A_wr_dat(0x00FF);/* -----------63 */
		}

		/* -------------  Display ON Setting  -------------------------- */
		HX8352A_wr_cmd(0x003C);
		HX8352A_wr_dat(0x00C0); 
		HX8352A_wr_cmd(0x003D);
		HX8352A_wr_dat(0x001C); 
		HX8352A_wr_cmd(0x0034);
		HX8352A_wr_dat(0x0038);
		HX8352A_wr_cmd(0x0035);
		HX8352A_wr_dat(0x0038);
		HX8352A_wr_cmd(0x0024);
		HX8352A_wr_dat(0x0038);
		_delay_ms(50);
		HX8352A_wr_cmd(0x0024);
		HX8352A_wr_dat(0x003C);
		HX8352A_wr_cmd(0x0016);
		HX8352A_wr_dat((1<<4)|(1<<3)|(1<<2)); 

		HX8352A_wr_cmd(0x0001);
		HX8352A_wr_dat(0x0006); 
		HX8352A_wr_cmd(0x0055);
		HX8352A_wr_dat(0x0000);
		
		HX8352A_wr_cmd(0x0002);
		HX8352A_wr_dat(0x0000); 
		HX8352A_wr_cmd(0x0003);
		HX8352A_wr_dat(0x0000);
		HX8352A_wr_cmd(0x0004);
		HX8352A_wr_dat(((MAX_X)>>8));
		HX8352A_wr_cmd(0x0005);
		HX8352A_wr_dat((MAX_X & 0xFF));
		
		HX8352A_wr_cmd(0x0006);
		HX8352A_wr_dat(0x0000); 
		HX8352A_wr_cmd(0x0007);
		HX8352A_wr_dat(0x0000);
		HX8352A_wr_cmd(0x0008);
		HX8352A_wr_dat(((MAX_Y)>>8));
		HX8352A_wr_cmd(0x0009);
		HX8352A_wr_dat((MAX_Y&0xFF));

	}

	else { for(;;);} /* Invalid Device Code!! */

	HX8352A_clear();

#if 0	/* test code RED */
	volatile uint32_t n;

	HX8352A_rect(0,MAX_X-1,0,MAX_Y-1);
	n = (uint32_t)(MAX_X) * (MAX_Y);

	do {
		HX8352A_wr_dat(COL_RED);
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
int HX8352A_draw_bmp(const uint8_t* ptr){

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
	HX8352A_clear();

    /* Limit to MAX_Y */
	m = (bh <= MAX_Y) ? biofs : biofs + (bh - MAX_Y) * iw;

	/* Set First data offset */  
	i = m % 512;

	/* Set BMP's Bottom-Left point */
	m = ye;

    /* Limit MAX_X */
	w = (bw > MAX_X) ? MAX_X : bw;

	do {
		HX8352A_rect(xs, xe, m, m);
		n = 0;
		p = (uint8_t*)ptr + i;
		do {
			n++;
			/* 65k colour access */
			d = *p++ >> 3;
			d |= (*p++ >> 2) << 5;
			d |= (*p++ >> 3) << 11;
			HX8352A_wr_gram(d);

		} while (n < w);
		i += iw;

	} while (m-- > ys);

	return 1;
}

/* End Of File ---------------------------------------------------------------*/
