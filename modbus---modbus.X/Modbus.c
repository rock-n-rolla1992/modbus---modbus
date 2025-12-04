#define MODBUS_C_DATA
#include "Modbus.h"
#include "main.h"



UINT8 Rx_Tx_data[2][RX_BUF_SIZE] = {0};
UINT8 TX9Dbit[2][RX_BUF_SIZE] = {0};
UINT8 Number_Rx_Byte[2] = {0};
UINT8 Number_Tx_Byte[2] = {0};
UINT8 size_Tx_frame[2] = {0};
//bit volatile Start_Recive = 0;
UINT8 TimeOutFrame_1_5[2] = {0};
UINT8 TimeOutFrame_3_5[2] = {0};
UINT8 Error_Recive_1_5[2] = {0};


modbus_timeOut_t modbus_timeOut[2] = {0};

const MB_Set_t MB_Set[] = {
    //общая конфигурация
    init_MB(0, 1, 1, 0, 0, 1, lock_signal) //блокировка управления по модбасу
    init_MB(100, 1, 1, 1, 1, 247, add_dev_reg[0]) //адрес девайса (1 -247)
    init_MB(101, 1, 1, 0, 2, 7, baud_rate_reg[0]) //бодрейт (0 - 9600/1 -14400/2 - 19200/3 - 28800/4 - 38400/5 - 57600/6-76800/7-115200)
    init_MB(102, 1, 1, 0, 1, 2, parity_reg[0]) //контроль четности (0 - нет / 1 - четность / 2 - нечетность)
    init_MB(103, 1, 1, 1, 1, 247, add_dev_reg[1]) //адрес девайса (1 -247)
    init_MB(104, 1, 1, 0, 2, 7, baud_rate_reg[1]) //бодрейт (0 - 9600/1 -14400/2 - 19200/3 - 28800/4 - 38400/5 - 57600/6-76800/7-115200)
    init_MB(105, 1, 1, 0, 1, 2, parity_reg[1]) //контроль четности (0 - нет / 1 - четность / 2 - нечетность)
    init_MB(106, 1, 0, 0, 0, 0xFFFF, err_conf) //ошибка записи конфигурации
    init_MB(107, 1, 0, 0, 0, 1, cfg_save) //сохранение конфигурации

    init_MB(65000, 1, 0, 0, 0, 1, all_reset)//3     //ошибка записи конфигурации
    init_MB(65002, 0, 0, 0, 0, 0xFFFF, debug_var)

    init_MB(0xFFF0, 0, 0, 0, 0x0600, 0xFFFF, id)//74               //id
    init_MB(0xFFF1, 0, 0, 0, (soft_ver_S0 << 8) | (soft_ver_S1 * 10 + soft_ver_S2), 0xFFFF, ver) //75  //ver
};
UINT8 MB_eepr_add[sizeof (MB_Set) / sizeof (MB_Set[0])] = {0};

void set_baud_rate()
{
    add_dev[0] = add_dev_reg[0];
    baud_rate[0] = baud_rate_reg[0];
    parity[0] = parity_reg[0];
    UINT32 Speed_devise_bit_sek = CALCUL_SPEED_DEV_BIT_S(baud_rate[0]);
    UINT16 tempSPBRG = CALCUL_SPBRG(Speed_devise_bit_sek);
    SPBRG1 = tempSPBRG;
    SPBRGH1 = tempSPBRG >> 8;
    TimeOutFrame_3_5[0] = CALCUL_T_3_5(Speed_devise_bit_sek);
    TimeOutFrame_1_5[0] = CALCUL_T_1_5(Speed_devise_bit_sek);

    TXSTA1bits.BRGH = 1;
    BAUDCON1bits.BRG16 = 1;
    RCSTA1bits.SPEN = 1;
    TXSTA1bits.SYNC = 0;
    RCSTA1bits.CREN = 1;
    TXSTA1bits.TXEN = 1;
    if (parity[0])
    {
        TXSTA1bits.TX9 = 1;
        RCSTA1bits.RX9 = 1;
    } else
    {
        TXSTA1bits.TX9 = 0;
        RCSTA1bits.RX9 = 0;
    }
    TX1IP = 0;
    RC1IP = 0;
    RC1IE = 1;
}

UINT8 error_ad(UINT16 add_reg_get_set, UINT8 Modif, UINT8 Number_reg, UINT16 * ad)
{
    const MB_Set_t *ad_current = MB_Set;
    UINT8 cnt_i = 0;
    for (ad_current = MB_Set; ad_current < (MB_Set + sizeof (MB_Set) / sizeof (MB_Set_t)); ad_current++)
    {
        if (add_reg_get_set == ad_current->mb_add)
        {
            *ad = cnt_i;
            break;
        }
        cnt_i++;
    }
    for (const MB_Set_t *ad_next = ad_current; ad_next < ad_current + Number_reg; ad_next++)
    {
        if (ad_next->rw < Modif || ad_next ->mb_add != add_reg_get_set || ad_next >= (MB_Set + sizeof (MB_Set) / sizeof (MB_Set_t)))
            return 1;
        add_reg_get_set++;
    }
    return 0;
}

