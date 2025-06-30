#ifndef TYPE_VISITOR_H
#define TYPE_VISITOR_H
#include "type_visitor.h"
string tiport;
bool InitValueType(InitValue* iv, const std::string& expectedType, TypeVisitor* visitor) {
    if (iv->isList) {
        for (auto elem : iv->list) {
            if (!InitValueType(elem, expectedType, visitor))
                return false;
        }
        return true;
    } else if (iv->value) {
        string t = iv->value->accept(visitor).type;
        return t == expectedType;
    }
    return false;
}
///////////////////////////////////////////////////////////////////////////////////

//0 = undefined
//1 = int
//2 = bool

void declara_array_type(
        const std::string& base,
        const std::vector<int>& dims,
        size_t dim,
        std::vector<int>& idxs,
        const std::string& tipo,
        Environment* env
) {
    if (dim == dims.size()) {
        std::string nombre = base + "[";
        for (size_t i = 0; i < idxs.size(); ++i) {
            nombre += std::to_string(idxs[i]);
            if (i + 1 < idxs.size()) nombre += ",";
        }
        nombre += "]";
        env->add_var(nombre, tipo);
        return;
    }
    for (int i = 0; i < dims[dim]; ++i) {
        idxs.push_back(i);
        declara_array_type(base, dims, dim + 1, idxs, tipo, env);
        idxs.pop_back();
    }
}

void TypeVisitor::check(Program* program){
    for (FunDec* f : program->func->Fundecs) {
        funciones[f->nombre] = f;
    }

    env->add_level();
    program->func->accept(this);
    env->remove_level();
}

void TypeVisitor::visit(Program *program) {
    program->func->accept(this);
}

// Exp

ImpValue TypeVisitor::visit(NumberExp* exp) {
    return ImpValue("int", 0, false, "");
}

ImpValue TypeVisitor::visit(BoolExp* exp) {
    return ImpValue("bool", 0, false,"");
}

ImpValue TypeVisitor::visit(StringLiteral *stm) {
    return ImpValue("char",0,false," ");
}

ImpValue TypeVisitor::visit(IdentifierExp* exp) {
    string t = env->lookup_type(exp->name);
    return ImpValue(t, 0, false,"");
}

ImpValue TypeVisitor::visit(BinaryExp* exp) {
    int result1 = 0;
    bool result2 = false;

    ImpValue left = exp->left->accept(this);
    ImpValue right;

    if (exp->right) {
        right = exp->right->accept(this);
    }

    switch (exp->op) {
        case PLUS_OP:
            result1 = left.int_value + right.int_value;
            return ImpValue("int", result1, false, "");
        case MINUS_OP:
            result1 = left.int_value - right.int_value;
            return ImpValue("int", result1, false, "");
        case MUL_OP:
            result1 = left.int_value * right.int_value;
            return ImpValue("int", result1, false, "");
        case DIV_OP:
            if (right.int_value != 0) {
                result1 = left.int_value / right.int_value;
            } else {
                cout << "Error: división por cero" << endl;
                result1 = 0;
            }
            return ImpValue("int", result1, false, "");

        case LT_OP:
            result2 = left.int_value < right.int_value;
            return ImpValue("bool", 0, result2, "");

        case EQ_OP:
            if (left.type == "int" && right.type == "int") {
                result2 = left.int_value == right.int_value;
            } else {
                result2 = left.bool_value == right.bool_value;
            }
            return ImpValue("bool", 0, result2, "");

        case AND_OP:
            result2 = left.bool_value && right.bool_value;
            return ImpValue("bool", 0, result2, NULL);
        case OR_OP:
            result2 = left.bool_value || right.bool_value;
            return ImpValue("bool", 0, result2, NULL);
        case NOT_OP:
            result2 = !left.bool_value;
            return ImpValue("bool", 0, result2, NULL);

        default:
            cout << "Operador desconocido" << endl;
            return ImpValue();
    }
}

ImpValue TypeVisitor::visit(ArrayAccessExp* exp) {
    string nombre = exp->arrayName + "[";
    for (size_t i = 0; i < exp->indices.size(); ++i) {
        ImpValue idxVal = exp->indices[i]->accept(this);
        if (ImpValue::get_basic_type(idxVal.type) != "int") {
            cout << "Error: índice de array debe ser int" << endl;
            exit(0);
        }
        int idx = idxVal.int_value;
        nombre += to_string(idx);
        if (i + 1 < exp->indices.size()) nombre += ",";
    }
    nombre += "]";
    string t = env->lookup_type(nombre);
    return ImpValue(t, 0, false, "");
}

