

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

UINT8 service_mode = 0;
UINT8 global_timer_ms = 0;
automat_state_t automat_state[] = {func_save_all, blinker_func};
UINT8 Switch_Transsmit_Recieve[2] = 0;

//переменные модбаса применяемые по команде 
UINT8 lock_signal;
UINT8 own_address;
UINT8 baud_rate[2];
UINT8 parity[2];

//модбас переменные
UINT8 lock_signal_reg;
UINT8 own_address_reg;
UINT8 add_dev[32];
UINT8 baud_rate_reg[2];
UINT8 parity_reg[2];
UINT8 err_conf;
UINT8 cfg_save;

UINT8 add_dev_begin_1;
UINT8 add_dev_end_1;
UINT8 add_dev_begin_2;
UINT8 add_dev_end_2;
UINT8 add_dev_begin_3;
UINT8 add_dev_end_3;

UINT8 all_reset;
UINT16 debug_var;

UINT16 id;
UINT16 ver;



__EEPROM_DATA("M", "S", "-", "1", "5", " ", " ", " ");
#if DEBUG
__EEPROM_DATA("D", "E", "B", "U", "G", "v", "e", "r");
#else
__EEPROM_DATA("V", "e", "r", " ", soft_ver_S0 + 48, ".", soft_ver_S1 + 48, soft_ver_S2 + 48);
#endif

void main(void)
{
    OSCCONbits.SCS = 0;
    OSCTUNEbits.PLLEN = 1; // 16*4 MGz

    ANSC7 = DIG;

    TRISC5 = OUT;
    TRISB5 = OUT;
    TRISC6 = OUT;
    TRISB6 = OUT;
    TRISA0 = OUT;
    TRISA1 = OUT;
    TRISC1 = OUT;
    TRISA4 = OUT;

    INTCON2bits.nRBPU = 0;
    WPUE3 = 1;
    WPUB = 0;


    CCP4CONbits.CCP4M = 0b1011;
    CCPTMRS1bits.C4TSEL = 0;
    CCPR4 = PERIOD_INTERRUPT_MKS * CCP_FREQ_MGZ - 1;
    CCP4IF = 0;
    CCP4IE = 1;
    CCP4IP = 1;

#if 0
    __delay_us(10);
    check_add(1);
#endif



    func_initialization();
    Reset_Lock();
    if (eepromRead(BEGIN_EEPR_ADD))
    {
        func_reset_all();
        eepromWrite(BEGIN_EEPR_ADD, 0);
    }
    func_get_val_reg();
    set_baud_rate(1);
    set_baud_rate(2);


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

    TranssmitOrRecieve_1 = Recive;
    TranssmitOrRecieve_2 = Recive;
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
        ModBusTxRxFunc(1);
        ModBusTxRxFunc(2);
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

void func_get_val_reg()
{
    
    own_address = (service_mode)?247:own_address_reg;
    lock_signal = lock_signal_reg;
    baud_rate[0] = baud_rate_reg[0];
    parity[0] = parity_reg[0];
    baud_rate[1] = baud_rate_reg[1];
    parity[1] = parity_reg[1];
}

void Reset_Lock()
{
    while (!BUTTON)
    {
        static UINT8 cnt = 0;
        if (++cnt >= 40)
        {
            service_mode = 1;
            //lock_signal_reg = 0;
            cnt = 0;
            return;
        }
        CLRWDT();
        __delay_ms(100);
    }
}

void blinker_func()
{
    UINT8 delta_time_ms = global_timer_ms;
    global_timer_ms -= delta_time_ms;
    static blinker_t blinker_5Gz_1_2 = {0};
    blinker_mac(blinker_5Gz_1_2, 100, 100);
    static blinker_t blinker_1Gz_1_10 = {0};
    blinker_mac(blinker_1Gz_1_10, 100, 900);
    LED_POWER = (service_mode) ? blinker_5Gz_1_2.state : blinker_1Gz_1_10.state;
}

void act_sluice(UINT8 index_mb, UINT8 temp_Number_Rx_Byte)
{
    /* if (COMMAND_REG == 10400)
     {
         COMMAND_REG = 0;
         eepromWrite(TEST_MOD_NOT_OK_ADD, 1);
     }*/
    Number_Tx_Byte[index_mb] = 0;

    if (index_mb == 0)
    {
        size_Tx_frame[1] = temp_Number_Rx_Byte;
        for (UINT8 index = 0; index < size_Tx_frame[1]; index++)
        {
            Rx_Tx_data[1][index] = Rx_Tx_data[0][index];
        }
        CalculTX9Dbit(1);
        Number_Tx_Byte[1] = 0;
        TranssmitOrRecieve_2 = Transsmit;
        //LED.led_Blue = 1;
        TX2IE = 1;
    } else if (index_mb == 1)
    {
        size_Tx_frame[0] = temp_Number_Rx_Byte;
        for (UINT8 index = 0; index < size_Tx_frame[0]; index++)
        {
            Rx_Tx_data[0][index] = Rx_Tx_data[1][index];
        }
        CalculTX9Dbit(0);
        Number_Tx_Byte[0] = 0;
        TranssmitOrRecieve_1 = Transsmit;
        //LED.led_Blue = 1;
        TX1IE = 1;
    }

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
            service_mode = 0;
            func_get_val_reg();
            set_baud_rate(1);
            set_baud_rate(2);
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

UINT8 check_add(UINT8 receiv_byte)
{
    for (UINT8 * add_add = add_dev; add_add <= &add_dev[31]; add_add++)
    {
        if (*add_add == receiv_byte)
        {
            return 1;
        }
    }
    if (receiv_byte >= add_dev_begin_1 && receiv_byte <= add_dev_end_1)
    {
        return 1;
    } else if (receiv_byte >= add_dev_begin_2 && receiv_byte <= add_dev_end_2)
    {
        return 1;
    } else if (receiv_byte >= add_dev_begin_3 && receiv_byte <= add_dev_end_3)
    {
        return 1;
    } else if (receiv_byte == own_address)
        return 1;
    return 0;
}

void low_priority interrupt func_interrupt_L(void)
{
    DEBUG_PIN = 1;
    UART_PORT_HUNDLER(1);
    UART_PORT_HUNDLER(2);
    DEBUG_PIN = 0;
}

void high_priority interrupt func_interrupt_h(void)
{
    //DEBUG_PIN = 1;

    if (CCP4IF)
    {
        UART_TRANSSMITED_ENDED(1);
        UART_TRANSSMITED_ENDED(2);
        static UINT8 internal_timer_1ms = 0;
        if (++internal_timer_1ms >= 1000 / PERIOD_INTERRUPT_MKS)
        {
            internal_timer_1ms = 0;
            global_timer_ms++;
        }
        global_timer_ms++;
        CCP4IF = 0;
    }
    //DEBUG_PIN = 0;
}