#include "codegen.h"

void GenCode::generar(Program* program) {
    out << ".data\n";
    out << "print_fmt: .string \"%ld\\n\"\n";
    out << ".text\n";
    out << ".globl main\n";
    program->func->accept(this);
    out << ".section .note.GNU-stack,\"\",@progbits\n";
}

void GenCode::visit(Program* program) {
    program->func->accept(this);
}

void GenCode::visit(FunDec* f) {
    entornoFuncion = true;
    memoria.clear();
    offset = -8;
    nombreFuncion = f->nombre;
    std::vector<std::string> argRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    //out << " .globl "<< f->nombre << std::endl;
    out << f->nombre <<":" << std::endl;
    out << "    pushq %rbp\n";
    out << "    movq %rsp, %rbp\n";

    int size = f->parametros.size();

    for (int i = 0; i < size; i++) {
        memoria[f->parametros[i]] = offset;
        out << "    movq " << argRegs[i] << ", " << offset << "(%rbp)\n";
        offset -= 8;
    }

    if (f->cuerpo->vardecs){
        f->cuerpo->vardecs->accept(this);
    }

    int reserva = -offset;
    if (reserva>0) {
        out << "    subq $" << reserva <<", %rsp" << std::endl;
    }

    if (f->cuerpo->vardecs) {
        for (auto vd : f->cuerpo->vardecs->vardecs) {
            for (auto var : vd->vars) {
                if (!var->dimList.empty()) {
                    int size = (*var->dimList.begin())->value;
                    for (int idx = 0; idx < size; ++idx) {
                        out << "    movq $0, " << (memoria[var->id] + idx * 8) << "(%rbp)\n";
                    }
                } else {
                    if (var->iv && var->iv->value) {
                        var->iv->value->accept(this);
                        out << "    movq %rax, " << memoria[var->id] << "(%rbp)\n";
                    } else {
                        out << "    movq $0, " << memoria[var->id] << "(%rbp)\n";
                    }
                }
            }
        }
    }

    if (f->cuerpo->slist)
        f->cuerpo->slist->accept(this);

    out << ".end_" << f->nombre <<":" << std::endl;
    out << "leave" << std::endl;
    out << "ret" << std::endl;
    entornoFuncion = false;
}

void GenCode::visit(VarDec* stm) {
    for (auto var : stm->vars) {
        if (!var->dimList.empty()) {
            int size = (*var->dimList.begin())->value;
            memoria[var->id] = offset;
            offset -= size * 8;
        } else {
            if (memoria.count(var->id) == 0)  {
                memoria[var->id] = offset;
                offset -= 8;
            }
        }
    }
}
void GenCode::visit(Body* b) {
    if (b->vardecs){
        b->vardecs->accept(this);
    }
    if (b->slist){
        b->slist->accept(this);
    }
}

void GenCode::visit(VarDecList* stm) {
    for (auto vd : stm->vardecs){
        vd->accept(this);
    }
}

void GenCode::visit(StatementList* stm) {
    for (auto s : stm->stms){
        s->accept(this);
    }
}

void GenCode::visit(AssignStatement* stm) {
    if (!stm->lvalue->indices.empty()) {
        stm->lvalue->indices[0]->accept(this);
        out << "    movq %rax, %rcx\n";
        stm->rhs->accept(this);
        out << "    movq %rax, " << memoria[stm->lvalue->id] << "(%rbp, %rcx, 8)\n";
    } else {
        stm->rhs->accept(this);
        out << "    movq %rax, " << memoria[stm->lvalue->id] << "(%rbp)\n";
    }
}

void GenCode::visit(PrintStatement* stm) {
    stm->e->accept(this);
    out <<
        "    movq %rax, %rsi\n"
        "    leaq print_fmt(%rip), %rdi\n"
        "    movl $0, %eax\n"
        "    call printf@PLT\n";
}

