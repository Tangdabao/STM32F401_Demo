#include "LIS2DH12_ACC_data.h"

status_t response = MEMS_SUCCESS;

/* 6dsl Accelerometer test parameters */
static struct
{
    u8_t				t_enable;
    LIS2DH12_ODR_t	t_odr;
    LIS2DH12_Mode_t	t_mode;
    LIS2DH12_Fullscale_t	t_fs;
} test_lis2dh12_xl =
{
    0,
    LIS2DH12_ODR_100Hz,
    LIS2DH12_HIGH_RES,
    LIS2DH12_FULLSCALE_2,
};

/* Read Device id */
status_t LIS2DH12_Detection(void *handle)
{
    response = MEMS_SUCCESS;
    uint8_t Device_ID = 0;
    response = LIS2DH12_GetWHO_AM_I(handle, &Device_ID);
    if((response == MEMS_SUCCESS) && (Device_ID == LIS2DH12_WHO_AM_I))
        return MEMS_SUCCESS;
    else
        return MEMS_ERROR;
}

/* Init the Accelerometer */
void init_LIS2DH12_ACC(void *handle)
{
    response = MEMS_SUCCESS;
    //Inizialize MEMS Sensor
    //set ODR (turn ON device)
    response = LIS2DH12_SetODR(handle, test_lis2dh12_xl.t_odr);
    if(response == MEMS_ERROR) while(1); //manage here comunication error
    //set PowerMode
    response = LIS2DH12_SetMode(handle, test_lis2dh12_xl.t_mode);
    if(response == MEMS_ERROR) while(1); //manage here comunication error
    //set Fullscale
    response = LIS2DH12_SetFullScale(handle, test_lis2dh12_xl.t_fs);
    if(response == MEMS_ERROR) while(1); //manage here comunication error
	
    test_lis2dh12_xl.t_enable = 1 ;

}
/**
 * @brief Get the LIS2DH12 accelerometer sensor raw axes
 * @param handle the device handle
 * @param pData pointer where the raw values of the axes are written
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
status_t LIS2DH12_X_Get_Axes_Raw( void *handle, int16_t *pData)
{

    uint8_t regValue[6] = {0, 0, 0, 0, 0, 0};

    if ( LIS2DH12_GetAccAxesRaw(handle, ( uint8_t * )regValue ) == MEMS_ERROR )
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
 * @brief Get the LIS2DH12 accelerometer sensor sensitivity
 * @param handle the device handle
 * @param sensitivity pointer where the sensitivity value is written
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
status_t LIS2DH12_X_Get_Sensitivity( void *handle, float *sensitivity )
{

    LIS2DH12_Fullscale_t fullScale;
    /* Read actual full scale selection from sensor. */
    if ( LIS2DH12_GetFullScale( (void *)handle, &fullScale ) == MEMS_ERROR )
    {
        return MEMS_ERROR;
    }

    /* Store the sensitivity based on actual full scale. */
    switch( fullScale )
    {
        case LIS2DH12_FULLSCALE_2:
            *sensitivity = ( float )LIS2DH12_ACC_SENSITIVITY_FOR_FS_2G;
            break;
        case LIS2DH12_FULLSCALE_4:
            *sensitivity = ( float )LIS2DH12_ACC_SENSITIVITY_FOR_FS_4G;
            break;
        case LIS2DH12_FULLSCALE_8:
            *sensitivity = ( float )LIS2DH12_ACC_SENSITIVITY_FOR_FS_8G;
            break;
        case LIS2DH12_FULLSCALE_16:
            *sensitivity = ( float )LIS2DH12_ACC_SENSITIVITY_FOR_FS_16G;
            break;
        default:
            *sensitivity = -1.0f;
            return MEMS_ERROR;
    }

    return MEMS_SUCCESS;
}



/**
 * @brief Get the LIS2DH12 accelerometer value
 * @param handle the device handle
 * @param Get the actual accelerometer value(Only LIS2DH12_HIGH_RES mode)
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
status_t LIS2DH12_X_Get_Axes( void *handle, SensorAxes_t *acceleration )
{

    int16_t dataRaw[3];
    float sensitivity = 0;

    /* Read raw data from LSM6DSL output register. */
    if ( LIS2DH12_X_Get_Axes_Raw( handle, dataRaw ) == MEMS_ERROR )
    {
        return MEMS_ERROR;
    }

    /* Get LSM6DSL actual sensitivity. */
    if ( LIS2DH12_X_Get_Sensitivity( handle, &sensitivity ) == MEMS_ERROR )
    {
        return MEMS_ERROR;
    }

    /* Calculate the data. */
    acceleration->AXIS_X = ( int32_t )( dataRaw[0] * sensitivity );
    acceleration->AXIS_Y = ( int32_t )( dataRaw[1] * sensitivity );
    acceleration->AXIS_Z = ( int32_t )( dataRaw[2] * sensitivity );

    return MEMS_SUCCESS;
}
