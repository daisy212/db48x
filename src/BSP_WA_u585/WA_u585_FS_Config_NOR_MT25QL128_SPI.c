/*********************************************************************
*                     SEGGER Microcontroller GmbH                    *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*       (c) 2003 - 2023  SEGGER Microcontroller GmbH                 *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
-------------------------- END-OF-HEADER -----------------------------

File    : FS_ConfigNOR_BM_SPIFI_STM32U585_ST_B_U585I_IOT02A_Discovery.c
Purpose : Configuration file for serial NOR flash connected via OCTO SPI.
*/

/*********************************************************************
*
*       #include section
*
**********************************************************************
*/
#include "FS.h"

#include "DBxxxx.h"

extern const FS_NOR_HW_TYPE_SPI FS_NOR_HW_SPI_MTQL128;


/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/
#ifndef   ALLOC_SIZE
#define ALLOC_SIZE           (32 * 1024)
#define CACHE_SIZE           (16 * 1024)
//  #define ALLOC_SIZE          0x8000          // Size of memory dedicated to the file system. This value should be fine tuned according for your system.
#endif


#define FLASH_BASE_ADDR             0x00000000    // Base address of the NOR flash device to be used as storage. 0 si non memory mapped 
#define FLASH_START_ADDR            0x00000000    // Start address of the first sector be used as storage. If the entire chip is used for file system, it is identical to the base address.

#define FLASH_SIZE                  0x01000000    // Number of bytes to be used for storage
#define BYTES_PER_SECTOR            512           // Logical sector size, pas ok avec 4096 ?


#define QSPI_BUS_FREQ_HZ            2000000      // QSPI bus frequency in Hz
#define QSPI_DEFAULT_INTR_PRIORITY  14             // Default interrupt priority for QSPI


/*********************************************************************
*
*       _apDeviceAll
static const FS_NOR_SPI_TYPE * _apDeviceMicron[] = {
//  &FS_NOR_SPI_DeviceMacronix,
  &FS_NOR_SPI_DeviceMicron,
//  &FS_NOR_SPI_DeviceMacronixOctal,
//  &FS_NOR_SPI_DeviceMacronixOctalSTR,
//  &FS_NOR_SPI_DeviceMacronixOctalDTR
};
*/

/*********************************************************************
*
*       _DeviceListMacronix
const FS_NOR_SPI_DEVICE_LIST _DeviceListMicron = {
  (U8)SEGGER_COUNTOF(_apDeviceMicron),
  _apDeviceMicron
};
*/

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

/*********************************************************************
*
*       Memory pool used for semi-dynamic allocation.
*/
#ifdef __ICCARM__
  #pragma location="FS_RAM"
  static __no_init U32 _aMemBlock[ALLOC_SIZE / 4];
#endif
#ifdef __CC_ARM
  static U32 _aMemBlock[ALLOC_SIZE / 4] __attribute__ ((section ("FS_RAM"), zero_init));
#endif
#if (!defined(__ICCARM__) && !defined(__CC_ARM))
  static U32 _aMemBlock[ALLOC_SIZE / 4];
static U32 _aCache[CACHE_SIZE / 4];

#endif

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/

/*********************************************************************
*
*       FS_X_AddDevices
*
*  Function description
*    This function is called by the FS during FS_Init().
*    It is supposed to add all devices, using primarily FS_AddDevice().
*
*  Note
*    (1) Other API functions may NOT be called, since this function is called
*        during initialization. The devices are not yet ready at this point.
https://www.segger.com/products/file-system/emfile/add-ons/device-driver-nor-flash/general-information/
*/


void FS_X_AddDevices(void) {
// um02001_emFile.pdf 6.4.5.2.16 
// Give file system memory to work with.
//
FS_AssignMemory(&_aMemBlock[0], sizeof(_aMemBlock));
//
// Configure the size of the logical sector and activate the file buffering.
//
FS_SetMaxSectorSize(BYTES_PER_SECTOR);
#if FS_SUPPORT_FILE_BUFFER
  FS_ConfigFileBufferDefault(BYTES_PER_SECTOR, FS_FILE_BUFFER_WRITE);
#endif
 //
  // Add and configure the NOR driver.
  //
  FS_AddDevice(&FS_NOR_BM_Driver);
  FS_NOR_BM_SetPhyType(0, &FS_NOR_PHY_SFDP);
  FS_NOR_BM_Configure(0, FLASH_BASE_ADDR, FLASH_START_ADDR, FLASH_SIZE);
  FS_NOR_BM_SetSectorSize(0, BYTES_PER_SECTOR);
  
//FS_NOR_BM_SetWriteVerification(0, 1);

//
// Configure the NOR physical layer.
//
   FS_NOR_DSPI_SetHWType(0, &FS_NOR_HW_SPI_MTQL128);
   FS_NOR_SFDP_SetDeviceList(0, &FS_NOR_SPI_DeviceList_All);  // new

}

/*********************************************************************
*
*       FS_X_GetTimeDate
*
*  Function description
*    Current time and date in a format suitable for the file system.
*
*  Additional information
*    Bit 0-4:   2-second count (0-29)
*    Bit 5-10:  Minutes (0-59)
*    Bit 11-15: Hours (0-23)
*    Bit 16-20: Day of month (1-31)
*    Bit 21-24: Month of year (1-12)
*    Bit 25-31: Count of years from 1980 (0-127)
*/
U32 FS_X_GetTimeDate(void) 
// Rtc read for file system time
{
   U32 r;
   U16 Sec, Min, Hour;
   U16 Day, Month, Year;

   RTC_TimeTypeDef      time_rtc;
   RTC_DateTypeDef      date_rtc;
// allway together !!!!!!
   HAL_RTC_GetTime(&hrtc, &time_rtc, RTC_FORMAT_BIN);
   HAL_RTC_GetDate(&hrtc, &date_rtc, RTC_FORMAT_BIN);

   Sec   = time_rtc.Seconds;     // 0 based.  Valid range: 0..59
   Min   = time_rtc.Minutes;     // 0 based.  Valid range: 0..59
   Hour  = time_rtc.Hours;       // 0 based.  Valid range: 0..23
   Day   = date_rtc.Date;        // 1 based.    Means that 1 is 1. Valid range is 1..31 (depending on month)
   Month = date_rtc.Month;       // 1 based.    Means that January is 1. Valid range is 1..12.
   Year  = date_rtc.Year + 20;      // 1980 based. Means that 2007 would be 27.
   r   = Sec / 2 + (Min << 5) + (Hour  << 11);
   r  |= (U32)(Day + (Month << 5) + (Year  << 9)) << 16;
   return r;


}







/*************************** End of file ****************************/
