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

#include "tos_k.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define STK_SIZE_TASK_DEMO      512

#define PRIO_TASK_DEMO          4

k_stack_t stack_task_demo[STK_SIZE_TASK_DEMO];

k_task_t task_demo;

#define FIFO_BUFFER_SIZE        5
uint8_t fifo_buffer[FIFO_BUFFER_SIZE];

k_chr_fifo_t fifo;

extern void entry_task_demo(void *arg);

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
void char_push(void)
{
    k_err_t err;
    int i = 0;
    uint8_t data;

    // ��fifo��ѹ��FIFO_BUFFER_SIZE���ַ����ֱ���a��b��c��d��e
    for (i = 0; i < FIFO_BUFFER_SIZE; ++i) {
        printf("char pushed: %c\n", 'a' + i);
        err = tos_chr_fifo_push(&fifo, 'a' + i);
        if (err != K_ERR_NONE) {
            printf("should never happen\n");
        }
    }

    // fifo������FIFO_BUFFER_SIZE���ַ��������߼����Ѿ�ѹ���������ַ�������ʱ����ѹ���ַ��᷵��K_ERR_FIFO_FULL��fifo������
    err = tos_chr_fifo_push(&fifo, 'z');
    if (err == K_ERR_RING_Q_FULL) {
        printf("fifo is full: %s\n", tos_chr_fifo_is_full(&fifo) ? "TRUE" : "FALSE");
    } else {
        printf("should never happen\n");
    }

    // ��fifo�а����е��ַ���������
    for (i = 0; i < FIFO_BUFFER_SIZE; ++i) {
        err = tos_chr_fifo_pop(&fifo, &data);
        if (err == K_ERR_NONE) {
            printf("%d pop: %c\n", i, data);
        } else {
            printf("should never happen\n");
        }
    }
    // ��ʱ���������ַ����᷵��K_ERR_FIFO_EMPTY��fifo�ѿգ�
    err = tos_chr_fifo_pop(&fifo, &data);
    if (err == K_ERR_RING_Q_EMPTY) {
        printf("fifo is empty: %s\n", tos_chr_fifo_is_empty(&fifo) ? "TRUE" : "FALSE");
    } else {
        printf("should never happen\n");
    }
}

void stream_push(void)
{
    int count = 0, i = 0;
    uint8_t stream[FIFO_BUFFER_SIZE] = { 'a', 'b', 'c', 'd', 'e' };
    uint8_t stream_dummy[1] = { 'z' };
    uint8_t stream_pop[FIFO_BUFFER_SIZE];

    // ѹ���ַ������ַ����ĳ�����5��������fifo����󳤶�FIFO_BUFFER_SIZE����ѹ��ɹ���������ѹ���ַ����ĳ���5
    count = tos_chr_fifo_push_stream(&fifo, &stream[0], FIFO_BUFFER_SIZE);
    if (count != FIFO_BUFFER_SIZE) {
        printf("should never happen\n");
    }

    // ����ѹ���ַ�������ʹ�ǳ���Ϊ1���ַ���������fifo�����޷�����ѹ�룬���س���0��ѹ��ʧ�ܣ�
    count = tos_chr_fifo_push_stream(&fifo, &stream_dummy[0], 1);
    if (count == 0) {
        printf("fifo is full: %s\n", tos_chr_fifo_is_full(&fifo) ? "TRUE" : "FALSE");
    } else {
        printf("should never happen\n");
    }

    // ��ǰ����ѹ����ַ���ȫ������������ǰ��ѹ����ַ�������5���������ַ������ȣ�
    count = tos_chr_fifo_pop_stream(&fifo, &stream_pop[0], FIFO_BUFFER_SIZE);
    if (count == FIFO_BUFFER_SIZE) {
        printf("stream popped:\n");
        for (i = 0; i < FIFO_BUFFER_SIZE; ++i) {
            printf("%c", stream_pop[i]);
        }
        printf("\n");
    } else {
        printf("should never happen\n");
    }

    // ������������fifo�ѿգ�����0
    count = tos_chr_fifo_pop_stream(&fifo, &stream_pop[0], 1);
    if (count == 0) {
        printf("fifo is empty: %s\n", tos_chr_fifo_is_empty(&fifo) ? "TRUE" : "FALSE");
    } else {
        printf("should never happen\n");
    }
}

void entry_task_demo(void *arg)
{
    // ������һ��������FIFO_BUFFER_SIZE���ַ���fifo
    tos_chr_fifo_create(&fifo, &fifo_buffer[0], FIFO_BUFFER_SIZE);

    printf("fifo, dealing with char\n");
    char_push();

    printf("fifo, dealing with stream\n");
    stream_push();
}

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
	
	tos_knl_init();
	(void)tos_task_create(&task_demo, "demo", entry_task_demo, NULL,
                        PRIO_TASK_DEMO, stack_task_demo, STK_SIZE_TASK_DEMO, 0);
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
