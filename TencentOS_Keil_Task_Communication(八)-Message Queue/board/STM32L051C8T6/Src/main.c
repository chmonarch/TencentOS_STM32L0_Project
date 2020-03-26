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
 这里演示如何使用消息队列在sender和receiver任务之间传递消息（一个指针，此案例中这个指针信息指向的是一个字符串）
 */
#define STK_SIZE_TASK_RECEIVER      512
#define STK_SIZE_TASK_SENDER        512

#define PRIO_TASK_RECEIVER_HIGHER_PRIO      4
#define PRIO_TASK_RECEIVER_LOWER_PRIO       (PRIO_TASK_RECEIVER_HIGHER_PRIO + 1)

#define MESSAGE_MAX     10
 
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
k_stack_t stack_task_receiver_higher_prio[STK_SIZE_TASK_RECEIVER];
k_stack_t stack_task_receiver_lower_prio[STK_SIZE_TASK_RECEIVER];
k_stack_t stack_task_sender[STK_SIZE_TASK_SENDER];

uint8_t msg_pool[MESSAGE_MAX * sizeof(void *)];

k_task_t task_receiver_higher_prio;
k_task_t task_receiver_lower_prio;
k_task_t task_sender;

k_msg_q_t msg_q;

extern void entry_task_receiver_higher_prio(void *arg);
extern void entry_task_receiver_lower_prio(void *arg);
extern void entry_task_sender(void *arg);

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
void entry_task_receiver_higher_prio(void *arg)
{
    k_err_t err;
    void *msg_received;

    while (K_TRUE) {
        err = tos_msg_q_pend(&msg_q, &msg_received, TOS_TIME_FOREVER);
        if (err == K_ERR_NONE) {
            printf("higher: msg incoming[%s]\n", (char *)msg_received);
        }
    }
}

void entry_task_receiver_lower_prio(void *arg)
{
    k_err_t err;
    void *msg_received;

    while (K_TRUE) {
        err = tos_msg_q_pend(&msg_q, &msg_received, TOS_TIME_FOREVER);
        if (err == K_ERR_NONE) {
            printf("lower: msg incoming[%s]\n", (char *)msg_received);
        }
    }
}

void entry_task_sender(void *arg)
{
    int i = 1;
    char *msg_to_one_receiver = "message for one receiver(with highest priority)";
    char *msg_to_all_receiver = "message for all receivers";

    while (K_TRUE) {
        if (i == 2) {
            printf("sender: send a message to one receiver, and shoud be the highest priority one\n");
            tos_msg_q_post(&msg_q, msg_to_one_receiver);
        }
        if (i == 3) {
            printf("sender: send a message to all recevier\n");
            tos_msg_q_post_all(&msg_q, msg_to_all_receiver);
        }
        if (i == 4) {
            printf("sender: send a message to one receiver, and shoud be the highest priority one\n");
            tos_msg_q_post(&msg_q, msg_to_one_receiver);
        }
        if (i == 5) {
            printf("sender: send a message to all recevier\n");
            tos_msg_q_post_all(&msg_q, msg_to_all_receiver);
        }
        tos_task_delay(1000);
        ++i;
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
	tos_msg_q_create(&msg_q, msg_pool, MESSAGE_MAX);
	(void)tos_task_create(&task_receiver_higher_prio, "receiver_higher_prio",
													entry_task_receiver_higher_prio, NULL, PRIO_TASK_RECEIVER_HIGHER_PRIO,
													stack_task_receiver_higher_prio, STK_SIZE_TASK_RECEIVER, 0);
	(void)tos_task_create(&task_receiver_lower_prio, "receiver_lower_prio",
													entry_task_receiver_lower_prio, NULL, PRIO_TASK_RECEIVER_LOWER_PRIO,
													stack_task_receiver_lower_prio, STK_SIZE_TASK_RECEIVER, 0);
	(void)tos_task_create(&task_sender, "sender", entry_task_sender, NULL,
													4, stack_task_sender, STK_SIZE_TASK_SENDER, 0);
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
