
#ifndef MODBUS_H
#define	MODBUS_H
#include "basic_definitions.h"
#include "main.h"

#define CF_read_some 0x03
#define CF_write 0x06
#define CF_write_some 0x10

#define RX_BUF_SIZE 74
#define MB_MEMORY_SIZE 80

//коды режимов
#define MODE_CNT_PULSE 0
#define MODE_TACHO 1

#define MODE_SUMM_1_AND_2 1
#define MODE_SUMM_1_DIRECT_2 2
#define MODE_REVERSIBLE_CNT 3
#define MODE_SUMM_1_DIFF_2 4

#define MODE_REVERSIBLE_TACHO 0
#define MODE_2_TACHO 1

//имена регистров
//общая конфигурация
#define MODE_OPERATION_DEV MODE_CNT_PULSE


//замениы пинов
#define TranssmitOrRecieve LATC1
#define TranssmitOrRecieve_2 LATC1
//режимы приемопередатчика
#define Transsmit 1
#define Recive 0

//инициализация скорости передачи 
#define CALCUL_SPEED_DEV_BIT_S(x) (x == 0?9600:x== 1? 14400: x==2?19200:x==3?28800:x== 4?38400:x== 5?57600:x== 6?76800:115200)
#define TIME_OUT_FRAME_MKS 100
#define CALCUL_T_3_5(x) (((UINT32)49500000*2/((UINT32)x*TIME_OUT_FRAME_MKS)+1)/2)
#define CALCUL_T_1_5(x) (((UINT32)27500000*2/((UINT32)x*TIME_OUT_FRAME_MKS) +1)/2)
#define CALCUL_SPBRG(x) (((UINT32)FREQ_OS_GZ*2/((UINT32)x*4) +1)/2 - 1)

#define init_MB(mb_add, rw, eepr_save, min, def, max, var_add_) {mb_add, rw, eepr_save, min, def, max, sizeof(var_add_), &var_add_},

typedef struct {
    UINT16 mb_add;
    UINT8 rw:1;
    UINT8 eepr_save:1;
    UINT16 min_val;
    UINT16 default_val;
    UINT16 max_val;
    UINT8 length;
    void * var_add;
} MB_Set_t;

typedef struct {
    UINT8 timer;
    //UINT8 overflow_flag;
} modbus_timeOut_t;


#ifndef MODBUS_C_DATA
extern UINT8 Rx_Tx_data[2][RX_BUF_SIZE];
extern UINT8 TX9Dbit[2][RX_BUF_SIZE];
extern UINT8 Number_Rx_Byte[2];
extern UINT8 Number_Tx_Byte[2];
extern UINT8 size_Tx_frame[2];
//bit volatile Start_Recive = 0;
extern UINT8 TimeOutFrame_1_5[2];
extern UINT8 TimeOutFrame_3_5[2];
extern UINT8 Error_Recive_1_5[2];


extern modbus_timeOut_t modbus_timeOut[2];
extern const MB_Set_t MB_Set[];
extern UINT8 MB_eepr_add[sizeof (MB_Set)/sizeof(MB_Set[0])];
#endif
void set_baud_rate();
UINT8 error_ad(UINT16 add_reg_get_set, UINT8 Modif, UINT8 Number_reg, UINT16 * ad);
void ModBusTxRxFunc();
void errorLogical(UINT8 codeError);
void actCF_write_some();
void actCF_write();
void actCF_read_some();
void CalculTX9Dbit();
void func_chek_value(UINT16 *value, UINT16 temp, UINT16 lo, UINT16 hi, UINT8 add);
void actCodeFunc();
UINT16 crc_chk(UINT8* byte, UINT8 length);

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

