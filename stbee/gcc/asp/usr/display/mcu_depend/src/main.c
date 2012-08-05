/*
 *	STBee LED点灯サンプル
 *	2010/4 Strawberry Linux Co.,Ltd.
 *
 *	割り込みも何も使わないLED点灯サンプルです。
 *
 *
 *
 *
 */



// I/Oなどの全ての#defineがあります。
#include "stm32f10x_conf.h"


#define LCD_BASE ((uint32_t)(0x6001fffe))
#define LCD ((LCD_TypeDef *) LCD_BASE)
typedef struct
{
	__IO uint16_t LCD_REG;
	__IO uint16_t LCD_RAM;
} LCD_TypeDef;


#define write_reg(a,b) {	\
	LCD->LCD_REG = a;	\
	LCD->LCD_RAM = b;	\
	}



void nmi_handler(void);
void hardfault_handler(void);




// 空ループでウェイトするルーチン
void Delay(volatile unsigned long delay)
{ 
	while(delay) delay--;
}


/*************************************************************************
 * Function Name: main
 * Parameters: none
 * Return: Int32U
 *
 * Description: The main subroutine
 *
 *************************************************************************/
int main(void)
{
	unsigned short deviceid=0x0000;

	
	// システムを初期化します（外部水晶をONにし72MHz動作に切り替えます）
	SystemInit();

                        

	/* FSMC Configuration */

	p.FSMC_AddressSetupTime = 30;
	p.FSMC_AddressHoldTime  = 0;
	p.FSMC_DataSetupTime    = 30;
	p.FSMC_BusTurnAroundDuration = 0x00;
	p.FSMC_CLKDivision = 0x00;
	p.FSMC_DataLatency = FSMC_AccessMode_A;

	FSMC_NORSRAMInitStructure.FSMC_Bank               = FSMC_Bank1_NORSRAM2;
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
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

	/* Enable FSMC Bank1_NOR Bank */
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM2, ENABLE);

	LCD->LCD_REG = 0x0000;
	deviceid = LCD->LCD_RAM;
	if (deviceid != 0x8989) {
		goto error;
	}




	/*
	 * LCD Initialize
	 */
	/* power on */
	LCD->LCD_REG = 0x0007;
	LCD->LCD_RAM = 0x0021;

	LCD->LCD_REG = 0x0000;
	LCD->LCD_RAM = 0x0001;

	LCD->LCD_REG = 0x0007;
	LCD->LCD_RAM = 0x0023;

	LCD->LCD_REG = 0x0010;
	LCD->LCD_RAM = 0x0000;

	Delay(3000000);

	LCD->LCD_REG = 0x0007;
	LCD->LCD_RAM = 0x0033;

	LCD->LCD_REG = 0x0011;
	LCD->LCD_RAM = 0x6070;

	LCD->LCD_REG = 0x0002;
	LCD->LCD_RAM = 0x0500;

	LCD->LCD_REG = 0x0003;
	LCD->LCD_RAM = 0x0804;

	LCD->LCD_REG = 0x000C;
	LCD->LCD_RAM = 0x0000;

	LCD->LCD_REG = 0x000D;
	LCD->LCD_RAM = 0x0808;

  write_reg(0x000E,0x2900);
    write_reg(0x001E,0x00B8);
    write_reg(0x0001,0x2B3F);//Çý¶¯Êä³ö¿ØÖÆ320*240  0x6B3F
    write_reg(0x0010,0x0000);

    write_reg(0x0005,0x0000);
    write_reg(0x0006,0x0000);
    write_reg(0x0016,0xEF1C);
    write_reg(0x0017,0x0003);
    write_reg(0x0007,0x0233);//0x0233

    write_reg(0x000B,0x0000|(3<<6));
    write_reg(0x000F,0x0000);//É¨Ãè¿ªÊ¼µØÖ·
    write_reg(0x0041,0x0000);
    write_reg(0x0042,0x0000);
    write_reg(0x0048,0x0000);

    write_reg(0x0049,0x013F);
    write_reg(0x004A,0x0000);
    write_reg(0x004B,0x0000);
    write_reg(0x0044,0xEF00);
    write_reg(0x0045,0x0000);

    write_reg(0x0046,0x013F);
    write_reg(0x0030,0x0707);
    write_reg(0x0031,0x0204);
    write_reg(0x0032,0x0204);
    write_reg(0x0033,0x0502);

    write_reg(0x0034,0x0507);
    write_reg(0x0035,0x0204);
    write_reg(0x0036,0x0204);
    write_reg(0x0037,0x0502);
    write_reg(0x003A,0x0302);

    write_reg(0x003B,0x0302);
    write_reg(0x0023,0x0000);
    write_reg(0x0024,0x0000);
    write_reg(0x0025,0x8000);   // 65hz
    write_reg(0x004f,0);        // ÐÐÊ×Ö·0
    write_reg(0x004e,0);        // ÁÐÊ×Ö·0


	/* set cursor */
	LCD->LCD_REG = 0x004e;
	LCD->LCD_RAM = 0x0000;
	
	LCD->LCD_REG = 0x004f;
	LCD->LCD_RAM = 0x0000;

	LCD->LCD_REG = 0x0034;
	LCD->LCD_RAM = 0xAA55;


	while(1); /* end here */

error:
	// GPIO Dポートを有効にします
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	
	// PD.4ポートを出力にします。PD.4=赤LED Lowで点灯
	GPIO_InitStructure.GPIO_Pin =  _BV(4);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);	// 初期化関数を読み出します。
	
	
	// GPIOコントロール関数を使うならこのようになります。
	while(1){
		GPIO_ResetBits(GPIOD, _BV(4));
	    Delay(200000);
		GPIO_SetBits(GPIOD, _BV(4));
	    Delay(200000);
	}

	// 直接I/Oを操作するにはこのようにすればできます。
	// 上記GPIO_ResetBits, GPIO_SetBitsの関数内ではこれと同じことをやっています。
/**********************
	while(1){
	    GPIOD->BRR = _BV(4);	// ４ビット目をクリアします。
	    						// BRRレジスタを立っているビットをクリアします。
	    						// それ以外のビットには影響を与えません
	    Delay(200000);
		GPIOD->BSRR = _BV(4);	// ４ビット目をセットします。
								// BSRRレジスタ（下位１６ビット）は立っているビットをセットします。
	    						// それ以外のビットには影響を与えません
	    Delay(200000);
	}
***********************/
}

