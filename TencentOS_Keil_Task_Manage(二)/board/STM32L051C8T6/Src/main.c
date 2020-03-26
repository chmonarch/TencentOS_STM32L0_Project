/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cmsis_os.h" 
#include "stdio.h"

#include "tos_k.h"		// ���TencentOS tiny�ں˽ӿ�ͷ�ļ�

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define STK_SIZE_TASK_PRIO4      512	// ���ȼ�Ϊ4������ջ��СΪ512
#define STK_SIZE_TASK_PRIO5      1024	// ���ȼ�Ϊ5������ջ��СΪ1024

k_stack_t stack_task_prio4[STK_SIZE_TASK_PRIO4];	// ���ȼ�Ϊ4������ջ�ռ�
k_stack_t stack_task_prio5[STK_SIZE_TASK_PRIO5];	// ���ȼ�Ϊ5������ջ�ռ�

k_task_t task_prio4;	// ���ȼ�Ϊ4��������
k_task_t task_prio5;	// ���ȼ�Ϊ5��������

extern void entry_task_prio4(void *arg);	// ���ȼ�Ϊ4����������ں���
extern void entry_task_prio5(void *arg);	// ���ȼ�Ϊ5����������ں���

uint32_t arg_task_prio4_array[3] = {	// ���ȼ�Ϊ4����������ں������
    1, 2, 3,
};

char *arg_task_prio5_string = "arg for task_prio5";	// ���ȼ�Ϊ5����������ں������
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
static void dump_uint32_array(uint32_t *array, size_t len)
{
    size_t i = 0;

    for (i = 0; i < len; ++i) {
        printf("%d\t", array[i]);
    }
    printf("\n\n");

}

void entry_task_prio4(void *arg)
{
    uint32_t *array_from_main = (uint32_t *)arg;	// ��������ߴ���Ĳ���

    printf("array from main:\n");
    dump_uint32_array(array_from_main, 3);	// dump����Ĳ��������飩

    while (K_TRUE) {
        printf("task_prio4 body\n");	// ���������壬���ϴ�ӡ������Ϣ
        tos_task_delay(1000);	// ˯��1000��ϵͳʱ�ӵδ����¼���systick������ΪTOS_CFG_CPU_TICK_PER_SECONDΪ1000��Ҳ����һ���ӻ���1000��systick�����˯��1000��systick����˯����1�롣
    }
}

void entry_task_prio5(void *arg)
{
    int i = 0;
    char *string_from_main = (char *)arg;
    printf("string from main:\n");
    printf("%s\n\n", string_from_main);	// ��ӡ�������ߴ�����ַ�������

    while (K_TRUE) {
        if (i == 2) {
            printf("i = %d\n", i);	// iΪ2ʱ������task_prio4��task_prio4ֹͣ����
            tos_task_suspend(&task_prio4);
        } else if (i == 4) {
            printf("i = %d\n", i);	// iΪ4ʱ���ָ�task_prio4������
            tos_task_resume(&task_prio4);
        } else if (i == 6) {
            printf("i = %d\n", i);	// iΪ6ʱ��ɾ��task_prio4��task_prio4��������
            tos_task_destroy(&task_prio4);
        }
        printf("task_prio5 body\n");
        tos_task_delay(1000);
        ++i;
    }
}
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	tos_knl_init();	// ��ʼ��TencentOS tiny�ں�
	// ����һ�����ȼ�Ϊ4������
	(void)tos_task_create(&task_prio4, "task_prio4", entry_task_prio4,
												(void *)(&arg_task_prio4_array[0]), 4,
												stack_task_prio4, STK_SIZE_TASK_PRIO4, 0);
	// ����һ�����ȼ�Ϊ5������
	(void)tos_task_create(&task_prio5, "task_prio5", entry_task_prio5,
												(void *)arg_task_prio5_string, 5,
												stack_task_prio5, STK_SIZE_TASK_PRIO5, 0);
	// ��ʼ�ں˵���
	tos_knl_start();
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
