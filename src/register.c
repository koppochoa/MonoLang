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

Register find_free_reg_from(bool* arr)
{
    Register reg = {0x0};
    reg.reg = REG_ERR;

    for(int i = 0; i < ARGUMENTS_REG_COUNT; i++)
    {
        if(arr[i] == false)
        {
            reg.reg = arguments_regs[i];
            return reg;
        }
    }

    return reg;
}

void use_reg_from(bool* arr, int idx)
{
    arr[idx] = true;
}