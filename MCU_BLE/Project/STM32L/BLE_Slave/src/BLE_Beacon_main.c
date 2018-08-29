/******************** (C) COPYRIGHT 2016 STMicroelectronics ********************
* File Name          : BLE_Beacon_main.c
* Author             : RF Application Team - AMG
* Version            : V1.0.0
* Date               : 20-November-2016
* Description        : NUCLEO-L152RE network coprocessor main file for beacon demo
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/** @addtogroup BlueNRG1_demonstrations_applications
*  BlueNRG-1 Beacon demo \see BLE_Beacon_main.c for documentation.
*
*@{
*/

/** @} */
/** \cond DOXYGEN_SHOULD_SKIP_THIS
*/
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "hal_types.h"
#include "hci.h"
#include "gp_timer.h"
#include "hal.h"
#include "osal.h"
#include "hci_const.h"
#include "bluenrg1_types.h"
#include "bluenrg1_gap.h"
#include "bluenrg1_aci.h"
#include "bluenrg1_hci_le.h"
#include "SDK_EVAL_Config.h"
#include "SDK_EVAL_Led.h"
#include "SDK_EVAL_Com.h"
#include "user_timer.h"

#if ENABLE_MICRO_SLEEP
#include "low_power.h"
#endif

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define BLE_BEACON_VERSION_STRING "1.0.0"

/** 
* @brief  Enable debug printf
*/ 
#ifndef DEBUG
#define DEBUG 0
#endif

/* Set to 1 for enabling Flags AD Type position at the beginning 
of the advertising packet */
#define ENABLE_FLAGS_AD_TYPE_AT_BEGINNING 1

/* Private macros ------------------------------------------------------------*/
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define ADV_INT_MIN 0x90
#define ADV_INT_MAX 0x90 

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#define COPY_UUID_128(uuid_struct, uuid_15, uuid_14, uuid_13, uuid_12, uuid_11, uuid_10, uuid_9, uuid_8, uuid_7, uuid_6, uuid_5, uuid_4, uuid_3, uuid_2, uuid_1, uuid_0) \
  do {\
  	uuid_struct.uuid128[0] = uuid_0; uuid_struct.uuid128[1] = uuid_1; uuid_struct.uuid128[2] = uuid_2; uuid_struct.uuid128[3] = uuid_3; \
	uuid_struct.uuid128[4] = uuid_4; uuid_struct.uuid128[5] = uuid_5; uuid_struct.uuid128[6] = uuid_6; uuid_struct.uuid128[7] = uuid_7; \
	uuid_struct.uuid128[8] = uuid_8; uuid_struct.uuid128[9] = uuid_9; uuid_struct.uuid128[10] = uuid_10; uuid_struct.uuid128[11] = uuid_11; \
	uuid_struct.uuid128[12] = uuid_12; uuid_struct.uuid128[13] = uuid_13; uuid_struct.uuid128[14] = uuid_14; uuid_struct.uuid128[15] = uuid_15; \
	}while(0)

uint16_t chatServHandle, TXCharHandle, RXCharHandle;