void ModBusTxRxFunc()
{
    //DEBUG_PIN = 1;
    if (TranssmitOrRecieve == Recive)
    {
        INTCONbits.GIEL = 0;
        UINT8 temp_Number_Rx_Byte = Number_Rx_Byte[0];
        UINT8 temp_Error_Recive_1_5 = Error_Recive_1_5[0];
        UINT8 temp_mtO = modbus_timeOut[0].timer;
        INTCONbits.GIEL = 1;
        if (temp_Number_Rx_Byte && (temp_mtO > TimeOutFrame_3_5[0]))
        {
            //LED_RED = 0;
#if 0 //вывод последних принятых запросов в ипром начиная с 80 адреса
            static UINT16 index_eep = 80;
            for (UINT8 index_byte = 0; index_byte < Number_Rx_Byte; index_byte++)
            {
                EEPR_WRITE_VAR(index_eep + index_byte, Rx_Tx_data[index_byte]);
            }
            EEPR_WRITE_VAR(index_eep + 13, temp_mtO);
            EEPR_WRITE_VAR(index_eep + 14, Error_Recive_1_5);
            EEPR_WRITE_VAR(index_eep + 15, Number_Rx_Byte);
            index_eep += 16;
            if (index_eep + 16 > 256)
            {
                index_eep = 80;
            }
#endif
            if (temp_Number_Rx_Byte > 5 && !temp_Error_Recive_1_5)
            {
                UINT16 crcRx = crc_chk(Rx_Tx_data[0], temp_Number_Rx_Byte - 2);
                if (crcRx == (Rx_Tx_data[0][temp_Number_Rx_Byte - 1] << 8 | Rx_Tx_data[0][temp_Number_Rx_Byte - 2]))
                {
                    RCSTA1bits.CREN = 0;
                    actCodeFunc();
                }
            }
            Number_Rx_Byte[0] = 0;
            Error_Recive_1_5[0] = 0;
        }
    }
    //DEBUG_PIN = 0;
}

void errorLogical(UINT8 codeError)
{
    Rx_Tx_data[0][1] = 0b10000000 | Rx_Tx_data[0][1]; //код функции свид. об ошибке
    Rx_Tx_data[0][2] = codeError; //код ошибки
    UINT16 crcTx = crc_chk(Rx_Tx_data[0], 3);
    Rx_Tx_data[0][3] = crcTx; //младший байт контрольной суммы
    Rx_Tx_data[0][4] = crcTx >> 8; //старший байт контрольной суммы
    size_Tx_frame[0] = 5;
}

void actCF_write_some()
{
#define MSB_adress_write 2
#define LSB_adress_write 3
#define MSB_Number_reg_write 4
#define LSB_Number_reg_write 5
    UINT16 adressReg_wr = (Rx_Tx_data[0][MSB_adress_write] << 8) | Rx_Tx_data[0][LSB_adress_write];
    UINT16 Number_reg_wr = (Rx_Tx_data[0][MSB_Number_reg_write] << 8) | Rx_Tx_data[0][LSB_Number_reg_write];
    UINT8 i = 0;
    UINT8 shift_pos = 0;
    UINT8 error_adr = error_ad(adressReg_wr, 1, Number_reg_wr, &adressReg_wr);
    if (!error_adr)
    {
        for (i = adressReg_wr; i < (adressReg_wr + Number_reg_wr); i++)
        {
            UINT16 temp_value = (Rx_Tx_data[0][7 + shift_pos] << 8) | Rx_Tx_data[0][8 + shift_pos];
            if (temp_value >= MB_Set[i].min_val && temp_value <= MB_Set[i].max_val)
            {
                for (UINT8 byte_cnt = 0; byte_cnt < MB_Set[i].length; byte_cnt++)
                {
                    *(UINT8*) (MB_Set[i].var_add + byte_cnt) = Rx_Tx_data[0][8 - byte_cnt + shift_pos];
                }
                //EEPR_WRITE_IN_POINT(MB_eepr_add[i], MB_Set[i].var_add, MB_Set[i].length);
            } else
                err_conf = 1;
            shift_pos += 2;
        }
        UINT16 crcTx = crc_chk(Rx_Tx_data[0], 6);
        Rx_Tx_data[0][6] = crcTx; //младший байт контрольной суммы
        Rx_Tx_data[0][7] = crcTx >> 8; //старший байт контрольной суммы
        size_Tx_frame[0] = 8;
    } else
        errorLogical(0x02); //ошибка адреса
}

