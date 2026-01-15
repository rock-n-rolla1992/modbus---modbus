#if !defined __MAIN_H
#define __MAIN_H

#include "xc.h"
#include "basic_definitions.h"
#include "stdint.h"
#include "st_func_peripheral.h"

//версия софта
#define SOFT_VER_S0 1
#define SOFT_VER_S1 0
#define SOFT_VER_S2 0

#define DEBUG 1

#define FREQ_OS_GZ 64000000
#define PERIOD_INTERRUPT_MKS 25 //  
#define CCP_FREQ_MGZ 16
#define PNP_PIN_LEVEL 1
#define MAX_INDEX_MAIN_P 6

//замены пинов
#define DEBUG_PIN LATA4

#define LED_POWER TRISA2
#define LED_1 LATA0
#define LED_2 LATA1
#define BUTTON RE3

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

#define max_delay_blink(x)\
        do{\
            static UINT16 delay_blink_LED = 0;\
            static UINT8 led_hide_completed = 0;\
            if (led_hide_completed)\
            {\
                if (led_##x##_state)\
                {\
                    LED_##x = 1;\
                    if (++delay_blink_LED > 80000 / PERIOD_INTERRUPT_MKS)\
                    {\
                        LED_##x = 0;\
                        led_##x##_state = 0;\
                        delay_blink_LED = 0;\
                        led_hide_completed = 0;\
                    }\
                }\
            } else\
            {\
                 if (++delay_blink_LED > 80000 / PERIOD_INTERRUPT_MKS)\
                 {\
                    led_hide_completed = 1;\
                    delay_blink_LED = 0;\
                 }\
            }\
        }while (0)

#define UART_PORT_HUNDLER(x)\
do{\
    if (TX##x##IE && TX##x##IF)\
    {\
        if (Number_Tx_Byte[x-1] < size_Tx_frame[x-1])\
        {\
            if (parity[x-1] == 1)\
                TXSTA##x##bits.TX9D = TX9Dbit[x-1][Number_Tx_Byte[x-1]];\
            else if (parity[x-1] == 2)\
                TXSTA##x##bits.TX9D = !TX9Dbit[x-1][Number_Tx_Byte[x-1]];\
            TXREG##x = Rx_Tx_data[x-1][Number_Tx_Byte[x-1]];\
            Number_Tx_Byte[x-1]++;\
            if (Number_Tx_Byte[x-1] == size_Tx_frame[x-1])\
            {\
                led_##x##_state = 1;\
                TX##x##IE = 0;\
                Switch_Transsmit_Recieve[x-1] = 1;\
            }\
        }\
    } else if (RC##x##IE && RC##x##IF)\
    {\
        UINT8 receiv_byte = RCREG##x;\
        INTCONbits.GIEH = 0;\
        UINT16 time_receiv_byte = modbus_timeOut[x-1].timer;\
        modbus_timeOut[x-1].timer = 0;\
        INTCONbits.GIEH = 1;\
        if (Number_Rx_Byte[x-1])\
        {\
            if (time_receiv_byte > TimeOutFrame_1_5[x-1])\
                Error_Recive_1_5[x-1] = 1;\
            if (Number_Rx_Byte[x-1] < sizeof (Rx_Tx_data[x-1]))\
            {\
                Rx_Tx_data[x-1][Number_Rx_Byte[x-1]] = receiv_byte;\
                Number_Rx_Byte[x-1]++;\
            }\
        } else\
        {\
            if ((time_receiv_byte > TimeOutFrame_3_5[x-1]))\
            {\
                if (check_add(receiv_byte))\
                {\
                    led_##x##_state = 1;\
                    Rx_Tx_data[x-1][0] = receiv_byte;\
                    Number_Rx_Byte[x-1] = 1;\
                }\
            }\
        }\
        if (RCSTA##x##bits.OERR)\
        {\
            Number_Rx_Byte[x-1] = 0;\
            Error_Recive_1_5[x-1] = 0;\
            RCSTA##x##bits.CREN = 0;\
            RCSTA##x##bits.CREN = 1;\
        }\
    }\
}while(0)

#define UART_TRANSSMITED_ENDED(x)\
do{\
        if (Switch_Transsmit_Recieve[x-1] && TXSTA##x##bits.TRMT)\
        {\
            TX_OR_RX_##x = RECIVE;\
            modbus_timeOut[x-1].timer = TimeOutFrame_3_5[x-1];\
            Switch_Transsmit_Recieve[x-1] = 0;\
            RCSTA1bits.CREN = RCSTA2bits.CREN = 1;\
        }\
        if (modbus_timeOut[x-1].timer != 0xFFFF)\
        {\
            static UINT8 internal_timer_time_Out = 0;\
            if (++internal_timer_time_Out >= TIME_OUT_FRAME_MKS / PERIOD_INTERRUPT_MKS)\
            {\
                internal_timer_time_Out = 0;\
                modbus_timeOut[x-1].timer++;\
            }\
        }\
        }while(0)


#define BLINKER_MAC(blinker, time_on, time_off, delta_time_ms) \
do{\
    if (blinker.state)\
    {\
        blinker.cnt += delta_time_ms;\
        if (blinker.cnt >= time_on)\
        {\
            blinker.state = 0;\
            blinker.cnt -= time_on;\
            blinker.period = 1;\
        }\
    } else\
    {\
        blinker.cnt += delta_time_ms;\
        if (blinker.cnt >= time_off)\
        {\
            blinker.state = 1;\
            blinker.cnt -= time_off;\
        }\
    }}while(0)

    typedef struct {
        UINT16 cnt;
        UINT8 state : 1;
        UINT8 period : 1;
    } blinker_t;

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

extern UINT8 lock_signal_reg;
extern UINT8 own_address_reg;
extern UINT8 add_dev[32];
extern UINT8 baud_rate_reg[2];
extern UINT8 parity_reg[2];
extern UINT8 err_conf;
extern UINT8 cfg_save;

extern UINT8 add_dev_begin_1;
extern UINT8 add_dev_end_1;
extern UINT8 add_dev_begin_2;
extern UINT8 add_dev_end_2;
extern UINT8 add_dev_begin_3;
extern UINT8 add_dev_end_3;

extern UINT8 all_reset;
extern UINT16 debug_var;

extern UINT16 id;
extern UINT16 ver;



#endif

void func_get_val_reg();
void func_initialization();
void func_reset_all();
void func_save_all();
void ModBusTxRxFunc2();
void act_sluice(UINT8 index_mb, UINT8 temp_Number_Rx_Byte);
void Reset_Lock();
void blinker_func();
UINT8 check_add(UINT8 receiv_byte);


#include "Modbus.h"
//#include "indicator.h"

#endif // __MAIN_H