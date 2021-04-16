/*
 * LCD1602.h
 *
 *  Created on: 2021年4月5日
 *      Author: Lee
 */

#ifndef LCD1602_H_
#define LCD1602_H_

#include "I2C.h"


/**************************************************************************************
 * LCD1602_I2C驱动程序
 * 作者：PurpleSword
 * 2014.8.7改进，改进说明：此程序应配模拟I2C程序使用，在一些细节上有所修改
 * 使用方式：
 * 程序初始化过程中，调用LCD_Init();
 * 要显示字符串时：  LCD_Display(0,0,lcd_disp)
 *                   注：第一个参数为行数，第二参数为列数，最后一个参数为要显示的字符串的首元素地址（直接用串名即可）
 *                  完成一次发送
 *************************************************************************************/
#define LCD_ClearLine0()    LCD_Display(0, 0, "                ")
#define LCD_ClearLine1()    LCD_Display(1, 0, "                ")

/*
**以下为本文件所有函数的声明，方便调用：
*/

void LCD_Init(void);
unsigned char LCD_GenPos(unsigned char line, unsigned char add);
void LCD_Blink(unsigned char line, unsigned char add);
void LCD_BlinkOff();
void LCD_Display(unsigned char line, unsigned char add, char *str);
void LCD_Write_CGRAM();
void LCD_Display_CGRAM(unsigned char line, unsigned char add, char font);
void LCD_MoveOn();
void LCD_ClearAll();

/****************************************************************
* 名    称：LCD_Init()
* 功    能：LCD初始化
* 入口参数：无
* 出口参数：无
****************************************************************/
void LCD_Init(void)
{
    I2C_Init();             // I2C初始化
        I2C_Start();            // 开始标志
    I2C_SetAdd(0x78);   // 选择 SLAVE ADDRESS

    I2C_SendByte(0x00); // 控制字节：Co=0,A0=0,
                            //表示以下传输的 N 个字节是指令
    I2C_SendByte(0x38); // Function set
    I2C_SendByte(0x0C); // Display ON/OFF
    I2C_SendByte(0x01); // Clear display
    I2C_SendByte(0x06); // Entry mode set
    I2C_Stop();     // 结束标志
    __delay_cycles((long)16000);
}


/****************************************************************
* 名    称：LCD_GenPos()
* 功    能：内部函数：由指定的行列计算对应的LCD显示地址
* 入口参数：行(0-1)，列(0-15)
* 出口参数：地址
****************************************************************/
unsigned char LCD_GenPos(unsigned char line, unsigned char add)
{
    return line ? (add + 0x80 + 0x40) : (add + 0x80);
}


/****************************************************************
* 名    称：LCD_Display()
* 功    能：LCD显示字符串
* 入口参数：行(0-1)，列(0-15)，字符串
* 出口参数：无
****************************************************************/
void LCD_Display(unsigned char line, unsigned char add, char *str)
{
    I2C_Start();
        I2C_SetAdd(0x78);       // 选择 SLAVE ADDRESS
    I2C_SendByte(0x80);         // 控制字节：Co=1,A0=0,表示以下传输的 1 个字节是指令
    I2C_SendByte(LCD_GenPos(line, add));    // 设置 DDRAM ADDRESS：第几行，第几列
    I2C_SendByte(0x40);         // 控制字节：Co=0,A0=1,表示以下传输的 n 个字节是数据
    while(*str && (add++ < 16) ) {
        I2C_SendByte(*str++);
    }
    I2C_Stop();
    __delay_cycles((long)100);
}
/****************************************************************
* 名    称：LCD_Display()
* 功    能：LCD显示字符串
* 入口参数：行(0-1)，列(0-15)，字符串
* 出口参数：无
****************************************************************/

unsigned char LCD_Table[8] = {0x1F, 0x0A, 0x1F, 0x1B, 0x1B, 0x11, 0x11, 0x1F};  //字符 西
unsigned char LCD_Table1[8] = {0x04, 0x1F, 0x15, 0x1F, 0x15, 0x1F, 0x04, 0x07}; //电
unsigned char LCD_Table2[8] = {0x04, 0x1F, 0x0E, 0x15, 0x0E, 0x02, 0x1F, 0x04}; //李
unsigned char LCD_Table3[8] = {0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00}; //一
unsigned char LCD_Table4[8] = {0x02, 0x02, 0x02, 0x07, 0x02, 0x02, 0x02, 0x02}; //
unsigned char LCD_Table5[8] = {0x1F, 0x0E, 0x0A, 0x0E, 0x0A, 0x0E, 0x00, 0x1F}; //恒

