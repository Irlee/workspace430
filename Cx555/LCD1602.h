/*
 * LCD1602.h
 *
 *  Created on: 2021��4��5��
 *      Author: Lee
 */

#ifndef LCD1602_H_
#define LCD1602_H_

#include "I2C.h"


/**************************************************************************************
 * LCD1602_I2C��������
 * ���ߣ�PurpleSword
 * 2014.8.7�Ľ����Ľ�˵�����˳���Ӧ��ģ��I2C����ʹ�ã���һЩϸ���������޸�
 * ʹ�÷�ʽ��
 * �����ʼ�������У�����LCD_Init();
 * Ҫ��ʾ�ַ���ʱ��  LCD_Display(0,0,lcd_disp)
 *                   ע����һ������Ϊ�������ڶ�����Ϊ���������һ������ΪҪ��ʾ���ַ�������Ԫ�ص�ַ��ֱ���ô������ɣ�
 *                  ���һ�η���
 *************************************************************************************/
#define LCD_ClearLine0()    LCD_Display(0, 0, "                ")
#define LCD_ClearLine1()    LCD_Display(1, 0, "                ")

/*
**����Ϊ���ļ����к�����������������ã�
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
* ��    �ƣ�LCD_Init()
* ��    �ܣ�LCD��ʼ��
* ��ڲ�������
* ���ڲ�������
****************************************************************/
void LCD_Init(void)
{
    I2C_Init();             // I2C��ʼ��
        I2C_Start();            // ��ʼ��־
    I2C_SetAdd(0x78);   // ѡ�� SLAVE ADDRESS

    I2C_SendByte(0x00); // �����ֽڣ�Co=0,A0=0,
                            //��ʾ���´���� N ���ֽ���ָ��
    I2C_SendByte(0x38); // Function set
    I2C_SendByte(0x0C); // Display ON/OFF
    I2C_SendByte(0x01); // Clear display
    I2C_SendByte(0x06); // Entry mode set
    I2C_Stop();     // ������־
    __delay_cycles((long)16000);
}


/****************************************************************
* ��    �ƣ�LCD_GenPos()
* ��    �ܣ��ڲ���������ָ�������м����Ӧ��LCD��ʾ��ַ
* ��ڲ�������(0-1)����(0-15)
* ���ڲ�������ַ
****************************************************************/
unsigned char LCD_GenPos(unsigned char line, unsigned char add)
{
    return line ? (add + 0x80 + 0x40) : (add + 0x80);
}


/****************************************************************
* ��    �ƣ�LCD_Display()
* ��    �ܣ�LCD��ʾ�ַ���
* ��ڲ�������(0-1)����(0-15)���ַ���
* ���ڲ�������
****************************************************************/
void LCD_Display(unsigned char line, unsigned char add, char *str)
{
    I2C_Start();
        I2C_SetAdd(0x78);       // ѡ�� SLAVE ADDRESS
    I2C_SendByte(0x80);         // �����ֽڣ�Co=1,A0=0,��ʾ���´���� 1 ���ֽ���ָ��
    I2C_SendByte(LCD_GenPos(line, add));    // ���� DDRAM ADDRESS���ڼ��У��ڼ���
    I2C_SendByte(0x40);         // �����ֽڣ�Co=0,A0=1,��ʾ���´���� n ���ֽ�������
    while(*str && (add++ < 16) ) {
        I2C_SendByte(*str++);
    }
    I2C_Stop();
    __delay_cycles((long)100);
}
/****************************************************************
* ��    �ƣ�LCD_Display()
* ��    �ܣ�LCD��ʾ�ַ���
* ��ڲ�������(0-1)����(0-15)���ַ���
* ���ڲ�������
****************************************************************/

unsigned char LCD_Table[8] = {0x1F, 0x0A, 0x1F, 0x1B, 0x1B, 0x11, 0x11, 0x1F};  //�ַ� ��
unsigned char LCD_Table1[8] = {0x04, 0x1F, 0x15, 0x1F, 0x15, 0x1F, 0x04, 0x07}; //��
unsigned char LCD_Table2[8] = {0x04, 0x1F, 0x0E, 0x15, 0x0E, 0x02, 0x1F, 0x04}; //��
unsigned char LCD_Table3[8] = {0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00}; //һ
unsigned char LCD_Table4[8] = {0x02, 0x02, 0x02, 0x07, 0x02, 0x02, 0x02, 0x02}; //
unsigned char LCD_Table5[8] = {0x1F, 0x0E, 0x0A, 0x0E, 0x0A, 0x0E, 0x00, 0x1F}; //��

