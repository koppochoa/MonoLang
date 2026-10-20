#include "register.h"

RegisterManager rm = {0x0};

Register find_free_reg()
{
    Register reg = {0x0};
    reg.reg = REG_ERR;

    for(int i = 0; i < ALLOCATABLE_REG_COUNT; i++)
    {
        if(rm.used[i] == false)
        {
            reg.reg = (RegisterId)i;
            return reg;
        }
    }
    
    return reg;
}

void use_reg(Register reg)
{
    rm.used[reg.reg] = true;
}

void free_reg(Register reg)
{
    rm.used[reg.reg] = false;
}