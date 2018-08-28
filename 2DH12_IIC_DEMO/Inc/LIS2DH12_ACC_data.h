#ifndef __LIS2DH12_ACC_DATA__H
#define __LIS2DH12_ACC_DATA__H
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "LIS2DH12_ACC_driver.h"

/** @addtogroup LIS2DH12_ACC_SENSITIVITY Accelero sensitivity values based on selected full scale
 * @{
 */

#define LIS2DH12_ACC_SENSITIVITY_FOR_FS_2G   0.0625//0.061  /**< Sensitivity value for 2 g full scale [mg/LSB] */
#define LIS2DH12_ACC_SENSITIVITY_FOR_FS_4G   0.125//0.122  /**< Sensitivity value for 4 g full scale [mg/LSB] */
#define LIS2DH12_ACC_SENSITIVITY_FOR_FS_8G   0.25//0.244  /**< Sensitivity value for 8 g full scale [mg/LSB] */
#define LIS2DH12_ACC_SENSITIVITY_FOR_FS_16G  0.75//0.488  /**< Sensitivity value for 16 g full scale [mg/LSB] */

/**
 * @}
 */
 
typedef struct
{
  int32_t AXIS_X;
  int32_t AXIS_Y;
  int32_t AXIS_Z;
} SensorAxes_t;


status_t LIS2DH12_Detection(void *handle);
void init_LIS2DH12_ACC(void *handle);
status_t LIS2DH12_X_Get_Axes( void *handle, SensorAxes_t *acceleration );
#endif
