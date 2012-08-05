/********************************************************************************/
/*!
	@file			font_if.h
	@author         Nemui Trinomius (http://nemuisan.blog.bai.ne.jp)
    @version        3.01
    @date           2011.09.17
	@brief          Interface of FONTX Driver								@n
                    Refered under URL thanks!								@n
					http://www.hmsoft.co.jp/lepton/software/dosv/fontx.htm	@n
					http://hp.vector.co.jp/authors/VA007110/technicalworkshop

    @section HISTORY
		2010.12.31	V1.00	Stable Release.
		2011.03.10	V2.00	C++ Ready.
		2011.05.11	V3.00	Selectable KanjiFonts to Reduce Memory Space.
		2011.09.17	V3.01	Fixed handling Selectable KanjiFonts.
		
    @section LICENSE
		BSD License. See Copyright.txt
*/
/********************************************************************************/
#ifndef __FONT_IF_H 
#define __FONT_IF_H 0x0301

#ifdef __cplusplus
 extern "C" {
#endif

/* Include Basis */
/* AAAAA: Masato INOUE */
#if defined TOPPERS_T_STDDEF_H
#include "t_stddef.h"
#else
#include <inttypes.h>
#endif

#include "font_if_datatable.h"

/* Device Dependent Macros */

/* To Read uC Flash Memory Little Endian */ 
#define	READ_ADDR_UNIT8(ADDR) 	((uint8_t)*(uint32_t*)(addr_ofs+ADDR))
#define	READ_ADDR_UNIT16(ADDR) 	((uint16_t)*(uint32_t*)(addr_ofs+ADDR))


/**************************************************************************/
/*! 
    FONTX2 Alphabet Numeric Kana Definitions !!
*/
/**************************************************************************/
/* FONTX2 Structure offset of ANK */
#define	ANK_HEADER		(0)			/*    +0(6 byte) Includes "FONTX2" String */
#define ANK_FONTNAME	(6)			/*    +6(8 byte) Name of FONTX2			  */
#define ANK_XSIZE		(14)		/*   +14(1 byte) Xsize(in pixel)		  */
#define ANK_YSIZE		(15)		/*   +15(1 byte) Ysize(in pixel)		  */
#define ANK_CODETYPE	(16)		/* 	 +16(1 byte) Codetype(usually 0x01)   */
#define ANK_DATSTART	(17)		/* 	 +17(- byte) Ank Fontdata start		  */

/* Structs */
typedef struct {
	const char* AnkFileOffset;		/* Ank FONTX2 File Address Offset 	*/
	uint8_t  AnkSize;				/* Size of Ank Font (in byte) 		*/
	uint8_t  X_Size;				/* Xsize(in pixel) 					*/
	uint8_t  Y_Size;				/* Ysize(in pixel)					*/
} FontX_Ank;

/* Externals */
extern void InitFont_Ank(FontX_Ank* AnkDat,const char* addr_ofs);
extern uint8_t* GetPtr_Ank(uint8_t AnkCode);
extern void ChangeCurrentAnk(FontX_Ank* AnkDat);
extern FontX_Ank* CurrentAnkDat;


/**************************************************************************/
/*! 
    FONTX2 Kanji(2 Byte Charactor) Definitions !!
*/
/**************************************************************************/
/* FONTX2 Structure offset of KANJI(Sift-JIS) */
#define	KANJI_HEADER	(0)			/*    +0(6 byte) Includes "FONTX2" String */
#define KANJI_FONTNAME	(6)			/*    +6(8 byte) Name of FONTX2			  */
#define KANJI_XSIZE		(14)		/*   +14(1 byte) Xsize(in pixel)		  */
#define KANJI_YSIZE		(15)		/*   +15(1 byte) Ysize(in pixel)		  */
#define KANJI_CODETYPE	(16)		/* 	 +16(1 byte) Codetype(usually 0x01)   */
#define KANJI_TABLENUM	(17)		/* 	 +17(1 byte) Number of Table offset	  */
#define KANJI_DATSTART	(18)		/* 	 +18(- byte) Kanji Data offset   	  */

/* Kanji DataBlock Macros */
#define KANJI_START(n)   READ_ADDR_UNIT16( (KANJI_DATSTART+ 4*(n)) 	 )
#define KANJI_ENDE(n)    READ_ADDR_UNIT16( (KANJI_DATSTART+ 4*(n) +2) )
/* Kanji Font Only */
#define	READ_ADDR_UNIT16_C(ADDR) 	((uint16_t)*(uint32_t*)(CurrentKanjiDat->KanjiFileOffset+ADDR))
#define KANJI_START_C(n) READ_ADDR_UNIT16_C( (KANJI_DATSTART+ 4*(n)) 	 )
#define KANJI_ENDE_C(n)  READ_ADDR_UNIT16_C( (KANJI_DATSTART+ 4*(n) +2) )

#define SJIS_TOOFU		(0x81A0)	/*  Toofu ga starto suru			      */ 

/* Structs */
typedef struct {
	const char* KanjiFileOffset;	/* Kanji FONTX2 File Address Offset */
	const char* KanjiStartOffset;	/* Kanji FONTX2 Data Address Offset */
	uint8_t  KanjiSize;				/* Size of Kanji Font (in byte) 	*/
	uint8_t  X_Size;				/* Xsize(in pixel) 					*/
	uint8_t  Y_Size;				/* Ysize(in pixel)					*/
	uint8_t  KanjiTableNum;			/* Kanji Font Table	 Number  		*/ 
	uint16_t KanjiSearchTable[256]; /* Kanji Font Search Table	   		*/ 
} FontX_Kanji;

/* Externals */
extern void InitFont_Kanji(FontX_Kanji* KanjiDat,const char* addr_ofs);
extern uint8_t* GetPtr_Kanji(uint16_t SjisCode);
extern void ChangeCurrentKanji(FontX_Kanji* KanjiDat);
extern FontX_Kanji* CurrentKanjiDat;

/* See "font_if_datatable.h" !*/
extern FontX_Ank   ANKFONT;
extern FontX_Kanji KANJIFONT;

#ifdef __cplusplus
}
#endif

#endif /* __FONT_IF */