void GenCode::visit(ReturnStatement* stm) {
    stm->e->accept(this);
    out << "    jmp .end_" << nombreFuncion << "\n";
}

void GenCode::visit(Var *v) {}

void GenCode::visit(WhileStatement *stmt) {
    int start_label = labelcont++;
    int end_label = labelcont++;

    out << "while_" << start_label << ":\n";
    stmt->condition->accept(this);
    out << "    cmpq $0, %rax\n";
    out << "    je endw_" << end_label << "\n";

    stmt->b->accept(this);

    out << "    jmp while_" << start_label << "\n";
    out << "endw_" << end_label << ":\n";
}

void GenCode::visit(IfStatement *stmt) {
    int endif_label = labelcont++;
    std::vector<int> else_labels;

    for (size_t i = 0; i < stmt->sent_if.size() - 1; ++i){
        else_labels.push_back(labelcont++);
    }

    for (size_t i = 0; i < stmt->sent_if.size(); ++i) {
        IFExp* ifexp = stmt->sent_if[i];

        if (ifexp->condi) {
            ifexp->condi->accept(this);
            if (i < else_labels.size())
                out << "    cmpq $0, %rax\n"
                    << "    je else_" << else_labels[i] << "\n";
            else
                out << "    cmpq $0, %rax\n"
                    << "    je endif_" << endif_label << "\n";
        }

        ifexp->body->accept(this);

        if (i < stmt->sent_if.size() - 1)
            out << "    jmp endif_" << endif_label << "\n";

        if (i < else_labels.size())
            out << "else_" << else_labels[i] << ":\n";
    }

    out << "endif_" << endif_label << ":\n";
}

void GenCode::visit(ForStatement *stmt) {
    if (memoria.count(stmt->id) == 0) {
        memoria[stmt->id] = offset;
        offset -= 8;
    }

    stmt->start->accept(this);
    out << "    movq %rax, " << memoria[stmt->id] << "(%rbp)\n";

    int loop_label = labelcont++;
    int end_label = labelcont++;

    out << "for_" << loop_label << ":\n";

    stmt->condition->accept(this);
    out << "    cmpq $0, %rax\n";
    out << "    je endfor_" << end_label << "\n";

    stmt->b->accept(this);
    stmt->step->accept(this);

    out << "    movq %rax, " << memoria[stmt->id] << "(%rbp)\n";

    out << "    jmp for_" << loop_label << "\n";
    out << "endfor_" << end_label << ":\n";
}

void GenCode::visit(FCallStatement *stm) {
    stm->call->accept(this);
}

void GenCode::visit(DoWhileStatement *stm) {
    int start_label = labelcont++;
    out << "dowhile_" << start_label << ":\n";
    stm->b->accept(this);
    stm->condition->accept(this);
    out << "    cmpq $0, %rax\n";
    out << "    jne dowhile_" << start_label << "\n";
}

void GenCode::visit(FunDecList *fdl) {
    for (auto f : fdl->Fundecs)
        f->accept(this);
}

// Exp

ImpValue GenCode::visit(StringLiteral *exp) {
    return ImpValue();
}

ImpValue GenCode::visit(BoolExp *exp) {
    out << "    movq $" << (exp->value ? 1 : 0) << ", %rax\n";
    return ImpValue();
}


ImpValue GenCode::visit(ArrayAccessExp *exp) {
    // Unidimensional
    exp->indices[0]->accept(this);
    out << "    movq %rax, %rdx\n";
    out << "    movq " << memoria[exp->arrayName] << "(%rbp, %rdx, 8), %rax\n";
    return ImpValue();
}

ImpValue GenCode::visit(IFExp *exp) {
    return ImpValue();
}

ImpValue GenCode::visit(LValue *exp) {
    if (exp->indices.empty()) {
        out << "    movq " << memoria[exp->id] << "(%rbp), %rax\n";
    } else {
        exp->indices[0]->accept(this);
        out << "    movq %rax, %rdx\n";
        out << "    movq " << memoria[exp->id] << "(%rbp, %rdx, 8), %rax\n";
    }
    return ImpValue();
}

