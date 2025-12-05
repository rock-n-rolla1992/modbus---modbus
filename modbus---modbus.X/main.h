#if !defined __MAIN_H
#define __MAIN_H

#include "xc.h"
#include "basic_definitions.h"
#include "stdint.h"
#include "st_func_peripheral.h"

//версия софта
#define soft_ver_S0 0
#define soft_ver_S1 0
#define soft_ver_S2 1

#define DEBUG 1

#define FREQ_OS_GZ 64000000
#define PERIOD_INTERRUPT_MKS 25 //  
#define CCP_FREQ_MGZ 16
#define PNP_PIN_LEVEL 1
#define MAX_INDEX_MAIN_P 6

//коды режимов работы счетчика импульсов
#define _1_Ch 0
#define _2_Ch 1
#define _4_Ch 2

//замены пинов
#define DEBUG_PIN LATA4

#define INPUT_1 PORTAbits.RA3
#define INPUT_2 PORTCbits.RC0
#define RESET_IN PORTBbits.RB0
#define LOCK_BUTT PORTBbits.RB1
#define PNP_NPN_SW_IN1 LATB6
#define PNP_NPN_SW_IN2 LATA2
#define PNP_NPN_SW_RES_IN LATB5
#define PNP_NPN_SW_LOCK_BUTT LATB7
#define RELAY_1 LATA0
#define RELAY_2 LATA1
#define FAZA PORTAbits.RA5


//работа с данными
#define GET_BYTE(numb, data) (*(((UINT8*)&data) + numb))
#define GET_WORD(numb, data) (*(((UINT16*)&data) + numb))
//ипром
#define EEPR_WRITE_VAR(add, data) do{ for (UINT8 byte_number = 0; byte_number < sizeof (data); byte_number++){eepromWrite(add - byte_number, GET_BYTE(byte_number, data));}}while(0)
#define EEPR_READ_VAR(add, data) do{ for (UINT8 byte_number = 0; byte_number < sizeof (data); byte_number++){GET_BYTE(byte_number, data) = eepromRead(add - byte_number);}}while(0)

#define EEPR_WRITE_IN_POINT(add, pointer, size) do{ for (UINT8 byte_number = 0; byte_number < size; byte_number++){eepromWrite(add - byte_number, *((UINT8*)pointer + byte_number));}}while(0)
#define EEPR_READ_IN_POINT(add, pointer, size) do{ for (UINT8 byte_number = 0; byte_number < size; byte_number++){*((UINT8*)pointer + byte_number)= eepromRead(add - byte_number);}}while(0)

//режимы пинов 
#define AN 1
#define DIG 0
#define IN 1
#define OUT 0


#define _XTAL_FREQ FREQ_OS_GZ //для правильной работы задержки __delay_us()



//адреса записи в ипром
#define BEGIN_EEPR_ADD 32


typedef void (*automat_state_t)(void);

typedef struct {
    UINT8 cnt_rx_byte;

    union {
        UINT8 general_error;

        struct {
            UINT8 error_crc;
            UINT8 framing_error : 1;
            UINT8 buff_overflow : 1;
        };
    };
} flag_read_t;


#ifndef MAIN_C_DATA
extern UINT8 lock_signal;

extern UINT8 add_dev[32];
extern UINT8 baud_rate[2];
extern UINT8 parity[2];
extern UINT8 add_dev_reg[32];
extern UINT8 baud_rate_reg[2];
extern UINT8 parity_reg[2];
extern UINT8 err_conf;
extern UINT8 cfg_save;

extern UINT8 all_reset;
extern UINT16 debug_var;

extern UINT16 id;
extern UINT16 ver;



#endif

void func_initialization();
void func_reset_all();
void func_save_all();
void ModBusTxRxFunc2();
void act_sluice(UINT8 index_mb, UINT8 temp_Number_Rx_Byte);
void set_baud_rate2();


#include "Modbus.h"
//#include "indicator.h"

#endif // __MAIN_H