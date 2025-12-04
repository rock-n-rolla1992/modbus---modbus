#include "st_func_peripheral.h"

UINT16 adc_convert(UINT8 ADCpin)
{
    ADCON0bits.CHS = ADCpin; // выбираем данный канал 
    _delay(9); // задержка дл€ зар€дки конденсатора
    ADCON0bits.GO = 1; // начать преобразование 
    while (ADCON0bits.GO) // ждем, пока преобразование завершит 
        continue;
    return (ADRES);
} // вернуть результат }

void eepromWrite(UINT8 ind, UINT8 data) {
    UINT8 intrGIEH;
    UINT8 intrGIEL;

    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;
    EECON1bits.WREN = 1;
    EEADR = ind;
    EEDATA = data;

    // запоминаем состо€ние флага разрешени€ прерываний
    intrGIEH = GIEH;
    intrGIEL = GIEL;
    GIEH = GIEL = 0;
    EECON2 = 0x55; // Ќ≈ –ј«ƒ≈Ћя“№
    EECON2 = 0xaa; // Ќ≈ –ј«ƒ≈Ћя“№
    EECON1bits.WR = 1;

    // восстанавливаем флаг разрешени€ прерываний
    GIEH = intrGIEH;
    GIEL = intrGIEL;
    while (EECON1bits.WR)
        continue;
}

UINT8 eepromRead(UINT8 ind) {
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;
    EEADR = ind;
    EECON1bits.RD = 1;
#if 0
    NOP();
    NOP();
    NOP();
    NOP();
#else
    while (EECON1bits.RD)
        continue;
#endif

    return EEDATA;
}
