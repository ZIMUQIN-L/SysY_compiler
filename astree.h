//
// Created by ZIMUQIN on 2021/10/30.
//

#ifndef GRAMMAR_ANA_ASTREE_H
#define GRAMMAR_ANA_ASTREE_H
#include "def.h"
enum fType {
    INTTYPE, VOIDTYPE
};

enum symType {
    EQU, NEQ, GT, LT, GET, LET, ADD, SUB, MULT, DIV, MOD
};

enum opType {
    NEGATIVE, POSITIVE, NOT
};

class UnaryOp;
class Ident;
class PrimaryExp;
class UnaryExp;
class MulExp;
class AddExp;
class Exp;
class LVal;
class RelExp;
class EqExp;
class LAndExp;
class LOrExp;
class Cond;
class ConstExp;
class BType;
class ConstInitVal;
class ConstDef;
class ConstDecl;
class InitVal;
class VarDef;
class VarDecl;
class Decl;
class FuncFParam;
class FuncFParams;
class FuncRParams;
class Stmt;
class BlockItem;
class Block;
class FuncDef;
class MainFuncDef;
class CompUnit;

enum Instruction {
    ADDIN, PUSH, CALL, VAR, ARR, ASSIGN, SUBU, MULTI,
    DIVI, MODI, NOTI, FUNC, PARA, INTFUNCTION, VOIDFUNCTION,
    ENDFUNCTION, PRINTVAR, PRINTSTR, LBRACE, RBRACE, GETARR,
    GETINT, ASSIGNARR, ENDCALL, BEFOREPUSH, LABEL, GOTO, RETURN,
    CONST, CMP, BNE, BGT, BLT, BGE, BLE, INT, VOID, BEQ, LEAVEBRACE
};

struct IR {
    Instruction instruction;
    std::string reg1;
    std::string reg2;
    std::string reg3;
    bool global = false;
};

class UnaryOp {
public:
    opType UnaryOp;
    void Analyze_Unary_Op() const;
};

class Ident {
public:
    std::string ident;
    int lineCount;

};

class PrimaryExp {
public:
    //0 : (exp)
    //1 : LVal
    //2 : Number
    int indicator;
    Exp * this_exp;
    LVal * this_lVal;
    int number;
    std::string generate_PrimaryExp_IR();
    std::string solve_PrimaryExp_Res();
    void primary_Error_Handler();
};

class UnaryExp {
public:
    //0 : primaryExp;
    //1 : Ident([FuncRParams])
    //2 : UnaryOp UnaryExp
    int flag;
    PrimaryExp * PriExp;
    Ident * this_Ident;
    //0 : no params
    //1 : is params
    int is_RParams;
    FuncRParams * this_FuncR;
    opType this_UnaryOp;
    UnaryExp * this_UnaryExp;
    std::string generate_UnaryExp_IR();
    std::string solve_UnaryExp_Res();
    void unaryExp_Error_Handler();
};

class MulExp {
public:
    std::vector<UnaryExp *> UnaryExps;
    std::vector<symType> Syms;
    std::string generate_MulExp_IR();
    std::string solve_MulExp_Res();
    void mulExp_Error_Handler();
};

class AddExp {
public:
    std::vector<MulExp *> MulExps;
    std::vector<symType> Syms;
    std::string generate_AddExp_IR();
    std::string solve_AddExp_Res();
    void addExp_Error_Handler();
};

class Exp {
public:
    AddExp * this_AddExp;
    std::string generate_Exp_IR();
    void exp_Error_Handler();
};

class LVal {
public:
    int dem;
    Ident * this_Ident;
    std::vector<Exp *> Exps;
    std::string generate_LVal_IR();
    std::string solve_LVal_Res();
    void lVal_Error_Handler();
};

class RelExp {
public:
    std::vector<AddExp *> AddExps;
    std::vector<symType> Syms;
    std::string generate_RelExp_IR();
    void relExp_Error_Handler();
};

class EqExp {
public:
    std::vector<RelExp *> RelExps;
    std::vector<symType> Syms;
    std::string generate_EqExp_IR(std::string begin_label, std::string out_label);
    void eqExp_Error_Handler();
};

class LAndExp {
public:
    std::vector<EqExp *> EqExps;
    void generate_LAndExp_IR(std::string begin_label, std::string out_label);
    void lAndExp_Error_Handler();
};

class LOrExp {
public:
    std::vector<LAndExp *> LAndExps;
    void generate_LOrExp_IR(std::string if_label, std::string else_label);
    void lOrExp_Error_Handler();
};

class Cond {
public:
    LOrExp * this_LOrExp;
    void generate_Cond_IR(std::string if_label, std::string else_label);
    void cond_Error_Handler();
};

