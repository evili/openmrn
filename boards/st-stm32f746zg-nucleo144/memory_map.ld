/*
 * Use the first 3 32K sectors of flash for EEPROM emulation,
 * Then the 4th 32K sector for bootloader, and the rest 892K,
 * for normal program Flash. (cfr. RM0385, page 74.)
 * 
 */
MEMORY
{
  EEPROMEMU  (r)   : ORIGIN = 0x08000000, LENGTH = 96K
  BOOTLOADER (rx)  : ORIGIN = 0x08018000, LENGTH = 32K
  FLASH      (rx)  : ORIGIN = 0x08020000, LENGTH = 892K
  RAM        (rwx) : ORIGIN = 0x20000000, LENGTH = 320K
}

__flash_start = ORIGIN(FLASH);
__flash_end = ORIGIN(FLASH) + LENGTH(FLASH);
__eeprom_start = ORIGIN(EEPROMEMU);
__eeprom_end = ORIGIN(EEPROMEMU) + LENGTH(EEPROMEMU);
__bootloader_start = ORIGIN(BOOTLOADER);
__app_header_offset = 0x270;
__bootloader_magic_ptr = ORIGIN(RAM);
