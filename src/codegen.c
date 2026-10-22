
#include "codegen.h"

void generate_code(FILE *out, ASTNode *node, Table_Manager *tm)
{
    switch (node->type)
    {
    case NODE_PROGRAM: {
        ASTList *current = node->program.list;
        while (current)
        {
            generate_code(out, current->statement, tm);
            current = current->next;
        }

        break;
    }

    case NODE_FUNCTION_DECL: {
        emit_label(out, node->function.name->identifier);

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
            generate_code(out, current->statement, node->function.tm);
            current = current->next;
        }

        break;
    }
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
            Register reg =
                generate_expr(out, node->var.op.right, node->var.type);

            Register dreg = { .reg = RBP };

            const struct Operand dest = {
                .type = OPERAND_MEMORY,
                .value = { .memory = { .offset = node->var.offset,
                                       .reg = RBP } }
            };

            fprintf(out, "#VAR DEF\n");

            emit_2(out, MOV, node->var.type, REG(reg), dest);

            free_reg(reg);

            var->created = true;

            break;
        }

        break;
    }
    case NODE_RETURN_STATMENT: {
        Register reg = generate_expr(out, node->retst.expr, node->retst.type);

        Register out_rax = { .reg = RAX, .size = node->retst.type };

        emit_2(out, MOV, node->retst.type, REG(reg), REG(out_rax));

        emit_0(out, LEAVE, INT64);
        emit_0(out, RET, INT64);

        break;
    }

    default:
        break;
    }
}

Register generate_expr(FILE *out, ASTNode *node, VarSize type)
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
        Register reg = generate_expr(out, node->unary.operand, type);
        return reg;
    }

    case NODE_UNARY_MINUS: {
        Register reg = generate_expr(out, node->unary.operand, type);

        // emit_1(out, "negq", REG(reg));
        emit_1(out, NEG, type, REG(reg));

        return reg;
    }

    case NODE_ADD: {
        Register r1 = generate_expr(out, node->op.right, type);
        Register r2 = generate_expr(out, node->op.left, type);

        emit_2(out, ADD, type, REG(r2), REG(r1));

        free_reg(r2);

        return r1;
    }

    case NODE_SUB: {
        Register r1 = generate_expr(out, node->op.right, type);
        Register r2 = generate_expr(out, node->op.left, type);

        emit_2(out, SUB, type, REG(r1), REG(r2));

        free_reg(r1);

        return r2;
    }

    case NODE_MUL: {
        Register r1 = generate_expr(out, node->op.right, type);
        Register r2 = generate_expr(out, node->op.left, type);

        emit_2(out, MUL, type, REG(r1), REG(r2));

        free_reg(r1);

        return r2;
    }

    case NODE_DIV: {
        Register r1 = generate_expr(out, node->op.right, type);
        Register r2 = generate_expr(out, node->op.left, type);

        emit_2(out, MOV, type, REG(r1), REG(REG_EAX));
        emit_2(out, MOV, type, IMMEDIATE(0), REG(REG_EDX));
        emit_2(out, MOV, type, REG(r2), REG(REG_EBX));

        emit_1(out, DIV, type, REG(REG_EBX));

        emit_2(out, MOV, type, REG(REG_EAX), REG(r2));

        free_reg(r1);

        return r2;
    }

    case NODE_VARIABLE_CALL: {
        Register reg = { .reg = RBP };

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