/* UUIDs */
Service_UUID_t service_uuid;
Char_UUID_t char_uuid;
uint8_t local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'B','l','u','e','N','R','G','1','_','C','h','a','t'}; 
/*******************************************************************************
* Function Name  : Add_ChatService
* Description    : Add the 'Accelerometer' service.
* Input          : None
* Return         : Status.
*******************************************************************************/
uint8_t Add_ChatService(void)
{
  uint8_t ret;

  /*
  UUIDs:
  D973F2E0-B19E-11E2-9E96-0800200C9A66
  D973F2E1-B19E-11E2-9E96-0800200C9A66
  D973F2E2-B19E-11E2-9E96-0800200C9A66
  */

  const uint8_t uuid[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe0,0xf2,0x73,0xd9};
  const uint8_t charUuidTX[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe1,0xf2,0x73,0xd9};
  const uint8_t charUuidRX[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe2,0xf2,0x73,0xd9};

  Osal_MemCpy(&service_uuid.Service_UUID_128, uuid, 16);
  
  ret = aci_gatt_add_service(UUID_TYPE_128, &service_uuid, PRIMARY_SERVICE, 6, &chatServHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;    

  Osal_MemCpy(&char_uuid.Char_UUID_128, charUuidTX, 16);
  ret =  aci_gatt_add_char(chatServHandle, UUID_TYPE_128, &char_uuid, 20, CHAR_PROP_NOTIFY, ATTR_PERMISSION_NONE, 0,
                16, 1, &TXCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;

  Osal_MemCpy(&char_uuid.Char_UUID_128, charUuidRX, 16);
  ret =  aci_gatt_add_char(chatServHandle, UUID_TYPE_128, &char_uuid, 20, CHAR_PROP_WRITE|CHAR_PROP_WRITE_WITHOUT_RESP, ATTR_PERMISSION_NONE, GATT_NOTIFY_ATTRIBUTE_WRITE,
                16, 1, &RXCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;

  printf("Chat Service added.\nTX Char Handle %04X, RX Char Handle %04X\n", TXCharHandle, RXCharHandle);
  return BLE_STATUS_SUCCESS; 

fail:
  printf("Error while adding Chat service.\n");
  return BLE_STATUS_ERROR ;
}




static void Device_Init(void)
{
  uint8_t ret;
  uint16_t service_handle;
  uint16_t dev_name_char_handle;
  uint16_t appearance_char_handle;
  uint8_t bdaddr[] = {0xf5, 0x00, 0x00, 0xE1, 0x80, 0x02};
  
  /* Set the TX Power to -2 dBm */
  ret = aci_hal_set_tx_power_level(1,4);
  if(ret != 0) {
    PRINTF ("Error in aci_hal_set_tx_power_level() 0x%04x\r\n", ret);
    while(1);
  }
  else
    printf ("aci_hal_set_tx_power_level() --> SUCCESS\r\n");
  
  /* Set the public address */
  ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET, CONFIG_DATA_PUBADDR_LEN, bdaddr);
  if(ret != 0) {
    PRINTF ("Error in aci_hal_write_config_data() 0x%04xr\n", ret);
    while(1);
  }
  else
    printf ("aci_hal_write_config_data() --> SUCCESS\r\n");
  
  /* Init the GATT */
  ret = aci_gatt_init();    
  if(ret != 0) {
    PRINTF ("Error in aci_gatt_init() 0x%04x\r\n", ret);
    while(1);
  }
  else
    printf ("aci_gatt_init() --> SUCCESS\r\n");
  
  /* Init the GAP */
  ret = aci_gap_init(GAP_PERIPHERAL_ROLE, 0, 0x08, &service_handle, &dev_name_char_handle, &appearance_char_handle);
  if(ret != 0) {
    PRINTF ("Error in aci_gap_init() 0x%04xr\n", ret);
    while(1);
  }
  else
    printf ("aci_gap_init() --> SUCCESS\r\n");
  
  /* Add Device Service & Characteristics */
  ret = Add_ChatService();
  if(ret != BLE_STATUS_SUCCESS) {
    PRINTF("Error while adding service: 0x%02x\n", ret);
  } else
  {
    printf ("Add_ChatService() --> SUCCESS\r\n");
  }
  //start adv//
  
  ret = aci_gap_set_discoverable(ADV_IND, ADV_INT_MIN, ADV_INT_MAX, PUBLIC_ADDR, NO_WHITE_LIST_USE,
                                 sizeof(local_name), local_name, 0, NULL, 0x6, 0x8); 
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gap_set_discoverable() failed: 0x%02x\n", ret);
  } else {
    PRINTF("aci_gap_set_discoverable() OK\n"); 
  }
  
  
  
  
}


/**
* @brief  Start beaconing
* @param  None 
* @retval None
*/
static void Start_Beaconing(void)
{  
  uint8_t ret = BLE_STATUS_SUCCESS;
  
#if ENABLE_FLAGS_AD_TYPE_AT_BEGINNING
  /* Set AD Type Flags at beginning on Advertising packet  */
  const uint8_t adv_data[] = {
    /* Advertising data: Flags AD Type */
    0x02, 
    0x01, 
    0x06, 
    /* Advertising data: manufacturer specific data */
    26, AD_TYPE_MANUFACTURER_SPECIFIC_DATA,  
    0x30, 0x00, //Company idSPI_INTERFACEentifier code (Default is 0x0030 - STMicroelectronics: To be customized for specific identifier)
    0x02,       // ID
    0x15,       //Length of the remaining payload
    0xE2, 0x0A, 0x39, 0xF4, 0x73, 0xF5, 0x4B, 0xC4, //Location UUID
    0xA1, 0x2F, 0x17, 0xD1, 0xAD, 0x07, 0xA9, 0x61,
    0x00, 0x00, // Major number 
    0x00, 0x00, // Minor number 
    0xC8        //2's complement of the Tx power (-56dB)};      
  };
#else
  uint8_t manuf_data[] = {26, AD_TYPE_MANUFACTURER_SPECIFIC_DATA, 
  0x30, 0x00, //Company identifier code (Default is 0x0030 - STMicroelectronics: To be customized for specific identifier)
  0x02,       // ID
  0x15,       //Length of the remaining payload
  0xE2, 0x0A, 0x39, 0xF4, 0x73, 0xF5, 0x4B, 0xC4, //Location UUID
  0xA1, 0x2F, 0x17, 0xD1, 0xAD, 0x07, 0xA9, 0x61,
  0x00, 0x00, // Major number 
  0x00, 0x00, // Minor number 
  0xC8        //2's complement of the Tx power (-56dB)};      
  };
#endif
  
  /* disable scan response */   
  ret = hci_le_set_scan_response_data(0,NULL);
  if (ret != BLE_STATUS_SUCCESS) {
    printf ("Error in hci_le_set_scan_resp_data() 0x%04x\r\n", ret);
    while(1);
  }
  else
    printf ("hci_le_set_scan_resp_data() --> SUCCESS\r\n");
  
  /* put device in non connectable mode */
  ret = aci_gap_set_discoverable(ADV_NONCONN_IND, 160, 160, PUBLIC_ADDR, NO_WHITE_LIST_USE,
                                 0, NULL, 0, NULL, 0, 0); 
  if (ret != BLE_STATUS_SUCCESS) {
    printf ("Error in aci_gap_set_discoverable() 0x%04x\r\n", ret);
    while(1);
  }
  else
    printf ("aci_gap_set_discoverable() --> SUCCESS\r\n");
  
#if ENABLE_FLAGS_AD_TYPE_AT_BEGINNING
  /* Set the  ADV data with the Flags AD Type at beginning of the 
  advertsing packet,  followed by the beacon manufacturer specific data */
  ret = hci_le_set_advertising_data (sizeof(adv_data), (uint8_t *)adv_data);
  if (ret != BLE_STATUS_SUCCESS) {
    printf ("Error in hci_le_set_advertising_data() 0x%04x\r\n", ret);
    while(1);
  }
  else
    printf ("hci_le_set_advertising_data() --> SUCCESS\r\n");
#else
  ret = aci_gap_delete_ad_type(AD_TYPE_TX_POWER_LEVEL); 
  if (ret != BLE_STATUS_SUCCESS) {
    printf ("Error in aci_gap_delete_ad_type() 0x%04x\r\n", ret);
    while(1);
  }
  else
    printf ("aci_gap_delete_ad_type() --> SUCCESS\r\n");
  
  ret = aci_gap_update_adv_data(sizeof(manuf_data), manuf_data);
  if (ret != BLE_STATUS_SUCCESS) {
    printf ("Error in aci_gap_update_adv_data() 0x%04x\r\n", ret);
    while(1);
  }
  else
    printf ("aci_gap_update_adv_data() --> SUCCESS\r\n");
#endif
}


/*******************************************************************************
* Function Name  : main.
* Description    : Main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int main(void)
{
  uint8_t ret;
  
  /* Device Initialization */
  Init_Device();
  
  /* BlueNRG-1 stack init */
  ret = BlueNRG_Stack_Initialization();
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Error in BlueNRG_Stack_Initialization() 0x%02x\r\n", ret);
    while(1);
  }
  
  /* Init the BlueNRG-1 device */
  Device_Init(); 
  
  /* Start Beacon Non Connectable Mode*/
  //Start_Beaconing();
  
  printf("BlueNRG-1 BLE Beacon Application (version: %s)\r\n", BLE_BEACON_VERSION_STRING); 
  
  /* Infinite loop */
  while(1) {     
    BTLE_StackTick();
    
#if ENABLE_MICRO_SLEEP
    System_Sleep();
#endif
  }
}




#ifdef USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {}
}
#endif

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
/** \endcond
*/
