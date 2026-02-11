#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include<sstream>
#include <cctype>
using namespace std;

enum TokenType {
    Token_INT,
    Token_IF,
    Token_IDENTIFIER,
    Token_NUMBER,
    Token_ASSIGN,
    Token_EQUALS,
    Token_MINUS,
    Token_PLUS,
    Token_DIV,
    Token_MUL,
    Token_LPAREN,
    Token_RPAREN,
    Token_LBRACE,
    Token_RBRACE,
    Token_SEMI,
    Token_EOF,
};

struct Token {
    TokenType type;
    string val;
    int linenum;
};
class Lexer {
    string src;
    int pos;
    int line;

public:
    Lexer(string code){
        src = code;
        pos = 0;
        line = 1;
    }

    Token nextToken(){
        Token t;
        t.linenum = line;
        while (pos < src.size() && isspace(src[pos])) {
            if (src[pos] == '\n') line++;
            pos++;
        }
        if (pos >= src.size()) {
            t.type = Token_EOF;
            t.val = "";
            return t;
        }
        if(pos +1 < src.size() && src[pos] == '/' && src[pos+1] == '/'){
            while(pos<src.size() && src[pos] != '\n') pos++;
            return nextToken();
        }

        if (isalpha(src[pos])) {
            string word ="";
            while (pos < src.size() && isalnum(src[pos])) {
                word += src[pos];
                pos++;
            }
            t.val = word;
            if(word == "int") t.type = Token_INT;
            else if(word == "if") t.type = Token_IF;
            else t.type = Token_IDENTIFIER;
            return t;
        }

        if (isdigit(src[pos])) {
            string num = "";
            while (pos < src.size() && isdigit(src[pos])) {
                num += src[pos];
                pos++;
            }
            t.type = Token_NUMBER;
            t.val = num;
            return t;
        }

        if (pos +1 <src.size() && src[pos] == '=' && src[pos+1] == '=') {
            t.type = Token_EQUALS;
            t.val = "==";
            pos += 2;
            return t;
        }

        char c = src[pos];
        pos++;
        t.val = string(1, c);

        if (c == '+') t.type = Token_PLUS;
        else if (c == '-') t.type = Token_MINUS;
        else if (c == '*') t.type = Token_MUL;
        else if (c == '/') t.type = Token_DIV;
        else if (c == '(') t.type = Token_LPAREN;
        else if (c == ')') t.type = Token_RPAREN;
        else if (c == '{') t.type = Token_LBRACE;
        else if (c == '}') t.type = Token_RBRACE;
        else if (c == ';') t.type = Token_SEMI;
        else if (c == '=') t.type = Token_ASSIGN;
        else t.type = Token_EOF;
        return t;
    }

};

enum NodeType {
    N_DECL,
    N_ASSIGN,
    N_IF,
    N_CMP,
    N_BINOP,
    N_NUM,
    N_VAR,
    N_BODY
};

struct Node {
    NodeType type;
    string val;
    vector<Node*> kids;
    Node(NodeType t, string v) {
        type = t;
        val = v;
    }

};
Node* makeNode(NodeType t, string v) {
    return new Node(t, v);
}

class parser {
    Lexer* lex;
    Token cur;

    void advance(){
        cur = lex->nextToken();
    }
    void expect(TokenType t , string what){
        if(cur.type != t){
            cerr << "Error: expected " << what << " at line " << cur.linenum << endl;
            exit(1);
        }
        advance();
    }

    Node* parseDecl(){
        expect(Token_INT, "int");
        string name = cur.val;
        expect(Token_IDENTIFIER, "variable name");
        expect(Token_SEMI, ";");
        return makeNode(N_DECL, name);
    }
    Node* parsePrimary(){
        if (cur.type == Token_NUMBER) {
            string v = cur.val;
            advance();
            return makeNode(N_NUM, v);
        }
        if (cur.type == Token_IDENTIFIER) {
            string v = cur.val;
            advance();
            return makeNode(N_VAR, v);
        }
        cerr << "Error: expected number or variable at line " << cur.linenum << endl;
        exit(1);
    }
    Node* parseExpr(){
        Node* left = parsePrimary();
        if( cur.type == Token_PLUS || cur.type == Token_MINUS){
            string op = cur.val;
            advance();
            Node* right = parsePrimary();
            Node* binop = makeNode(N_BINOP, op);
            binop->kids.push_back(left);
            binop->kids.push_back(right);
            return binop;
        }
        return left;
    }

