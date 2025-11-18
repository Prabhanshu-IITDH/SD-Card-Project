#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "ff.h"

// ---------------- LED functions ----------------

//*****************************************************************************
// FatFs mandatory RTC function. Returns current time in FAT format.
// This example returns a fixed time: 1st Jan 2025, 0:00:00.
//*****************************************************************************
DWORD get_fattime (void)
{
    /* Packed time format:
     * Bit 31:25 - Year offset from 1980 (0..127)
     * Bit 24:21 - Month (1..12)
     * Bit 20:16 - Day (1..31)
     * Bit 15:11 - Hour (0..23)
     * Bit 10:5  - Minute (0..59)
     * Bit 4:0   - Second / 2 (0..29)
     */
    return ((DWORD)(2025 - 1980) << 25) // Year = 2025 (45)
          | ((DWORD)1 << 21)           // Month = 1 (January)
          | ((DWORD)1 << 16)           // Day = 1
          | ((DWORD)0 << 11)           // Hour = 0
          | ((DWORD)0 << 5)            // Minute = 0
          | ((DWORD)0 >> 1);           // Second = 0
}

// ---------------- LED functions ----------------
// ... rest of your main.c code ...


void LED_Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));

    GPIOUnlockPin(GPIO_PORTF_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,
                          GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
}

void LED(uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t val = 0;
    if(r) val |= GPIO_PIN_1;
    if(b) val |= GPIO_PIN_2;
    if(g) val |= GPIO_PIN_3;
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, val);
}

void Delay_ms(uint32_t ms)
{
    SysCtlDelay((SysCtlClockGet()/3000)*ms);
}

// ---------------- Main ----------------
FATFS fs;
FIL file;
UINT bw, br;
uint8_t buf[32];

int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL |
                   SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    LED_Init();
    LED(1,0,0);   // RED: start

    // --- Stage 1: Mount SD ---
    if (f_mount(0, &fs) != FR_OK)
    {
        while(1) LED(1,0,0); // ERROR red
    }

    LED(0,1,0);   // GREEN: mounted
    Delay_ms(500);

    // --- Stage 2: Create file ---
    if (f_open(&file, "test.txt", FA_WRITE | FA_CREATE_ALWAYS) != FR_OK)
    {
        while(1) LED(1,0,0); // ERROR red
    }

    LED(0,0,1);   // BLUE: file created
    Delay_ms(500);

    // --- Stage 3: Write ---
    if (f_write(&file, "HELLO WORLD\n", 12, &bw) != FR_OK)
    {
        while(1) LED(1,0,0);
    }

    f_close(&file);

    LED(1,1,0);   // YELLOW: write done
    Delay_ms(500);

    // --- Stage 4: Read ---
    if (f_open(&file, "test.txt", FA_READ) != FR_OK)
    {
        while(1) LED(1,0,0);
    }

    if (f_read(&file, buf, 12, &br) != FR_OK)
    {
        while(1) LED(1,0,0);
    }

    f_close(&file);

    LED(1,0,1);   // PURPLE: read done

    while(1)
    {
        LED(1,0,1);
        Delay_ms(300);
        LED(0,0,0);
        Delay_ms(300);
    }
}
