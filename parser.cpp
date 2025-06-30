#include <iostream>
#include <stdexcept>
#include "token.h"
#include "scanner.h"
#include "exp.h"
#include "parser.h"

using namespace std;
string tipo_rt;
Parser::~Parser() {}

bool Parser::match(Token::Type ttype) {
    if (check(ttype)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(Token::Type ttype) {
    if (isAtEnd()) return false;
    return current->type == ttype;
}

bool Parser::advance() {
    if (!isAtEnd()) {
        Token* temp = current;
        if (previous) delete previous;
        current = scanner->nextToken();
        previous = temp;
        if (check(Token::ERR)) {
            cout << "Error de análisis, carácter no reconocido: " << current->text << endl;
            exit(1);
        }
        return true;
    }
    return false;
}

bool Parser::isAtEnd() {
    return (current->type == Token::END);
}

bool Parser::checkTypeToken() {
    return current->type == Token::NUM ||
           current->type == Token::STRING ||
           current->type == Token::BOOLEAN ||
           current->type == Token::INT ||
            current->type == Token::VOID;
}

Parser::Parser(Scanner* sc):scanner(sc) {
    previous = NULL;
    current = scanner->nextToken();
    if (current->type == Token::ERR) {
        cout << "Error en el primer token: " << current->text << endl;
        exit(1);
    }
}

Body* Parser::ParseBody() {
    VarDecList* vdl = nullptr;
    StatementList* smtl = nullptr;
    bool hasVars = false, hasStms = false;

    while (!check(Token::LLD) && !isAtEnd()) {
        if (check(Token::INT) || check(Token::STRING) || check(Token::BOOLEAN)) {
            if (!vdl) vdl = new VarDecList();
            VarDec* vd = ParseVarDec();
            vdl->add(vd);
            hasVars = true;
        } else {
            if (!smtl) smtl = new StatementList();
            Stm* s = ParseStatement();
            smtl->add(s);
            hasStms = true;
        }
    }

    return new Body(vdl, smtl);
}

// Fun

FunDecList* Parser::ParseFunDecList() {
    list<FunDec*> fundecs;

    if (!checkTypeToken()) {
        cout << "Error: se esperaba al menos una declaración de función." << endl;
        exit(1);
    }

    while (checkTypeToken()){
        FunDec* f = ParseFunDec();
        fundecs.push_back(f);
    }

    return new FunDecList(fundecs);
}

FunDec* Parser::ParseFunDec() {
    string type;
    if (!checkTypeToken()) {
        cout << "Error: se esperaba un tipo al comienzo de la declaración." << endl;
        exit(1);
    }
    type = current->text;
    tipo_rt= type;
    advance();

    if (!match(Token::ID)) {
        cout << "Error: se esperaba un id." << endl;
        exit(1);
    }
    string id = previous->text;

    if (!match(Token::PI)) {
        cout << "Error: se esperaba '(' después del nombre de la función." << endl;
        exit(1);
    }

    vector<string> tipos;
    vector<string> parametros;

    if (!check(Token::PD)) {
        do {
            if (!checkTypeToken()) {
                cout << "Error: se esperaba un tipo en la lista de parámetros." << endl;
                exit(1);
            }
            string tipoParam = current->text;
            advance();

            if (!match(Token::ID)) {
                cout << "Error: se esperaba un nombre de parámetro." << endl;
                exit(1);
            }
            string nombreParam = previous->text;

            tipos.push_back(tipoParam);
            parametros.push_back(nombreParam);
        } while (match(Token::COMA));
    }

    if (!match(Token::PD)) {
        cout << "Error: se esperaba ')' al final de la lista de parámetros." << endl;
        exit(1);
    }

    if (!match(Token::LLI)) {
        cout << "Error: se esperaba '{' después de los parámetros." << endl;
        exit(1);
    }

    Body* b = ParseBody();

    if (!match(Token::LLD)) {
        cout << "Error: se esperaba '}' al final del cuerpo de la función." << endl;
        exit(1);
    }

    return new FunDec(id, type, parametros, tipos, b);
}

// Var

VarDecList* Parser::ParseVarDecList() {
    VarDecList* vdl = new VarDecList();
    while (check(Token::INT) || check(Token::STRING) || check(Token::BOOLEAN)) {
        VarDec* vd = ParseVarDec();
        vdl->add(vd);
    }
    return vdl;
}

VarDec* Parser::ParseVarDec() {
    string type;
    if (!checkTypeToken()) {
        cout << "Error: se esperaba un tipo al comienzo de la declaración." << endl;
        exit(1);
    }
    type = current->text;
    advance();

    list<Var*> vars;
    vars.push_back(ParseVar());

    while (match(Token::COMA)) {
        vars.push_back(ParseVar());
    }
    if (!match(Token::PC)) {
        cout << "Error: se esperaba ';' al final de la declaración." << endl;
        exit(1);
    }
    return new VarDec(type, vars);
}

InitValue* Parser::ParseInitValue() {
    if(match(Token::LLI)) {
        vector<InitValue*> items;
        items.push_back(ParseInitValue());
        while (match(Token::COMA)) {
            items.push_back(ParseInitValue());
        }
        if (!match(Token::LLD)) {
            cout << "Error: se esperaba '}' al final de la lista de inicialización." << endl;
            exit(1);
        }
        return new InitValue(items);
    } else {
        Exp* e = parseAExp();
        return new InitValue(e);
    }
}

Var* Parser::ParseVar() {
    string id;
    if(!match(Token::ID)) {
        cout << "Error: se esperaba un id." << endl;
        exit(1);
    }
    id = previous->text;

    list<NumberExp*> dl;
    while(match(Token::CI)) {
        if(!match(Token::NUM)) {
            cout << "Error: se esperaba un número después del corchete izquierdo" << endl;
            exit(1);
        }
        int n = stoi(previous->text);
        NumberExp* ne = new NumberExp(n);

        if(!match(Token::CD)) {
            cout << "Error: se esperaba un ']' después de la expresión de índice." << endl;
            exit(1);
        }
        dl.push_back(ne);
    }
    InitValue* iv = nullptr;
    if (match(Token::ASSIGN)) {
        iv = ParseInitValue();
    }
    return new Var(id, dl, iv);
}

Program* Parser::ParseProgram() {
    //VarDecList* vdl = ParseVarDecList();
    VarDecList* vdl = nullptr;
    FunDecList* fdl = ParseFunDecList();
    return new Program(vdl, fdl);
}

// Stm

StatementList* Parser::ParseStatementList() {
    StatementList* sl = new StatementList();
    sl->add(ParseStatement());
    // No debe llegar al final o llave derech que cierra la función
    while (!check(Token::LLD) && !isAtEnd()) {
        sl->add(ParseStatement());
    }
    return sl;
}

Stm* Parser::ParseStatement(){
    Stm* s = NULL;
    Exp* e = NULL;

    if (current == NULL) {
        cout << "Error: Token actual es NULL" << endl;
        exit(1);
    }
    if (match(Token::ID)) {
        string id = previous->text;

        if (check(Token::PI)) {
            advance();
            vector<Exp*> args;
            if (!check(Token::PD)) {
                args.push_back(parseAExp());
                while (match(Token::COMA)) {
                    args.push_back(parseAExp());
                }
            }
            if (!match(Token::PD)) {
                cout << "Error: se esperaba ')' al final de los argumentos de la función." << endl;
                exit(1);
            }
            if (!match(Token::PC)) {
                cout << "Error: se esperaba ';' al final de la llamada a función." << endl;
                exit(1);
            }
            s = new FCallStatement(new FCallExp(id, args));
        }
        else {
            vector<Exp*> indices;
            while(match(Token::CI)) {
                Exp* indexExp = parseAExp();
                if(!match(Token::CD)) {
                    cout << "Error: se esperaba un ']' después de la expresión de índice." << endl;
                    exit(1);
                }
                indices.push_back(indexExp);
            }
            if (!match(Token::ASSIGN)) {
                cout << "Error: se esperaba '=' después de LValue." << endl;
                exit(1);
            }
            Exp* rhs = parseAExp();
            if (!match(Token::PC)) {
                cout << "Error: se esperaba ';' al final de la asignación." << endl;
                exit(1);
            }
            LValue* lv = new LValue(id, indices);
            s = new AssignStatement(lv, rhs);
        }
    }
    else if (match(Token::PRINTF)) {
        if (!match(Token::PI)) {
            cout << "Error: se esperaba un '(' después del printf." << endl;
            exit(1);
        }
        
        // Acepta printf con o sin argumentos adicionales
        if (!match(Token::STRING)) {
            cout << "Error: se esperaba un string de formato (ej: \"%d\") después del '('." << endl;
            exit(1);
        }
        string format = previous->text;

        // Argumentos adicionales (opcionales)
        vector<Exp*> args;
        while (match(Token::COMA)) {
            args.push_back(parseAExp());
        }

        if (!match(Token::PD)) {
            cout << "Error: se esperaba un ')' después de los argumentos." << endl;
            exit(1);
        }
        if (!match(Token::PC)) {
            cout << "Error: se esperaba ';' al final del printf." << endl;
            exit(1);
        }

        s = new PrintStatement(format, args);  // Asegúrate que PrintStatement acepte un vector
    }
    else if (match(Token::IF)) {
        vector<IFExp*> ifexpl;
        if (!match(Token::PI)) {
            cout << "Error: se esperaba un '(' después del if." << endl;
            exit(1);
        }
        e = parseAExp();
        if (!match(Token::PD)) {
            cout << "Error: se esperaba un ')' después de la expresión." << endl;
            exit(1);
        }
        if (!match(Token::LLI)) {
            cout << "Error: se esperaba un '{' después del paréntesis izquierdo." << endl;
            exit(1);
        }
        Body* b = ParseBody();
        if (!match(Token::LLD)) {
            cout << "Error: se esperaba un '}' después del paréntesis derecho." << endl;
            exit(1);
        }
        IFExp* ie = new IFExp(e, b);

        ifexpl.push_back(ie);

        while(match(Token::ELSEIF)){
            if (!match(Token::PI)) {
                cout << "Error: se esperaba un '(' después del else if." << endl;
                exit(1);
            }
            Exp* e1 = parseAExp();
            if (!match(Token::PD)) {
                cout << "Error: se esperaba un ')' después de la expresión." << endl;
                exit(1);
            }
            if (!match(Token::LLI)) {
                cout << "Error: se esperaba un '{' después del paréntesis derecho." << endl;
                exit(1);
            }
            Body* b1 = ParseBody();
            if (!match(Token::LLD)) {
                cout << "Error: se esperaba un '}' después del body." << endl;
                exit(1);
            }
            IFExp* ie_new = new IFExp(e1, b1);
            ifexpl.push_back(ie_new);
        }

        if (match(Token::ELSE)) {
            if (!match(Token::LLI)) {
                cout << "Error: se esperaba un '{' después del else." << endl;
                exit(1);
            }
            Body* b2 = ParseBody();
            if (!match(Token::LLD)) {
                cout << "Error: se esperaba un '}' después del body." << endl;
                exit(1);
            }
            IFExp* ie_new1 = new IFExp(nullptr, b2);
            ifexpl.push_back(ie_new1);
        }
        s = new IfStatement(ifexpl);
    }
    else if (match(Token::FOR)){
        if (!match(Token::PI)) {
            cout << "Error: se esperaba un '(' después del while." << endl;
            exit(1);
        }
        if (!checkTypeToken()) {
            cout << "Error: se esperaba declaración de variable en la inicialización del for (por ejemplo: int i = 0)." << endl;
            exit(1);
        }
        string type=current->text;
        advance();
        if (!match(Token::ID)) {
            cout << "Error:se esperaba un id después de la expresión." << endl;
            exit(1);
        }
        string id = previous->text;
        if (!match(Token::ASSIGN)) {
            cout << "Error: se esperaba un '=' después del id." << endl;
            exit(1);
        }
        Exp* start = parseCExp();
        if (!match(Token::PC)) {
            cout << "Error: se esperaba un ';' después de la expresión." << endl;
            exit(1);
        }
        Exp* condition = parseCExp();
        if (!match(Token::PC)) {
            cout << "Error: se esperaba un ';' después de la expresión." << endl;
            exit(1);
        }
        if (!match(Token::ID)) {
            cout << "Error: se esperaba el identificador del paso (e.g., i++)." << endl;
            exit(1);
        }
        string stepId = previous->text;
        if (stepId != id) {
            cout << "Error: el identificador en el paso no coincide con el inicial del 'for'." << endl;
            exit(1);
        }
        BinaryOp op;
        if (match(Token::DPLUS)) {
            op = INC_OP;
        } else if (match(Token::DMINUS)) {
            op = DEC_OP;
        } else {
            cout << "Error: se esperaba '++' o '--' en el paso del for." << endl;
            exit(1);
        }
        Exp* step = new BinaryExp(new IdentifierExp(stepId), new NumberExp(1), op);
        if (!match(Token::PD)) {
            cout << "Error: se esperaba un ')' después de la expresión." << endl;
            exit(1);
        }
        if (!match(Token::LLI)) {
            cout << "Error: se esperaba un '{' después del paréntesis derecho." << endl;
            exit(1);
        }
        Body* b = ParseBody();
        if (!match(Token::LLD)) {
            cout << "Error: se esperaba un '}' después del body." << endl;
            exit(1);
        }
        return new ForStatement( id, type,start, condition, step, b);
    }
    else if (match(Token::WHILE)){
        if (!match(Token::PI)) {
            cout << "Error: se esperaba un '(' después del while." << endl;
            exit(1);
        }
        e = parseCExp();
        if (!match(Token::PD)) {
            cout << "Error: se esperaba un ')' después de la expresión." << endl;
            exit(1);
        }
        if (!match(Token::LLI)) {
            cout << "Error: se esperaba un '{' después del paréntesis derecho." << endl;
            exit(1);
        }
        Body* b = ParseBody();
        if (!match(Token::LLD)) {
            cout << "Error: se esperaba un '}' después del body." << endl;
            exit(1);
        }
        s = new WhileStatement(e, b);
    }
    else if (match(Token::DO)){
        if (!match(Token::LLI)) {
            cout << "Error: se esperaba un '{' después del do." << endl;
            exit(1);
        }
        Body* b = ParseBody();
        if (!match(Token::LLD)) {
            cout << "Error: se esperaba un '}' después de la expresión." << endl;
            exit(1);
        }
        if (!match(Token::WHILE)) {
            cout << "Error: se esperaba un while después del corchete derecho" << endl;
            exit(1);
        }
        if (!match(Token::PI)) {
            cout << "Error: se esperaba un '(' después del while." << endl;
            exit(1);
        }
        e = parseAExp();
        if (!match(Token::PD)) {
            cout << "Error: se esperaba un ')' después de la expresión." << endl;
            exit(1);
        }
        if (!match(Token::PC)) {
            cout << "Error: se esperaba un ';' al final del dowhile." << endl;
            exit(1);
        }
        s = new DoWhileStatement(e, b);
    }
    else if (match(Token::RETURN)){
        if(tipo_rt=="int" or tipo_rt=="bool" or tipo_rt=="string") {
            if(current->text==";") {
                cout << "Error: se esperaba un valor de retorno en función de tipo " << tipo_rt << endl;
                exit(1);
            }
            e = parseAExp();
            if (!match(Token::PC)) {
                cout << "Error de tipos" << endl;
                exit(1);
            }

        }
        else if (tipo_rt=="void") {
            e=nullptr;
            if(current->text!=";") {
                cout << "Error: función void no debe retornar valor." << endl;
                exit(1);
            }
            match(Token::PC);
        }
        s = new ReturnStatement(e);
    }
    else {
        cout << "Error: Se esperaba un identificador o 'print', pero se encontró: " << *current << endl;
        exit(1);
    }
    return s;
}

// Exp

Exp* Parser::parseAExp() {
    Exp* left = parseBExp();
    while (match(Token::AND) || match(Token::OR)) {
        BinaryOp op = (previous->type == Token::AND) ? AND_OP : OR_OP;
        Exp* right = parseBExp();
        left = new BinaryExp(left, right, op);
    }

    return left;
}

Exp* Parser::parseBExp() {
    if (match(Token::NOT)) {
        Exp* operand = parseBExp();
        return new BinaryExp(operand, nullptr, NOT_OP);
    }
    return parseCExp();
}

Exp* Parser::parseCExp() {
    Exp* left = parseExpression();
    if(match(Token::EQUAL) || match(Token::LET) || match(Token::LT) || match(Token::DIFF) || match(Token::GET) || match(Token::GT)){
        BinaryOp op = EQ_OP;

        if(previous->type == Token::EQUAL) op = EQ_OP;
        else if(previous->type == Token::LET) op = LET_OP;
        else if(previous->type == Token::GET) op = GET_OP;
        else if(previous->type == Token::LT) op = LT_OP;
        else if(previous->type == Token::GT) op = GT_OP;
        else if(previous->type == Token::DIFF) op = DIFF_OP;

        Exp* right = parseExpression();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

Exp *Parser::parseExpression() {
    Exp* left = parseTerm();
    while (match(Token::PLUS) || match(Token::MINUS)) {
        BinaryOp op;
        if (previous->type == Token::PLUS) op = PLUS_OP;
        else if (previous->type == Token::MINUS) op = MINUS_OP;
        Exp* right = parseTerm();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

Exp* Parser::parseTerm() {
    Exp* left = parseFactor();
    while (match(Token::MUL) || match(Token::DIV)) {
        BinaryOp op;
        if (previous->type == Token::MUL) op = MUL_OP;
        else if (previous->type == Token::DIV) op = DIV_OP;
        Exp* right = parseFactor();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

Exp* Parser::parseFactor() {
    if (match(Token::TRUE)){
        return new BoolExp(1);
    }
    else if (match(Token::FALSE)){
        return new BoolExp(0);
    }
    else if (match(Token::NUM)){
        return new NumberExp(stoi(previous->text));
    }
    else if (match(Token::ID)) {
        string id = previous->text;
        if (match(Token::PI)) {
            vector<Exp*> args;
            if (!check(Token::PD)) {
                args.push_back(parseAExp());
                while (match(Token::COMA)) {
                    args.push_back(parseAExp());
                }
            }
            if (!match(Token::PD)) {
                cout << "Error: se esperaba ')' al final de los argumentos de la función." << endl;
                exit(1);
            }
            return new FCallExp(id, args);
        }

        vector<Exp*> indices;
        while (match(Token::CI)) {
            Exp* indexExp = parseAExp();
            if (!match(Token::CD)) {
                cout << "Error: se esperaba un ']' después de la expresión de índice." << endl;
                exit(1);
            }
            indices.push_back(indexExp);
        }

        if (!indices.empty()) {
            return new ArrayAccessExp(id, indices);
        }

        return new IdentifierExp(id);
    }
    else if (match(Token::STRING)){
        return new StringLiteral(previous->text);
    }
        // números negativos
    else if(match(Token::MINUS)){
        if(match(Token::NUM)) return new NumberExp(stoi(previous->text)* -1);
        cout << "Error, se esperaba un número negativo." << endl;
        exit(0);
    }
    else if (match(Token::PI)) {
        Exp* e = parseAExp();
        if (!match(Token::PD)) {
            cout << "Error: se esperaba ')' después de la expresión entre paréntesis." << endl;
            exit(1);
        }
        return e;
    }
    cout << "Error: se esperaba un número, bool, string, identificador o expresión entre paréntesis." << endl;
    exit(0);
}