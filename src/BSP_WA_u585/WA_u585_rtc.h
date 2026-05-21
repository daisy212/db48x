#ifndef WA_u585_RTC_H
#define WA_u585_RTC_H

#include "stdbool.h"
#include "stdint.h"
#include "RTOS.h"
#include <time.h>
#include <stdio.h>


#include "stm32u5xx.h"

uint32_t RTC_GetDebugStatus(void);


#define RTC_MAGIC_NUMBER   (0x32F2)



extern time_t  time_t_week;
extern uint32_t time_t_w_calc_t;

extern RTC_HandleTypeDef hrtc;


//void MX_RTC_Init(void);


bool RTC_Config_1024_Granularity(bool force_reset);

uint32_t rtc_read_ticks(void);
uint32_t rtc_elapsed_ticks(uint32_t rtc_ticks);


//   #define RECORD(rec, fmt, ...)   do { dbxxxx_recorder( #rec, fmt, ##__VA_ARGS__); } while(0)
// #define record(...)             RECORD(__VA_ARGS__)


void RTT_vprintf( const char * sFormat, ...);
void RTT_vprintf_L( uint32_t level, const char * sFormat, ...);

void RTT_vprintf_cr_time( const char * sFormat, ...);
void RTT_vprintf_cr_time_fct( const char * sFctName, const char * sFormat, ...);
void RTT_vprintf_cr_time_fct_level( const char * sFctName, uint32_t level, const char * sFormat, ...);


#define RTT_vprintf_cr_time_f(rec, fmt, ...)    do {RTT_vprintf_cr_time_fct(#rec, fmt, ##__VA_ARGS__);} while(0)
#define RTT_vprintf_cr_T_F(...)    RTT_vprintf_cr_time_f(__VA_ARGS__)


#define RTT_vprintf_cr_time_f_lev(rec, level, fmt, ...)    do {RTT_vprintf_cr_time_fct_level(#rec, level, fmt, ##__VA_ARGS__);} while(0)
#define RTT_vprintf_cr_T_F_L(...)    RTT_vprintf_cr_time_f_lev(__VA_ARGS__)






void RTC_SetBuildTime(void);
uint8_t RTC_IsConfigured(void);


void RTC_set_time_t(time_t t_time);



/* to do

bool bkSRAM_Init(void);
void bkSRAM_ReadString(uint16_t read_adress, char* read_data, uint32_t length);
void bkSRAM_WriteString(uint16_t read_adress, char* write_data, uint32_t length);
void bkSRAM_ReadVariable(uint16_t read_adress, uint32_t* read_data);
void bkSRAM_WriteVariable(uint16_t write_adress,uint32_t vall);


char buffer[256];

strftime(buffer, sizeof(buffer), "%a %b %d %H:%M:%S %Y", &your_tm);

gmtime : Convert time_t to tm as UTC time
tm : struct
Créer une variable time_t of the week, uint32_t rtc_read_ticks(void) giving time in msec during the week


*/



#endif // WA_u585_RTC_H
