#include <msp430.h> 
#include <stdio.h>
#include <string.h>
#include "uart.h"
#include "lcd1602.h"

uint32_t nCCR0 = 0;
uint32_t timestamp0 = 0;
uint32_t timestamp1 = 0;
uint32_t cnt = 0;
uint16_t fst_catch = 0;
uint16_t sec_catch = 0;
float freq = 0, Cx = 0;                     //555��Ƶ�ʣ�����Ӧ����ֵ
char LCD_Line1[16] = {'\0'};                //LCD�ڶ�����ʾ������
char LCD_Line2[16] = {'\0'};                //LCD�ڶ�����ʾ������
char clear[16] = "               ";         //16λ�ո�
float jiao = 0;                   //��У׼�ĵ���ֵ
float k = 0.9050, b = -3.7;     //У׼ϵ��k_initial=0.9050;b_initial = -3.67
float k_adjusted = 0, b_adjusted = 0;   //У׼ϵ�����뻺��
int key[8] = {0};                 //���̻�����

#define C1 ((P2IN & BIT2) == BIT2)              //P2.2
#define C2 ((P1IN & BIT7) == BIT7)              //P1.7
#define C3 ((P1IN & BIT6) == BIT6)              //P1.6
#define C4 ((P2IN & BIT5) == BIT5)              //P2.5
#define R1_H P2OUT |= BIT0; P1OUT &= ~(BIT0|BIT4|BIT5)                  //P2.0
#define R2_H P1OUT |= BIT5; P2OUT &= ~BIT0; P1OUT &= ~(BIT0|BIT4)       //P1.5
#define R3_H P1OUT |= BIT4; P2OUT &= ~BIT0; P1OUT &= ~(BIT0|BIT5)       //P1.4
#define R4_H P1OUT |= BIT0; P2OUT &= ~BIT0; P1OUT &= ~(BIT4|BIT5)       //P1.0

void InitSystemClock(void);
void LCD_fresh(void);

/**
 * main.c
 */
int main(void)
{
    float afterk;
    float afterb;
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	InitSystemClock();
	InitUART();
	LCD_Init();

    //TIME1���в�׽����555�񵴵�·������PWM�źŵ�Ƶ��
    TA1CTL |= TASSEL_2;
    TA1CTL |= MC_1;
    TA1CTL |= TAIE;
    TA1CCR0 = 9999;
    TA1CCTL1 |= CAP;
    TA1CCTL1 |= CM_1;
    TA1CCTL1 |= SCS;
    TA1CCTL1 &= ~(CCIS0 + CCIS1);   //CCI1A
    P2DIR &= ~BIT1;
    P2SEL |= BIT1;
    TA1CCTL1 |= CCIE;

    /*��ʼ���мĴ���*/ //P2.2 P1.7 P1.6 P2.5
    P1DIR &= ~(BIT6|BIT7);
    P1REN |= (BIT6|BIT7);
    P1OUT &= ~(BIT6|BIT7);
    P2DIR &= ~(BIT2|BIT5);
    P2REN |= (BIT2|BIT5);
    P2OUT &= ~(BIT2|BIT5);

    /*��ʼ���мĴ���*/ //P2.0 P1.5 P1.4 P1.0
    P2DIR |= BIT0;
    P1DIR |= (BIT0|BIT4|BIT5);

   /*���ð�����������λ�жϣ�SW2*/
    P1DIR &= ~BIT3;
    P1REN |= BIT3;
    P1OUT |= BIT3;
    P1IE |= BIT3;       //ʹ��P1.3�ж�
    P1IES |= BIT3;      //����P1.3���жϴ�������Ϊ�½���
    P1IFG &= ~BIT3;      //����жϱ�־λ

    __bis_SR_register(GIE);
	while(1)
	{
	    freq = (float)1000000.0 / cnt;
	    if(0 < freq < 32767)
	    {
            Cx = 1.44/(4.7 * freq) * 1000000;
            jiao = k * Cx + b;
            strcpy(LCD_Line1,clear);
            strcpy(LCD_Line2,clear);
            sprintf(LCD_Line1, "c=%dnF Cx=%dnF", (int)Cx, (int)jiao);
            afterk = (k-(int)k)*10000;
            afterb = (b-(int)b)*10;
            if(afterb < 0)
                afterb = -afterb;
            sprintf(LCD_Line2, "k=%d.%d b=%d.%d", (int)k,(int)afterk,(int)b,(int)afterb);
	    }
	    else
	    {
	        sprintf(LCD_Line1, "F=%dHz c=%dnF", 0, 0);
            sprintf(LCD_Line2, "%d.%d %d.%d Cx=%d", 0,0,0,0,0);
	    }

	    UartSendString("F=", 2);
	    PrintfFloat(freq);
	    UartSendString("Hz   ", 5);
        UartSendString("C=", 2);
        PrintfFloat(jiao);
        UartSendString("nF\n", 3);


        LCD_fresh();
        //__delay_cycles(1000000);
	}
	return 0;
}

