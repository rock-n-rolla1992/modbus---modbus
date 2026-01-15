#define MODBUS_C_DATA
#include "Modbus.h"
#include "main.h"



UINT8 Rx_Tx_data[2][RX_BUF_SIZE] = {0};
UINT8 TX9Dbit[2][RX_BUF_SIZE] = {0};
UINT8 Number_Rx_Byte[2] = {0};
UINT8 Number_Tx_Byte[2] = {0};
UINT8 size_Tx_frame[2] = {0};
//bit volatile Start_Recive = 0;
UINT16 TimeOutFrame_1_5[2] = {0};
UINT16 TimeOutFrame_3_5[2] = {0};
UINT8 Error_Recive_1_5[2] = {0};


modbus_timeOut_t modbus_timeOut[2] = {0};

const MB_Set_t MB_Set[] = {
    //общая конфигурация
    init_MB(0, 1, 1, 0, 0, 1, lock_signal_reg) //блокировка управления по модбасу

    init_MB(100, 1, 1, 1, OWN_ADDR_DEF_VAL, 247, own_address_reg) //собственный адрес
    init_MB(101, 1, 1, 0, BOUD_DEF_VAL, 10, baud_rate_reg[0]) //бодрейт 0-1200/1-2400/2-4800/3-9600/4-14400/5-19200/6-28800/7-38400/8-57600/9-76800/10-115200
    init_MB(102, 1, 1, 0, PARITY_DEF_VAL, 2, parity_reg[0]) //контроль четности (0 - нет / 1 - четность / 2 - нечетность)

    init_MB(106, 1, 0, 0, 0, 0xFFFF, err_conf) //ошибка записи конфигурации
    init_MB(107, 1, 0, 0, 0, 1, cfg_save) //сохранение конфигурации

    init_MB(201, 1, 1, 0, BOUD_DEF_VAL, 10, baud_rate_reg[1]) //бодрейт (0 - 9600/1 -14400/2 - 19200/3 - 28800/4 - 38400/5 - 57600/6-76800/7-115200)
    init_MB(202, 1, 1, 0, PARITY_DEF_VAL, 2, parity_reg[1]) //контроль четности (0 - нет / 1 - четность / 2 - нечетность)

    init_MB(300, 1, 1, 1, 255, 255, add_dev[0]) //пропускаемый (1 -247)
    init_MB(301, 1, 1, 1, 255, 255, add_dev[1]) //пропускаемый (1 -247)
    init_MB(302, 1, 1, 1, 255, 255, add_dev[2]) //пропускаемый (1 -247)
    init_MB(303, 1, 1, 1, 255, 255, add_dev[3]) //пропускаемый (1 -247)
    init_MB(304, 1, 1, 1, 255, 255, add_dev[4]) //пропускаемый (1 -247)
    init_MB(305, 1, 1, 1, 255, 255, add_dev[5]) //пропускаемый (1 -247)
    init_MB(306, 1, 1, 1, 255, 255, add_dev[6]) //пропускаемый (1 -247)
    init_MB(307, 1, 1, 1, 255, 255, add_dev[7]) //пропускаемый (1 -247)
    init_MB(308, 1, 1, 1, 255, 255, add_dev[8]) //пропускаемый (1 -247)
    init_MB(309, 1, 1, 1, 255, 255, add_dev[9]) //пропускаемый (1 -247)
    init_MB(310, 1, 1, 1, 255, 255, add_dev[10]) //пропускаемый (1 -247)
    init_MB(311, 1, 1, 1, 255, 255, add_dev[11]) //пропускаемый (1 -247)
    init_MB(312, 1, 1, 1, 255, 255, add_dev[12]) //пропускаемый (1 -247)
    init_MB(313, 1, 1, 1, 255, 255, add_dev[13]) //пропускаемый (1 -247)
    init_MB(314, 1, 1, 1, 255, 255, add_dev[14]) //пропускаемый (1 -247)
    init_MB(315, 1, 1, 1, 255, 255, add_dev[15]) //пропускаемый (1 -247)
    init_MB(316, 1, 1, 1, 255, 255, add_dev[16]) //пропускаемый (1 -247)
    init_MB(317, 1, 1, 1, 255, 255, add_dev[17]) //пропускаемый (1 -247)
    init_MB(318, 1, 1, 1, 255, 255, add_dev[18]) //пропускаемый (1 -247)
    init_MB(319, 1, 1, 1, 255, 255, add_dev[19]) //пропускаемый (1 -247)
    init_MB(320, 1, 1, 1, 255, 255, add_dev[20]) //пропускаемый (1 -247)
    init_MB(321, 1, 1, 1, 255, 255, add_dev[21]) //пропускаемый (1 -247)
    init_MB(322, 1, 1, 1, 255, 255, add_dev[22]) //пропускаемый (1 -247)
    init_MB(323, 1, 1, 1, 255, 255, add_dev[23]) //пропускаемый (1 -247)
    init_MB(324, 1, 1, 1, 255, 255, add_dev[24]) //пропускаемый (1 -247)
    init_MB(325, 1, 1, 1, 255, 255, add_dev[25]) //пропускаемый (1 -247)
    init_MB(326, 1, 1, 1, 255, 255, add_dev[26]) //пропускаемый (1 -247)
    init_MB(327, 1, 1, 1, 255, 255, add_dev[27]) //пропускаемый (1 -247)
    init_MB(328, 1, 1, 1, 255, 255, add_dev[28]) //пропускаемый (1 -247)
    init_MB(329, 1, 1, 1, 255, 255, add_dev[29]) //пропускаемый (1 -247)
    init_MB(330, 1, 1, 1, 255, 255, add_dev[30]) //пропускаемый (1 -247)
    init_MB(331, 1, 1, 1, 255, 255, add_dev[31]) //пропускаемый (1 -247)

    init_MB(400, 1, 1, 1, 255, 255, add_dev_begin_1) //пропускаемый (1 -247)
    init_MB(401, 1, 1, 1, 255, 255, add_dev_end_1) //пропускаемый (1 -247)
    init_MB(402, 1, 1, 1, 255, 255, add_dev_begin_2) //пропускаемый (1 -247)
    init_MB(403, 1, 1, 1, 255, 255, add_dev_end_2) //пропускаемый (1 -247)
    init_MB(404, 1, 1, 1, 255, 255, add_dev_begin_3) //пропускаемый (1 -247)
    init_MB(405, 1, 1, 1, 255, 255, add_dev_end_3) //пропускаемый (1 -247)

    init_MB(65000, 1, 0, 0, 0, 1, all_reset)//3     //ошибка записи конфигурации
    init_MB(65002, 0, 0, 0, 0, 0xFFFF, debug_var)

    init_MB(0xFFF0, 0, 0, 0, 0x0600, 0xFFFF, id)//74               //id
    init_MB(0xFFF1, 0, 0, 0, (SOFT_VER_S0 << 8) | (SOFT_VER_S1 * 10 + SOFT_VER_S2), 0xFFFF, ver) //75  //ver
};
UINT8 MB_eepr_add[sizeof (MB_Set) / sizeof (MB_Set[0])] = {0};

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