void actCF_write()
{
#define MSB_adress_write 2
#define LSB_adress_write 3
#define MSB_write 4
#define LSB_write 5
    UINT16 adressReg_wr = (Rx_Tx_data[0][MSB_adress_write] << 8) | Rx_Tx_data[0][LSB_adress_write];
    UINT8 error_adr = error_ad(adressReg_wr, 1, 1, &adressReg_wr);
    if (!error_adr)
    {
        UINT16 temp_value = (Rx_Tx_data[0][MSB_write] << 8) | Rx_Tx_data[0][LSB_write];
        if (temp_value >= MB_Set[adressReg_wr].min_val && temp_value <= MB_Set[adressReg_wr].max_val)
        {
            /**(MB_Set[adressReg_wr].var_add) = temp_value;
            if (MB_Set[adressReg_wr].eepr_save)
            {
                EEPR_WRITE(MB_eepr_add[adressReg_wr], temp_value);
            }*/

            for (UINT8 byte_cnt = 0; byte_cnt < MB_Set[adressReg_wr].length; byte_cnt++)
            {
                *(UINT8*) (MB_Set[adressReg_wr].var_add + byte_cnt) = Rx_Tx_data[0][LSB_adress_write - byte_cnt];
            }
            //EEPR_WRITE_IN_POINT(MB_eepr_add[adressReg_wr], MB_Set[adressReg_wr].var_add, MB_Set[adressReg_wr].length);
        } else
            err_conf = 1;
        size_Tx_frame[0] = 8;
    } else
        errorLogical(0x02);
}

void actCF_read_some()
{
#define MSB_adress_read 2
#define LSB_adress_read 3
#define MSB_Number_reg_read 4
#define LSB_Number_reg_read 5
    UINT16 adressReg_read = (Rx_Tx_data[0][MSB_adress_read] << 8) | Rx_Tx_data[0][LSB_adress_read];
    UINT16 Number_reg_read = (Rx_Tx_data[0][MSB_Number_reg_read] << 8) | Rx_Tx_data[0][LSB_Number_reg_read];
    Rx_Tx_data[0][2] = 2 * Number_reg_read; //число читаемых байт данных
    UINT8 i = 0;
    UINT8 shift_pos = 0;
    UINT8 error_adr = error_ad(adressReg_read, 0, Number_reg_read, &adressReg_read);
    if (!error_adr)
    {
        for (i = adressReg_read; i < (adressReg_read + Number_reg_read); i++)
        {
            for (UINT8 byte_cnt = 0; byte_cnt < MB_Set[i].length; byte_cnt++)
            {
                Rx_Tx_data[0][4 - byte_cnt + shift_pos] = *(UINT8*) (MB_Set[i].var_add + byte_cnt); //младший байт данных
            }
            if (MB_Set[i].length == 1)
                Rx_Tx_data[0][3 + shift_pos] = 0;
            /*if (MB_Set[i].length == 1)
            {
                Rx_Tx_data[3 + shift_pos] = 0; //старший байт данных
                Rx_Tx_data[4 + shift_pos] = *(UINT8*) (MB_Set[i].var_add); //младший байт данных
            } else
            {
                Rx_Tx_data[3 + shift_pos] = *(UINT16*) (MB_Set[i].var_add) >> 8; //старший байт данных
                Rx_Tx_data[4 + shift_pos] = *(UINT16*) (MB_Set[i].var_add); //младший байт данных
            }*/
            shift_pos += 2;
        }
        UINT16 crcTx = crc_chk(Rx_Tx_data[0], 3 + shift_pos);
        Rx_Tx_data[0][3 + shift_pos] = crcTx; //младший байт контрольной суммы
        Rx_Tx_data[0][4 + shift_pos] = crcTx >> 8; //старший байт контрольной суммы
        size_Tx_frame[0] = 5 + shift_pos;
    } else
        errorLogical(0x02); //ошибка адреса
}

void CalculTX9Dbit()
{
    UINT8 cntTB = 0;
    while (cntTB < size_Tx_frame[0])
    {
        UINT8 cntBit = 0;
        UINT8 TempResult = Rx_Tx_data[0][cntTB] & 1;
        while (cntBit < 7)
        {

            cntBit++;
            TempResult ^= (Rx_Tx_data[0][cntTB] >> cntBit & 1);
        }
        TX9Dbit[0][cntTB] = TempResult;
        cntTB++;
    }
}

void actCodeFunc()
{
    switch (Rx_Tx_data[0][1]) //код функции
    {
        case CF_read_some:
            actCF_read_some();
            break;
        case CF_write:
            actCF_write();
            break;
        case CF_write_some:
            actCF_write_some();
            break;
        default:
            errorLogical(0x01); //ошибка функции
            break;
    }
    /* if (COMMAND_REG == 10400)
     {
         COMMAND_REG = 0;
         eepromWrite(TEST_MOD_NOT_OK_ADD, 1);
     }*/
    Number_Tx_Byte[0] = 0;
    CalculTX9Dbit();
    TranssmitOrRecieve = Transsmit;
    //LED.led_Blue = 1;
    TX1IE = 1;
}

UINT16 crc_chk(UINT8* byte, UINT8 length)
{
    UINT8 j;
    UINT16 reg_crc = 0xFFFF;
    while (length--)
    {
        reg_crc ^= *byte++;
        for (j = 0; j < 8; j++)
        {
            if (reg_crc & 0x01)
                reg_crc = (reg_crc >> 1) ^ 0xA001; // LSB(b0)=1

            else
                reg_crc = reg_crc >> 1;
        }
    }
    return reg_crc;
}




