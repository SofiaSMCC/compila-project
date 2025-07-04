#include "codegen.h"

string GenCode::escapeString(const std::string& raw) {
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
// permite manejar variables locales
void GenCode::pushScope() {
    scopeStack.push(unordered_map<string, int>());
    offsetStack.push(offset);
}

// elimina del mapa global las variables
void GenCode::popScope() {
    if (!scopeStack.empty()) {
        auto currentScope = scopeStack.top();
        scopeStack.pop();
        int previousOffset = offsetStack.top();
        offsetStack.pop();
        int toFree = previousOffset - offset;
        if (!scopeStack.empty() && toFree > 0) {
            out << "    addq $" << toFree << ", %rsp\n";
        }
        offset = previousOffset;
        for (auto& pair : currentScope) {
            memoria.erase(pair.first);
        }
    }
}
// declara una nueva variable local y
// evita declarar dos veces la misma variable

void GenCode::declareVariable(const string& name, int size) {
    if (!scopeStack.empty() && scopeStack.top().count(name)) return;
    memoria[name] = offset;
    isArray[name] = (size > 1);
    offset -= size * 8;
    if (!scopeStack.empty()) {
        scopeStack.top()[name] = memoria[name];
    }
    if (!primeraPasada){out << "    subq $" << (size * 8) << ", %rsp\n";}
}

//  verifica si una variable ya está declarada
bool GenCode::isVariableInCurrentScope(const string& name) {
    return !scopeStack.empty() && scopeStack.top().count(name);
}

void GenCode::generar(Program* program) {
    primeraPasada = true;
    string format;
    program->accept(this);

    out << ".data\n";
    out << "print_fmt: .string \"%d\\n\"\n";
    for (const auto& entry : stringLiterals) {
        out << entry.first << ": .string \"" << escapeString(entry.second) << "\"\n";
    }
    //emitir código
    out << ".text\n";
    out << ".globl main\n";
    primeraPasada = false;
    program->accept(this);
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
        out << f->nombre <<":" << std::endl;
        out << "    pushq %rbp\n";
        out << "    movq %rsp, %rbp\n";
    }

    int size = f->parametros.size();

    for (int i = 0; i < size; i++) {
        memoria[f->parametros[i]] = offset;
         if (!primeraPasada){
            out << "    movq " << argRegs[i] << ", " << offset << "(%rbp)\n";
        }
        offset -= 8;
    }

    if (f->cuerpo->vardecs){
        f->cuerpo->vardecs->accept(this);
    }

    if (f->cuerpo->slist)
        f->cuerpo->slist->accept(this);

     if (!primeraPasada){
        out << ".end_" << f->nombre <<":" << std::endl;
        out << "leave" << std::endl;
        out << "ret" << std::endl;
    }
    entornoFuncion = false;
}

