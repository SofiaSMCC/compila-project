#include "codegen.h"

// Asume que 'out' es std::ostream&
std::string GenCode::escapeString(const std::string& raw) {
    std::string out;
    for (char c : raw) {
        switch (c) {
            case '\n': out += "\\n"; break;
            case '\t': out += "\\t"; break;
            case '\"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            default: out += c;
        }
    }
    return out;
}
string GenCode::registrarStringLiteral(const std::string& val) {
    if (literalToLabel.count(val)) return literalToLabel[val];
    string label = "str_" + to_string(stringLabelCounter++);
    literalToLabel[val] = label;
    stringLiterals[label] = val;
    return label;
}

void GenCode::generar(Program* program) {
    primeraPasada = true;
    env.add_level();
    program->accept(this);
    env.remove_level();
    out << ".data\n";
    out << "print_fmt: .string \"%d\\n\"\n";
    for (const auto& entry : stringLiterals) {
        out << entry.first << ": .string \"" << escapeString(entry.second) << "\"\n";
    }
    // Segunda pasada: emitir código
    out << ".text\n";
    out << " .globl main\n";
    primeraPasada = false;
    env.add_level();
    program->accept(this);
    env.remove_level();
    out << ".section .note.GNU-stack,\"\",@progbits\n";

}

void GenCode::visit(Program* program) {
    if (program->func) program->func->accept(this);
}

void GenCode::visit(FunDec* f) {
    entornoFuncion = true;
    memoria.clear();
    offset = -8;
    nombreFuncion = f->nombre;
    std::vector<std::string> argRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    if (!primeraPasada){
        out << " .globl "<< f->nombre << "\n";
        out << f->nombre <<":\n";
        out << "    pushq %rbp\n";
        out << "    movq %rsp, %rbp\n";
    }

    int size = f->parametros.size();
    for (int i = 0; i < size && i < 6; i++) {
        memoria[f->parametros[i]] = offset;
           if (!primeraPasada) {out << "    movq " << argRegs[i] << ", " << offset << "(%rbp)\n";}
        offset -= 8;
    }

    if (f->cuerpo->vardecs) {
        f->cuerpo->vardecs->accept(this);
    }
    if (f->cuerpo->slist)
        f->cuerpo->slist->accept(this);

       if (!primeraPasada){
        out << ".end_" << f->nombre << ":\n";
        out << "    leave\n";
        out << "    ret\n";
    }
    entornoFuncion = false;
}

void GenCode::visit(VarDec* stm) {
    for (auto var : stm->vars) {
        if (!var->dimList.empty() && var->dimList.back() == nullptr && stm->type == "char" && var->iv && !var->iv->isList) {
            StringLiteral* str_lit = dynamic_cast<StringLiteral*>(var->iv->value);
            if (!str_lit) {
                cout << "Error: se esperaba un literal string en la inicialización de char[]\n";
                exit(1);
            }
            env.add_var(var->id, str_lit->value, "char");
        }
        if (var->iv && var->iv->value) {
            auto strLiteral = dynamic_cast<StringLiteral*>(var->iv->value);
            if (strLiteral) {
                std::string label = registrarStringLiteral(strLiteral->value);
                memoria[var->id] = offset;
                offset -= 8;
                   if (!primeraPasada){
                    out << "    subq $8, %rsp\n";
                    out << "    leaq " << label << "(%rip), %rax\n";
                    out << "    movq %rax, " << memoria[var->id] << "(%rbp)\n";
                }
                continue;
            }
        }
        int size = var->dimList.empty() ? 1 : var->dimList.front()->value;
        memoria[var->id] = offset;
        offset -= size * 8;
        if (!primeraPasada){out << "    subq $" << (size * 8) << ", %rsp\n";}
        for (int idx = 0; idx < size; ++idx) {
               if (!primeraPasada){
                out << "    movq $0, " << (memoria[var->id] + idx * 8) << "(%rbp)\n";
            }
        }
        if (var->iv && var->iv->isList) {
            int idx = 0;
            for (auto val : var->iv->list) {
                val->value->accept(this);
                   if (!primeraPasada){out << "    movq %rax, " << (memoria[var->id] + idx*8) << "(%rbp)\n";}
                idx++;
            }
        }
        else if (var->iv && !var->iv->isList) {
            var->iv->value->accept(this);
               if (!primeraPasada){out << "    movq %rax, " << memoria[var->id] << "(%rbp)\n";}
        }
    }
}

void GenCode::visit(Body* b) {
    if (b->vardecs)
        b->vardecs->accept(this);
    if (b->slist)
        b->slist->accept(this);
}

void GenCode::visit(VarDecList* stm) {
    for (auto vd : stm->vardecs)
        vd->accept(this);
}

