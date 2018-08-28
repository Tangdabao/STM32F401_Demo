#ifndef __LSM6DSL_ACC_GYRO_DATA__H
#define __LSM6DSL_ACC_GYRO_DATA__H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "LSM6DSL_ACC_GYRO_driver.h" 

/** @addtogroup LSM6DSL_ACC_SENSITIVITY Accelero sensitivity values based on selected full scale
 * @{
 */

#define LSM6DSL_ACC_SENSITIVITY_FOR_FS_2G   0.061  /**< Sensitivity value for 2 g full scale [mg/LSB] */
#define LSM6DSL_ACC_SENSITIVITY_FOR_FS_4G   0.122  /**< Sensitivity value for 4 g full scale [mg/LSB] */
#define LSM6DSL_ACC_SENSITIVITY_FOR_FS_8G   0.244  /**< Sensitivity value for 8 g full scale [mg/LSB] */
#define LSM6DSL_ACC_SENSITIVITY_FOR_FS_16G  0.488  /**< Sensitivity value for 16 g full scale [mg/LSB] */

/**
 * @}
 */
 
/** @addtogroup LSM6DSL_GYRO_SENSITIVITY Gyro sensitivity values based on selected full scale
 * @{
 */

#define LSM6DSL_GYRO_SENSITIVITY_FOR_FS_125DPS   04.375  /**< Sensitivity value for 125 dps full scale [mdps/LSB] */
#define LSM6DSL_GYRO_SENSITIVITY_FOR_FS_245DPS   08.750  /**< Sensitivity value for 245 dps full scale [mdps/LSB] */
#define LSM6DSL_GYRO_SENSITIVITY_FOR_FS_500DPS   17.500  /**< Sensitivity value for 500 dps full scale [mdps/LSB] */
#define LSM6DSL_GYRO_SENSITIVITY_FOR_FS_1000DPS  35.000  /**< Sensitivity value for 1000 dps full scale [mdps/LSB] */
#define LSM6DSL_GYRO_SENSITIVITY_FOR_FS_2000DPS  70.000  /**< Sensitivity value for 2000 dps full scale [mdps/LSB] */

/**
 * @}
 */

typedef struct
{
  int32_t AXIS_X;
  int32_t AXIS_Y;
  int32_t AXIS_Z;
} SensorAxes_t;

void init_LSM6DSL_ACC(void *handle);
void init_LSM6DSL_GYRO(void *handle);
status_t LSM6DSL_Detection(void *handle);
void LSM6DSL_ACC_data(void *handle,SensorAxes_t *acceleration);
status_t LSM6DSL_X_Get_Axes( void *handle, SensorAxes_t *acceleration );
status_t LSM6DSL_G_Get_Axes( void *handle, SensorAxes_t *angular_velocity );
#endif
