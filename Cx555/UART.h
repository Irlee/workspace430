/*
 * UART.h
 *
 *  Created on: 2021��4��5��
 *      Author: Lee
 */

#ifndef UART_H_
#define UART_H_

#include <msp430g2553.h>
#include <stdint.h>

/*******************************************************************************
 * LaunchPadӲ��UART��������
 * ���ߣ���
 * ʹ�÷�ʽ��
 * �����ʼ�������У�����InitUART(void);
 * ������Ϣ������ʱ��
 *                   UartSendString(uint8_t *str, uint8_t leng);
 *                      ���һ���ַ�������
 *                   void PrintfFloat(float f);
 *                      ���һ�θ���������
 *                   void PrintfInt(uint16_t num);
 *                      ���һ����������
*******************************************************************************/

/*
**����Ϊ���ļ����к�����������������ã�
*/
void InitUART(void);
void UartSendString(uint8_t *str, uint8_t leng);
void PrintfFloat(float f);
void PrintfInt(uint16_t num);

/*
 * @fn:     void InitUART(void)
 * @brief:  ��ʼ������,���ò����ʵ�
 * @para:   none
 * @return: none
 */
void InitUART(void)
{
    //��λUSCI_A0�ļĴ���,��������̬
    UCA0CTL1 |= UCSWRST;
    //ѡ��USCI_A0ΪUARTģʽ
    UCA0CTL0 |= UCMODE_0;
    UCA0CTL0 &= ~UCSYNC;
    //ѡ�񴮿ڹ���ʱ��ΪSMCLK
    UCA0CTL1 |= UCSSEL_2;
    //���ò�����Ϊ9600 @ 1MHz
    UCA0BR0 = 104;
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS_1;
    //P1.1��P1.2�˿ڵڶ�����ʹ��
    P1SEL |= (BIT1 + BIT2);
    P1SEL2 |= (BIT1 + BIT2);
    //�����λλ,ʹUSCI���ڹ���״̬
    UCA0CTL1 &= ~UCSWRST;
}
/*
 * @fn      void UartSendString(uint8_t str, uint8_t leng)
 * @brief   �����ַ���
 * @para    str:�ַ����׵�ַ, leng:�ַ�������
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
 * @brief:   ͨ���������������͸�����
 * @para:    f: ��������������
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
     * @brief   ���ڷ�����������
     * @para    num: �����͵�����
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