void GenCode::visit(StatementList* stm) {
    for (auto s : stm->stms)
        s->accept(this);
}

void GenCode::visit(AssignStatement* stm) {
    if (!stm->lvalue->indices.empty()) {
        stm->lvalue->indices[0]->accept(this);
           if (!primeraPasada){out << "    movq %rax, %rcx\n";}
        stm->rhs->accept(this);
          if (!primeraPasada){ out << "    movq %rax, " << memoria[stm->lvalue->id] << "(%rbp, %rcx, 8)\n";}
    } else {
        stm->rhs->accept(this);
           if (!primeraPasada){out << "    movq %rax, " << memoria[stm->lvalue->id] << "(%rbp)\n";}
    }
}

void GenCode::visit(PrintStatement* stm) {
    if(stm->e->accept(this).type=="char") {
            auto lab=registrarStringLiteral(stm->e->accept(this).string_value);
            if (!primeraPasada){
            out << "    leaq " <<lab<< "(%rip), %rdi\n";
            out << "    call puts@PLT\n";
        }

    }
    for (auto arg : stm->args) {
        if (arg->accept(this).type=="char") {
            auto label=registrarStringLiteral(arg->accept(this).string_value);
               if (!primeraPasada){
                out << "    leaq " << label << "(%rip), %rdi\n";
                out << "    call puts@PLT\n";
            }
        } else {
            arg->accept(this);
               if (!primeraPasada){
                out <<
                   "    movq %rax, %rsi\n"
                   "    leaq print_fmt(%rip), %rdi\n"
                   "    movl $0, %eax\n"
                   "    call printf@PLT\n";
            }
        }
    }

}

void GenCode::visit(ReturnStatement* stm) {
    stm->e->accept(this);
       if (!primeraPasada){out << "    jmp .end_" << nombreFuncion << "\n";}
}

void GenCode::visit(Var *v) {}

void GenCode::visit(WhileStatement *stmt) {
    int start_label = labelcont++;
    int end_label = labelcont++;
       if (!primeraPasada){out << "while_" << start_label << ":\n";}
    stmt->condition->accept(this);
       if (!primeraPasada){out << "    cmpq $0, %rax\n";}
       if (!primeraPasada){out << "    je endwhile_" << end_label << "\n";}
    stmt->b->accept(this);
       if (!primeraPasada){
        out << "    jmp while_" << start_label << "\n";
        out << "endwhile_" << end_label << ":\n";
    }
}

void GenCode::visit(IfStatement *stmt) {
    int endif_label = labelcont++;
    std::vector<int> else_labels;
    for (size_t i = 0; i < stmt->sent_if.size() - 1; ++i)
        else_labels.push_back(labelcont++);
    for (size_t i = 0; i < stmt->sent_if.size(); ++i) {
        IFExp* ifexp = stmt->sent_if[i];
        if (ifexp->condi) {
            ifexp->condi->accept(this);
            if (i < else_labels.size())
                   if (!primeraPasada){
                out << "    cmpq $0, %rax\n"
                   << "    je else_" << else_labels[i] << "\n";
            }
            else
                   if (!primeraPasada){
                out << "    cmpq $0, %rax\n"
                   << "    je endif_" << endif_label << "\n";
            }
        }
        ifexp->body->accept(this);
        if (i < stmt->sent_if.size() - 1)    if (!primeraPasada){
            out << "    jmp endif_" << endif_label << "\n";
        }
        if (i < else_labels.size())
               if (!primeraPasada){out << "else_" << else_labels[i] << ":\n";}
    }
       if (!primeraPasada){out << "endif_" << endif_label << ":\n";}
}

void GenCode::visit(ForStatement *stmt) {
    if (memoria.count(stmt->id) == 0) {
        memoria[stmt->id] = offset;
        offset -= 8;
        out << "    subq $8, %rsp\n";
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
    int end_label = labelcont++;
    out << "dowhile_" << start_label << ":\n";
    stm->b->accept(this);
    stm->condition->accept(this);
    out << "    cmpq $0, %rax\n";
    out << "    jne dowhile_" << start_label << "\n";
    out << "enddowhile_" << end_label << ":\n";
}

void GenCode::visit(FunDecList *fdl) {
    for (auto f : fdl->Fundecs)
        f->accept(this);
}

// Expresiones

ImpValue GenCode::visit(StringLiteral *exp) {
    string label = registrarStringLiteral(exp->value);
    if (!primeraPasada) {
        out << "    leaq " << label << "(%rip), %rax\n";
    }

    return ImpValue("char",0,false,exp->value);
}
ImpValue GenCode::visit(BoolExp *exp) {
       if (!primeraPasada){out << "    movq $" << (exp->value ? 1 : 0) << ", %rax\n";}
    return ImpValue();
}

ImpValue GenCode::visit(ArrayAccessExp *exp) {
    exp->indices[0]->accept(this);     // %rax = i
       if (!primeraPasada){
        out << "    movq %rax, %rdx\n";
        out << "    movq " << memoria[exp->arrayName] << "(%rbp, %rdx, 8), %rax\n";
    }
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
           if (!primeraPasada){out << "    movq %rax, " << argRegs[i] << "\n";}
    }
       if (!primeraPasada){
        out << "    call " << exp->nombre<< "\n";
    }
    return ImpValue();
}

