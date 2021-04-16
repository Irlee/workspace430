/*
 * UART.h
 *
 *  Created on: 2021年4月5日
 *      Author: Lee
 */

#ifndef UART_H_
#define UART_H_

#include <msp430g2553.h>
#include <stdint.h>

/*******************************************************************************
 * LaunchPad硬件UART驱动程序
 * 作者：李
 * 使用方式：
 * 程序初始化过程中，调用InitUART(void);
 * 发送信息给主机时：
 *                   UartSendString(uint8_t *str, uint8_t leng);
 *                      完成一次字符串发送
 *                   void PrintfFloat(float f);
 *                      完成一次浮点数发送
 *                   void PrintfInt(uint16_t num);
 *                      完成一次整数发送
*******************************************************************************/

/*
**以下为本文件所有函数的声明，方便调用：
*/
void InitUART(void);
void UartSendString(uint8_t *str, uint8_t leng);
void PrintfFloat(float f);
void PrintfInt(uint16_t num);

/*
 * @fn:     void InitUART(void)
 * @brief:  初始化串口,设置波特率等
 * @para:   none
 * @return: none
 */
void InitUART(void)
{
    //复位USCI_A0的寄存器,处于设置态
    UCA0CTL1 |= UCSWRST;
    //选择USCI_A0为UART模式
    UCA0CTL0 |= UCMODE_0;
    UCA0CTL0 &= ~UCSYNC;
    //选择串口工作时钟为SMCLK
    UCA0CTL1 |= UCSSEL_2;
    //设置波特率为9600 @ 1MHz
    UCA0BR0 = 104;
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS_1;
    //P1.1和P1.2端口第二功能使能
    P1SEL |= (BIT1 + BIT2);
    P1SEL2 |= (BIT1 + BIT2);
    //清除复位位,使USCI处于工作状态
    UCA0CTL1 &= ~UCSWRST;
}
/*
 * @fn      void UartSendString(uint8_t str, uint8_t leng)
 * @brief   发送字符串
 * @para    str:字符串首地址, leng:字符串长度
 * @return  none
 */
void UartSendString(uint8_t *str, uint8_t leng)
{
    uint8_t i = 0;
    for(i = 0; i < leng; i++)
    {
        while(UCA0STAT & UCBUSY)
        {

        }
        UCA0TXBUF = *(str + i);
    }
}
/*
 * @fn:      void PrintfFloat(float f)
 * @brief:   通过串口向主机发送浮点数
 * @para:    f: 待发浮点型数据
 * @return:  none
 */
void PrintfFloat(float f)
{
    float Xiao;
    int i = 0;
    int Zheng = 0;
    uint8_t zheng[6] = {'0'};
    uint8_t xiao[5] = {'0','0','0','0','0'};

    Zheng = (int)f;
    Xiao = f - Zheng;
    for(i = 5; i >= 0; i--)
    {
        zheng[i] = (int)Zheng % 10 + '0';
        Zheng = Zheng / 10;
    }
    for(i = 0; i <= 3; i++)
    {
        Xiao = Xiao * 10;
        xiao[i] = (int)(Xiao) % 10 + '0';
    }
    UartSendString(zheng, 6);
    UartSendString(".", 1);
    UartSendString(xiao, 5);
}

    /*
     * @fn      void PrintNum(uint16_t num)
     * @brief   串口发送整型数据
     * @para    num: 待发送的整数
     * @return  none
     */
void PrintfInt(uint16_t num)
{
    int i = 0;
    uint8_t zheng[5] = {'0', '0', '0', '0', '0'};

    for(i = 4; i >= 0; i--)
    {
        zheng[i] = num % 10 + '0';
        num = num / 10;
    }
    UartSendString(zheng, 5);
}




#endif /* UART_H_ */
