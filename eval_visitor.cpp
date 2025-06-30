#include "eval_visitor.h"
#include <token.h>

void recolectaValores(InitValue* iv, vector<ImpValue>& valores, EVALVisitor* visitor) {
    if (iv->isList) {
        for (auto elem : iv->list) {
            recolectaValores(elem, valores, visitor);
        }
    } else if (iv->value) {
        valores.push_back(iv->value->accept(visitor));
    }
}

void declara_array_eval(
        const std::string& base,
        const std::vector<int>& dims,
        size_t dim,
        std::vector<int>& idxs,
        const std::string& tipo,
        Environment& env,
        const std::vector<ImpValue>& valores,
        size_t& valor_idx
) {
    if (dim == dims.size()) {
        std::string nombre = base + "[";
        for (size_t i = 0; i < idxs.size(); ++i) {
            nombre += std::to_string(idxs[i]);
            if (i + 1 < idxs.size()) nombre += ",";
        }
        nombre += "]";
        if (valor_idx < valores.size()) {
            if (tipo == "int")
                env.add_var(nombre, valores[valor_idx++].int_value, tipo);
            else if (tipo == "bool")
                env.add_var(nombre, valores[valor_idx++].bool_value ? 1 : 0, tipo);
            else if (tipo == "string")
                env.add_var(nombre, valores[valor_idx++].string_value, tipo);
        } else {
            env.add_var(nombre, tipo);
        }
        return;
    }
    for (int i = 0; i < dims[dim]; ++i) {
        idxs.push_back(i);
        declara_array_eval(base, dims, dim + 1, idxs, tipo, env, valores, valor_idx);
        idxs.pop_back();
    }
}

void EVALVisitor::ejecutar(Program* program) {
    env.add_level();
    for (auto f : program->func->Fundecs) {
        funciones[f->nombre] = f;
    }
    if (funciones.count("main")) {
        FunDec* mainFun = funciones["main"];
        env.add_level();
        mainFun->cuerpo->accept(this);
    }
    cout << "\nEstado del environment antes de limpiar:" << endl;
    env.debug_print();
    env.remove_level();
    env.remove_level();
}

void EVALVisitor::visit(Program* program) {
    if (program->func)
        program->func->accept(this);
}

// Exp

ImpValue EVALVisitor::visit(BinaryExp *exp) {
    int result1 = 0;
    bool result2 = false;

    ImpValue left = exp->left->accept(this);
    ImpValue right;

    if (exp->right) {
        right = exp->right->accept(this);
    }

    switch (exp->op) {
        // arithmetic
        case PLUS_OP:
            result1 = left.int_value + right.int_value;
            return ImpValue("int", result1, false,"");
        case MINUS_OP:
            result1 = left.int_value - right.int_value;
            return ImpValue("int", result1, false,"");
        case MUL_OP:
            result1 = left.int_value * right.int_value;
            return ImpValue("int", result1, false,"");
        case DIV_OP:
            if (right.int_value != 0) {
                result1 = left.int_value / right.int_value;
            } else {
                cout << "Error: división por cero" << endl;
                result1 = 0;
            }
            return ImpValue("int", result1, false,"");
            // comparison
        case LT_OP:  // <
            result2 = left.int_value < right.int_value;
            return ImpValue("bool", 0, result2,"");
        case LET_OP: // <=
            result2 = left.int_value <= right.int_value;
            return ImpValue("bool", 0, result2, "");
        case EQ_OP:  // ==
            if (left.type == "int" && right.type == "int") {
                result2 = left.int_value == right.int_value;
            } else {
                result2 = left.bool_value == right.bool_value;
            }
            return ImpValue("bool", 0, result2,"");
        case NOT_OP:
        {
            return ImpValue("bool", 0, !(left.bool_value), "");
        }
            case DIFF_OP: //!=
            result2 = left.int_value != right.int_value;
            return ImpValue("bool", 0, result2, "");
        case GT_OP:   // >
            result2 = left.int_value > right.int_value;
            return ImpValue("bool", 0, result2, "");
        case GET_OP:  // >=
            result2 = left.int_value >= right.int_value;
            return ImpValue("bool", 0, result2, "");
        case AND_OP:
            if (!left.bool_value) {
                return ImpValue("bool", 0, false, "");
            }
            right = exp->right->accept(this);
            result2 = right.bool_value;
            return ImpValue("bool", 0, result2, "");
        case OR_OP:
            if (left.bool_value) {
                return ImpValue("bool", 0, true, "");
            }
            right = exp->right->accept(this);
            result2 = right.bool_value;
            return ImpValue("bool", 0, result2, "");
        case INC_OP:
            result1 = left.int_value + 1;
            return ImpValue("int", result1, false, "");
        case DEC_OP:
            result1 = left.int_value - 1;
            return ImpValue("int", result1, false, "");
        default:
            cout << "Operador desconocido" << endl;
            return ImpValue();
    }
}

