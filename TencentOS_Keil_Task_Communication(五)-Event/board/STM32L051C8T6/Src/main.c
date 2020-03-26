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
#define STK_SIZE_TASK_LISTENER      512
#define STK_SIZE_TASK_TRIGGER       512
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
k_stack_t stack_task_listener1[STK_SIZE_TASK_LISTENER];
k_stack_t stack_task_listener2[STK_SIZE_TASK_LISTENER];
k_stack_t stack_task_trigger[STK_SIZE_TASK_TRIGGER];

k_task_t task_listener1;
k_task_t task_listener2;
k_task_t task_trigger;

extern void entry_task_listener1(void *arg);
extern void entry_task_listener2(void *arg);
extern void entry_task_trigger(void *arg);

const k_event_flag_t event_eeny     = (k_event_flag_t)(1 << 0);
const k_event_flag_t event_meeny    = (k_event_flag_t)(1 << 1);
const k_event_flag_t event_miny     = (k_event_flag_t)(1 << 2);
const k_event_flag_t event_moe      = (k_event_flag_t)(1 << 3);

k_event_t event;


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
void entry_task_listener1(void *arg)
{
    k_event_flag_t flag_match;
    k_err_t err;

    while (K_TRUE) {
        // 此任务监听四个事件，因为使用了TOS_OPT_EVENT_PEND_ALL选项，因此必须是四个事件同时到达此任务才会被唤醒
        err = tos_event_pend(&event, event_eeny | event_meeny | event_miny | event_moe,
                                &flag_match, TOS_TIME_FOREVER, TOS_OPT_EVENT_PEND_ALL | TOS_OPT_EVENT_PEND_CLR);
        if (err == K_ERR_NONE) {
            printf("entry_task_listener1:\n");
            printf("eeny, meeny, miny, moe, they all come\n");
        }
    }
}

void entry_task_listener2(void *arg)
{
    k_event_flag_t flag_match;
    k_err_t err;

    while (K_TRUE) {
        // 此任务监听四个事件，因为使用了TOS_OPT_EVENT_PEND_ANY选项，因此四个事件任意一个到达（包括四个事件同时到达）任务就会被唤醒
        err = tos_event_pend(&event, event_eeny | event_meeny | event_miny | event_moe,
                                &flag_match, TOS_TIME_FOREVER, TOS_OPT_EVENT_PEND_ANY | TOS_OPT_EVENT_PEND_CLR);
        if (err == K_ERR_NONE) {
            printf("entry_task_listener2:\n");
            // 有事件到达，判断具体是哪个事件
            if (flag_match == event_eeny) {
                printf("eeny comes\n");
            }
            if (flag_match == event_meeny) {
                printf("meeny comes\n");    
            }
            if (flag_match == event_miny) {
                printf("miny comes\n");
            }
            if (flag_match == event_moe) {
                printf("moe comes\n");
            }
            if (flag_match == (event_eeny | event_meeny | event_miny | event_moe)) {
                printf("all come\n");
            }
        }
    }
}

void entry_task_trigger(void *arg)
{
    int i = 1;

    while (K_TRUE) {
        if (i == 2) {
            printf("entry_task_trigger:\n");
            printf("eeny will come\n");
            // 发送eeny事件，task_listener2会被唤醒
            tos_event_post(&event, event_eeny);
        }
        if (i == 3) {
            printf("entry_task_trigger:\n");
            printf("meeny will come\n");
            // 发送eeny事件，task_listener2会被唤醒
            tos_event_post(&event, event_meeny);
        }
        if (i == 4) {
            printf("entry_task_trigger:\n");
            printf("miny will come\n");
            // 发送eeny事件，task_listener2会被唤醒
            tos_event_post(&event, event_miny);
        }
        if (i == 5) {
            printf("entry_task_trigger:\n");
            printf("moe will come\n");
            // 发送eeny事件，task_listener2会被唤醒
            tos_event_post(&event, event_moe);
        }
        if (i == 6) {
            printf("entry_task_trigger:\n");
            printf("all will come\n");
            // 同时发送四个事件，因为task_listener1的优先级高于task_listener2，因此这里task_listener1会被唤醒
            tos_event_post(&event, event_eeny | event_meeny | event_miny | event_moe);
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
	tos_event_create(&event, (k_event_flag_t)0u);
	// 这里task_listener1的优先级比task_listener2高，因此在task_trigger发送所有事件时，task_listener1会被唤醒
	// 读者可以尝试将task_listener1优先级修改为5（比task_listener2低），此设置下，在task_trigger发送所有事件时，task_listener2将会被唤醒。
	(void)tos_task_create(&task_listener1, "listener1", entry_task_listener1, NULL,
													3, stack_task_listener1, STK_SIZE_TASK_LISTENER, 0);
	(void)tos_task_create(&task_listener2, "listener2", entry_task_listener2, NULL,
													4, stack_task_listener2, STK_SIZE_TASK_LISTENER, 0);
	(void)tos_task_create(&task_trigger, "trigger", entry_task_trigger, NULL,
													4, stack_task_trigger, STK_SIZE_TASK_TRIGGER, 0);
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
