/*******************************************************************************
** �ļ���: 		mian.c
** �汾��  		2.0
** ��������: 	RealView MDK-ARM 4.1.0
** ����: 		jeansonm
** ��������: 	2013-07-25
** ����:		SPI ��д AT45DB161 FLASH
** ����ļ�:	��
** �޸���־��	2013-07-25   �����ĵ�
*******************************************************************************/

/******************************
** ��ţ������(V2.0)
** SPI ��д AT45DB161 FLASH
** ��̳��bbs.openmcu.com
** ������www.openmcu.com
** ���䣺support@openmcu.com
*******************************/

#include "stm32f10x.h"
#include "sys_config.h"
#include <stdio.h>
#include "sys_config.h"
#include "Gpio_Led.h"
#include "spi_flash.h"

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define  FLASH_WriteAddress     0x000000
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress
#define  AT45DB161D_FLASH_ID    0x1F260000
#define  BufferSize (countof(Tx_Buffer)-1)
#define countof(a) (sizeof(a) / sizeof(*(a)))

u8 Tx_Buffer[] = "STM32F10x SPI Firmware Library Example: communication with an AT45DB161D SPI FLASH";//����
u8 Index, Rx_Buffer[BufferSize];
vu32 flash_ID = 0;
volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = PASSED;

TestStatus Buffercmp(u8* pBuffer1, u8* pBuffer2, u16 BufferLength);


/*******************************************************************************
  * @��������	main
  * @����˵��   ������ 
  * @�������   ��
  * @�������   ��
  * @���ز���   ��
  *****************************************************************************/
int main(void)
{
  RCC_Configuration();
	NVIC_Configuration();
	GPIO_Configuration();
	GpioLed_Init();
  SPI_FLASH_Init();

	flash_ID = SPI_FLASH_ReadID();      //��ID
	
	if(flash_ID == AT45DB161D_FLASH_ID)	//���ID�Ƿ�ƥ��
	{
	    GPIO_ResetBits(GPIO_LED, LED1); //ID��ȷ����LED1
	}
	else
	{
	    GPIO_SetBits(GPIO_LED, LED1);   //ID���󣬹ر�LED1
	}

	/* ��д���� */
	SPI_FLASH_PageErase(FLASH_SectorToErase);  //ҳ����
	SPI_FLASH_BufferWrite(Tx_Buffer, FLASH_WriteAddress, BufferSize); //д������
	SPI_FLASH_BufferRead(Rx_Buffer, FLASH_ReadAddress, BufferSize);   //��������

	/* ���д��������������Ƿ���ͬ */
	TransferStatus1 = Buffercmp(Tx_Buffer, Rx_Buffer, BufferSize);    //�Ƚ϶�����д�������

	if(TransferStatus1 == PASSED)
	{
	    GPIO_ResetBits(GPIO_LED, LED2); //д���������������ͬ LED2��
	}
	else
	{
	    GPIO_SetBits(GPIO_LED, LED2);   //д���������������ͬ LED2��
	}	    

	/* �����������ݲ��� */
	SPI_FLASH_PageErase(FLASH_SectorToErase);   //ҳ����
	SPI_FLASH_BufferRead(Rx_Buffer, FLASH_ReadAddress, BufferSize);	//�������������
	
	for (Index = 0; Index < BufferSize; Index++)  //��������Ƿ���0XFF
	{
		if (Rx_Buffer[Index] != 0xFF)
		{
			TransferStatus2 = FAILED;
		}
	}

	if(TransferStatus2 == PASSED)
	{
	    GPIO_ResetBits(GPIO_LED, LED3); //�����󣬶�������ȫΪ0XFF ������ȷ LED3��
	}
	else
	{
	    GPIO_SetBits(GPIO_LED, LED3);   //�����󣬶������ݲ�ȫΪ0XFF �������� LED3��
	}
    while (1)
    {
    }
}

/*******************************************************************************
* Function Name  : Buffercmp
* Description    : Compares two buffers.
* Input          : - pBuffer1, pBuffer2: buffers to be compared.
*                : - BufferLength: buffer's length
* Output         : None
* Return         : PASSED: pBuffer1 identical to pBuffer2
*                  FAILED: pBuffer1 differs from pBuffer2
*******************************************************************************/
TestStatus Buffercmp(u8* pBuffer1, u8* pBuffer2, u16 BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }

    pBuffer1++;
    pBuffer2++;
  }

  return PASSED;
}

#ifdef  USE_FULL_ASSERT
/*******************************************************************************
  * @��������	assert_failed
  * @����˵��   �����ڼ�������������ʱ��Դ�ļ����ʹ�������
  * @�������   file: Դ�ļ���
  				line: ������������ 
  * @�������   ��
  * @���ز���   ��
  *****************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
    while (1)
    {
    }
}
#endif

/***********************************�ļ�����***********************************/
