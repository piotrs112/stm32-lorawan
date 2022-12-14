/******************************************************************************
 * @file    bsp.c
 * @author  MCD Application Team
 * @version V1.1.1
 * @date    01-June-2017
 * @brief   manages the sensors on the application
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
 * All rights reserved.</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
  
  /* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include "hw_conf.h"
#include "bsp.h"

#if ( defined(USE_MDM32L07X01) && defined(SENSOR_ENABLED) )
#if defined (X_NUCLEO_IKS01A1)
#warning "Do not forget to select X_NUCLEO_IKS01A1 files group instead of X_NUCLEO_IKS01A2"
#include "x_nucleo_iks01a1_humidity.h"
#include "x_nucleo_iks01a1_pressure.h"
#include "x_nucleo_iks01a1_temperature.h"
#else  /* not X_NUCLEO_IKS01A1 */
#include "x_nucleo_iks01a2_humidity.h"
#include "x_nucleo_iks01a2_pressure.h"
#include "x_nucleo_iks01a2_temperature.h"
#endif  /* X_NUCLEO_IKS01A1 */
#elif USE_I_NUCLEO_LRWAN1
#include "i_nucleo_lrwan1_humidity.h"
#include "i_nucleo_lrwan1_pressure.h"
#include "i_nucleo_lrwan1_temperature.h"
#endif


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define STSOP_LATTITUDE ((float) 43.618622 )
#define STSOP_LONGITUDE ((float) 7.051415  )
#define MAX_GPS_POS ((int32_t) 8388607  ) // 2^23 - 1
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

#define LEDn_MODEM                              1

/* GPIO configuration for I_NUCLEO_LRWAN1 modem's led*/
GPIO_TypeDef* LED_MODEM_PORT[LEDn_MODEM] = {GPIOA};

const uint16_t LED_MODEM_PIN[LEDn_MODEM] = {GPIO_PIN_0};


#ifdef SENSOR_ENABLED
void *HUMIDITY_handle = NULL;
void *TEMPERATURE_handle = NULL;
void *PRESSURE_handle = NULL;
#endif

void BSP_sensor_Read( sensor_t *sensor_data)
{

  float HUMIDITY_Value = 0;
  float TEMPERATURE_Value = 0;
  float PRESSURE_Value = 0;

#ifdef SENSOR_ENABLED
  BSP_HUMIDITY_Get_Hum(HUMIDITY_handle, &HUMIDITY_Value);
  BSP_TEMPERATURE_Get_Temp(TEMPERATURE_handle, &TEMPERATURE_Value);
  BSP_PRESSURE_Get_Press(PRESSURE_handle, &PRESSURE_Value);
#endif  
  sensor_data->humidity    = HUMIDITY_Value;
  sensor_data->temperature = TEMPERATURE_Value;
  sensor_data->pressure    = PRESSURE_Value;
  
  sensor_data->latitude  = (int32_t) ((STSOP_LATTITUDE  * MAX_GPS_POS) /90);
  sensor_data->longitude = (int32_t) ((STSOP_LONGITUDE  * MAX_GPS_POS )/180);

}

void BSP_sensor_Init( void  )
{

#ifdef SENSOR_ENABLED
  /* Initialize sensors */
  BSP_HUMIDITY_Init( HTS221_H_0, &HUMIDITY_handle );
  BSP_TEMPERATURE_Init( HTS221_T_0, &TEMPERATURE_handle );
  BSP_PRESSURE_Init( PRESSURE_SENSORS_AUTO, &PRESSURE_handle );
  
  /* Enable sensors */
  BSP_HUMIDITY_Sensor_Enable( HUMIDITY_handle );
  BSP_TEMPERATURE_Sensor_Enable( TEMPERATURE_handle );
  BSP_PRESSURE_Sensor_Enable( PRESSURE_handle );
#endif

}


/**
  * @brief  Configures LED GPIO for external modem.
  * @param  Led: Led to be configured. 
  * @retval None
  */
void BSP_LED_Modem_Init(Led_TypeDef Led)
{
  GPIO_InitTypeDef  gpioinitstruct;
  
  /* Enable the GPIO_LED Clock */
  LEDx_GPIO_CLK_ENABLE(Led);

  /* Configure the GPIO_LED pin */
  gpioinitstruct.Pin = LED_MODEM_PIN[Led];
  gpioinitstruct.Mode = GPIO_MODE_OUTPUT_PP;
  gpioinitstruct.Pull = GPIO_NOPULL;
  gpioinitstruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  
  HAL_GPIO_Init(LED_MODEM_PORT[Led], &gpioinitstruct);

  /* Reset PIN to switch off the LED */
  HAL_GPIO_WritePin(LED_MODEM_PORT[Led], LED_MODEM_PIN[Led], GPIO_PIN_RESET); 
}



/**
  * @brief  DeInit LEDs GPIO for external modem.
  * @param  Led: LED to be de-init. 
  * @note Led DeInit does not disable the GPIO clock nor disable the Mfx 
  * @retval None
  */
void BSP_LED_Modem_DeInit(Led_TypeDef Led)
{
  GPIO_InitTypeDef  gpio_init_structure;

  /* Turn off LED */
  HAL_GPIO_WritePin(LED_MODEM_PORT[Led],LED_MODEM_PIN[Led], GPIO_PIN_RESET);
  /* DeInit the GPIO_LED pin */
  gpio_init_structure.Pin = LED_MODEM_PIN[Led];
  HAL_GPIO_DeInit(LED_MODEM_PORT[Led], gpio_init_structure.Pin);
}


/**
  * @brief  Turns selected LED Modem On.
  * @param  Led: Specifies the Led to be set on. 
  * @retval None
  */
void BSP_LED_Modem_On(Led_TypeDef Led)
{
  HAL_GPIO_WritePin(LED_MODEM_PORT[Led], LED_MODEM_PIN[Led], GPIO_PIN_SET); 
}

/**
  * @brief  Turns selected LED Modem Off. 
  * @param  Led: Specifies the Led to be set off. 
  * @retval None
  */
void BSP_LED_Modem_Off(Led_TypeDef Led)
{
  HAL_GPIO_WritePin(LED_MODEM_PORT[Led], LED_MODEM_PIN[Led], GPIO_PIN_RESET); 
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
