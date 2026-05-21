/*********************************************************************
*                     SEGGER Microcontroller GmbH                    *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*       (c) 1995 - 2025 SEGGER Microcontroller GmbH                  *
*                                                                    *
*       Internet: segger.com  Support: support_embos@segger.com      *
*                                                                    *
**********************************************************************
*                                                                    *
*       embOS-Ultra * Real time operating system                     *
*                                                                    *
*       Please note:                                                 *
*                                                                    *
*       Knowledge of this file may under no circumstances            *
*       be used to write a similar product or a real-time            *
*       operating system for in-house use.                           *
*                                                                    *
*       Thank you for your fairness !                                *
*                                                                    *
**********************************************************************
*                                                                    *
*       OS version: V5.20.0.0                                        *
*                                                                    *
**********************************************************************
multiple prevailing defs for 'runner_get_key'
-------------------------- END-OF-HEADER -----------------------------
File    : BSP.c
Purpose : BSP for ST STM32U575ZI Nucleo
*/


  /* The SystemCoreClock variable is updated in three ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      2) by calling HAL API function HAL_RCC_GetHCLKFreq()
      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
         Note: If you use this function to configure the system clock; then there
               is no need to call the 2 first functions listed above, since SystemCoreClock
               variable is updated automatically.
  */


#include "RTOS.h"
#include "SEGGER_RTT.h" // utilisation SEGGER_RTT_printf
#include "BSP.h"
//#include "main.h"
//#include "db_hardware_def.h"

#include "DBxxxx.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

#define LED0_BIT           (13)     // LD6, Green, PH6
#define LED1_BIT           (13)     // LD7, Red,   PH6
#define LED0_PORT           GPIOC     // 


#define RCC_BASE_ADDR      (0x56020C00u)
#define RCC_AHB2ENR1       (*(volatile unsigned int*)(RCC_BASE_ADDR + 0x8Cu))
#define RCC_GPIOHEN_BIT    (7)
#define RCC_GPIOCEN_BIT    (2)

#define GPIOC_BASE_ADDR    (0x52020800)
#define GPIOH_BASE_ADDR    (0x52021C00)
#define GPIOH_MODER        (*(volatile unsigned int*)(GPIOH_BASE_ADDR + 0x00u))
#define GPIOH_PUPDR        (*(volatile unsigned int*)(GPIOH_BASE_ADDR + 0x0Cu))
#define GPIOH_ODR          (*(volatile unsigned int*)(GPIOH_BASE_ADDR + 0x14u))
#define GPIOH_BSRR         (*(volatile unsigned int*)(GPIOH_BASE_ADDR + 0x18u))
#define GPIOC_MODER        (*(volatile unsigned int*)(GPIOC_BASE_ADDR + 0x00u))
#define GPIOC_PUPDR        (*(volatile unsigned int*)(GPIOC_BASE_ADDR + 0x0Cu))
#define GPIOC_ODR          (*(volatile unsigned int*)(GPIOC_BASE_ADDR + 0x14u))
#define GPIOC_BSRR         (*(volatile unsigned int*)(GPIOC_BASE_ADDR + 0x18u))

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/

/*    from stm32cubemx   */

I2C_HandleTypeDef hi2c1;

extern LPTIM_HandleTypeDef hlptim1;
LPTIM_HandleTypeDef hlptim2;

OSPI_HandleTypeDef hospi1;


//RTC_HandleTypeDef hrtc;

//SPI_HandleTypeDef hspi1;
//SPI_HandleTypeDef hspi2;
//DMA_HandleTypeDef handle_GPDMA1_Channel0;
//DMA_HandleTypeDef handle_GPDMA1_Channel2;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

DCACHE_HandleTypeDef hdcache1;

RAMCFG_HandleTypeDef hramcfg_SRAM1;
RAMCFG_HandleTypeDef hramcfg_SRAM2;
RAMCFG_HandleTypeDef hramcfg_SRAM3;
RAMCFG_HandleTypeDef hramcfg_SRAM4;
RAMCFG_HandleTypeDef hramcfg_BKPRAM;

/* ADC handle declaration */
//ADC_HandleTypeDef hadc1;

