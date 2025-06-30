#ifndef TYPE_VISITOR_H
#define TYPE_VISITOR_H

#include "type_visitor.h"

///////////////////////////////////////////////////////////////////////////////////

//0 = undefined
//1 = int
//2 = bool

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
    return ImpValue("string",0,false," ");
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
    return ImpValue();
}

ImpValue TypeVisitor::visit(IFExp* exp) {
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
    return ImpValue(env->lookup_type(exp->id), 0, false, "");
}

// Statements

void TypeVisitor::visit(AssignStatement* stm) {
    string tipo_variable = env->lookup_type(stm->lvalue->id);
    ImpValue rhs_value = stm->rhs->accept(this);

    if (ImpValue::get_basic_type(rhs_value.type) != ImpValue::get_basic_type(tipo_variable)) {
        cout << "Error de tipos: se esperaba " << tipo_variable
             << " pero se encontró " << rhs_value.type << endl;
        exit(0);
    }
}

void TypeVisitor::visit(VarDec* vd) {
    std::string t = ImpValue::get_basic_type(vd->type);
    if (t != "bool" && t != "int" && t != "string") {
        cout << "type error en declaración de variable" << endl;
        exit(0);
    }

    for(auto i: vd->vars){
        if (!i->dimList.empty() && i->iv) {
            cout << "array";
            for (int in = 0; in < i->iv->list.size(); in++) {
                int val = i->iv->list[in]->value->accept(this).int_value;
                cout << " D: " << val;
                env->add_var(i->id, val, t); // Aquí probablemente también debas diferenciar por índice
            }
        }

        else if(i->iv){
            ImpValue valor_init = i->iv->accept(this);
            if(ImpValue::get_basic_type(valor_init.type) != t){
                cout << "Error de tipo en inicialización de variable: " << i->id << endl;
                exit(0);
            }
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
    if(stm->format == "%s\n" && ImpValue::get_basic_type(arg.type) != "string"){
        cout << "Error: printf espera string, pero se pasa " << arg.type << endl;
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
    if (stm->b) stm->b->accept(this);
}

void TypeVisitor::visit(DoWhileStatement* stm) {
    if (stm->b) stm->b->accept(this);
    ImpValue cond = stm->condition->accept(this);
    if (ImpValue::get_basic_type(cond.type) != "bool") {
        cout << "Error: condición del do-while debe ser booleana" << endl;
        exit(0);
    }
}

void TypeVisitor::visit(ForStatement* stm) {
    ImpValue rhs=stm->start->accept(this);
    if(rhs.type!=stm->type) {
        cout << "Error en el type del for" << endl;
        exit(0);
    }
    if (stm->type=="string") {
        cout << "No se puede declarar string dentro del for" << endl;
        exit(0);
    }
}

void TypeVisitor::visit(ReturnStatement* stm) {
    ImpValue ret = stm->e->accept(this);
    if(ImpValue::get_basic_type(ret.type) != "int") {
        cout << "Error: return debe ser int" << endl;
        exit(0);
    }
}

void TypeVisitor::visit(FCallStatement* stm) {
}

ImpValue TypeVisitor::visit(InitValue* iv) {
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
    if(fd->cuerpo) fd->cuerpo->accept(this);
    env->remove_level();
}

void TypeVisitor::visit(FunDecList* fdl) {
    for(auto i: fdl->Fundecs){
        i->accept(this);
    }
}

#endif