void GenCode::visit(VarDec* stm) {
    for (auto var : stm->vars) {
        if (!var->dimList.empty() and var->dimList.back() == nullptr and stm->type == "char" and var->iv and !var->iv->isList) {
            StringLiteral* str_lit = dynamic_cast<StringLiteral*>(var->iv->value);
            if (!str_lit) {
                cout << "Error: se esperaba un literal string en la inicialización de char[]\n";
                exit(1);
            }
         //   env.add_var(var->id, str_lit->value, "char");
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
        if (isVariableInCurrentScope(var->id)) continue;

        int size = var->dimList.empty() ? 1 : var->dimList.front()->value;
        declareVariable(var->id, size);

        for (int idx = 0; idx < size; ++idx) {
            int elementOffset = memoria[var->id] - (idx * 8);
            if (!primeraPasada){
                out << "    movq $0, " << elementOffset << "(%rbp)\n";
            }
        }

        if (var->iv && var->iv->isList) {
            int idx = 0;
            for (auto val : var->iv->list) {
                val->value->accept(this);
                int elementOffset = memoria[var->id] - (idx * 8);
                if (!primeraPasada){out << "    movq %rax, " << elementOffset << "(%rbp)\n";}
                idx++;
            }
        } else if (var->iv && !var->iv->isList) {
            var->iv->value->accept(this);
            if (!primeraPasada){out << "    movq %rax, " << memoria[var->id] << "(%rbp)\n";}
        }
    }
}

void GenCode::visit(Body* b) {
    pushScope();
    if (b->vardecs){
        b->vardecs->accept(this);
    }
    if (b->slist){
        b->slist->accept(this);
    }
    popScope();
}

void GenCode::visit(VarDecList* stm) {
    //cout << "var dec list" << endl;
    for (auto vd : stm->vardecs){
        vd->accept(this);
    }
}

void GenCode::visit(StatementList* stm) {
    //cout << "stm list" << endl;
    for (auto s : stm->stms){
        s->accept(this);
    }
}

void GenCode::visit(AssignStatement* stm) {
    if (!stm->lvalue->indices.empty()) {
        stm->lvalue->indices[0]->accept(this);  // Get index into %rax
        if (!primeraPasada){
            out << "    movq %rax, %rdx\n";
            out << "    negq %rdx\n";
            out << "    imulq $8, %rdx\n";
            out << "    addq $" << memoria[stm->lvalue->id] << ", %rdx\n";
        }
        stm->rhs->accept(this);
       if (!primeraPasada)
           { out << "    movq %rax, (%rbp, %rdx)\n";}
    } else {
        stm->rhs->accept(this);
        if (!primeraPasada){out << "    movq %rax, " << memoria[stm->lvalue->id] << "(%rbp)\n";}
    }
}
void GenCode::visit(PrintStatement* stm) {
    for (size_t i = 0; i < stm->args.size(); ++i) {
        ImpValue val = stm->args[i]->accept(this); // Resultado en %rax
        if (val.type == "char") {
            auto label = registrarStringLiteral(val.string_value);
            if (!primeraPasada) {
                out << "    leaq " << label << "(%rip), %rdi\n";
                out << "    call puts@PLT\n";
            }
        } else {
            if (!primeraPasada) {
                out <<
                   "    movq %rsp, %rcx\n"
                   "    andq $0xf, %rcx\n"
                   "    cmpq $0, %rcx\n"
                   "    je .aligned_printf_" << labelcont << "\n"
                   "    subq $8, %rsp\n"
                   "    movq %rax, %rsi\n"
                   "    leaq print_fmt(%rip), %rdi\n"
                   "    movl $0, %eax\n"
                   "    call printf@PLT\n"
                   "    addq $8, %rsp\n"
                   "    jmp .after_printf_" << labelcont << "\n"
                   ".aligned_printf_" << labelcont << ":\n"
                   "    movq %rax, %rsi\n"
                   "    leaq print_fmt(%rip), %rdi\n"
                   "    movl $0, %eax\n"
                   "    call printf@PLT\n"
                   ".after_printf_" << labelcont << ":\n";
                labelcont++;
            }}
    }
}

void GenCode::visit(ReturnStatement* stm) {
    //cout << "return" << endl;
    stm->e->accept(this);
    if (!primeraPasada){out << "    jmp .end_" << nombreFuncion << "\n";}
}

void GenCode::visit(Var *v) {}

void GenCode::visit(WhileStatement *stmt) {
    int start_label = labelcont++;
    int end_label = labelcont++;

    if (!primeraPasada){out << "while_" << start_label << ":\n";}
    stmt->condition->accept(this);
    if (!primeraPasada){
        out << "    cmpq $0, %rax\n";
        out << "    je endwhile_" << end_label << "\n";
    }

    stmt->b->accept(this);

    if (!primeraPasada){
        out << "    jmp while_" << start_label << "\n";
        out << "endwhile_" << end_label << ":\n";
    }
}
void GenCode::visit(IfStatement *stmt) {
    //cout << "if statemetn" << endl;
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
                if (!primeraPasada){
                out << "    cmpq $0, %rax\n"
                   << "    je else_" << else_labels[i] << "\n";
            }
            else {
                if (!primeraPasada){
                    out << "    cmpq $0, %rax\n"
                       << "    je endif_" << endif_label << "\n";
                }
            }
        }

        ifexp->body->accept(this);

        if (i < stmt->sent_if.size() - 1)
            if (!primeraPasada){
            out << "    jmp endif_" << endif_label << "\n";
        }

        if (i < else_labels.size())
            if (!primeraPasada){
            out << "else_" << else_labels[i] << ":\n";
        }
    }

    if (!primeraPasada){out << "endif_" << endif_label << ":\n";}
}