ImpValue GenCode::visit(InitValue *iv) {
    return ImpValue();
}

ImpValue GenCode::visit(NumberExp* exp) {
       if (!primeraPasada){
        out << "    movq $" << exp->value << ", %rax\n";
    }
    return ImpValue("int",exp->value,0,"");
}

ImpValue GenCode::visit(IdentifierExp* exp) {
       if (!primeraPasada){
        out << "    movq " << memoria[exp->name] << "(%rbp), %rax\n";
    }
    string t = env.lookup_type(exp->name);

    if (t == "int") {
        int n = env.lookup(exp->name).first;
        return ImpValue(t, n, false, "");
    }
    else if (t == "bool") {
        bool v = env.lookup(exp->name).first;
        return ImpValue(t, 0, v, "");
    }
    else if (t == "char") {
        string s = env.lookup(exp->name).second;
        return ImpValue(t, 0, false, s);
    }
    return ImpValue("char",0,false,exp->name);
}

ImpValue GenCode::visit(BinaryExp* exp) {
    if (exp->op == INC_OP) {
        exp->left->accept(this);
           if (!primeraPasada){out << "    addq $1, %rax\n";}
        return ImpValue();
    }
    if (exp->op == DEC_OP) {
        exp->left->accept(this);
           if (!primeraPasada){out << "    subq $1, %rax\n";}
        return ImpValue();
    }
    if (exp->op == NOT_OP && exp->right == nullptr) {
        exp->left->accept(this);
           if (!primeraPasada){
            out <<
               "    cmpq $0, %rax\n"
               "    movl $0, %eax\n"
               "    sete %al\n"
               "    movzbq %al, %rax\n";
        }
        return ImpValue();
    }
    exp->left->accept(this);
       if (!primeraPasada){out << "    pushq %rax\n";}
    exp->right->accept(this);
       if (!primeraPasada){
        out << "    movq %rax, %rcx\n";
        out << "    popq %rax\n";
    }
       if (!primeraPasada){
        switch (exp->op) {
            case PLUS_OP:
                out << "    addq %rcx, %rax\n"; break;
            case MINUS_OP:
                out << "    subq %rcx, %rax\n"; break;
            case MUL_OP:
                out << "    imulq %rcx, %rax\n"; break;
            case DIV_OP:
                out << "    cqto\n"
                       "    idivq %rcx\n"; break;
            case LT_OP:
                out << "    cmpq %rcx, %rax\n"
                       "    movl $0, %eax\n"
                       "    setl %al\n"
                       "    movzbq %al, %rax\n"; break;
            case LET_OP:
                out << "    cmpq %rcx, %rax\n"
                       "    movl $0, %eax\n"
                       "    setle %al\n"
                       "    movzbq %al, %rax\n"; break;
            case GT_OP:
                out << "    cmpq %rcx, %rax\n"
                       "    movl $0, %eax\n"
                       "    setg %al\n"
                       "    movzbq %al, %rax\n"; break;
            case GET_OP:
                out << "    cmpq %rcx, %rax\n"
                       "    movl $0, %eax\n"
                       "    setge %al\n"
                       "    movzbq %al, %rax\n"; break;
            case EQ_OP:
                out << "    cmpq %rcx, %rax\n"
                       "    movl $0, %eax\n"
                       "    sete %al\n"
                       "    movzbq %al, %rax\n"; break;
            case DIFF_OP:
                out << "    cmpq %rcx, %rax\n"
                       "    movl $0, %eax\n"
                       "    setne %al\n"
                       "    movzbq %al, %rax\n"; break;
            case AND_OP:
                out << "    setne %al\n"
                       "    movzbq %al, %rax\n"
                       "    setne %cl\n"
                       "    movzbq %cl, %rcx\n"
                       "    andq %rcx, %rax\n"; break;
            case OR_OP:
                out << "    setne %al\n"
                       "    movzbq %al, %rax\n"
                       "    setne %cl\n"
                       "    movzbq %cl, %rcx\n"
                       "    orq %rcx, %rax\n"; break;
            default:
                break;
        }
    }
    return ImpValue();
}