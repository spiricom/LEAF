/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "bdma.h"
#include "dma.h"
#include "fatfs.h"
#include "i2c.h"
#include "rng.h"
#include "sai.h"
#include "sdmmc.h"
#include "tim.h"
#include "usb_host.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ui.h"
#include "leaf.h"
#include "audiostream.h"
#include "eeprom.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t count;

//FLASH storage EEPROM emulation variables
FLASH_OBProgramInitTypeDef OBInit;
uint16_t VarDataTab = 0;
uint16_t VarValue = 0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_USB_HOST_Process(void);

/* USER CODE BEGIN PFP */
void MPU_Conf(void);
void SDRAM_Initialization_sequence(void);
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
  MPU_Conf();
  /* USER CODE END 1 */
  

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  //disabling I and D cache because they cause issues with the USB initialization when -o3 optimization is on
  /* Enable I-Cache---------------------------------------------------------*/
  SCB_DisableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_DisableDCache();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */


  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_BDMA_Init();
  MX_DMA_Init();
  MX_FMC_Init();
  MX_ADC1_Init();
  MX_I2C2_Init();
  MX_SDMMC1_SD_Init();
  MX_FATFS_Init();
  MX_SAI1_Init();
  MX_RNG_Init();
  MX_I2C4_Init();
  MX_USB_HOST_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  /// it seems we need to enable caching after setting up the USB Host Controller -
  // otherwise turning on -o3 optimization causes unreliable behavior where it's not set up correctly and never reaches the USB interrupt for connection
  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();


  //HAL_Delay(1);
  // Emulated EEPROM Init
  HAL_FLASH_Unlock();
  if( EE_Init() != EE_OK)
  {
    Error_Handler();
  }
  if((EE_ReadVariable(VirtAddVarTab[0],  &VarDataTab)) != HAL_OK) // read what the preset was before last power-off
  {
	//if it can't read something, it's probably because this brain has never been programmed, so write a value in there to start with
	  if((EE_WriteVariable(VirtAddVarTab[0],  0)) != HAL_OK)
	{
		Error_Handler();
	}
  }
  if (VarDataTab < PresetNil) //make sure the stored data is a number not past the number of available presets
  {
	  currentPreset = VarDataTab; //if it's good, start at that remembered preset number
  }
  else
  {
	  currentPreset = 0; //if the data is messed up for some reason, just initialize at the first preset (preset 0)
  }





  //pull reset pin on audio codec low to make sure it's stable
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);

  uint32_t tempFPURegisterVal = __get_FPSCR();
  tempFPURegisterVal |= (1<<24); // set the FTZ (flush-to-zero) bit in the FPU control register
  __set_FPSCR(tempFPURegisterVal);


  HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);

  if (HAL_ADC_Start_DMA(&hadc1,(uint32_t*)&ADC_values, NUM_ADC_CHANNELS) != HAL_OK)
	{
	  Error_Handler();
	}

  HAL_Delay(10);
  OLED_init(&hi2c4);

  //HAL_Delay(10);


  SDRAM_Initialization_sequence();

  audioInit(&hi2c2, &hsai_BlockA1, &hsai_BlockB1);


  OLED_writePreset();

  /* USER CODE END 2 */
 
 

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    MX_USB_HOST_Process();

    /* USER CODE BEGIN 3 */

	if (hi2c4.State == HAL_I2C_STATE_READY)
	{
	  OLED_draw();
	}

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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Supply configuration update enable 
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Configure the main internal regulator output voltage 
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Macro to configure the PLL clock source 
  */
  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 10;
  RCC_OscInitStruct.PLL.PLLN = 384;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_1;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RNG|RCC_PERIPHCLK_SAI1
                              |RCC_PERIPHCLK_SDMMC|RCC_PERIPHCLK_I2C2
                              |RCC_PERIPHCLK_ADC|RCC_PERIPHCLK_I2C4
                              |RCC_PERIPHCLK_USB|RCC_PERIPHCLK_FMC;
  PeriphClkInitStruct.PLL2.PLL2M = 25;
  PeriphClkInitStruct.PLL2.PLL2N = 344;
  PeriphClkInitStruct.PLL2.PLL2P = 7;
  PeriphClkInitStruct.PLL2.PLL2Q = 2;
  PeriphClkInitStruct.PLL2.PLL2R = 1;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_0;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.PLL3.PLL3M = 25;
  PeriphClkInitStruct.PLL3.PLL3N = 384;
  PeriphClkInitStruct.PLL3.PLL3P = 2;
  PeriphClkInitStruct.PLL3.PLL3Q = 8;
  PeriphClkInitStruct.PLL3.PLL3R = 2;
  PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_0;
  PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
  PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
  PeriphClkInitStruct.FmcClockSelection = RCC_FMCCLKSOURCE_D1HCLK;
  PeriphClkInitStruct.SdmmcClockSelection = RCC_SDMMCCLKSOURCE_PLL;
  PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLL2;
  PeriphClkInitStruct.RngClockSelection = RCC_RNGCLKSOURCE_HSI48;
  PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1;
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL3;
  PeriphClkInitStruct.I2c4ClockSelection = RCC_I2C4CLKSOURCE_D3PCLK1;
  PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable USB Voltage detector 
  */
  HAL_PWREx_EnableUSBVoltageDetector();
}