void GenCode::visit(ForStatement* stmt) {
    pushScope();

    if (!isVariableInCurrentScope(stmt->id)) {
        declareVariable(stmt->id, 1);
    }

    stmt->start->accept(this);
    if (!primeraPasada){
        out << "    movq %rax, " << memoria[stmt->id] << "(%rbp)\n";
    }

    int loop_label = labelcont++;
    int end_label = labelcont++;

    if (!primeraPasada){
        out << "for_" << loop_label << ":\n";
        out << "    movq " << memoria[stmt->id] << "(%rbp), %rax\n";
    }
    stmt->condition->accept(this);
    if (!primeraPasada){
        out << "    cmpq $0, %rax\n";
        out << "    je endfor_" << end_label << "\n";
    }

    stmt->b->accept(this);

    if (!primeraPasada){out << "    movq " << memoria[stmt->id] << "(%rbp), %rcx\n";}
    stmt->step->accept(this);
    if (!primeraPasada){out << "    movq %rax, " << memoria[stmt->id] << "(%rbp)\n";}

    if (!primeraPasada){
        out << "    jmp for_" << loop_label << "\n";
        out << "endfor_" << end_label << ":\n";
    }

    popScope();
}
void GenCode::visit(FCallStatement *stm) {
    //cout << "f call stm" << endl;
    stm->call->accept(this);
}

void GenCode::visit(DoWhileStatement *stm) {
    int start_label = labelcont++;
    int end_label = labelcont++;

    if (!primeraPasada){out << "dowhile_" << start_label << ":\n";}

    stm->b->accept(this);


    stm->condition->accept(this);
    if (!primeraPasada){
        out << "    cmpq $0, %rax\n";
        out << "    jne dowhile_" << start_label << "\n";
        out << "enddowhile_" << end_label << ":\n";
    }
}


void GenCode::visit(FunDecList *fdl) {
    //cout << "fun dec list" << endl;
    for (auto f : fdl->Fundecs)
        f->accept(this);
}

// Exp

ImpValue GenCode::visit(StringLiteral *exp) {
    string label = registrarStringLiteral(exp->value);
    if (!primeraPasada) {
        out << "    leaq " << label << "(%rip), %rax\n";
    }
    return ImpValue("char", 0, false, exp->value);
}

ImpValue GenCode::visit(BoolExp *exp) {
    //cout << "bool" << endl;
   if (!primeraPasada){ out << "    movq $" << (exp->value ? 1 : 0) << ", %rax\n";}
    return ImpValue();
}


ImpValue GenCode::visit(ArrayAccessExp *exp) {
    exp->indices[0]->accept(this);     // %rax = i
    if (!primeraPasada){
        out << "    movq %rax, %rdx\n";
        out << "    negq %rdx\n";
        out << "    imulq $8, %rdx\n";
        out << "    addq $" << memoria[exp->arrayName] << ", %rdx\n";
        out << "    movq (%rbp, %rdx), %rax\n";
    }
    return ImpValue();
}

ImpValue GenCode::visit(IFExp *exp) {
    //cout << "if exp" << endl;
    return ImpValue();
}

ImpValue GenCode::visit(LValue *exp) {
    //cout << "l value" << endl;
    if (exp->indices.empty()) {
        if (!primeraPasada){out << "    movq " << memoria[exp->id] << "(%rbp), %rax\n";}
    } else {
        exp->indices[0]->accept(this);
        if (!primeraPasada){
            out << "    movq %rax, %rdx\n";
            out << "    movq " << memoria[exp->id] << "(%rbp, %rdx, 8), %rax\n";
        }
    }
    return ImpValue();
}