/*
 * @fn:     void InitSystemClock(void)
 * @brief:  ��ʼ��ϵͳʱ��
 * @para:   none
 * @return: none
 */
void InitSystemClock(void)
{
    //DCOCLK����Ϊ1MHz
    DCOCTL = CALDCO_1MHZ;
    BCSCTL1 = CALBC1_1MHZ;
    //SMCLK��ʱ��Դѡ��ΪDCOCLK
    BCSCTL2 &= ~SELS;
    //SMCLK��Ƶ����Ϊ1
    BCSCTL2 |= DIVS_0;
}
/*
 * @fn:     void InitSystemClock(void)
 * @brief:  ��ʼ��ϵͳʱ��
 * @para:   none
 * @return: none
 */
void LCD_fresh(void)
{
    LCD_Display(0,0,LCD_Line1);
    LCD_Display(1,0,LCD_Line2);
}



#pragma vector = TIMER1_A1_VECTOR
__interrupt void TIMER1_CCR1_Capture(void)
{
    static uint8_t flag = 0;
    __bis_SR_register(GIE);         //�����ж�Ƕ��
    switch(TA1IV)
    {
    case 0x02:
        if(flag == 0)
        {
            timestamp0 = nCCR0;
            fst_catch = TA1CCR1;
            flag++;
        }
        else
        {
            timestamp1 = nCCR0;
            sec_catch = TA1CCR1;
            flag = 0;
            cnt = (timestamp1 - timestamp0) * 10000 - fst_catch + sec_catch;
        }
        break;
    case 0x04: break;
    case 0x0A: nCCR0++; break;
    default: break;
    }
}



#pragma vector = PORT1_VECTOR
__interrupt void SW2_ISR(void)
{
    int i = 0, j = 0;
    int value_current;
    volatile int flag3 ;

    flag3 = P1IFG & BIT3;
    if(flag3 == BIT3)
    {
        P1IFG &= ~BIT3;      //����жϱ�־λ

        while(j<8)
        {
            for(i = 1; i <= 4; i++)
            {
                switch(i)
                {
                case 1: R1_H; break;        //����һ�е�ƽ����
                case 2: R2_H; break;        //���ڶ��е�ƽ����
                case 3: R3_H; break;        //�������е�ƽ����
                case 4: R4_H; break;        //�������е�ƽ����
                default : break;
                }

                if(C1+C2+C3+C4 == 0)        //��ɨ�����û�а�������
                    value_current = 0;
                else                        //��ɨ������а�������
                {
                    __delay_cycles(3000);
                    if(C1+C2+C3+C4 != 0)
                    {
                        value_current = 1*C1 + 2*C2 + 3*C3 + 4*C4 + (i-1) * 4;
                        while(C1+C2+C3+C4 != 0);
                        key[j] = value_current - 1;
                        j++;
                    }
                    break;
                }
            }
        }

        k_adjusted = key[0]*1 + key[1]*0.1 + key[2]*0.01 + key[3]*0.001 + key[4]*0.0001;
        b_adjusted = key[6]*1 + key[7]*0.1;
        if(k != k_adjusted)
            k = k_adjusted;
        if(key[5] != 0)
            b_adjusted = -b_adjusted;
        if(b != b_adjusted)
            b = b_adjusted;
    }
}
