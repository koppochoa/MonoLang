
#include "codegen.h"

void generate_code(FILE *out, ASTNode *node, Function_Table *ft)
{
    switch (node->type)
    {
    case NODE_PROGRAM: {
        ASTList *current = node->program.list;
        while (current)
        {
            generate_code(out, current->statement, ft);
            current = current->next;
        }

        break;
    }

    case NODE_FUNCTION_DECL: {
        emit_label(out, node->function.name);

        Register rbp = { .reg = RBP, .size = INT64 };
        Register rsp = { .reg = RSP, .size = INT64 };

        emit_1(out, PUSH, INT64, REG(rbp));
        emit_2(out, MOV, INT64, REG(rsp), REG(rbp));
        emit_2(out, SUB, INT64, IMMEDIATE(node->function.tm->offset), REG(rsp));

        unsigned long int varcount = 0;
        int varoffset = 16;

        fprintf(out, "#START ARGUMENTS INITIALIZATION\n");

        ASTList *args_current = node->function.args;
        while (args_current)
        {
            generate_args(out, args_current->statement, node->function.used,
                          &varcount, &varoffset);
            args_current = args_current->next;
        }
        fprintf(out, "#END ARGUMENTS INITIALIZATION\n");

        ASTList *current = node->function.body;
        while (current)
        {
            // generate_code(out, current->statement, node->function.tm);
            generate_function(out, current->statement, ft, node->function.tm);
            current = current->next;
        }

        break;
    }

    default:
        break;
    }
}

void generate_function(FILE *out, ASTNode *node, Function_Table *ft,
                       Table_Manager *tm)
{
    switch (node->type)
    {
    case NODE_VARIABLE: {
        if (!tm)
        {
            printf("No Table Manager\n");
            break;
        }

        // Check si la variable existe
        Variable *var = lookupvariable(tm, node->var.op.left->identifier);
        if (var == NULL)
        {
            fprintf(stderr, "Variable inconnue\n");
            return;
        }
        if (var->created == false)
        {
            fprintf(out, "#VAR DEF\n");

            Register reg =
                generate_expr(out, node->var.op.right, ft, node->var.type);

            const struct Operand dest = {
                .type = OPERAND_MEMORY,
                .value = { .memory = { .offset = node->var.offset,
                                       .reg = RBP } }
            };

            emit_2(out, MOV, node->var.type, REG(reg), dest);

            free_reg(reg);

            var->created = true;

            break;
        }

        break;
    }
    case NODE_RETURN_STATMENT: {
        Register reg =
            generate_expr(out, node->retst.expr, ft, node->retst.type);

        Register out_rax = { .reg = RAX, .size = node->retst.type };

        emit_2(out, MOV, node->retst.type, REG(reg), REG(out_rax));

        emit_0(out, LEAVE, INT64);
        emit_0(out, RET, INT64);

        break;
    }
    }
}