void Error_HandlerMsg(char *buff)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
if (buff != NULL) SEGGER_RTT_printf(0, "\nHAL err, %s", buff); 

  /* USER CODE END Error_Handler_Debug */
}




/**
  * @brief System Clock Configuration
  * @retval None
not used

  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
      Error_Handler(1);
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI
                              | RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_MSIK;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON_RTC_ONLY;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_3;
//  RCC_OscInitStruct.MSIKClockRange = RCC_MSIKRANGE_0;
  RCC_OscInitStruct.MSIKClockRange = RCC_MSIKRANGE_2;       // 16Mhz
  RCC_OscInitStruct.MSIKState = RCC_MSIK_ON;

  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLMBOOST = RCC_PLLMBOOST_DIV1;
  RCC_OscInitStruct.PLL.PLLM = 3;
  RCC_OscInitStruct.PLL.PLLN = 80;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLLVCIRANGE_0;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
      Error_Handler(2);
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
      Error_Handler(3);
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLModeSelection(RCC_MSIKPLL_MODE_SEL);
  HAL_RCCEx_EnableMSIPLLMode();
}


/**
  * @brief USB_OTG_FS Initialization Function
  * @param None
  * @retval None
  */
void MX_USB_OTG_FS_PCD_Init(void)
{

  /* USER CODE BEGIN USB_OTG_FS_Init 0 */

  /* USER CODE END USB_OTG_FS_Init 0 */

  /* USER CODE BEGIN USB_OTG_FS_Init 1 */

  /* USER CODE END USB_OTG_FS_Init 1 */
  hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
  hpcd_USB_OTG_FS.Init.dev_endpoints = 6;
  hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_OTG_FS.Init.Sof_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.battery_charging_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
  hpcd_USB_OTG_FS.Init.vbus_sensing_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK)
{
  Error_HandlerMsg("\nHAL USB");
}
 /* USER CODE BEGIN USB_OTG_FS_Init 2 */

  /* USER CODE END USB_OTG_FS_Init 2 */

HAL_PCD_MspInit(&hpcd_USB_OTG_FS); // juste l'horloge et les pins !


}

/**
  * @brief DCACHE1 Initialization Function
  * @param None
  * @retval None
  */
void MX_DCACHE1_Init(void)
{

  /* USER CODE BEGIN DCACHE1_Init 0 */

  /* USER CODE END DCACHE1_Init 0 */

  /* USER CODE BEGIN DCACHE1_Init 1 */

  /* USER CODE END DCACHE1_Init 1 */
  hdcache1.Instance = DCACHE1;
  hdcache1.Init.ReadBurstType = DCACHE_READ_BURST_WRAP;
  if (HAL_DCACHE_Init(&hdcache1) != HAL_OK)
  {
    //Error_Handler();
  }

  /* USER CODE BEGIN DCACHE1_Init 2 */

  /* USER CODE END DCACHE1_Init 2 */

}





/**
  * @brief ICACHE Initialization Function
  * @param None
  * @retval None
  */
void MX_ICACHE_Init(void)
{

  /* USER CODE BEGIN ICACHE_Init 0 */

  /* USER CODE END ICACHE_Init 0 */

  /* USER CODE BEGIN ICACHE_Init 1 */

  /* USER CODE END ICACHE_Init 1 */

  /** Enable instruction cache (default 2-ways set associative cache)
  */
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler(5);
  }
  /* USER CODE BEGIN ICACHE_Init 2 */

  /* USER CODE END ICACHE_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
   __HAL_RCC_GPIOA_CLK_ENABLE();
   __HAL_RCC_GPIOB_CLK_ENABLE();
   __HAL_RCC_GPIOC_CLK_ENABLE();

// PH0 PH1, mode analog, connected to quartz, not used  
/*   __HAL_RCC_GPIOH_CLK_ENABLE();
   GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1; 
   GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
   GPIO_InitStruct.Pull = GPIO_NOPULL;             // Important: no pull-up/down resistors
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;    // Lowest speed for power saving
   HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
   __HAL_RCC_GPIOH_CLK_DISABLE();
*/
   __HAL_RCC_GPIOH_CLK_ENABLE();
   HAL_GPIO_WritePin(GPIOH, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_RESET);
   GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1; 
   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;             // Important: no pull-up/down resistors
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;    // Lowest speed for power saving
   HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
   __HAL_RCC_GPIOH_CLK_DISABLE();


   GPIO_InitStruct.Pin = LED0_BIT; 
   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;    // Lowest speed for power saving
   HAL_GPIO_Init(LED0_PORT, &GPIO_InitStruct);


}