void LCD_Write_CGRAM()
{
    char i;
    I2C_Start();
        I2C_SetAdd(0x78);       // ѡ�� SLAVE ADDRESS

    I2C_SendByte(0x80); /*�����ֽڣ�Co=1,A0=0,��ʾ���´��������ֻ��1 ���ֽ���ָ��*/
    I2C_SendByte(0x40); //����XGRAM ADDRESS���ڼ���CGRAM��0X40 ��ʾ��0 ����
    I2C_SendByte(0x40); /*�����ֽڣ�Co=0,A0=1,��ʾ���´����n ���ֽ�������*/

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
* ��    �ƣ�LCD_Display()
* ��    �ܣ�LCD��ʾ�ַ���
* ��ڲ�������(0-1)����(0-15)���ַ���
* ���ڲ�������
****************************************************************/
void LCD_Display_CGRAM(unsigned char line, unsigned char add, char font)
{
    I2C_Start();
        I2C_SetAdd(0x78);       /*ѡ��SLAVE ADDRESS*/
    I2C_SendByte(0x80);
    I2C_SendByte(LCD_GenPos(line, add));    // ���� DDRAM ADDRESS���ڼ��У��ڼ���
    I2C_SendByte(0x40);
        I2C_SendByte(font);
    I2C_Stop();
    __delay_cycles((long)100);
}
/****************************************************************
* ��    �ƣ�LCD_Blink()
* ��    �ܣ�LCD�����˸
* ��ڲ�������(0-1)����(0-15)
* ���ڲ�������
****************************************************************/
void LCD_Blink(unsigned char line, unsigned char add)
{
                            // ѡ�� SLAVE ADDRESS
    I2C_Start();
        I2C_SetAdd(0x78);
    I2C_SendByte(0x00);                         // �����ֽڣ�Co=0,A0=0,��ʾ���´���� N ���ֽ���ָ��
    I2C_SendByte(LCD_GenPos(line, add));        // ���� DDRAM ADDRESS���ڼ��У��ڼ���
    I2C_SendByte(0x0F);
    I2C_Stop();
    __delay_cycles((long)100);
}


/****************************************************************
* ��    �ƣ�LCD_BlinkOff()
* ��    �ܣ�LCD�����˸�ر�
* ��ڲ�������
* ���ڲ�������
****************************************************************/
void LCD_BlinkOff()
{
                            // ѡ�� SLAVE ADDRESS
    I2C_Start();
        I2C_SetAdd(0x78);
    I2C_SendByte(0x80);                         // �����ֽڣ�Co=1,A0=0,��ʾ���´���� 1 ���ֽ���ָ��
    I2C_SendByte(0x0C);
    I2C_Stop();
    __delay_cycles((long)100);
}
/****************************************************************
* ��    �ƣ�LCD_Move()
* ��    �ܣ�LCD��������
* ��ڲ�������
* ���ڲ�������
****************************************************************/
void LCD_MoveOn()
{
    I2C_Start();
    I2C_SetAdd(0x78);           // ѡ�� SLAVE ADDRESS
    I2C_SendByte(0x80);         // �����ֽڣ�Co=1,A0=0,��ʾ���´���� 1 ���ֽ���ָ��
    I2C_SendByte(0x18);         // ����
    I2C_Stop();
    __delay_cycles((long)100);
}
/****************************************************************
* ��    �ƣ�LCD_ClearAll()
* ��    �ܣ�����
* ��ڲ�������
* ���ڲ�������
****************************************************************/
void LCD_ClearAll()
{
    I2C_Start();
    I2C_SetAdd(0x78);           // ѡ�� SLAVE ADDRESS
    I2C_SendByte(0x80);         // �����ֽڣ�Co=1,A0=0,��ʾ���´���� 1 ���ֽ���ָ��
    I2C_SendByte(0x01);         // ����
    I2C_Stop();
    __delay_cycles((long)100);
}



#endif /* LCD1602_H_ */