ImpValue TypeVisitor::visit(IFExp* exp) {
    if (exp->condi) {
        ImpValue cond = exp->condi->accept(this);
        if (ImpValue::get_basic_type(cond.type) != "bool") {
            cout << "Error: condición del if debe ser booleana" << endl;
            exit(0);
        }
    }
    return ImpValue();
}


ImpValue TypeVisitor::visit(FCallExp* exp) {
    if (funciones.count(exp->nombre) == 0) {
        cout << "Error: función no declarada: " << exp->nombre << endl;
        exit(0);
    }
    FunDec* f = funciones[exp->nombre];
    if (exp->argumentos.size() != f->parametros.size()) {
        cout << "Error: número incorrecto de argumentos en llamada a " << exp->nombre << endl;
        exit(0);
    }

    for (size_t i = 0; i < exp->argumentos.size(); ++i) {
        ImpValue argval = exp->argumentos[i]->accept(this);
        string tipo_param = f->tipos[i];
        if (ImpValue::get_basic_type(argval.type) != tipo_param) {
            cout << "Error: tipo del argumento " << (i+1) << " en llamada a " << exp->nombre
                 << " es " << argval.type << ", se esperaba " << tipo_param << endl;
            exit(0);
        }
    }

    return ImpValue(f->tipo, 0, false, "");
}

ImpValue TypeVisitor::visit(LValue* exp) {
    string nombre = exp->id;
    if (!exp->indices.empty()) {
        nombre += "[";
        for (size_t i = 0; i < exp->indices.size(); ++i) {
            ImpValue idxVal = exp->indices[i]->accept(this);
            if (ImpValue::get_basic_type(idxVal.type) != "int") {
                cout << "Error: índice de array debe ser int" << endl;
                exit(0);
            }
            int idx = idxVal.int_value;
            nombre += to_string(idx);
            if (i + 1 < exp->indices.size()) nombre += ",";
        }
        nombre += "]";
    }
    string t = env->lookup_type(nombre);
    return ImpValue(t, 0, false, "");
}

// Statements

void TypeVisitor::visit(AssignStatement* stm) {
    // Soporta variables y arrays
    string nombre = stm->lvalue->id;
    if (!stm->lvalue->indices.empty()) {
        nombre += "[";
        for (size_t i = 0; i < stm->lvalue->indices.size(); ++i) {
            ImpValue idxVal = stm->lvalue->indices[i]->accept(this);
            if (ImpValue::get_basic_type(idxVal.type) != "int") {
                cout << "Error: índice de array debe ser int" << endl;
                exit(0);
            }
            int idx = idxVal.int_value;
            nombre += to_string(idx);
            if (i + 1 < stm->lvalue->indices.size()) nombre += ",";
        }
        nombre += "]";
    }
    string tipo_variable = env->lookup_type(nombre);
    ImpValue rhs_value = stm->rhs->accept(this);
    if (ImpValue::get_basic_type(rhs_value.type) != ImpValue::get_basic_type(tipo_variable)) {
        cout << "Error de tipos: se esperaba " << tipo_variable
             << " pero se encontró " << rhs_value.type << endl;
        exit(0);
    }
}


void TypeVisitor::visit(VarDec* vd) {
    string t = ImpValue::get_basic_type(vd->type);
    if (t != "bool" && t != "int" && t != "char") {
        cout << "type error en declaración de variable" << endl;
        exit(0);
    }
    for (auto i : vd->vars) {
        if (!i->dimList.empty()) {
            vector<int> dims;
            for (auto nexp : i->dimList) {
                dims.push_back(nexp->value);
                if(!InitValueType(i->iv,t,this)) {
                    cout << "Error de tipo en inicialización de array: '" << i->id
                     << "'. Elemento no es de tipo '" << t << "'" << endl;
                    exit(1);
                }

            }
            vector<int> idxs;
            declara_array_type(i->id, dims, 0, idxs, t, env);
        } else if (i->iv) {
            ImpValue valor_init = i->iv->accept(this);
            if (ImpValue::get_basic_type(valor_init.type) != t) {
                cout << "Error de tipo en inicialización de variable: " << i->id << endl;
                exit(0);
            }
            env->add_var(i->id, t);
        }

        else {
            env->add_var(i->id, t);
        }
    }
}

