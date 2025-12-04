
#ifndef __st_func_peripheral_H
#define __st_func_peripheral_H

#include "basic_definitions.h"
#include "xc.h"

//чтение 16 бит из ипрома
#define eepr_read_16b_mac(adr) ((eepromRead(adr) << 8) | eepromRead(adr + 1))
//запись 16 бит в ипром
#define eepr_write_16b_mac(adr, value) (eepromWrite(adr, value >> 8), eepromWrite(adr+1, value))

UINT16 adc_convert(UINT8 ADCpin);
void eepromWrite(UINT8 ind, UINT8 data);
UINT8 eepromRead(UINT8 ind);

#endif	/* __st_func_peripheral_H*/