void errorLogical(UINT8 codeError, UINT8 index_mb)
{
    Rx_Tx_data[index_mb][1] = 0b10000000 | Rx_Tx_data[index_mb][1]; //код функции свид. об ошибке
    Rx_Tx_data[index_mb][2] = codeError; //код ошибки
    UINT16 crcTx = crc_chk(Rx_Tx_data[index_mb], 3);
    Rx_Tx_data[index_mb][3] = crcTx; //младший байт контрольной суммы
    Rx_Tx_data[index_mb][4] = crcTx >> 8; //старший байт контрольной суммы
    size_Tx_frame[index_mb] = 5;
}

void actCF_write_some(UINT8 index_mb)
{
#define MSB_adress_write 2
#define LSB_adress_write 3
#define MSB_Number_reg_write 4
#define LSB_Number_reg_write 5
    UINT16 adressReg_wr = (Rx_Tx_data[index_mb][MSB_adress_write] << 8) | Rx_Tx_data[index_mb][LSB_adress_write];
    UINT16 Number_reg_wr = (Rx_Tx_data[index_mb][MSB_Number_reg_write] << 8) | Rx_Tx_data[index_mb][LSB_Number_reg_write];
    UINT8 i = 0;
    UINT8 shift_pos = 0;
    UINT8 error_adr = error_ad(adressReg_wr, 1, Number_reg_wr, &adressReg_wr);
    if (!error_adr)
    {
        for (i = adressReg_wr; i < (adressReg_wr + Number_reg_wr); i++)
        {
            UINT16 temp_value = (Rx_Tx_data[index_mb][7 + shift_pos] << 8) | Rx_Tx_data[index_mb][8 + shift_pos];
            if (temp_value >= MB_Set[i].min_val && temp_value <= MB_Set[i].max_val)
            {
                for (UINT8 byte_cnt = 0; byte_cnt < MB_Set[i].length; byte_cnt++)
                {
                    *(UINT8*) (MB_Set[i].var_add + byte_cnt) = Rx_Tx_data[index_mb][8 - byte_cnt + shift_pos];
                }
                //EEPR_WRITE_IN_POINT(MB_eepr_add[i], MB_Set[i].var_add, MB_Set[i].length);
            } else
                err_conf = 1;
            shift_pos += 2;
        }
        UINT16 crcTx = crc_chk(Rx_Tx_data[index_mb], 6);
        Rx_Tx_data[index_mb][6] = crcTx; //младший байт контрольной суммы
        Rx_Tx_data[index_mb][7] = crcTx >> 8; //старший байт контрольной суммы
        size_Tx_frame[index_mb] = 8;
    } else
        errorLogical(0x02, index_mb); //ошибка адреса
}

