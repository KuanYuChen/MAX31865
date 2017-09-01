/** \file main.c ******************************************************
*
* Project: MAX31865
* Filename: main.c
* Description: This module contains the Main application for the MAX31865 example program.
*

*
* --------------------------------------------------------------------
*
* This code follows the following naming conventions:
*
*\n	char				ch_pmod_value
*\n	char (array)			s_pmod_string[16]
*\n	float				f_pmod_value
*\n	int				n_pmod_value
*\n	int (array)                     an_pmod_value[16]
*\n     int16_t			        w_pmod_value
*\n     int16_t (array)		        aw_pmod_value[16]
*\n	uint16_t		        uw_pmod_value
*\n	uint16_t (array)	        auw_pmod_value[16]
*\n	uint8_t			        uch_pmod_value
*\n	uint8_t (array)		        auch_pmod_buffer[16]
*\n	unsigned int			un_pmod_value
*\n	int *			        pn_pmod_value
*
* ------------------------------------------------------------------------- */

/*!\mainpage Main Page

*/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32_spi.h"
#include "MAX31865drv.h"
#include "stm32_usart1.h"
#include <stdio.h>
#include "math.h"



char s[64];
uint8_t uch_fault_status;
uint8_t auch_rtd[2];
uint16_t w_rtd_value;
float   f_temperature;
float   f_resistance;
void Delay(uint32_t ms);

max31865_configuration  configuration;
int main(void)
{
  RCC_DeInit();
  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
  SPI_Inital();           //��ʼ��SPI
  USART1_Init();           //��ʼ������1
   
  Set_DRDY_Pin_INPUT();    //��MAX31865 DRDY��Ӧ��������Ϊ����
  
/*����MAX31865����ģʽ*/  
  
  configuration.Vbias=ON;
  
  configuration.Rtd_wire=RTD_2wire;
  configuration.Filter=Filter_50Hz;
  maxim_31865_init(&configuration);
  
  /*���ù������ޣ�����Ϊ����ֵ*/
  
  maxim_set_fault_threshold(400,0);
  
  while(1)
  {
  
  /*���й��ϼ��*/
  
  uch_fault_status=maxim_manual_fault_detection();
  /*
   *�����⵽RTD���ϣ���������ϴ��룬ֹͣ�������У����򣬽�����ѭ������
  *��ѭ�������У����õ��β�����ʽ��ÿ�����һ�Σ����������ת�����¶������
  *�����ѭ�������м�⵽RTD���ϣ�ͬ��������ϴ��룬���ǣ���ѭ����⣬һ��
  *����������������������¶Ȳɼ�
  */
  if((uch_fault_status&0xFC)==0)
     {
         Delay(1000);                                                   //�ӳٳ��� 1s
         configuration.Conversion_mode=One_Shot_Conversion;
         maxim_31865_init(&configuration);                              //����һ�β���
         while(DRDY_Pin_Value()==SET);                                  //�ȴ���������
         maxim_get_rtd_value(auch_rtd);                                 //��ȡ�������
         if((auch_rtd[1]&0x01)==0x01)                                   //������ϱ�ʶ��λ�����й��ϼ�⣬������ϴ���
            {
             uch_fault_status=maxim_manual_fault_detection();           //�����ֶ����ϼ�⣬Ҳ���������Զ����ϼ��
             USART1_SendString("\033[2J");
             USART1_SendString("MAX31865 Error Code:      ");           //������ϴ���
             sprintf(s,"%x",uch_fault_status&0xFC);
             USART1_SendString(s);
             USART1_SendString("\r\n");
             maxim_clear_fault_status();                                //������ϼĴ���
            }
         else
            {                                                           //���û�й��ϣ�������¶�ת��������¶�ת�����
             w_rtd_value=(auch_rtd[0]<<8|auch_rtd[1])>>1;               //����RTD����ֵ����
             f_resistance=w_rtd_value*REF_RES/32768.000;
             f_temperature=(0-R0*A+sqrt(pow(R0,2)*pow(A,2)-4*R0*B*(R0-f_resistance)))/(2*R0*B); //������ֵת�����¶ȣ��ù�ʽ��������T>0ʱ��
             sprintf(s,"%.2f",f_temperature);						//���ü���������¶�ֵ�������õ����¶�ֵ��ֵ������S��
             USART1_SendString("\033[2J");
             USART1_SendString("Current Temperature:   ");              //����¶�ת�����
             USART1_SendString(s);													//���ڴ�ӡ������������¶�ֵ
             USART1_SendString(" ��\r\n");
            }
         
      }
  else
    {
     USART1_SendString("MAX31865 Error Code:         ");
     sprintf(s,"%x",uch_fault_status&0xFC);
     USART1_SendString(s);
     USART1_SendString("\r\n"); 
     maxim_clear_fault_status();
    }
  }
}

//�ӳ�1ms���ó����ѡ���CPUʱ���йأ��ɸ���ʱ�ӽ��е���
void Delay(uint32_t ms)
{
  uint32_t temp;
  //temp=0x000640;
  temp=0x0007D0;
  while(ms)
  {
    while (temp!=0)
        temp--;
    temp=0x0007D0;
    ms--;
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */



