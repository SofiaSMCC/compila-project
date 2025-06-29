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
    vector<string> argRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    out << f->nombre << ":\n";
    out << "    pushq %rbp\n";
    out << "    movq %rsp, %rbp\n";
    int size = f->parametros.size();
    for (int i = 0; i < size; i++) {
        memoria[f->parametros[i]] = offset;
        out << "    movq " << argRegs[i] << ", " << offset << "(%rbp)\n";
        offset -= 8;
    }
    f->cuerpo->vardecs->accept(this);
    int reserva = -offset -8 ;
    if (reserva > 0)
        out << "    subq $" << reserva << ", %rsp\n";
    f->cuerpo->slist->accept(this);
    out << ".end_" << f->nombre << ":\n";
    out << "    leave\n";
    out << "    ret\n";
    entornoFuncion = false;
}


void GenCode::visit(Body* b) {
    if (b->vardecs) b->vardecs->accept(this);
    if (b->slist) b->slist->accept(this);
}

void GenCode::visit(VarDecList* stm) {
    for (auto vd : stm->vardecs){
        vd->accept(this);
    }
}

void GenCode::visit(VarDec* stm) {
    for (auto var : stm->vars) {
        memoria[var->id] = offset;
        if (var->iv->value) {
            var->iv->value->accept(this);
            out << "    movq %rax, " << offset << "(%rbp)\n";
        }
        offset -= 8;
    }
}

void GenCode::visit(StatementList* stm) {
    for (auto s : stm->stms){
        s->accept(this);
    }
}

void GenCode::visit(AssignStatement* stm) {
    stm->rhs->accept(this);
    out << "    movq %rax, " << memoria[stm->lvalue->id] << "(%rbp)\n";
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

void GenCode::visit(Var *v) {
}

void GenCode::visit(WhileStatement *stmt) {}

void GenCode::visit(IfStatement *stmt) {

}

void GenCode::visit(ForStatement *stmt) {}

void GenCode::visit(FCallStatement *stm) {}

void GenCode::visit(DoWhileStatement *stm) {}
void GenCode::visit(FunDecList *fdl) {
    for (auto f : fdl->Fundecs)
        f->accept(this);
}

// Exp

ImpValue GenCode::visit(StringLiteral *exp) {
    return ImpValue();
}

ImpValue GenCode::visit(BoolExp *exp) {
    return ImpValue();
}

ImpValue GenCode::visit(ArrayAccessExp *exp) {
    return ImpValue();
}

ImpValue GenCode::visit(IFExp *exp) {return ImpValue();}

ImpValue GenCode::visit(LValue *exp) {return ImpValue();}

ImpValue GenCode::visit(FCallExp *exp) {
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
    exp->left->accept(this);
    out << "    pushq %rax\n";
    exp->right->accept(this);
    out << "    movq %rax, %rcx\n";
    out << "    popq %rax\n";
    switch (exp->op) {
        case PLUS_OP:
            out << "    addq %rcx, %rax\n"; break;
        case MINUS_OP:
            out << "    subq %rcx, %rax\n"; break;
        case MUL_OP:
            out << "    imulq %rcx, %rax\n"; break;
        case DIV_OP:
            out << "    cqto\n";
            out << "    idivq %rcx\n";
            break;
        case LT_OP:     // <
            out << "    cmpq %rcx, %rax\n"
                   "    movl $0, %eax\n"
                   "    setl %al\n"
                   "    movzbq %al, %rax\n";
            break;
        case LET_OP:    // <=
            out << "    cmpq %rcx, %rax\n"
                   "    movl $0, %eax\n"
                   "    setle %al\n"
                   "    movzbq %al, %rax\n";
            break;
        case GT_OP:      // >
            out << "    cmpq %rcx, %rax\n"
                   "    movl $0, %eax\n"
                   "    setg %al\n"
                   "    movzbq %al, %rax\n";
            break;
        case GET_OP:     // >=
            out << "    cmpq %rcx, %rax\n"
                   "    movl $0, %eax\n"
                   "    setge %al\n"
                   "    movzbq %al, %rax\n";
            break;
        case EQ_OP:      // ==
            out << "    cmpq %rcx, %rax\n"
                   "    movl $0, %eax\n"
                   "    sete %al\n"
                   "    movzbq %al, %rax\n";
            break;
        case DIFF_OP:    // !=
            out << "    cmpq %rcx, %rax\n"
                   "    movl $0, %eax\n"
                   "    setne %al\n"
                   "    movzbq %al, %rax\n";
            break;
        case AND_OP:     // &&
            // Logical AND: (a && b) => result 1 if both nonzero
            out << "    setne %al\n"
                   "    movzbq %al, %rax\n"
                   "    setne %cl\n"
                   "    movzbq %cl, %rcx\n"
                   "    andq %rcx, %rax\n";
            break;
        case OR_OP:      // ||
            out << "    setne %al\n"
                   "    movzbq %al, %rax\n"
                   "    setne %cl\n"
                   "    movzbq %cl, %rcx\n"
                   "    orq %rcx, %rax\n";
            break;
        case INC_OP:     // ++
            out << "    addq $1, %rax\n";
            break;
        case DEC_OP:     // --
            out << "    subq $1, %rax\n";
        case NOT_OP:    // !
            out << "    cmpq $0, %rax\n"
                   "    movl $0, %eax\n"
                   "    sete %al\n"
                   "    movzbq %al, %rax\n";
            break;
    }
    return ImpValue();
}