class ConstExp {
public:
    //the ident that used must be constant
    AddExp * this_AddExp;
    std::string generate_ConstExp_IR();
    std::string solve_ConstExp_Res();
};

class BType {
public:
    std::string type = "int";
};

class ConstInitVal {
public:
    //0 : means it's a constExp
    //1 : means it's constInits
    int flag;
    ConstExp * this_ConstExp;
    std::vector<ConstInitVal *> ConstInits;
};

class ConstDef {
public:
    Ident * this_Ident;
    int dem;
    std::vector<ConstExp *> ConstExps;
    ConstInitVal * InitVal;
    void generate_ConstDef_IR();
};

class ConstDecl {
public:
    BType * this_BType;
    std::vector<ConstDef *> ConstDefs;
    void generate_ConstDecl_IR();
};

class InitVal {
public:
    //0 : means it's an exp
    //1 : means it's Inits
    int flag;
    Exp * this_Exp;
    std::vector<InitVal *> Inits;
};

class VarDef {
public:
    Ident * this_Ident;
    int dem;
    std::vector<ConstExp *> ConstExps;
    //if 1, there's init, else no
    int flag_Init;
    InitVal * this_InitVal;
    void generate_VarDef_IR();
};

class VarDecl {
public:
    BType * this_BType;
    std::vector<VarDef *> VarDefs;
    void generate_VarDecl_IR();
};

class Decl {
public:
    //if 0, means constDecl, if 1, means varDecl
    int flag;
    ConstDecl * this_ConstDecl;
    VarDecl * this_VarDecl;
    void generate_Decl_IR();
    void decl_Error_Handler();
};

class FuncFParam {
public:
    BType * this_BType;
    Ident * this_Ident;
    int dem;
    std::vector<ConstExp *> ConstExps;
    struct param generate_FuncFParam_IR();
};

class FuncFParams {
public:
    std::vector<FuncFParam *> funcFs;
};

class FuncRParams {
public:
    int count;
    std::vector<Exp *> Exps;
    void generate_FuncRParams_IR();
};

class Stmt {
public:
    //0 : LVal = Exp;
    //1 : [Exp];
    //2 : Block
    //3 : if (Cond) Stmt [else Stmt]
    //4 : while (Cond) Stmt
    //5 : break;
    //6 : return [Exp];
    //7 : LVal = getint();
    //8 : printf(Formatstring{,Exp});
    //9 : continue;
    int flag;
    //if there is an if, whether an else
    //1 means there is, and 0 means no
    int is_Else;
    int lineCount;
    //0 : no return exp;
    //1 : is return exp;
    int is_Return_Exp = 0;
    LVal * this_LVal;
    Block * this_Block;
    Exp * assignExp;
    Cond * this_Cond;
    Stmt * if_Stmt;
    Stmt * else_Stmt;
    Stmt * while_Stmt;
    Exp * returnExp;
    Exp * oneExp;
    std::vector<Exp *> Exps;
    std::vector<Stmt *> Stmts;
    std::string formatString;
    void generate_Stmt_IR();
    void stmt_Error_Handler();
};

class BlockItem {
public:
    //if 0, means it's a Decl, if 1, means it's a Stmt
    int flag;
    Decl * this_Decl;
    Stmt * this_Stmt;
    void generate_BlockItem_IR();
    void blockItem_Error_Handler();
};

class Block {
public:
    //0 : there is no blockItems;
    //1 : there is some blockItems;
    int flag;
    int lineCount;
    std::vector<BlockItem *> BlockItems;
    void generate_Block_IR();
    void block_Error_Handler();
};

class FuncDef {
public:
    //whether it's a true funcdef
    //-1 : no
    //1 : yes
    int flag;
    fType funcType;
    Ident * this_Ident;
    //0 : no params
    //1 : is params
    int isParm;
    FuncFParams * fParams;
    Block * this_Block;
    void generate_FuncDef_IR();
    void funcDef_Error_Handler();
};

class MainFuncDef {
public:
    Block * this_Block;
    void generate_MainFuncDef_IR();
    void mainFuncDef_Error_Handler();
};

class CompUnit {
public:
    std::vector<Decl *> Decls;
    std::vector<FuncDef *> FuncDefs;
    MainFuncDef * this_MainFuncDef;
    void generate_CompUnit_IR();
    void error_Analyzer();
};