ImpValue EVALVisitor::visit(NumberExp *exp) {
    int a = exp->value;
    return ImpValue("int", a, false, "");
}

ImpValue EVALVisitor::visit(BoolExp *exp) {
    bool a = exp->value;
    return ImpValue("bool", 0, a, "");
}

ImpValue EVALVisitor::visit(StringLiteral *exp) {
    string a = exp->value;
    return ImpValue("string", 0, false, a);
}

ImpValue EVALVisitor::visit(IdentifierExp *exp) {
    string t = env.lookup_type(exp->name);

    if (t == "int") {
        int n = env.lookup(exp->name).first;
        return ImpValue(t, n, false, "");
    }
    else if (t == "bool") {
        bool v = env.lookup(exp->name).first;
        return ImpValue(t, 0, v, "");
    }
    else if (t == "string") {
        string s = env.lookup(exp->name).second;
        return ImpValue(t, 0, false, s);
    }
    return ImpValue();
}

ImpValue EVALVisitor::visit(LValue *exp) {
    // Si no es array, accede normal
    if (exp->indices.empty()) {
        string t = env.lookup_type(exp->id);
        if (t == "int") {
            int n = env.lookup(exp->id).first;
            return ImpValue(t, n, false, "");
        } else if (t == "bool") {
            bool v = env.lookup(exp->id).first;
            return ImpValue(t, 0, v, "");
        } else if (t == "string") {
            string s = env.lookup(exp->id).second;
            return ImpValue(t, 0, false, s);
        }
    } else {
        // Soporte para arrays: nombre[i] -> "nombre[0]", etc.
        string nombre = exp->id + "[";
        for (size_t i = 0; i < exp->indices.size(); ++i) {
            int idx = exp->indices[i]->accept(this).int_value;
            nombre += to_string(idx);
            if (i + 1 < exp->indices.size()) nombre += ",";
        }
        nombre += "]";
        string t = env.lookup_type(nombre);
        if (t == "int") {
            int n = env.lookup(nombre).first;
            return ImpValue(t, n, false, "");
        } else if (t == "bool") {
            bool v = env.lookup(nombre).first;
            return ImpValue(t, 0, v, "");
        } else if (t == "string") {
            string s = env.lookup(nombre).second;
            return ImpValue(t, 0, false, s);
        }
    }
    return ImpValue();
}

ImpValue EVALVisitor::visit(IFExp *exp) {
    if (!exp->condi || exp->condi->accept(this).bool_value) {
        exp->body->accept(this);
    }
    return ImpValue();
}

ImpValue EVALVisitor::visit(FCallExp *exp) {
    if (funciones.count(exp->nombre) == 0) {
        std::cerr << "Error: función no encontrada: " << exp->nombre << std::endl;
        return ImpValue();
    }
    FunDec* f = funciones[exp->nombre];

    vector<ImpValue> args;
    for (auto arg_exp : exp->argumentos) {
        args.push_back(arg_exp->accept(this));
    }

    env.add_level();

    for (size_t i = 0; i < f->parametros.size(); ++i) {
        std::string pname = f->parametros[i];
        std::string ptype = f->tipos[i];
        if (ptype == "int")
            env.add_var(pname, args[i].int_value, "int");
        else if (ptype == "bool")
            env.add_var(pname, args[i].bool_value ? 1 : 0, "bool");
        else if (ptype == "string")
            env.add_var(pname, args[i].string_value, "string");
    }

    this->return_encountered = false;
    this->return_value = ImpValue();

    f->cuerpo->accept(this);
    env.remove_level();
    return this->return_value;
}

ImpValue EVALVisitor::visit(ArrayAccessExp *exp) {
    // Construir el nombre tipo "matriz[2,5]"
    string nombre = exp->arrayName + "[";
    for (size_t i = 0; i < exp->indices.size(); ++i) {
        int idx = exp->indices[i]->accept(this).int_value;
        nombre += to_string(idx);
        if (i + 1 < exp->indices.size()) nombre += ",";
    }
    nombre += "]";

    string t = env.lookup_type(nombre);
    if (t == "int") {
        int n = env.lookup(nombre).first;
        return ImpValue(t, n, false, "");
    } else if (t == "bool") {
        bool v = env.lookup(nombre).first;
        return ImpValue(t, 0, v, "");
    } else if (t == "string") {
        string s = env.lookup(nombre).second;
        return ImpValue(t, 0, false, s);
    }
    return ImpValue();
}

ImpValue EVALVisitor::visit(InitValue *iv) {
    return iv->value->accept(this);
}

// Statement