/*********************************************************************
*
*       BSP_SetLED()
*/
void BSP_SetLED(int Index) {
  if (Index == 0) {
    GPIOC_BSRR = (1u << (LED0_BIT + 16));    // Switch LED0 on
  } else if (Index == 1) {
    GPIOC_BSRR = (1u << (LED1_BIT + 16));    // Switch LED1 on
  }
}

/*********************************************************************
*
*       BSP_ClrLED()
*/
void BSP_ClrLED(int Index) {
  if (Index == 0) {
    GPIOC_BSRR = (1u << (LED0_BIT));    // Switch LED0 off
  } else if (Index == 1) {
    GPIOC_BSRR = (1u << (LED1_BIT));    // Switch LED1 off
  }
}

/*********************************************************************
*
*       BSP_ToggleLED()
*/
void BSP_ToggleLED(int Index) {
  if (Index == 0) {
    if (GPIOC_ODR & (1u << LED0_BIT)) {
      GPIOC_BSRR = (1u << (LED0_BIT + 16));  // Switch LED on
    } else {
      GPIOC_BSRR = (1u << LED0_BIT);         // Switch LED off
    }
  } else if (Index == 1) {
    if (GPIOC_ODR & (1u << LED1_BIT)) {
      GPIOC_BSRR = (1u << (LED1_BIT + 16));  // Switch LED on
    } else {
      GPIOC_BSRR = (1u << LED1_BIT);         // Switch LED off
    }
  }
}


/**
  * @brief RAMCFG Initialization Function
  * @param None
  * @retval None
  */
static void MX_RAMCFG_Init(void)
{

  /* USER CODE BEGIN RAMCFG_Init 0 */

  /* USER CODE END RAMCFG_Init 0 */

  /* USER CODE BEGIN RAMCFG_Init 1 */

  /* USER CODE END RAMCFG_Init 1 */

  /** Initialize RAMCFG SRAM1
  */
  hramcfg_SRAM1.Instance = RAMCFG_SRAM1;
  if (HAL_RAMCFG_Init(&hramcfg_SRAM1) != HAL_OK)
  {
    Error_Handler(1);
  }

  /** Initialize RAMCFG SRAM2
  */
  hramcfg_SRAM2.Instance = RAMCFG_SRAM2;
  if (HAL_RAMCFG_Init(&hramcfg_SRAM2) != HAL_OK)
  {
    Error_Handler(2);
  }

  /** Initialize RAMCFG SRAM3
  */
  hramcfg_SRAM3.Instance = RAMCFG_SRAM3;
  if (HAL_RAMCFG_Init(&hramcfg_SRAM3) != HAL_OK)
  {
    Error_Handler(3);
  }

  /** Initialize RAMCFG SRAM4
  */
  hramcfg_SRAM4.Instance = RAMCFG_SRAM4;
  if (HAL_RAMCFG_Init(&hramcfg_SRAM4) != HAL_OK)
  {
    Error_Handler(4);
  }

  /** Initialize RAMCFG BKPRAM
  */
  hramcfg_BKPRAM.Instance = RAMCFG_BKPRAM;
  if (HAL_RAMCFG_Init(&hramcfg_BKPRAM) != HAL_OK)
  {
    Error_Handler(5);
  }
  /* USER CODE BEGIN RAMCFG_Init 2 */

  /* USER CODE END RAMCFG_Init 2 */

}




/*********************************************************************
*
*       BSP_Init()
*/
void BSP_Init(void) {


   MX_GPIO_Init();

   MX_ICACHE_Init();
// no external ram
//   MX_DCACHE1_Init();    
   MX_RAMCFG_Init();
}


/*************************** End of file ****************************/