//struct unaryOp {
//    char op;
//};
//
//struct ident {
//    std::string Ident;
//    int lineCount;
//};
//
//struct primaryExp {
//    //which is the primaryExp
//    //0 : (Exp)
//    //1 : LVal
//    //2 : Number
//    int indicator;
//    exp * this_exp;
//    lVAl * this_LVal;
//    int number;
//};
//
//struct unaryExp {
//    //0 : primaryExp;
//    //1 : Ident([FuncRParams])
//    //2 : UnaryOp UnaryExp
//    int flag;
//    primaryExp * PriExp;
//    ident * Ident;
//    //0 : no params
//    //1 : is params
//    int is_RParams = 0;
//    funcRParams * funcR;
//    unaryOp * UnaryOp;
//    unaryExp * UnaryExp;
//};
//
//struct mulExp {
//    std::vector<unaryExp *> UnaryExps;
//    std::vector<char> symbols;
//};
//
//struct addExp {
//    std::vector<mulExp *> MulExps;
//    std::vector<char> symbols;
//};
//
//struct exp {
//    addExp * AddExp;
//};
//
//struct lVAl {
//    int dem;
//    std::string ident;
//    std::vector<exp *> exps;
//};
//
//struct relExp {
//    std::vector<addExp *> AddExps;
//    std::vector<symType> Syms;
//};
//
//struct eqExp {
//    std::vector<relExp *> RelExps;
//    std::vector<symType> Syms;
//};
//
//struct lAndExp {
//    std::vector<eqExp *> EqExps;
//};
//
//struct lOrExp {
//    std::vector<lAndExp *> LAndExps;
//};
//
//struct cond {
//    lOrExp * LOrExp;
//};
//
//struct constExp {
//    //the ident that used must be constant
//    addExp * AddExp;
//};
//
//struct bType {
//    std::string type = "int";
//};
//
//struct constInitVal {
//    //0 : means it's a constExp
//    //1 : means it's constInits
//    int flag;
//    constExp * ConstExp;
//    std::vector<constInitVal *> constInits;
//};
//
//struct constDef {
//    ident * Ident;
//    int dem = 0;
//    std::vector<constExp *> constExps;
//    constInitVal * initValue;
//};
//
//struct constDecl {
//    bType * BType;
//    std::vector<constDef *> ConstDefs;
//};
//
//struct initVal {
//    //0 : means it's an exp
//    //1 : means it's Inits
//    int flag;
//    exp * Exp;
//    std::vector<initVal *> Inits;
//};
//
//struct varDef {
//    ident * Ident;
//    int dem;
//    std::vector<constExp *> ConstExps;
//    //if 1, there's init, else no
//    int flag_Init = 0;
//    initVal * initValue;
//};
//
//struct varDecl {
//    bType * BType;
//    std::vector<varDef *> VarDefs;
//};
//
//struct decl {
//    //if 0, means constDecl, if 1, means varDecl
//    int flag = -1;
//    constDecl * ConstDecl;
//    varDecl * VarDecl;
//};
//
//struct funcType {
//    fType type;
//};
//
//struct funcFParam {
//    bType * BType;
//    ident * Ident;
//    int dem;
//    std::vector<constExp *> ConstExps;
//};
//
//struct funcFParams {
//    std::vector<funcFParam *> funcFs;
//};
//
//struct funcRParams {
//    int count;
//    std::vector<exp *> Exps;
//};
//
//struct stmt {
//    //0 : LVal = Exp;
//    //1 : [Exp];
//    //2 : Block
//    //3 : if (Cond) Stmt [else Stmt]
//    //4 : while (Cond) Stmt
//    //5 : break;
//    //6 : return [Exp];
//    //7 : LVal = getint();
//    //8 : printf(Formatstring{,Exp});
//    //9 : continue;
//    int flag;
//    //if there is an if, whether an else
//    //1 means there is, and 0 means no
//    int is_Else;
//    //0 : no return exp;
//    //1 : is return exp;
//    int is_Return_Exp = 0;
//    lVAl * LVal;
//    block * Block;
//    exp * EqualExp;
//    cond * Cond;
//    stmt * if_Stmt;
//    stmt * else_Stmt;
//    stmt * while_Stmt;
//    exp * return_Exp;
//    std::vector<exp *> Exps;
//    std::vector<stmt *> Stmts;
//    std::string formatString;
//};
//
//struct blockItem {
//    //if 0, means it's a Decl, if 1, means it's a Stmt
//    int flag;
//    struct decl * Decl;
//    struct stmt * Stmt;
//};
//
//struct block {
//    //0 : there is no blockItems;
//    //1 : there is some blockItems;
//    int flag;
//    std::vector<struct blockItem *> blockItems;
//};
//
//struct funcDef {
//    struct funcType * FuncType;
//    struct ident * Ident;
//    //0 : no params
//    //1 : is params
//    int isParm;
//    struct funcFParams * fParams;
//    struct block * Block;
//};
//
//struct mainFuncDef {
//    struct block * Block;
//};
//
//struct compUnit {
//    std::vector<struct decl *> Decls;
//    std::vector<struct funcDef *> FuncDefs;
//    struct mainFuncDef * MainFuncDef;
//};

#endif //GRAMMAR_ANA_ASTREE_H