/* USER CODE BEGIN 4 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	//HAL_MDMA_Start_IT(&hmdma_mdma_channel40_dma1_stream0_tc_0, (uint32_t)&ADC_valuesDMA, (uint32_t)&ADC_values, 10, 1);
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
	;
}

#define SDRAM_TIMEOUT ((uint32_t)0xFFFF)

#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0003)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

//#define SDRAM_REFRESH_COUNT                   	 ((uint32_t)956)// 7.9us in cycles of 8.333333ns + 20 cycles as recommended by datasheet page 866/3289 for STM32H743
#define SDRAM_REFRESH_COUNT                   	 ((uint32_t)0x0569)// 7.9us in cycles of 8.333333ns + 20 cycles as recommended by datasheet page 866/3289 for STM32H743
void SDRAM_Initialization_sequence(void)
{
    __IO uint32_t tmpmrd = 0;
    FMC_SDRAM_CommandTypeDef Command;
    /* Step 1: Configure a clock configuration enable command */
    Command.CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
    Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition = 0;

    /* Send the command */
    HAL_SDRAM_SendCommand(&hsdram1, &Command, SDRAM_TIMEOUT);

    /* Step 2: Insert 100 us minimum delay */
    /* Inserted delay is equal to 1 ms due to systick time base unit (ms) */
    HAL_Delay(1);

    /* Step 3: Configure a PALL (precharge all) command */
    Command.CommandMode = FMC_SDRAM_CMD_PALL;
    Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition = 0;

    /* Send the command */
    HAL_SDRAM_SendCommand(&hsdram1, &Command, SDRAM_TIMEOUT);

    /* Step 5: Program the external memory mode register */
    tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_4 | SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL
        | SDRAM_MODEREG_CAS_LATENCY_2 | SDRAM_MODEREG_OPERATING_MODE_STANDARD
        | SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

    Command.CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
    Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition = tmpmrd;

    /* Send the command */
    HAL_SDRAM_SendCommand(&hsdram1, &Command, SDRAM_TIMEOUT);

    /* Step 4: Configure the 1st Auto Refresh command */
    Command.CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
    Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
    Command.AutoRefreshNumber = 8;
    Command.ModeRegisterDefinition = 0;

    /* Send the command */
    HAL_SDRAM_SendCommand(&hsdram1, &Command, SDRAM_TIMEOUT);

    /* Step 2: Insert 100 us minimum delay */
    /* Inserted delay is equal to 1 ms due to systick time base unit (ms) */
    HAL_Delay(1);

    /* Step 5: Configure the 2nd Auto Refresh command */
    Command.CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
    Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
    Command.AutoRefreshNumber = 8;
    Command.ModeRegisterDefinition = 0;

    /* Send the command */
    HAL_SDRAM_SendCommand(&hsdram1, &Command, SDRAM_TIMEOUT);

    /* Step 6: Set the refresh rate counter */
    /* Set the device refresh rate */
    HAL_SDRAM_ProgramRefreshRate(&hsdram1, SDRAM_REFRESH_COUNT);
}

float randomNumber(void) {

	uint32_t rand;
	HAL_RNG_GenerateRandomNumber(&hrng, &rand);
	float num = (float)rand * INV_TWO_TO_32;
	return num;
}