    Node* parseAssign(){
        string name = cur.val;
        expect(Token_IDENTIFIER, "variable name");
        expect(Token_ASSIGN, "=");
        Node* n = makeNode(N_ASSIGN, name);
        n->kids.push_back(makeNode(N_VAR, name));
        n->kids.push_back(parseExpr()); 
        expect(Token_SEMI, ";");
        return n;
    }

    Node* parseCond(){
        Node* left = parsePrimary();
        expect(Token_EQUALS, "==");
        Node* right = parsePrimary();
        Node* cmp = makeNode(N_CMP, "==");
        cmp->kids.push_back(left);
        cmp->kids.push_back(right);
        return cmp;
    }

     Node* parseIf(){
        expect(Token_IF, "if");
        expect(Token_LPAREN, "(");
        Node* cond = parseCond();
        expect(Token_RPAREN, ")");
        expect(Token_LBRACE, "{");
        Node* ifnode = makeNode(N_IF, "if");
        ifnode-> kids.push_back(cond);
        Node* body = makeNode(N_BODY, "body");
        while(cur.type != Token_RBRACE && cur.type != Token_EOF){
            body->kids.push_back(parseStmt());
        }
        ifnode->kids.push_back(body);
        expect(Token_RBRACE, "}");
        return ifnode;
     }

    Node* parseStmt(){
        if(cur.type == Token_INT) return parseDecl();
        if(cur.type == Token_IF) return parseIf();
        if(cur.type == Token_IDENTIFIER) return parseAssign();
        advance();
        return nullptr;
    }   
    public:
    parser(Lexer* l){
        lex = l;
        advance();
    }

    vector<Node*> parse(){
        vector<Node*> stmts;
        while(cur.type != Token_EOF){
            Node* stmt = parseStmt();
            if(stmt != nullptr) stmts.push_back(stmt);
        }
        return stmts;
    }
};

class CodeGen {
    map<string, int> symtable;
    int memaddr;
    int lablecount;
    stringstream asm_output;
    void genNode(Node* n){
        if( n == nullptr) return;
        if (n->type == N_DECL) genDecl(n);
        else if (n->type == N_ASSIGN) genAssign(n);
        else if (n->type == N_IF) genIf(n);
        else if (n->type == N_BODY){
            for(Node* child : n->kids){
                genNode(child);
            }
        }
    }
    void genDecl(Node* n){
        symtable[n->val] = memaddr;
        asm_output<< "; declare " << n->val << "->address " << memaddr << '\n';
        memaddr ++;
    }
    void genAssign(Node* n){
        string varname = n->kids[0]->val;
        int addr = symtable[varname];
        Node* rhs = n->kids[1];

        if (rhs->type == N_NUM){
            asm_output << "ldi A"<< rhs->val << '\n';
            asm_output << "mov M" << addr << '\n';
        }
        else if(rhs->type == N_VAR){
            asm_output << "mov A M" << symtable[rhs->val] << '\n';
            asm_output << "mov M" << addr << '\n';
        }
        else if (rhs->type == N_BINOP){
            genBinOp(rhs, addr);
        }
    }