void actCF_write(UINT8 index_mb)
{
#define MSB_adress_write 2
#define LSB_adress_write 3
#define MSB_write 4
#define LSB_write 5
    UINT16 adressReg_wr = (Rx_Tx_data[index_mb][MSB_adress_write] << 8) | Rx_Tx_data[index_mb][LSB_adress_write];
    UINT8 error_adr = error_ad(adressReg_wr, 1, 1, &adressReg_wr);
    if (!error_adr)
    {
        UINT16 temp_value = (Rx_Tx_data[index_mb][MSB_write] << 8) | Rx_Tx_data[index_mb][LSB_write];
        if (temp_value >= MB_Set[adressReg_wr].min_val && temp_value <= MB_Set[adressReg_wr].max_val)
        {
            /**(MB_Set[adressReg_wr].var_add) = temp_value;
            if (MB_Set[adressReg_wr].eepr_save)
            {
                EEPR_WRITE(MB_eepr_add[adressReg_wr], temp_value);
            }*/

            for (UINT8 byte_cnt = 0; byte_cnt < MB_Set[adressReg_wr].length; byte_cnt++)
            {
                *(UINT8*) (MB_Set[adressReg_wr].var_add + byte_cnt) = Rx_Tx_data[index_mb][LSB_adress_write - byte_cnt];
            }
            //EEPR_WRITE_IN_POINT(MB_eepr_add[adressReg_wr], MB_Set[adressReg_wr].var_add, MB_Set[adressReg_wr].length);
        } else
            err_conf = 1;
        size_Tx_frame[index_mb] = 8;
    } else
        errorLogical(0x02, index_mb);
}

void actCF_read_some(UINT8 index_mb)
{
#define MSB_adress_read 2
#define LSB_adress_read 3
#define MSB_Number_reg_read 4
#define LSB_Number_reg_read 5
    UINT16 adressReg_read = (Rx_Tx_data[index_mb][MSB_adress_read] << 8) | Rx_Tx_data[index_mb][LSB_adress_read];
    UINT16 Number_reg_read = (Rx_Tx_data[index_mb][MSB_Number_reg_read] << 8) | Rx_Tx_data[index_mb][LSB_Number_reg_read];
    Rx_Tx_data[index_mb][2] = 2 * Number_reg_read; //число читаемых байт данных
    UINT8 i = 0;
    UINT8 shift_pos = 0;
    UINT8 error_adr = error_ad(adressReg_read, 0, Number_reg_read, &adressReg_read);
    if (!error_adr)
    {
        for (i = adressReg_read; i < (adressReg_read + Number_reg_read); i++)
        {
            for (UINT8 byte_cnt = 0; byte_cnt < MB_Set[i].length; byte_cnt++)
            {
                Rx_Tx_data[index_mb][4 - byte_cnt + shift_pos] = *(UINT8*) (MB_Set[i].var_add + byte_cnt); //младший байт данных
            }
            if (MB_Set[i].length == 1)
                Rx_Tx_data[index_mb][3 + shift_pos] = 0;
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
        UINT16 crcTx = crc_chk(Rx_Tx_data[index_mb], 3 + shift_pos);
        Rx_Tx_data[index_mb][3 + shift_pos] = crcTx; //младший байт контрольной суммы
        Rx_Tx_data[index_mb][4 + shift_pos] = crcTx >> 8; //старший байт контрольной суммы
        size_Tx_frame[index_mb] = 5 + shift_pos;
    } else
        errorLogical(0x02, index_mb); //ошибка адреса
}

void CalculTX9Dbit(UINT8 index_mb)
{
    UINT8 cntTB = 0;
    while (cntTB < size_Tx_frame[index_mb])
    {
        UINT8 cntBit = 0;
        UINT8 TempResult = Rx_Tx_data[index_mb][cntTB] & 1;
        while (cntBit < 7)
        {

            cntBit++;
            TempResult ^= (Rx_Tx_data[index_mb][cntTB] >> cntBit & 1);
        }
        TX9Dbit[index_mb][cntTB] = TempResult;
        cntTB++;
    }
}

void actCodeFunc(UINT8 index_mb)
{
    switch (Rx_Tx_data[index_mb][1]) //код функции
    {
        case CF_read_some:
            actCF_read_some(index_mb);
            break;
        case CF_write:
            actCF_write(index_mb);
            break;
        case CF_write_some:
            actCF_write_some(index_mb);
            break;
        default:
            errorLogical(0x01, index_mb); //ошибка функции
            break;
    }
    /* if (COMMAND_REG == 10400)
     {
         COMMAND_REG = 0;
         eepromWrite(TEST_MOD_NOT_OK_ADD, 1);
     }*/
    Number_Tx_Byte[index_mb] = 0;
    if (index_mb == 0)
    {
        CalculTX9Dbit(index_mb);
        TX_OR_RX_1 = TRANSSMIT;
        //LED.led_Blue = 1;
        TX1IE = 1;
    } else if (index_mb == 1)
    {
        CalculTX9Dbit(index_mb);
        TX_OR_RX_2 = TRANSSMIT;
        //LED.led_Blue = 1;
        TX2IE = 1;
    }
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




