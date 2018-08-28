#include "LSM6DSL_ACC_GYRO_data.h"
status_t response=MEMS_SUCCESS;

/* 6dsl Accelerometer test parameters */
static struct {
  u8_t				t_enable;
  LSM6DSL_ACC_GYRO_ODR_XL_t	t_odr;
  u16_t				t_odr_hz_val;
  LSM6DSL_ACC_GYRO_FS_XL_t	t_fs;
  u8_t                          t_decimation;
  u8_t                          t_samples_num_in_pattern;
} test_6dsl_xl = {
                  0,
                  LSM6DSL_ACC_GYRO_ODR_XL_416Hz,
                  0,
                  LSM6DSL_ACC_GYRO_FS_XL_2g,
                  0,
                  0,
                 };

								 
/* 6dsl Gyroscope test parameters */
static struct {
  u8_t				t_enable;
  LSM6DSL_ACC_GYRO_ODR_G_t	t_odr;
  u16_t				t_odr_hz_val;
  LSM6DSL_ACC_GYRO_FS_G_t	t_fs;
  u8_t                          t_decimation;
  u8_t                          t_samples_num_in_pattern;
} test_6dsl_gyro = {
                  0,
                  LSM6DSL_ACC_GYRO_ODR_G_104Hz,
                  0,
                  LSM6DSL_ACC_GYRO_FS_G_2000dps,
                  0,
                  0,
                };

								
/* Read Device id */
status_t LSM6DSL_Detection(void *handle)
{
	response=MEMS_SUCCESS;
	uint8_t Device_ID;
	response=LSM6DSL_ACC_GYRO_R_WHO_AM_I(handle,&Device_ID);	
	if((response ==MEMS_SUCCESS)&&(Device_ID ==LSM6DSL_ACC_GYRO_WHO_AM_I))
    return MEMS_SUCCESS;
 else
    return MEMS_ERROR;
}
	


								
/* Init the Accelerometer */
void init_LSM6DSL_ACC(void *handle)
{
  response=MEMS_SUCCESS;
  /* Set ACC ODR  */
  response = LSM6DSL_ACC_GYRO_W_ODR_XL(handle,test_6dsl_xl.t_odr);
  if(response==MEMS_ERROR) while(1); //manage here comunication error
  
  /* Set ACC full scale */
  response = LSM6DSL_ACC_GYRO_W_FS_XL(handle, test_6dsl_xl.t_fs);
  if(response==MEMS_ERROR) while(1); //manage here comunication error

  /* BDU Enable */
  response = LSM6DSL_ACC_GYRO_W_BDU(handle, LSM6DSL_ACC_GYRO_BDU_BLOCK_UPDATE);
  if(response==MEMS_ERROR) while(1); //manage here comunication error
  test_6dsl_gyro.t_enable= 1 ;	
}




/* Init the Gyroscope */
void init_LSM6DSL_GYRO(void *handle)
{
  response=MEMS_SUCCESS;
  /* Gyro ODR and full scale */
  response = LSM6DSL_ACC_GYRO_W_ODR_G(handle, test_6dsl_gyro.t_odr);
  if(response==MEMS_ERROR) while(1); //manage here comunication error

  response = LSM6DSL_ACC_GYRO_W_FS_G(handle, test_6dsl_gyro.t_fs);
  if(response==MEMS_ERROR) while(1); //manage here comunication error

  /* Enable gyro */
  test_6dsl_gyro.t_enable = 1;

}






/**
 * @brief Get the LSM6DSL accelerometer sensor raw axes
 * @param handle the device handle
 * @param pData pointer where the raw values of the axes are written
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
status_t LSM6DSL_X_Get_Axes_Raw( void *handle, int16_t *pData)
{

  uint8_t regValue[6] = {0, 0, 0, 0, 0, 0};

  /* Read output registers from LSM6DSL_ACC_GYRO_OUTX_L_XL to LSM6DSL_ACC_GYRO_OUTZ_H_XL. */
  if ( LSM6DSL_ACC_GYRO_GetRawAccData(handle, ( uint8_t* )regValue ) == MEMS_ERROR )
  {
    return MEMS_ERROR;
  }

  /* Format the data. */
  pData[0] = ( ( ( ( int16_t )regValue[1] ) << 8 ) + ( int16_t )regValue[0] );
  pData[1] = ( ( ( ( int16_t )regValue[3] ) << 8 ) + ( int16_t )regValue[2] );
  pData[2] = ( ( ( ( int16_t )regValue[5] ) << 8 ) + ( int16_t )regValue[4] );

  return MEMS_SUCCESS;
}

/**
 * @brief Get the LSM6DSL accelerometer sensor sensitivity
 * @param handle the device handle
 * @param sensitivity pointer where the sensitivity value is written
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
status_t LSM6DSL_X_Get_Sensitivity( void *handle, float *sensitivity )
{

  LSM6DSL_ACC_GYRO_FS_XL_t fullScale;

  /* Read actual full scale selection from sensor. */
  if ( LSM6DSL_ACC_GYRO_R_FS_XL( (void *)handle, &fullScale ) == MEMS_ERROR )
  {
    return MEMS_ERROR;
  }

  /* Store the sensitivity based on actual full scale. */
  switch( fullScale )
  {
    case LSM6DSL_ACC_GYRO_FS_XL_2g:
      *sensitivity = ( float )LSM6DSL_ACC_SENSITIVITY_FOR_FS_2G;
      break;
    case LSM6DSL_ACC_GYRO_FS_XL_4g:
      *sensitivity = ( float )LSM6DSL_ACC_SENSITIVITY_FOR_FS_4G;
      break;
    case LSM6DSL_ACC_GYRO_FS_XL_8g:
      *sensitivity = ( float )LSM6DSL_ACC_SENSITIVITY_FOR_FS_8G;
      break;
    case LSM6DSL_ACC_GYRO_FS_XL_16g:
      *sensitivity = ( float )LSM6DSL_ACC_SENSITIVITY_FOR_FS_16G;
      break;
    default:
      *sensitivity = -1.0f;
      return MEMS_ERROR;
  }

  return MEMS_SUCCESS;
}





