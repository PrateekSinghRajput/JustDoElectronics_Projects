
#include<stdint.h>

int main(void)
{
    // Clock control registers
	uint32_t *pClkCtrlReg =   (uint32_t*)0x40023830;
    uint32_t *pPortDModeReg = (uint32_t*)0x40020C00;//port D mode
    uint32_t *pPortDOutReg =  (uint32_t*)0x40020C14; //
    uint32_t *pPortAModeReg = (uint32_t*)0x40020000; //port A mode
    uint32_t *pPortAInReg =   (uint32_t*)0x40020010;

    *pClkCtrlReg |= (1 << 0); // Enable clock for GPIOA
    *pClkCtrlReg |= (1 << 3); // Enable clock for GPIOD

    *pPortDModeReg &= ~( 1 << 25);
    *pPortDModeReg |= ( 1 << 24);


    *pPortAModeReg &= ~(1 << 0);
    *pPortAModeReg &= ~(1 << 1);

    while(1)
    {

        if ((*pPortAInReg & (1 << 0)))
        {


             *pPortDOutReg |= (1 << 12);

        }
        else
        {

            *pPortDOutReg &= ~(1 << 12);

        }

        for(uint32_t i = 0; i < 100000; i++);
    }
}
