

/*
 */
#define MAIN_C_DATA
#include "main.h"

// CONFIG1H
#pragma config FOSC = HSMP      // Oscillator Selection bits (HS oscillator (medium power 4-16 MHz))
#pragma config PLLCFG = ON      // 4X PLL Enable (Oscillator multiplied by 4)
#pragma config PRICLKEN = ON    // Primary clock enable bit (Primary clock enabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRTEN = ON      // Power-up Timer Enable bit (Power up timer enabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 285       // Brown Out Reset Voltage bits (VBOR set to 2.85 V nominal)

// CONFIG2H
#pragma config WDTEN = ON       // Watchdog Timer Enable bits (WDT is always enabled. SWDTEN bit has no effect)
#pragma config WDTPS = 512      // Watchdog Timer Postscale Select bits (1:512)

// CONFIG3H
#pragma config CCP2MX = PORTB3  // CCP2 MUX bit (CCP2 input/output is multiplexed with RB3)
#pragma config PBADEN = ON      // PORTB A/D Enable bit (PORTB<5:0> pins are configured as analog input channels on Reset)
#pragma config CCP3MX = PORTB5  // P3A/CCP3 Mux bit (P3A/CCP3 input/output is multiplexed with RB5)
#pragma config HFOFST = ON      // HFINTOSC Fast Start-up (HFINTOSC output and ready status are not delayed by the oscillator stable status)
#pragma config T3CMX = PORTB5   // Timer3 Clock input mux bit (T3CKI is on RB5)
#pragma config P2BMX = PORTB5   // ECCP2 B output mux bit (P2B is on RB5)
#pragma config MCLRE = INTMCLR  // MCLR Pin Enable bit (RE3 input pin enabled; MCLR disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = ON         // Code Protection Block 0 (Block 0 (000800-001FFFh) code-protected)
#pragma config CP1 = ON         // Code Protection Block 1 (Block 1 (002000-003FFFh) code-protected)
#pragma config CP2 = ON         // Code Protection Block 2 (Block 2 (004000-005FFFh) code-protected)
#pragma config CP3 = ON         // Code Protection Block 3 (Block 3 (006000-007FFFh) code-protected)

// CONFIG5H
#pragma config CPB = ON         // Boot Block Code Protection bit (Boot block (000000-0007FFh) code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection Block 0 (Block 0 (000800-001FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection Block 1 (Block 1 (002000-003FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection Block 2 (Block 2 (004000-005FFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection Block 3 (Block 3 (006000-007FFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection Block 0 (Block 0 (000800-001FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection Block 1 (Block 1 (002000-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection Block 2 (Block 2 (004000-005FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection Block 3 (Block 3 (006000-007FFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)

UINT8 lock_signal = 0;

UINT16 Timer_Interrupt_3 = 0;


automat_state_t automat_state[] = {func_save_all, ModBusTxRxFunc};

UINT8 Switch_Transsmit_Recieve[2] = 0;

UINT8 add_dev[2];
UINT8 baud_rate[2];
UINT8 parity[2];
UINT8 add_dev_reg[2];
UINT8 baud_rate_reg[2];
UINT8 parity_reg[2];
UINT8 err_conf;
UINT8 cfg_save;

UINT8 all_reset;
UINT16 debug_var;

UINT16 id;
UINT16 ver;



__EEPROM_DATA("m", "b", " ", "t", "o", " ", "m", "b");
#if DEBUG
__EEPROM_DATA("D", "E", "B", "U", "G", "v", "e", "r");
#else
__EEPROM_DATA("V", "e", "r", " ", soft_ver_S0 + 48, ".", soft_ver_S1 + 48, soft_ver_S2 + 48);
#endif