void TypeVisitor::visit(VarDecList* vdl) {
    for(auto i: vdl->vardecs){
        i->accept(this);
    }
}

void TypeVisitor::visit(PrintStatement* stm) {
    ImpValue arg = stm->e->accept(this);
    if (stm->format == "%s\n" && ImpValue::get_basic_type(arg.type) != "char") {
        cout << "Error: printf espera string, pero se pasa " << arg.type << endl;
        exit(0);
    }
    if (stm->format == "%d\n" && ImpValue::get_basic_type(arg.type) != "int") {
        cout << "Error: printf espera int, pero se pasa " << arg.type << endl;
        exit(0);
    }
}

void TypeVisitor::visit(IfStatement* stm) {
    for (auto ifexp : stm->sent_if) {
        if (ifexp->condi) {
            ImpValue cond = ifexp->condi->accept(this);
            if (ImpValue::get_basic_type(cond.type) != "bool") {
                cout << "Error: condición del if/else-if debe ser booleana" << endl;
                exit(0);
            }
        }
        if (ifexp->body) ifexp->body->accept(this);
    }
}

void TypeVisitor::visit(WhileStatement* stm) {
    ImpValue cond = stm->condition->accept(this);
    if (ImpValue::get_basic_type(cond.type) != "bool") {
        cout << "Error: condición del while debe ser booleana" << endl;
        exit(0);
    }
    if (stm->b){
        stm->b->accept(this);
    }
}

void TypeVisitor::visit(DoWhileStatement* stm) {
    if (stm->b){
        stm->b->accept(this);
    }
    ImpValue cond = stm->condition->accept(this);
    if (ImpValue::get_basic_type(cond.type) != "bool") {
        cout << "Error: condición del do-while debe ser booleana" << endl;
        exit(0);
    }
}

void TypeVisitor::visit(ForStatement* stm) {
    ImpValue rhs = stm->start->accept(this);
    if(rhs.type != stm->type) {
        cout << "Error en el type del for" << endl;
        exit(0);
    }
    if (stm->type == "char") {
        cout << "No se puede declarar string dentro del for" << endl;
        exit(0);
    }
}

void TypeVisitor::visit(ReturnStatement* stm) {
    if (stm->e) {
        ImpValue ret = stm->e->accept(this);
        if(ImpValue::get_basic_type(ret.type) != "int") {
            cout << "Error: return debe ser int" << endl;
            exit(0);
        }
    }
}

void TypeVisitor::visit(FCallStatement* stm) {
    stm->call->accept(this);
}

ImpValue TypeVisitor::visit(InitValue* iv) {
    if (iv->isList) {
        for (auto val : iv->list) {
            ImpValue v = val->accept(this);

        }
        if (!iv->list.empty()) return iv->list[0]->accept(this);
        else return ImpValue();
    }
    return iv->value->accept(this);
}

void TypeVisitor::visit(Var* var) {
    cout<<var->id;
    cout<<"slo "<<var->iv->value;
}

void TypeVisitor::visit(StatementList* stm) {
    for(auto i: stm->stms){
        i->accept(this);
    }
}

void TypeVisitor::visit(Body* b) {
    if(b->vardecs) b->vardecs->accept(this);
    if(b->slist) b->slist->accept(this);
}

void TypeVisitor::visit(FunDec* fd) {
    env->add_level();
    for (size_t i = 0; i < fd->parametros.size(); ++i) {
        string nombre = fd->parametros[i];
        string tipo = fd->tipos[i];
        env->add_var(nombre, tipo);
    }
    if(fd->cuerpo){
        fd->cuerpo->accept(this);
    }
    env->remove_level();
}

void TypeVisitor::visit(FunDecList* fdl) {
    bool main=0;
    for(auto i: fdl->Fundecs){
       // if(fdl->Fundecs.back()->nombre!="main") {
       // //   cout<<"ERROR: falta funcion main";
       // // exit(1);
        //}
        if(i->nombre=="main") {
            main=1;
        }
        i->accept(this);
    }
    if(!main) {
        cout<<"ERROR: falta funcion main";
        exit(1);
    }
}

#endif