void LCD_Write_CGRAM()
{
    char i;
    I2C_Start();
        I2C_SetAdd(0x78);       // 选择 SLAVE ADDRESS

    I2C_SendByte(0x80); /*控制字节：Co=1,A0=0,表示以下传输的有且只有1 个字节是指令*/
    I2C_SendByte(0x40); //设置XGRAM ADDRESS：第几个CGRAM，0X40 表示第0 个。
    I2C_SendByte(0x40); /*控制字节：Co=0,A0=1,表示以下传输的n 个字节是数据*/

    for(i=0;i<8;i++)
    {
        I2C_SendByte(LCD_Table[i]);
    }
    for(i=0;i<8;i++)
    {
        I2C_SendByte(LCD_Table1[i]);
    }
    for(i=0;i<8;i++)
    {
        I2C_SendByte(LCD_Table2[i]);
    }
    for(i=0;i<8;i++)
    {
        I2C_SendByte(LCD_Table3[i]);
    }
    for(i=0;i<8;i++)
    {
        I2C_SendByte(LCD_Table4[i]);
    }
    for(i=0;i<8;i++)
    {
        I2C_SendByte(LCD_Table5[i]);
    }
    I2C_Stop();
    __delay_cycles((long)100);
}
/****************************************************************
* 名    称：LCD_Display()
* 功    能：LCD显示字符串
* 入口参数：行(0-1)，列(0-15)，字符串
* 出口参数：无
****************************************************************/
void LCD_Display_CGRAM(unsigned char line, unsigned char add, char font)
{
    I2C_Start();
        I2C_SetAdd(0x78);       /*选择SLAVE ADDRESS*/
    I2C_SendByte(0x80);
    I2C_SendByte(LCD_GenPos(line, add));    // 设置 DDRAM ADDRESS：第几行，第几列
    I2C_SendByte(0x40);
        I2C_SendByte(font);
    I2C_Stop();
    __delay_cycles((long)100);
}
/****************************************************************
* 名    称：LCD_Blink()
* 功    能：LCD光标闪烁
* 入口参数：行(0-1)，列(0-15)
* 出口参数：无
****************************************************************/
void LCD_Blink(unsigned char line, unsigned char add)
{
                            // 选择 SLAVE ADDRESS
    I2C_Start();
        I2C_SetAdd(0x78);
    I2C_SendByte(0x00);                         // 控制字节：Co=0,A0=0,表示以下传输的 N 个字节是指令
    I2C_SendByte(LCD_GenPos(line, add));        // 设置 DDRAM ADDRESS：第几行，第几列
    I2C_SendByte(0x0F);
    I2C_Stop();
    __delay_cycles((long)100);
}


/****************************************************************
* 名    称：LCD_BlinkOff()
* 功    能：LCD光标闪烁关闭
* 入口参数：无
* 出口参数：无
****************************************************************/
void LCD_BlinkOff()
{
                            // 选择 SLAVE ADDRESS
    I2C_Start();
        I2C_SetAdd(0x78);
    I2C_SendByte(0x80);                         // 控制字节：Co=1,A0=0,表示以下传输的 1 个字节是指令
    I2C_SendByte(0x0C);
    I2C_Stop();
    __delay_cycles((long)100);
}
/****************************************************************
* 名    称：LCD_Move()
* 功    能：LCD整屏左移
* 入口参数：无
* 出口参数：无
****************************************************************/
void LCD_MoveOn()
{
    I2C_Start();
    I2C_SetAdd(0x78);           // 选择 SLAVE ADDRESS
    I2C_SendByte(0x80);         // 控制字节：Co=1,A0=0,表示以下传输的 1 个字节是指令
    I2C_SendByte(0x18);         // 左移
    I2C_Stop();
    __delay_cycles((long)100);
}
/****************************************************************
* 名    称：LCD_ClearAll()
* 功    能：清屏
* 入口参数：无
* 出口参数：无
****************************************************************/
void LCD_ClearAll()
{
    I2C_Start();
    I2C_SetAdd(0x78);           // 选择 SLAVE ADDRESS
    I2C_SendByte(0x80);         // 控制字节：Co=1,A0=0,表示以下传输的 1 个字节是指令
    I2C_SendByte(0x01);         // 左移
    I2C_Stop();
    __delay_cycles((long)100);
}



#endif /* LCD1602_H_ */