void main(void)
{
    OSCCONbits.SCS = 0;
    OSCTUNEbits.PLLEN = 1; // 16*4 MGz

    ANSB0 = DIG;
    ANSB1 = DIG;
    ANSB3 = DIG;
    ANSB4 = DIG;
    ANSA3 = DIG;
    ANSB2 = DIG;
    ANSC7 = DIG;
    ANSA5 = DIG;

    TRISA4 = OUT;
    TRISC1 = OUT;
    TRISC2 = OUT;
    TRISC3 = OUT;
    TRISC5 = OUT;
    TRISB7 = OUT;
    TRISB6 = OUT;
    TRISA2 = OUT;
    TRISB5 = OUT;
    TRISA0 = OUT;
    TRISA1 = OUT;

    CCP4CONbits.CCP4M = 0b1011;
    CCPTMRS1bits.C4TSEL = 0;
    CCPR4 = PERIOD_INTERRUPT_MKS * CCP_FREQ_MGZ - 1;
    CCP4IF = 0;
    CCP4IE = 1;
    CCP4IP = 1;

#if 0
    UINT16 temp = 33;
    for (UINT8 index = 0; index < MB_MEMORY_SIZE; index++)
    {


        EEPR_WRITE_VAR(temp, temp);
        temp += 2;
    }
    while (1);
#endif


    //ResetAdress();
    func_initialization();
    if (eepromRead(BEGIN_EEPR_ADD))
    {
        func_reset_all();
        eepromWrite(BEGIN_EEPR_ADD, 0);
    }
    set_baud_rate();


#if 0
    //TX1IE = 1;
    while (1)
    {
        if (TX1IF)
        {
            TXREG1 = 0b10101010;
        }
    }
#endif
    __delay_us(10);
    TranssmitOrRecieve = Recive;
    RCONbits.IPEN = 1;
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;
    __delay_ms(500);
    T1CON = 1;
    /*T0CONbits.T08BIT = 0;
     T0CONbits.T0CS = 0;
     T0CONbits.T0PS = 0b011;
     TMR0ON = 1;*/
    //LED_GREEN = 1;

    while (1)
    {
        CLRWDT();

#if !DEBUG

#else
        static UINT8 i_automat_state = 0;
        automat_state[i_automat_state]();
        if (++i_automat_state == (sizeof (automat_state) / sizeof (automat_state[0])))
        {
            debug_var = i_automat_state;
            i_automat_state = 0;
        }
#endif

    }
}

void CalculTX9Dbit2()
{
    UINT8 cntTB = 0;
    while (cntTB < size_Tx_frame[1])
    {
        UINT8 cntBit = 0;
        UINT8 TempResult = Rx_Tx_data[1][cntTB] & 1;
        while (cntBit < 7)
        {

            cntBit++;
            TempResult ^= (Rx_Tx_data[1][cntTB] >> cntBit & 1);
        }
        TX9Dbit[1][cntTB] = TempResult;
        cntTB++;
    }
}

void actCodeFunc2()
{
  
    /* if (COMMAND_REG == 10400)
     {
         COMMAND_REG = 0;
         eepromWrite(TEST_MOD_NOT_OK_ADD, 1);
     }*/
    Number_Tx_Byte[1] = 0;
    CalculTX9Dbit2();
    TranssmitOrRecieve_2 = Transsmit;
    //LED.led_Blue = 1;
    TX2IE = 1;
}

void ModBusTxRxFunc2()
{
    //DEBUG_PIN = 1;
    if (TranssmitOrRecieve_2 == Recive)
    {
        INTCONbits.GIEL = 0;
        UINT8 temp_Number_Rx_Byte = Number_Rx_Byte[1];
        UINT8 temp_Error_Recive_1_5 = Error_Recive_1_5[1];
        UINT8 temp_mtO = modbus_timeOut[1].timer;
        INTCONbits.GIEL = 1;
        if (temp_Number_Rx_Byte && (temp_mtO > TimeOutFrame_3_5[1]))
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
                UINT16 crcRx = crc_chk(Rx_Tx_data[1], temp_Number_Rx_Byte - 2);
                if (crcRx == (Rx_Tx_data[1][temp_Number_Rx_Byte - 1] << 8 | Rx_Tx_data[1][temp_Number_Rx_Byte - 2]))
                {
                    RCSTA2bits.CREN = 0;
                    actCodeFunc2();
                }
            }
            Number_Rx_Byte[1] = 0;
            Error_Recive_1_5[1] = 0;
        }
    }
    //DEBUG_PIN = 0;
}