Register generate_expr(FILE *out, ASTNode *node, Function_Table *ft,
                       VarSize type)
{
    switch (node->type)
    {
    case NODE_NUMBER: {
        Register free_reg = find_free_reg();
        free_reg.size = type;

        emit_2(out, MOV, type, IMMEDIATE(node->value_int), REG(free_reg));

        use_reg(free_reg);

        return free_reg;
    }

    case NODE_UNARY_PLUS: {
        Register reg = generate_expr(out, node->unary.operand, ft, type);
        return reg;
    }

    case NODE_UNARY_MINUS: {
        Register reg = generate_expr(out, node->unary.operand, ft, type);

        // emit_1(out, "negq", REG(reg));
        emit_1(out, NEG, type, REG(reg));

        return reg;
    }

    case NODE_ADD: {
        Register r1 = generate_expr(out, node->op.right, ft, type);
        Register r2 = generate_expr(out, node->op.left, ft, type);

        emit_2(out, ADD, type, REG(r2), REG(r1));

        free_reg(r2);

        return r1;
    }

    case NODE_SUB: {
        Register r1 = generate_expr(out, node->op.right, ft, type);
        Register r2 = generate_expr(out, node->op.left, ft, type);

        emit_2(out, SUB, type, REG(r1), REG(r2));

        free_reg(r1);

        return r2;
    }

    case NODE_MUL: {
        Register r1 = generate_expr(out, node->op.right, ft, type);
        Register r2 = generate_expr(out, node->op.left, ft, type);

        emit_2(out, MUL, type, REG(r1), REG(r2));

        free_reg(r1);

        return r2;
    }

    case NODE_DIV: {
        Register r1 = generate_expr(out, node->op.right, ft, type);
        Register r2 = generate_expr(out, node->op.left, ft, type);

        emit_2(out, MOV, type, REG(r1), REG(REG_EAX));
        emit_2(out, MOV, type, IMMEDIATE(0), REG(REG_EDX));
        emit_2(out, MOV, type, REG(r2), REG(REG_EBX));

        emit_1(out, DIV, type, REG(REG_EBX));

        emit_2(out, MOV, type, REG(REG_EAX), REG(r2));

        free_reg(r1);

        return r2;
    }
    case NODE_FUNCTION_CALL: {
        fprintf(out, "#FUNC CALL\n");
        struct Operand op = { .type = OPERAND_LABEL,
                              .value = { .label = node->function_call.name } };

        Function *func = lookupfunction(ft, node->function_call.name);
        if (!func)
        {
            // no func err
        }

        ASTList *current_parameter = node->function_call.args;
        ASTList *current_function_parameter_slot = func->args;

        int param_idx = 0;

        while (current_parameter && current_function_parameter_slot)
        {
            VarSize slot_type =
                current_function_parameter_slot->statement->var.type;

            generate_params(out, current_parameter->statement,
                            node->function_call.used, ft, slot_type,
                            &param_idx);

            current_parameter = current_parameter->next;
            current_function_parameter_slot =
                current_function_parameter_slot->next;
        }

        fprintf(out, "   call %s\n", node->function_call.name);

        Register res = {.reg = RAX};
        return res;

        break;
    }
    case NODE_VARIABLE_CALL: {
        const struct Operand src = {
            .type = OPERAND_MEMORY,
            .value = { .memory = { .offset = node->varcall.offset,
                                   .reg = RBP } }
        };
        fprintf(out, "#VAR USE\n");

        Register freereg = find_free_reg();
        freereg.size = node->varcall.type;

        emit_2(out, MOV, node->varcall.type, src, REG(freereg));

        use_reg(freereg);

        return freereg;
    }

    default:
        break;
    }

    Register r = { r.reg = REG_ERR };
    return r;
}

void generate_params(FILE *out, ASTNode *node, bool *used, Function_Table *ft,
                     VarSize type, int *param_idx)
{
    // const struct Operand vout = {
    //     .type = OPERAND_MEMORY,
    //     .value = { .memory = { .offset = node->var.offset,
    //                            .reg = { .reg = RBP, .size = INT64 } } }
    // };

    // if (*varcount >= ARGUMENTS_REG_COUNT)
    // {
    //     const struct Operand vin = {
    //         .type = OPERAND_MEMORY,
    //         .value = { .memory = { .offset = *varoffset, .reg = RBP } }
    //     };

    //     Register tmpr = find_free_reg();
    //     tmpr.size = node->var.type;

    //     emit_2(out, MOV, node->var.type, vin, REG(tmpr));
    //     emit_2(out, MOV, node->var.type, REG(tmpr), vout);

    //     free_reg(tmpr);

    //     (*varoffset) += 8;
    //     return;
    // }

    printf("        Creating params\n");

    Register reg = find_free_reg_from(used);
    reg.size = type;

    Register val = generate_expr(out, node, ft, type);

    use_reg_from(used, *param_idx);
    *param_idx++;

    emit_2(out, MOV, type, REG(val), REG(reg));
}

void generate_args(FILE *out, ASTNode *node, bool *used,
                   unsigned long int *varcount, int *varoffset)
{
    const struct Operand vout = {
        .type = OPERAND_MEMORY,
        .value = { .memory = { .offset = node->var.offset,
                               .reg = { .reg = RBP, .size = INT64 } } }
    };

    if (*varcount >= ARGUMENTS_REG_COUNT)
    {
        const struct Operand vin = {
            .type = OPERAND_MEMORY,
            .value = { .memory = { .offset = *varoffset, .reg = RBP } }
        };

        Register tmpr = find_free_reg();
        tmpr.size = node->var.type;

        emit_2(out, MOV, node->var.type, vin, REG(tmpr));
        emit_2(out, MOV, node->var.type, REG(tmpr), vout);

        free_reg(tmpr);

        (*varoffset) += 8;
        return;
    }

    Register reg = find_free_reg_from(used);
    reg.size = node->var.type;

    emit_2(out, MOV, node->var.type, REG(reg), vout);

    use_reg_from(used, *varcount);
    (*varcount)++;
}