void MPU_Conf(void)
{
	//code from Keshikan https://github.com/keshikan/STM32H7_DMA_sample
  //Thanks, Keshikan! This solves the issues with accessing the SRAM in the D2 area properly. -JS
	//should test the different possible settings to see what works best while avoiding needing to manually clear the cache -JS

	MPU_Region_InitTypeDef MPU_InitStruct;

	HAL_MPU_Disable();

  //currently leaving D1 SRAM not configured by the MPU - just set as normal default memory.

	//the following code configures D2 and D3 SRAM

  MPU_InitStruct.Enable = MPU_REGION_ENABLE;

  //D2 Domain�SRAM1
  MPU_InitStruct.BaseAddress = 0x30000000;
  // Increased region size to 256k. In Keshikan's code, this was 512 bytes (that's all that application needed).
  // Each audio buffer takes up the frame size * 8 (16 bits makes it *2 and stereo makes it *2 and double buffering makes it *2)
  // So a buffer size for read/write of 4096 would take up 64k = 4096*8 * 2 (read and write).
  // I increased that to 256k so that there would be room for the ADC knob inputs and other peripherals that might require DMA access.
  // we have a total of 256k in SRAM1 (128k, 0x30000000-0x30020000) and SRAM2 (128k, 0x30020000-0x3004000) of D2 domain.
  // There is an SRAM3 in D2 domain as well (32k, 0x30040000-0x3004800) that is currently not mapped by the MPU (memory protection unit) controller.

  MPU_InitStruct.Size = MPU_REGION_SIZE_256KB;

  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;

  //AN4838
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;

  //Shared Device
	  //MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
	  //MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
	  //MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
	  //MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;


  MPU_InitStruct.Number = MPU_REGION_NUMBER1;

  MPU_InitStruct.SubRegionDisable = 0x00;


  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;


  HAL_MPU_ConfigRegion(&MPU_InitStruct);


  //now set up D3 domain RAM

  MPU_InitStruct.Enable = MPU_REGION_ENABLE;

  //D3 Domain�SRAM1
  MPU_InitStruct.BaseAddress = 0x38000000;


  MPU_InitStruct.Size = MPU_REGION_SIZE_64KB;

  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;

  //AN4838
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;

  //Shared Device
//	  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
//	  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
//	  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
//	  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;


  MPU_InitStruct.Number = MPU_REGION_NUMBER2;

  MPU_InitStruct.SubRegionDisable = 0x00;


  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;


  HAL_MPU_ConfigRegion(&MPU_InitStruct);


  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}


volatile uint32_t r0;
volatile uint32_t r1;
volatile uint32_t r2;
volatile uint32_t r3;
volatile uint32_t r12;
volatile uint32_t lr; // Link register.
volatile uint32_t pc; // Program counter.
volatile uint32_t psr;// Program status register.

/*
	static void HardFault_Handler(void)
	{
	    __asm volatile
	    (
	        " tst lr, #4                                                n"
	        " ite eq                                                    n"
	        " mrseq r0, msp                                             n"
	        " mrsne r0, psp                                             n"
	        " ldr r1, [r0, #24]                                         n"
	        " ldr r2, handler2_address_const                            n"
	        " bx r2                                                     n"
	        " handler2_address_const: .word prvGetRegistersFromStack    n"
	    );
	}

*/
void prvGetRegistersFromStack( uint32_t *pulFaultStackAddress )
{
//These are volatile to try and prevent the compiler/linker optimising them
//away as the variables never actually get used.  If the debugger won't show the
//values of the variables, make them global by moving their declaration outside
//of this function.



	r0 = pulFaultStackAddress[ 0 ];
	r1 = pulFaultStackAddress[ 1 ];
	r2 = pulFaultStackAddress[ 2 ];
	r3 = pulFaultStackAddress[ 3 ];

	r12 = pulFaultStackAddress[ 4 ];
	lr = pulFaultStackAddress[ 5 ];
	pc = pulFaultStackAddress[ 6 ];
	psr = pulFaultStackAddress[ 7 ];

	// When the following line is hit, the variables contain the register values.
	for( ;; );
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
while(1)
{
;
}
/* The prototype shows it is a naked function - in effect this is just an
assembly function. */
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