void set_baud_rate2()
{
    add_dev[1] = add_dev_reg[1];
    baud_rate[1] = baud_rate_reg[1];
    parity[1] = parity_reg[1];
    UINT32 Speed_devise_bit_sek = CALCUL_SPEED_DEV_BIT_S(baud_rate[1]);
    UINT16 tempSPBRG = CALCUL_SPBRG(Speed_devise_bit_sek);
    SPBRG2 = tempSPBRG;
    SPBRGH2 = tempSPBRG >> 8;
    TimeOutFrame_3_5[1] = CALCUL_T_3_5(Speed_devise_bit_sek);
    TimeOutFrame_1_5[1] = CALCUL_T_1_5(Speed_devise_bit_sek);

    TXSTA2bits.BRGH = 1;
    BAUDCON2bits.BRG16 = 1;
    RCSTA2bits.SPEN = 1;
    TXSTA2bits.SYNC = 0;
    RCSTA2bits.CREN = 1;
    TXSTA2bits.TXEN = 1;
    if (parity[1])
    {
        TXSTA2bits.TX9 = 1;
        RCSTA2bits.RX9 = 1;
    } else
    {
        TXSTA2bits.TX9 = 0;
        RCSTA2bits.RX9 = 0;
    }
    TX2IP = 0;
    RC2IP = 0;
    RC2IE = 1;
}

void func_initialization()
{

    UINT16 begin_eepr_add = BEGIN_EEPR_ADD;
    for (UINT8 index = 0;; index++)
    {
        if (MB_Set[index].eepr_save)
        {
            begin_eepr_add += MB_Set[index].length;
            MB_eepr_add[index] = begin_eepr_add;
            EEPR_READ_IN_POINT(MB_eepr_add[index], MB_Set[index].var_add, MB_Set[index].length);

        } else
        {
            for (UINT8 cnt_byte = 0; cnt_byte < MB_Set[index].length; cnt_byte++)
                *(UINT8*) (MB_Set[index].var_add + cnt_byte) = GET_BYTE(cnt_byte, MB_Set[index].default_val);
        }
        if (MB_Set[index].mb_add == 0xFFF1)
        {
            //debug_var = index;
            break;
        }
    }
    //debug_var = begin_eepr_add;

}

void func_save_all()
{
    if (cfg_save)
    {
        static UINT8 index = 0;
        if (MB_Set[index].eepr_save && MB_Set[index].rw)
        {
            EEPR_WRITE_IN_POINT(MB_eepr_add[index], MB_Set[index].var_add, MB_Set[index].length);
        }
        if (MB_Set[index].mb_add == 0xFFF1)
        {
            index = 0;
            cfg_save = 0;
            set_baud_rate();
            set_baud_rate2();
        } else
            index++;
    }
}

void func_reset_all()
{
    for (UINT8 index = 0;; index++)
    {
        for (UINT8 cnt_byte = 0; cnt_byte < MB_Set[index].length; cnt_byte++)
            *(UINT8*) (MB_Set[index].var_add + cnt_byte) = GET_BYTE(cnt_byte, MB_Set[index].default_val);
        if (MB_Set[index].eepr_save)
        {
            EEPR_WRITE_IN_POINT(MB_eepr_add[index], MB_Set[index].var_add, MB_Set[index].length);
        }
        if (MB_Set[index].mb_add == 0xFFF1)
        {
            //ch1_q_fact = index;
            break;
        }
    }
}