ImpValue GenCode::visit(FCallExp* exp) {
    //cout << "fcallexp" << endl;
    int nargs = exp->argumentos.size();
    std::vector<std::string> argRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    for (int i = 0; i < nargs && i < 6; ++i) {
        exp->argumentos[i]->accept(this);
        if (!primeraPasada){out << "    movq %rax, " << argRegs[i] << "\n";}
    }
    if (!primeraPasada){out << "    call " << exp->nombre<< "\n";}
    return ImpValue();
}


ImpValue GenCode::visit(InitValue *iv) {
    //cout << "initvalue" << endl;
    return ImpValue();
}

ImpValue GenCode::visit(NumberExp* exp) {
    //cout << "number" << endl;
    if (!primeraPasada){out << "    movq $" << exp->value << ", %rax\n";}
    return ImpValue();
}

ImpValue GenCode::visit(IdentifierExp* exp) {
    if (!primeraPasada) {out << "    movq " << memoria[exp->name] << "(%rbp), %rax\n";}
    return ImpValue();
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
    if (!primeraPasada){out << "    movq %rax, %rcx\n";}
    exp->right->accept(this);

    switch (exp->op) {
        case PLUS_OP:
            if (!primeraPasada){out << "    addq %rcx, %rax\n"; break;}
        case MINUS_OP:
            if (!primeraPasada){
            out << "    movq %rax, %rdx\n"
                  "    movq %rcx, %rax\n"
                  "    subq %rdx, %rax\n"; break;
        }
        case MUL_OP:
            if (!primeraPasada){
            out << "    imulq %rcx, %rax\n"; break;
        }
        case DIV_OP:
            if (!primeraPasada){
            out << "    movq %rax, %rdx\n"
                  "    movq %rcx, %rax\n"
                  "    cqto\n"
                  "    idivq %rdx\n"; break;
        }
        case LT_OP: if (!primeraPasada){
            out << "    cmpq %rax, %rcx\n"
                  "    movl $0, %eax\n"
                  "    setl %al\n"
                  "    movzbq %al, %rax\n"; break;
        }
        case LET_OP:
            if (!primeraPasada){
            out << "    cmpq %rax, %rcx\n"
                  "    movl $0, %eax\n"
                  "    setle %al\n"
                  "    movzbq %al, %rax\n"; break;
        }
        case GT_OP: if (!primeraPasada){
            out << "    cmpq %rax, %rcx\n"
                  "    movl $0, %eax\n"
                  "    setg %al\n"
                  "    movzbq %al, %rax\n"; break;
        }
        case GET_OP: if (!primeraPasada){
            out << "    cmpq %rax, %rcx\n"
                  "    movl $0, %eax\n"
                  "    setge %al\n"
                  "    movzbq %al, %rax\n"; break;
        }
        case EQ_OP: if (!primeraPasada){
            out << "    cmpq %rax, %rcx\n"
                  "    movl $0, %eax\n"
                  "    sete %al\n"
                  "    movzbq %al, %rax\n"; break;
        }
        case DIFF_OP: if (!primeraPasada){
            out << "    cmpq %rax, %rcx\n"
                  "    movl $0, %eax\n"
                  "    setne %al\n"
                  "    movzbq %al, %rax\n"; break;
        }
        case AND_OP: if (!primeraPasada){
            out << "    setne %al\n"
                  "    movzbq %al, %rax\n"
                  "    setne %cl\n"
                  "    movzbq %cl, %rcx\n"
                  "    andq %rcx, %rax\n"; break;
        }
        case OR_OP: if (!primeraPasada){
            out << "    setne %al\n"
                  "    movzbq %al, %rax\n"
                  "    setne %cl\n"
                  "    movzbq %cl, %rcx\n"
                  "    orq %rcx, %rax\n"; break;
        }
        default:
            break;
    }
    return ImpValue();
}