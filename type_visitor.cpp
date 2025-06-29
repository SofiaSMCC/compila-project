#ifndef TYPE_VISITOR_H
#define TYPE_VISITOR_H

#include "type_visitor.h"

///////////////////////////////////////////////////////////////////////////////////

//0 = undefined
//1 = int
//2 = bool

void TypeVisitor::check(Program* program){
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
    return ImpValue();
}

ImpValue TypeVisitor::visit(ArrayAccessExp* exp) {
    return ImpValue();
}

ImpValue TypeVisitor::visit(IFExp* exp) {
    return ImpValue();
}

ImpValue TypeVisitor::visit(FCallExp* exp) {
    return ImpValue();
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
    cout << "Asignación exitosa" << endl;
}

void TypeVisitor::visit(VarDec* vd) {
    std::string t = ImpValue::get_basic_type(vd->type);
    if (t != "bool" && t != "int" && t != "string") {
        cout << "type error en declaración de variable" << endl;
        exit(0);
    }
    for(auto i: vd->vars){
        env->add_var(i->id, t);
        if(i->iv){
            ImpValue valor_init = i->iv->accept(this);
            if(ImpValue::get_basic_type(valor_init.type) != t){
                cout << "Error de tipo en inicialización de variable: " << i->id << endl;
                exit(0);
            }
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
}
void TypeVisitor::visit(WhileStatement* stm) {
}

void TypeVisitor::visit(DoWhileStatement* stm) {
}

void TypeVisitor::visit(ForStatement* stm) {
    ImpValue rhs=stm->start->accept(this);
    if(rhs.type!=stm->type) {
        cout<<"Error en el type del for"<<endl;
    }
    if (stm->type=="string") {
        cout<<"No se puede declarar string dentro del for"<<endl;
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
    if(fd->cuerpo) fd->cuerpo->accept(this);
    env->remove_level();
}

void TypeVisitor::visit(FunDecList* fdl) {
    for(auto i: fdl->Fundecs){
        i->accept(this);
    }
}

#endif