void low_priority interrupt func_interrupt_L(void)
{
    //DEBUG_PIN = 1;

    if (TX1IE && TX1IF)
    {
        if (Number_Tx_Byte[0] < size_Tx_frame[0])
        {
            if (parity[0] == 1)
                TXSTA1bits.TX9D = TX9Dbit[0][Number_Tx_Byte[0]];
            else if (parity[0] == 2)
                TXSTA1bits.TX9D = !TX9Dbit[0][Number_Tx_Byte[0]];
            TXREG1 = Rx_Tx_data[0][Number_Tx_Byte[0]];
            Number_Tx_Byte[0]++;
            if (Number_Tx_Byte[0] == size_Tx_frame[0])
            {
                TX1IE = 0;
                Switch_Transsmit_Recieve[0] = 1;
            }
        }
    } else if (RC1IE && RC1IF)
    {
        //DEBUG_PIN = 1;
        UINT8 receiv_byte = RCREG1;
        if (Number_Rx_Byte[0])
        {
            if (modbus_timeOut[0].timer > TimeOutFrame_1_5[0])
                Error_Recive_1_5[0] = 1;
            if (Number_Rx_Byte[0] < sizeof (Rx_Tx_data[0]))
            {
                Rx_Tx_data[0][Number_Rx_Byte[0]] = receiv_byte;
                Number_Rx_Byte[0]++;
            }
        } else
        {
            if ((modbus_timeOut[0].timer > TimeOutFrame_3_5[0]) && receiv_byte == (UINT8) add_dev)
            {
                //LED_RED = 1;
                Rx_Tx_data[0][0] = receiv_byte;
                Number_Rx_Byte[0] = 1;
            }
        }
        modbus_timeOut[0].timer = 0;
        if (RCSTA1bits.OERR)
        {
            Number_Rx_Byte[0] = 0;
            Error_Recive_1_5[0] = 0;
            RCSTA1bits.CREN = 0;
            RCSTA1bits.CREN = 1;
        }
        //DEBUG_PIN = 0;
    }
    
    if (TX2IE && TX2IF)
    {
        if (Number_Tx_Byte[1] < size_Tx_frame[1])
        {
            if (parity[1] == 1)
                TXSTA2bits.TX9D = TX9Dbit[1][Number_Tx_Byte[1]];
            else if (parity[1] == 2)
                TXSTA2bits.TX9D = !TX9Dbit[1][Number_Tx_Byte[1]];
            TXREG2 = Rx_Tx_data[1][Number_Tx_Byte[1]];
            Number_Tx_Byte[1]++;
            if (Number_Tx_Byte[1] == size_Tx_frame[1])
            {
                TX2IE = 0;
                Switch_Transsmit_Recieve[1] = 1;
            }
        }
    } else if (RC2IE && RC2IF)
    {
        //DEBUG_PIN = 1;
        UINT8 receiv_byte = RCREG2;
        if (Number_Rx_Byte[1])
        {
            if (modbus_timeOut[1].timer > TimeOutFrame_1_5[1])
                Error_Recive_1_5[1] = 1;
            if (Number_Rx_Byte[1] < sizeof (Rx_Tx_data[1]))
            {
                Rx_Tx_data[1][Number_Rx_Byte[1]] = receiv_byte;
                Number_Rx_Byte[1]++;
            }
        } else
        {
            if ((modbus_timeOut[1].timer > TimeOutFrame_3_5[1]) && receiv_byte == (UINT8) add_dev)
            {
                //LED_RED = 1;
                Rx_Tx_data[1][0] = receiv_byte;
                Number_Rx_Byte[1] = 1;
            }
        }
        modbus_timeOut[0].timer = 0;
        if (RCSTA2bits.OERR)
        {
            Number_Rx_Byte[0] = 0;
            Error_Recive_1_5[0] = 0;
            RCSTA2bits.CREN = 0;
            RCSTA2bits.CREN = 1;
        }
        //DEBUG_PIN = 0;
    }
   
    //DEBUG_PIN = 0;
}

void high_priority interrupt func_interrupt_h(void)
{
    //DEBUG_PIN = 1;

    if (CCP4IF)
    {

        if (Switch_Transsmit_Recieve[0] && TXSTA1bits.TRMT) //переключение приемопередатчика с передачи на прием
        {
            TranssmitOrRecieve = Recive;
            // LED.led_Blue = 0;
            modbus_timeOut[0].timer = TimeOutFrame_3_5[0];
            Switch_Transsmit_Recieve[0] = 0;
            RCSTA1bits.CREN = 1; //UART включение приемника
        }
        if (Switch_Transsmit_Recieve[1] && TXSTA2bits.TRMT) //переключение приемопередатчика с передачи на прием
        {
            TranssmitOrRecieve_2 = Recive;
            // LED.led_Blue = 0;
            modbus_timeOut[1].timer = TimeOutFrame_3_5[1];
            Switch_Transsmit_Recieve[1] = 0;
            RCSTA2bits.CREN = 1; //UART включение приемника
        }


        if (modbus_timeOut[0].timer != 0xFF)
        {
            static UINT8 internal_timer_time_Out = 0;
            if (++internal_timer_time_Out >= TIME_OUT_FRAME_MKS / PERIOD_INTERRUPT_MKS)
            {
                internal_timer_time_Out = 0;
                modbus_timeOut[0].timer++;
            }
        }
        if (modbus_timeOut[1].timer != 0xFF)
        {
            static UINT8 internal_timer_time_Out = 0;
            if (++internal_timer_time_Out >= TIME_OUT_FRAME_MKS / PERIOD_INTERRUPT_MKS)
            {
                internal_timer_time_Out = 0;
                modbus_timeOut[1].timer++;
            }
        }
        

        Timer_Interrupt_3++;
        CCP4IF = 0;
    }
    //DEBUG_PIN = 0;
}