void EVALVisitor::visit(AssignStatement *stm) {
    string var_name;
    // Si el LValue es array
    if (!stm->lvalue->indices.empty()) {
        var_name = stm->lvalue->id + "[";
        for (size_t i = 0; i < stm->lvalue->indices.size(); ++i) {
            int idx = stm->lvalue->indices[i]->accept(this).int_value;
            var_name += to_string(idx);
            if (i + 1 < stm->lvalue->indices.size()) var_name += ",";
        }
        var_name += "]";
    } else {
        var_name = stm->lvalue->id;
    }

    string t = env.lookup_type(var_name);
    ImpValue val = stm->rhs->accept(this);

    if (t == "bool") {
        env.update(var_name, val.bool_value ? 1 : 0);
    } else if (t == "int") {
        env.update(var_name, val.int_value);
    } else if (t == "string") {
        env.update(var_name, val.string_value);
    }
}
void EVALVisitor::visit(IfStatement *stm) {
    for (auto stms : stm->sent_if) {
        if (!stms->condi || stms->condi->accept(this).bool_value) {
            stms->body->accept(this);
            break;
        }
    }
}

void EVALVisitor::visit(ForStatement *stm) {
    env.add_level();

    env.add_var(stm->id, 0, stm->type);
    int init = stm->start->accept(this).int_value;
    env.update(stm->id, init);

    while (stm->condition->accept(this).bool_value) {
        stm->b->accept(this);
        int next = stm->step->accept(this).int_value;
        env.update(stm->id, next);
    }

    env.remove_level();
}

void EVALVisitor::visit(StatementList *stm) {
    for (auto s : stm->stms) {
        s->accept(this);
        if (this->return_encountered) break;
    }
}

void EVALVisitor::visit(DoWhileStatement *stm) {
    stm->b->accept(this);
    if(stm->condition->accept(this).bool_value) {
        stm->b->accept(this);
    }
}

void EVALVisitor::visit(ReturnStatement *stm) {
     if (stm->e) {
        this->return_value = stm->e->accept(this);
        this->return_encountered = true;
    }
}

void EVALVisitor::visit(WhileStatement *stm) {
    while (stm->condition->accept(this).bool_value) {
        stm->b->accept(this);
        if (this->return_encountered){
            break;
        }
    }
}

void EVALVisitor::visit(PrintStatement *stm) {
    ImpValue arg = stm->e->accept(this);
    if (stm->format == "%s\n") {
        if (arg.type == "string") {
            cout << arg.string_value << endl;
        } else if (arg.type == "bool") {
            cout << (arg.bool_value ? "true" : "false") << endl;
        } else {
            cout << "Error: printf formato %s pero no string/bool" << endl;
        }
    } else if (stm->format == "%d\n") {
        if (arg.type == "int" || arg.type == "bool") {
            cout << (arg.type == "int" ? arg.int_value : (arg.bool_value ? 1 : 0)) << endl;
        } else {
            cout << "Error: printf formato %d pero no int/bool" << endl;
        }
    }
}

void EVALVisitor::visit(FCallStatement *stm) {
    ImpValue unused = stm->call->accept(this);
}

void EVALVisitor::visit(VarDecList *stm) {
    for (auto vd : stm->vardecs) {
        vd->accept(this);
    }
}

void EVALVisitor::visit(FunDecList *fdl) {
    for (auto vd : fdl->Fundecs) {
        funciones[vd->nombre] = vd;
    }
}

void EVALVisitor::visit(FunDec *fd) {
    env.add_level();
    fd->cuerpo->accept(this);
    env.remove_level();
}


void EVALVisitor::visit(Body *b) {
    if (b->vardecs) b->vardecs->accept(this);
    if (b->slist) b->slist->accept(this);
}

void EVALVisitor::visit(VarDec *stm) {
    for (auto i : stm->vars) {
        // Si es array multidimensional
        if (!i->dimList.empty()) {
            // Calcula las dimensiones
            std::vector<int> dims;
            for (auto nexp : i->dimList) {
                dims.push_back(nexp->value);
            }
            std::vector<ImpValue> valores;
            if (i->iv && i->iv->isList) {
                recolectaValores(i->iv, valores, this);
            }
            std::vector<int> idxs;
            size_t valor_idx = 0;
            declara_array_eval(i->id, dims, 0, idxs, stm->type, env, valores, valor_idx);
        }
        // Caso variable simple (no array)
        else if (i->iv) {
            ImpValue val = i->iv->accept(this);
            if (stm->type == "int") {
                env.add_var(i->id, val.int_value, stm->type);
            } else if (stm->type == "bool") {
                env.add_var(i->id, val.bool_value ? 1 : 0, stm->type);
            } else if (stm->type == "string") {
                env.add_var(i->id, val.string_value, stm->type);
            }
        } else {
            env.add_var(i->id, stm->type);
        }
    }
}

void EVALVisitor::visit(Var *stm) {
}