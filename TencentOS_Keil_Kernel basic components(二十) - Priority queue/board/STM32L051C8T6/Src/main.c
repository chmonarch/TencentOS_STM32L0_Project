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
/*
 此案例展示了优先级队列的出队规则：依次入队优先级为5、4、3、2、1的元素，出队时的顺序是按照优先级来的，也就是按优先级从高到低（数值越大，优先级越小），依次出队优先级为1、2、3、4、5元素。
 */
#define STK_SIZE_TASK_DEMO      512

#define PRIO_TASK_DEMO          4

k_stack_t stack_task_demo[STK_SIZE_TASK_DEMO];

k_task_t task_demo;

typedef struct item_st {
    int a;
    int b;
    int c;
} item_t;

#define PRIO_QUEUE_ITEM_MAX     5
uint8_t ring_q_buffer[PRIO_QUEUE_ITEM_MAX * sizeof(item_t)];
uint8_t mgr_pool[TOS_PRIO_Q_MGR_ARRAY_SIZE(PRIO_QUEUE_ITEM_MAX)];

k_prio_q_t prio_q;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
void entry_task_demo(void *arg)
{
    k_err_t err;
    int i = 0;
    item_t item;
    k_prio_t prio;
    size_t item_size;

    tos_prio_q_create(&prio_q, mgr_pool, ring_q_buffer, PRIO_QUEUE_ITEM_MAX, sizeof(item_t));

    for (i = PRIO_QUEUE_ITEM_MAX; i > 0; --i) {
        printf("enqueue: %d  %d  %d\n", i, i, i);
        item.a = i;
        item.b = i;
        item.c = i;
        err = tos_prio_q_enqueue(&prio_q, &item, sizeof(item_t), i);
        if (err != K_ERR_NONE) {
            printf("should never happen\n");
        }
    }

    err = tos_prio_q_enqueue(&prio_q, &item, sizeof(item_t), i);
    if (err == K_ERR_PRIO_Q_FULL) {
        printf("priority queue is full: %s\n", tos_prio_q_is_full(&prio_q) ? "TRUE" : "FALSE");
    } else {
        printf("should never happen\n");
    }

    for (i = 0; i < PRIO_QUEUE_ITEM_MAX; ++i) {
        err = tos_prio_q_dequeue(&prio_q, &item, &item_size, &prio);
        if (err == K_ERR_NONE) {
            printf("dequeue: %d %d %d, prio: %d\n", item.a, item.b, item.c, prio);
        } else {
            printf("should never happen\n");
        }
    }

    err = tos_prio_q_dequeue(&prio_q, &item, &item_size, &prio);
    if (err == K_ERR_PRIO_Q_EMPTY) {
        printf("priority queue is empty: %s\n", tos_prio_q_is_empty(&prio_q) ? "TRUE" : "FALSE");
    } else {
        printf("should never happen\n");
    }
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
                            PRIO_TASK_DEMO, stack_task_demo, STK_SIZE_TASK_DEMO,
                            0);
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