    void genBinOp(Node* n, int destaddr){
        Node* left = n->kids[0];
        Node* right = n->kids[1];
        
        if(left->type == N_VAR){
            asm_output << "mov A M" << symtable[left->val] << '\n';
        }
        else{
            asm_output << "ldi A" << left->val << '\n';
        }
        
        if(right->type == N_VAR){
            asm_output << "mov B M" << symtable[right->val] << '\n';
        }
        else{
            asm_output << "ldi B" << right->val << '\n';
        }
        if (n->val == "+") asm_output << "add\n";
        else    asm_output << "sub\n";
        asm_output << "mov M A" << destaddr << '\n';
        
    }
    void genIf(Node* n){
        int lnum = lablecount++;
        Node* cond = n->kids[0];
        Node* body = n->kids[1];
        Node* Left = cond->kids[0];
        Node* Right = cond->kids[1];
        if(Left->type == N_VAR){
            asm_output << "mov A M" << symtable[Left->val] << '\n';
        }
        else{
            asm_output << "ldi A" << Left->val << '\n';
        }
        if(Right->type == N_VAR){
            asm_output << "mov B M" << symtable[Right->val] << '\n';
        }
        else{
            asm_output << "ldi B" << Right->val << '\n';
        }
        asm_output << "cmp\n";
        asm_output << "jne end_if" << lnum << '\n';
        genNode(body);
        asm_output << "end_if_" << lnum << ":\n";
    }

public:
    CodeGen(){
        memaddr = 0;
        lablecount = 0;
    }
    string generate(vector<Node*>& stmts){
        asm_output << "; output from simplelang compiler\n\n";
        for(Node* s : stmts){
            if(s->type == N_DECL) genNode(s);
        }
        asm_output<< "\n";
        for (Node* s : stmts){
            if(s->type != N_DECL) genNode(s);
        }
        asm_output <<"hlt\n";
        return asm_output.str();
    }

};

void printTress(Node* n, int depth){
    if ( n == nullptr) return;
    for(int i = 0; i<depth; i++) cout << "   "; 
    switch(n->type){
        case N_DECL: cout << "DECL( " << n->val << ")\n"; break;
        case N_ASSIGN: cout << "ASSIGN\n"; break;
        case N_IF: cout << "IF\n" << endl; break;
        case N_CMP: cout << "Cmp(==)\n"; break;
        case N_BINOP: cout << "BinOp( " << n->val << ")\n"; break;
        case N_NUM: cout << "Num( " << n->val << ")\n"; break;
        case N_VAR: cout << "Var( " << n->val << ")\n"; break;
        case N_BODY: cout << "Body\n"; break;
    }
    for (Node* child : n->kids){
        printTress(child, depth +1);
    }
}

int main (int argc, char* argv[]){
    cout << "Simplelang Compiler\n";
    if( argc != 2){
        cerr << "Usage: ./compiler <file.simple>\n";
        cout << "Example: ./compiler ../programs/input.simple\n";
        return 1;
    }
    string infile = argv[1];
    string outfiles = infile;
    int dotpos = outfiles.find(".simple");
    if (dotpos != string::npos){
        outfiles.replace(dotpos, 7, ".asm");
    }
    else{
        outfiles += ".asm";
    }
    string justname = outfiles;
    int slashpos = justname.rfind('/');

    if (slashpos != string::npos){
        justname = justname.substr(slashpos +1);
    }
    string copypath = "../output/";
    copypath += justname;

    ifstream fin(infile);
    if (!fin.is_open()) {
        cerr << "Error: cannot open file " << infile << endl;
        return 1;
    }
    string code = "";
    string line = "";
    while (getline(fin, line)){
        code += line + '\n';
    }
    fin.close();

    cout << "intput :" << infile << "\n";
    cout << "output :" << outfiles << "\n";
    cout << "copy to :" << copypath << "\n";

    Lexer lex(code);
    parser parser(&lex);
    vector<Node*> tree = parser.parse();  
    cout<< "Abstract Syntax Tree:\n";
    for(Node* n : tree){
        printTress(n, 0);
    }cout << "\n";
    CodeGen codegen;
    string output = codegen.generate(tree);
    ofstream fout(copypath);
    if (!fout.is_open()) {
        cerr << "Error: cannot write to file " << copypath << "\n";
        return 1;
     }
    fout << output;
    fout.close();
        cout << "saved " << outfiles << "\n";
    ofstream fcopy(copypath);
    if (!fcopy.is_open()) {
        cerr << "Error: cannot copy to file " << copypath << "\n";
    }
    else{
        fcopy << output;
        fcopy.close();
        cout << "copied to " << copypath << "\n";
    }
    return 0;
}