//
// Created by Admin on 7/06/2025.
//
    #include <iostream>
    #include "exp.h"
    #include "visitor.h"
    #include <unordered_map>
    using namespace std;

    ///////////////////////////////////////////////////////////////////////////////////
    ImpValue BinaryExp::accept(Visitor* visitor) {
        return visitor->visit(this);
    }
    ImpValue IFExp::accept(Visitor* visitor) {
        return visitor->visit(this);
    }
    ImpValue NumberExp::accept(Visitor* visitor) {
        return visitor->visit(this);
    }
    ImpValue BoolExp::accept(Visitor* visitor) {
        return visitor->visit(this);
    }
    ImpValue IdentifierExp::accept(Visitor* visitor) {
        return visitor->visit(this);
    }
    int AssignStatement::accept(Visitor* visitor) {
        visitor->visit(this);
        cout<<"  llego?   ";
        return 0;
    }

    int PrintStatement::accept(Visitor* visitor) {
        visitor->visit(this);
        return 0;
    }

    int IfStatement::accept(Visitor* visitor) {
        visitor->visit(this);
        return 0;
    }

    int WhileStatement::accept(Visitor* visitor) {
        visitor->visit(this);
        return 0;
    }
    int ForStatement::accept(Visitor* visitor) {
        visitor->visit(this);
        return 0;
    }

    int VarDec::accept(Visitor* visitor) {
        visitor->visit(this);
        return 0;
    }

    int VarDecList::accept(Visitor* visitor) {
        visitor->visit(this);
        return 0;
    }
    int StatementList::accept(Visitor* visitor) {
        visitor->visit(this);
        return 0;
    }
    int Body::accept(Visitor* visitor) {
        visitor->visit(this);
        return 0;
    }
    ImpValue StringLiteral::accept(Visitor* visitor) {
        visitor->visit(this);
        return visitor->visit(this);
    }

    ///////////////////////////////////////////////////////////////////////////////////

    ImpValue PrintVisitor::visit(BinaryExp* exp) {
        if (exp->op == NOT_OP) {
            cout << "not ";
            exp->left->accept(this);
        } else {
            exp->left->accept(this);
            cout << ' ' << Exp::binopToChar(exp->op) << ' ';
            exp->right->accept(this);
        }
        return ImpValue();
    }

    ImpValue PrintVisitor::visit(NumberExp* exp) {
        cout << exp->value;
        return ImpValue();
    }

    ImpValue PrintVisitor::visit(BoolExp* exp) {
        if(exp->value) cout << "true";
        else cout << "false";
        return ImpValue();
    }

    ImpValue PrintVisitor::visit(IdentifierExp* exp) {
        cout << exp->name;
        return ImpValue();
    }

    void PrintVisitor::visit(AssignStatement* stm) {
        cout << stm->id << " = ";
        stm->rhs->accept(this);
        cout << ";";
    }

    void PrintVisitor::visit(PrintStatement* stm) {
        cout << "print(";
        stm->e->accept(this);
        cout << ");";
    }

    void PrintVisitor::visit(IfStatement* stm) {
        cout << "if ";
        stm->condition->accept(this);
        cout << " then" << endl;
        stm->then->accept(this);
        if(stm->els){
            cout << "else" << endl;
            stm->els->accept(this);
        }
        cout << "endif";
    }

    void PrintVisitor::imprimir(Program* program){
        program->body->accept(this);
    };


    ImpValue PrintVisitor::visit(IFExp* pepito) {
        cout<< "ifexp(";
        pepito->cond->accept(this);
        cout<< ",";
        pepito->left->accept(this);
        cout<< ",";
        pepito->right->accept(this);
        cout << ")" ;
        return ImpValue();
    }

    void PrintVisitor::visit(WhileStatement* stm){
        cout << "while ";
        stm->condition->accept(this);
        cout << " do" << endl;
        stm->b->accept(this);
        cout << "endwhile";
    }

    void PrintVisitor::visit(ForStatement* stm){
        cout << "for " << stm->id << " in range (";
        stm->start->accept(this);
        cout << ",";
        stm->end->accept(this);
        cout << ",";
        stm->step->accept(this);
        cout << ")" << endl;
        stm->b->accept(this);
        cout << "endfor";
    }
    void PrintVisitor::visit(VarDec* stm){
        cout << "var ";
        cout << stm->type;
        cout << " ";
        for(auto i: stm->vars){
            cout << i;
            if(i != stm->vars.back()) cout << ", ";
        }
        cout << ";";
    }

    void PrintVisitor::visit(VarDecList* stm){
        for(auto i: stm->vardecs){
            i->accept(this);
            cout << endl;
        }
    }

    void PrintVisitor::visit(StatementList* stm){
        for(auto i: stm->stms){
            i->accept(this);
            cout << endl;
        }
    }

    void PrintVisitor::visit(Body* stm){
        stm->vardecs->accept(this);
        cout << endl;
        stm->slist->accept(this);
    }

    ImpValue PrintVisitor::visit(StringLiteral *stm) {

        cout<<stm->value;
        return stm->accept(this);
    }


    ///////////////////////////////////////////////////////////////////////////////////
    ImpValue EVALVisitor::visit(BinaryExp* exp) {
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

            case LT_OP:
                result2 = left.int_value < right.int_value;
                return ImpValue("bool", 0, result2,"");
            case LE_OP:
                result2 = left.int_value <= right.int_value;
                return ImpValue("bool", 0, result2,"");
            case EQ_OP:
                if (left.type == "int" && right.type == "int") {
                    result2 = left.int_value == right.int_value;
                } else {
                    result2 = left.bool_value == right.bool_value;
                }
                return ImpValue("bool", 0, result2,"");

            case AND_OP:
                result2 = left.bool_value && right.bool_value;
                return ImpValue("bool", 0, result2,"");
            case OR_OP:
                result2 = left.bool_value || right.bool_value;
                return ImpValue("bool", 0, result2,"");
            case NOT_OP:
                result2 = !left.bool_value;
                return ImpValue("bool", 0, result2,"");

            default:
                cout << "Operador desconocido" << endl;
                return ImpValue();
        }
    }
    ImpValue  EVALVisitor::visit(NumberExp* exp) {
        int a = exp->value;
        return ImpValue("int",a,false,"");
    }

    ImpValue EVALVisitor::visit(BoolExp* exp) {
        bool a = exp->value;
        return ImpValue("bool",0,a,"");
    }

    ImpValue EVALVisitor::visit(IdentifierExp* exp) {
        string t= env.lookup_type(exp->name);
        if (t!="bool") {
            int v= env.lookup(exp->name).first;
            return ImpValue(t,v,false,"");
        }
        else if(t=="bool") {
            bool v=env.lookup(exp->name).first;
            return ImpValue(t,0,v,"");
        }
        else if(t=="string") {
            string s=env.lookup(exp->name).second;

        }
        return ImpValue();
    }

    void EVALVisitor::visit(AssignStatement* stm) {
        string t =env.lookup_type(stm->id); //tipo
        ImpValue val = stm->rhs->accept(this);
        cout<<"no llega :(";
        if(t=="bool") {
            env.update(stm->id, val.bool_value);
        }
        else if (t=="int"){
            env.update(stm->id, val.int_value);
        }
        else if(t=="string") {
            env.update(stm->id,val.string_value);

        }
    }

    void EVALVisitor::visit(PrintStatement* stm) {
        ImpValue val = stm->e->accept(this);
        if(val.type == "bool") {
            cout << val.bool_value << endl;
        }
        else if(val.type=="int") {
            cout << val.int_value << endl;
        }
        else if(val.type=="string") {
            cout << val.string_value << endl;
        }

    }

    void EVALVisitor::ejecutar(Program* program){
        env.add_level();
        program->body->accept(this);
        env.remove_level();
    }

    void EVALVisitor::visit(IfStatement* stm) {
        env.add_level();
        if(stm->condition->accept(this).int_value==1 or  stm->condition->accept(this).bool_value==1  ) {
            for (Stm* s : stm->then->slist->stms) {
                s->accept(this);
            }
        }
        else {
            for (Stm* s : stm->els->slist->stms) {
                s->accept(this);
            }
        }
        env.remove_level();
    }

    void EVALVisitor::visit(WhileStatement* stm) {
        env.add_level();
        if(stm->condition->accept(this).type == "bool"){
            while(stm->condition->accept(this).bool_value) {
                stm->b->accept(this);
            }
        }else {
            while(stm->condition->accept(this).int_value) {
                stm->b->accept(this);
            }
        }
        env.add_level();
    }


    ImpValue EVALVisitor::visit(IFExp* exp) {
        ImpValue cond = exp->cond->accept(this);
        if (cond.bool_value || cond.int_value) {
            return exp->left->accept(this);
        } else {
            return exp->right->accept(this);
        }
    }

    void EVALVisitor::visit(ForStatement* stm){
        env.add_level();

        int vstart = stm->start->accept(this).int_value;
        int vend   = stm->end  ->accept(this).int_value;
        int vstep  = stm->step ->accept(this).int_value;

        env.add_var(stm->id, "int");

        for(int i = vstart; i < vend; i += vstep) {
            env.update(stm->id, i);
            stm->b->accept(this);
        }

        env.remove_level();
    }


    void EVALVisitor::visit(VarDec* stm){
        for(auto i: stm->vars){
            env.add_var(i,stm->type);
        }
    }

    void EVALVisitor::visit(VarDecList* stm){
        for(auto i: stm->vardecs){
            i->accept(this);
        }
    }

    void EVALVisitor::visit(StatementList* stm){
        for(auto i: stm->stms){
            i->accept(this);
        }
    }

    void EVALVisitor::visit(Body* b){
        env.add_level();
        b->vardecs->accept(this);
        b->slist->accept(this);
        env.remove_level();
    }

    ImpValue EVALVisitor::visit(StringLiteral *stm) {
        return stm->accept(this);
    }

    ///////////////////////////////////////////////////////////////////////////////////

    //0 = undefined
    //1 = int
    //2 = bool

    void TypeVisitor::check(Program* program){
        env.add_level();
        program->body->accept(this);
        env.remove_level();
    }

    ImpValue TypeVisitor::visit(BinaryExp* exp) {
        ImpValue left, right;
        if (exp->op == NOT_OP) {
            left = exp->left->accept(this);
            if (left.type != "bool") {
                cout << "unsupported operand type";
                exit(0);
            }
            return ImpValue("bool", 0, false,"");
        } else {
            left = exp->left->accept(this);
            right = exp->right->accept(this);

            switch (exp->op) {
                case AND_OP:
                    if (left.type != "bool" || right.type != "bool") {
                        cout << "unsupported operand type";
                        exit(0);
                    }
                    return ImpValue("bool", 0, false,"");
                case OR_OP:
                    if (left.type != "bool" || right.type != "bool") {
                        cout << "unsupported operand type";
                        exit(0);
                    }
                    return ImpValue("bool", 0, false,"");

                case PLUS_OP:
                    if (left.type == "int" or  right.type == "int") {
                        cout << "unsupported operand type";
                        exit(0);
                    }
                    return ImpValue("int", 0, false,"");
                case MINUS_OP:
                    if (left.type == "int" or  right.type == "int") {
                        cout << "unsupported operand type";
                        exit(0);
                    }
                    return ImpValue("int", 0, false,"");
                case MUL_OP:
                    if (left.type != "int" || right.type != "int") {
                        cout << "unsupported operand type";
                        exit(0);
                    }
                    return ImpValue("int", 0, false,"");
                case DIV_OP:
                    if (left.type != "int" || right.type != "int") {
                        cout << "unsupported operand type";
                        exit(0);
                    }
                    return ImpValue("int", 0, false,"");
                default:
                    break;
            }
        }
        return ImpValue();
    }

    ImpValue TypeVisitor::visit(NumberExp* exp) {
        return ImpValue("int", 1, false,"");
    }

    ImpValue TypeVisitor::visit(BoolExp* exp) {
        return ImpValue("bool", 0, exp->value,"");
    }

    ImpValue TypeVisitor::visit(IdentifierExp* exp) {
        string t = env.lookup_type(exp->name);
        return ImpValue(t, 0, false,"");
    }

    ImpValue TypeVisitor::visit(IFExp* exp) {
        ImpValue cond = exp->cond->accept(this);
        if (cond.type != "bool") {
            exit(0);
        }
        ImpValue left = exp->left->accept(this);
        ImpValue right = exp->right->accept(this);
        if (left.type != right.type) {
            exit(0);
        }
        return left;
    }

    void TypeVisitor::visit(AssignStatement* stm) {
        string tipo_varibale = env.lookup_type(stm->id);
        if (stm->rhs->accept(this).int_value==1) {
            if (tipo_varibale=="int") {
                cout << "exitoso"<<endl;
            }
            else {
                cout << "Error en variable"<<endl;
                exit(0);
            }
        }
        if (stm->rhs->accept(this).bool_value==1) {
            if (tipo_varibale=="int") {
                cout << "Error en variable"<<endl;
                exit(0);
            }
            else {
                cout << "exito"<<endl;
            }
        }
        if (stm->rhs->accept(this).string_value!="") {
            if (tipo_varibale=="int" or tipo_varibale=="bool") {
                cout << "Error en variable"<<endl;
                exit(0);
            }
            else {
                cout << "exito"<<endl;
            }
        }
    }

    void TypeVisitor::visit(PrintStatement* stm) {
        stm->e->accept(this);
    }

    void TypeVisitor::visit(IfStatement* stm) {
        ImpValue cond = stm->condition->accept(this);
        if (cond.type != "bool") {
            cout << "Boolean is expected in if";
            exit(0);
        }
        stm->then->accept(this);
        if (stm->els) stm->els->accept(this);
    }

    void TypeVisitor::visit(WhileStatement* stm) {
        ImpValue cond = stm->condition->accept(this);
        if (cond.type != "bool") {
            cout << "Boolean is expected in while" << endl;
            exit(0);
        }
        stm->b->accept(this);
    }

    void TypeVisitor::visit(ForStatement* stm) {
        string t = env.lookup_type(stm->id);
        if (t != "int") {
            cout << "type error in for";
            exit(0);
        }
        ImpValue s = stm->start->accept(this);
        ImpValue e = stm->end->accept(this);
        ImpValue step = stm->step->accept(this);
        if (s.type != "int" || e.type != "int" || step.type != "int") {
            cout << "type error in for range bounds";
            exit(0);
        }
        stm->b->accept(this);
    }

    void TypeVisitor::visit(VarDec* stm) {
        string t = stm->type;
        if (t != "bool" && t != "int" && t!="string") {
            cout << "type error" << endl;
            exit(0);
        }
        for(auto i: stm->vars){
            env.add_var(i, stm->type);
            cout<<"sin valor";
        }
    }

    void TypeVisitor::visit(VarDecList* stm) {
        for(auto i: stm->vardecs){
            i->accept(this);
        }
    }

    void TypeVisitor::visit(StatementList* stm) {
        for(auto i: stm->stms){
            i->accept(this);
        }
    }

    void TypeVisitor::visit(Body* b) {
        b->vardecs->accept(this);
        b->slist->accept(this);
    }

    ImpValue TypeVisitor::visit(StringLiteral *stm) {
        return stm->accept(this);
    }