status_t LSM6DSL_X_Get_Axes( void *handle, SensorAxes_t *acceleration )
{

  int16_t dataRaw[3];
  float sensitivity = 0;

  /* Read raw data from LSM6DSL output register. */
  if ( LSM6DSL_X_Get_Axes_Raw( handle, dataRaw ) == MEMS_ERROR )
  {
    return MEMS_ERROR;
  }

  /* Get LSM6DSL actual sensitivity. */
  if ( LSM6DSL_X_Get_Sensitivity( handle, &sensitivity ) == MEMS_ERROR )
  {
    return MEMS_ERROR;
  }

  /* Calculate the data. */
  acceleration->AXIS_X = ( int32_t )( dataRaw[0] * sensitivity );
  acceleration->AXIS_Y = ( int32_t )( dataRaw[1] * sensitivity );
  acceleration->AXIS_Z = ( int32_t )( dataRaw[2] * sensitivity );

  return MEMS_SUCCESS;
}




/**
 * @brief Get the LSM6DSL gyroscope sensor raw axes
 * @param handle the device handle
 * @param pData pointer where the raw values of the axes are written
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
status_t LSM6DSL_G_Get_Axes_Raw(void *handle, int16_t *pData)
{

  uint8_t regValue[6] = {0, 0, 0, 0, 0, 0};

  /* Read output registers from LSM6DSL_ACC_GYRO_OUTX_L_G to LSM6DSL_ACC_GYRO_OUTZ_H_G. */
  if ( LSM6DSL_ACC_GYRO_GetRawGyroData(handle, ( uint8_t* )regValue ) == MEMS_ERROR )
  {
    return MEMS_ERROR;
  }

  /* Format the data. */
  pData[0] = ( ( ( ( int16_t )regValue[1] ) << 8 ) + ( int16_t )regValue[0] );
  pData[1] = ( ( ( ( int16_t )regValue[3] ) << 8 ) + ( int16_t )regValue[2] );
  pData[2] = ( ( ( ( int16_t )regValue[5] ) << 8 ) + ( int16_t )regValue[4] );

  return MEMS_SUCCESS;
}



/**
 * @brief Get the LSM6DSL gyroscope sensor sensitivity
 * @param handle the device handle
 * @param sensitivity pointer where the sensitivity value is written
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
status_t LSM6DSL_G_Get_Sensitivity( void *handle, float *sensitivity )
{

  LSM6DSL_ACC_GYRO_FS_125_t fullScale125;
  LSM6DSL_ACC_GYRO_FS_G_t   fullScale;

  /* Read full scale 125 selection from sensor. */
  if ( LSM6DSL_ACC_GYRO_R_FS_125( (void *)handle, &fullScale125 ) == MEMS_ERROR )
  {
    return MEMS_ERROR;
  }

  if ( fullScale125 == LSM6DSL_ACC_GYRO_FS_125_ENABLED )
  {
    *sensitivity = ( float )LSM6DSL_GYRO_SENSITIVITY_FOR_FS_125DPS;
  }

  else
  {

    /* Read actual full scale selection from sensor. */
    if ( LSM6DSL_ACC_GYRO_R_FS_G( (void *)handle, &fullScale ) == MEMS_ERROR )
    {
      return MEMS_ERROR;
    }

    /* Store the sensitivity based on actual full scale. */
    switch( fullScale )
    {
      case LSM6DSL_ACC_GYRO_FS_G_245dps:
        *sensitivity = ( float )LSM6DSL_GYRO_SENSITIVITY_FOR_FS_245DPS;
        break;
      case LSM6DSL_ACC_GYRO_FS_G_500dps:
        *sensitivity = ( float )LSM6DSL_GYRO_SENSITIVITY_FOR_FS_500DPS;
        break;
      case LSM6DSL_ACC_GYRO_FS_G_1000dps:
        *sensitivity = ( float )LSM6DSL_GYRO_SENSITIVITY_FOR_FS_1000DPS;
        break;
      case LSM6DSL_ACC_GYRO_FS_G_2000dps:
        *sensitivity = ( float )LSM6DSL_GYRO_SENSITIVITY_FOR_FS_2000DPS;
        break;
      default:
        *sensitivity = -1.0f;
        return MEMS_ERROR;
    }
  }

  return MEMS_SUCCESS;
}



status_t LSM6DSL_G_Get_Axes( void *handle, SensorAxes_t *angular_velocity )
{

  int16_t dataRaw[3];
  float   sensitivity = 0;

  /* Read raw data from LSM6DSL output register. */
  if ( LSM6DSL_G_Get_Axes_Raw( handle, dataRaw ) == MEMS_ERROR )
  {
    return MEMS_ERROR;
  }

  /* Get LSM6DSL actual sensitivity. */
  if ( LSM6DSL_G_Get_Sensitivity( handle, &sensitivity ) == MEMS_ERROR )
  {
    return MEMS_ERROR;
  }

  /* Calculate the data. */
  angular_velocity->AXIS_X = ( int32_t )( dataRaw[0] * sensitivity );
  angular_velocity->AXIS_Y = ( int32_t )( dataRaw[1] * sensitivity );
  angular_velocity->AXIS_Z = ( int32_t )( dataRaw[2] * sensitivity );

  return MEMS_SUCCESS;
}