ImpValue GenCode::visit(FCallExp* exp) {
    int nargs = exp->argumentos.size();
    std::vector<std::string> argRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    for (int i = 0; i < nargs && i < 6; ++i) {
        exp->argumentos[i]->accept(this);
        out << "    movq %rax, " << argRegs[i] << "\n";
    }
    out << "    call " << exp->nombre<< "\n";
    return ImpValue();
}


ImpValue GenCode::visit(InitValue *iv) {
    return ImpValue();
}

ImpValue GenCode::visit(NumberExp* exp) {
    out << "    movq $" << exp->value << ", %rax\n";
    return ImpValue();
}

ImpValue GenCode::visit(IdentifierExp* exp) {
    out << "    movq " << memoria[exp->name] << "(%rbp), %rax\n";
    return ImpValue();
}

ImpValue GenCode::visit(BinaryExp* exp) {
    if (exp->op == INC_OP) {
        exp->left->accept(this);
        out << "    addq $1, %rax\n";
        return ImpValue();
    }
    if (exp->op == DEC_OP) {
        exp->left->accept(this);
        out << "    subq $1, %rax\n";
        return ImpValue();
    }
    if (exp->op == NOT_OP && exp->right == nullptr) {
        exp->left->accept(this);
        out <<
            "    cmpq $0, %rax\n"
            "    movl $0, %eax\n"
            "    sete %al\n"
            "    movzbq %al, %rax\n";
        return ImpValue();
    }

    exp->left->accept(this);
    out << "    pushq %rax\n";
    exp->right->accept(this);
    out << "    movq %rax, %rdx\n";
    out << "    popq %rax\n";
    switch (exp->op) {
        case PLUS_OP:
            out << "    addq %rdx, %rax\n"; break;
        case MINUS_OP:
            out << "    subq %rdx, %rax\n"; break;
        case MUL_OP:
            out << "    imulq %rdx, %rax\n"; break;
        case DIV_OP:
            out << "    cqto\n";
            out << "    idivq %rdx\n";
            break;
        case LT_OP:
            out << "    cmpq %rdx, %rax\n"
                   "    movl $0, %eax\n"
                   "    setl %al\n"
                   "    movzbq %al, %rax\n";
            break;
        case LET_OP:
            out << "    cmpq %rdx, %rax\n"
                   "    movl $0, %eax\n"
                   "    setle %al\n"
                   "    movzbq %al, %rax\n";
            break;
        case GT_OP:
            out << "    cmpq %rdx, %rax\n"
                   "    movl $0, %eax\n"
                   "    setg %al\n"
                   "    movzbq %al, %rax\n";
            break;
        case GET_OP:
            out << "    cmpq %rdx, %rax\n"
                   "    movl $0, %eax\n"
                   "    setge %al\n"
                   "    movzbq %al, %rax\n";
            break;
        case EQ_OP:
            out << "    cmpq %rdx, %rax\n"
                   "    movl $0, %eax\n"
                   "    sete %al\n"
                   "    movzbq %al, %rax\n";
            break;
        case DIFF_OP:
            out << "    cmpq %rdx, %rax\n"
                   "    movl $0, %eax\n"
                   "    setne %al\n"
                   "    movzbq %al, %rax\n";
            break;
        case AND_OP:
            out << "    setne %al\n"
                   "    movzbq %al, %rax\n"
                   "    setne %dl\n"
                   "    movzbq %dl, %rdx\n"
                   "    andq %rdx, %rax\n";
            break;
        case OR_OP:
            out << "    setne %al\n"
                   "    movzbq %al, %rax\n"
                   "    setne %dl\n"
                   "    movzbq %dl, %rdx\n"
                   "    orq %rdx, %rax\n";
            break;
        default:
            break;
    }
    return ImpValue();
}