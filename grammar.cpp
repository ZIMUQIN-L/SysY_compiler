//
// Created by ZIMUQIN on 2021/10/20.
//
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <malloc.h>
#include "def.h"
#include "grammar.h"
#include "astree.h"
#include "error.h"

using namespace std;
extern vector<couple> couples;
extern ofstream outfile;
int place = 0;
couple now_couple;
couple pre_couple;
couple pre_pre_couple;

void getNextTokin() {
    static int size = couples.size();
    if (size == place) {
        outfile << now_couple.type << " " << now_couple.content << endl;
    }
    if (place < size) {
        if (place != 0) {
            outfile << now_couple.type << " " << now_couple.content << endl;
        }
        now_couple = couples[place];
        if (place + 1 < size) {
            pre_couple = couples[place + 1];
        }
        if (place + 2 < size) {
            pre_pre_couple = couples[place + 2];
        }
        place ++;
    }
}

int getEqualThere() {
    static int size = couples.size();
    int pos = place;
    while (pos < size) {
        if (strcmp(couples[pos].content, ";") == 0) {
            return 0;
        }
        if (strcmp(couples[pos].content, "=") == 0) {
            return 1;
        }
        if (strcmp(couples[pos].type, "IDENFR") == 0 &&
            strcmp(couples[pos - 1].content, "*") != 0 &&
            strcmp(couples[pos - 1].content, "/") != 0 &&
            strcmp(couples[pos - 1].content, "+") != 0 &&
            strcmp(couples[pos - 1].content, "-") != 0 &&
            strcmp(couples[pos - 1].content, "%") != 0 &&
            strcmp(couples[pos - 1].content, "[") != 0 &&
            strcmp(couples[pos - 1].content, "(") != 0) {
            return 0;
        }
        pos ++;
    }
    return 0;
}

BType * grammar_BType_Analyzer() {
    if (strcmp(now_couple.content, "int") != 0) {
        printf("error!\n");
    }
    BType * bType;
    bType = (BType *) malloc(sizeof(BType));
    memset(bType, 0, sizeof(BType));
    //outfile << "<BType>" << endl;
    getNextTokin();
    return bType;
}

Ident * grammar_Ident_Analyzer() {
    if (strcmp(now_couple.type, "IDENFR") != 0) {
        printf("error!\n");
    }
    Ident * ident;
    ident = (Ident *) malloc(sizeof(Ident));
    memset(ident, 0, sizeof(Ident));
    ident->ident = now_couple.content;
    ident->lineCount = now_couple.lineCount;
    //outfile << "<Ident>" << endl;
    getNextTokin();
    return ident;
}

InitVal * grammar_InitVal_Analyzer() {
    InitVal * Init;
    Init = (InitVal *) malloc(sizeof(InitVal));
    memset(Init, 0, sizeof(InitVal));
    int res = 1;
    if (strcmp(now_couple.content, "{") == 0) {
        Init->flag = 1;
        getNextTokin();
        if (strcmp(now_couple.content, "}") != 0) {
            Init->Inits.push_back(grammar_InitVal_Analyzer());
//            if (res != 1) {
//                printf("error!\n");
//                return -1;
//            }
            while (strcmp(now_couple.content, ",") == 0) {
                getNextTokin();
                Init->Inits.push_back(grammar_InitVal_Analyzer());
//                if (res != 1) {
//                    printf("error!\n");
//                    return -1;
//                }
            }
            if (strcmp(now_couple.content, "}") != 0) {
                printf("error!\n");
                //return -1;
            }
            getNextTokin();
            outfile << "<InitVal>" << endl;
            return Init;
        }
        else {
            getNextTokin();
            outfile << "<InitVal>" << endl;
            //return 1;
            return Init;
        }
    }
    else {
        Init->flag = 0;
        Init->this_Exp = grammar_Exp_Analyzer();
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
        outfile << "<InitVal>" << endl;
        //return 1;
        return Init;
    }
}

VarDef * grammar_VarDef_Analyzer() {
    VarDef * varDef;
    varDef = (VarDef *) malloc(sizeof(VarDef));
    memset(varDef, 0, sizeof(VarDef));
    int res = 1;
    varDef->this_Ident = grammar_Ident_Analyzer();
//    if (res != 1) {
//        printf("error!\n");
//        return -1;
//    }
    int dem = 0;
    while (strcmp(now_couple.content, "[") == 0) {
        getNextTokin();
        varDef->ConstExps.push_back(grammar_ConstExp_Analyzer());
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
        if (strcmp(now_couple.content, "]") != 0) {
            insertError(couples[place - 2].lineCount, "k");
            //return -1;
        }
        else {
            getNextTokin();
        }
        dem ++;
    }
    varDef->dem = dem;
    if (strcmp(now_couple.content, "=") == 0) {
        varDef->flag_Init = 1;
        getNextTokin();
        varDef->this_InitVal = grammar_InitVal_Analyzer();
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
        outfile << "<VarDef>" << endl;
        return varDef;
    }
    else {
        varDef->flag_Init = 0;
        outfile << "<VarDef>" << endl;
        //return 1;
        return varDef;
    }
}

VarDecl * grammar_VarDecl_Analyzer() {
    VarDecl * varDecl;
    varDecl = (VarDecl *) malloc(sizeof(VarDecl));
    memset(varDecl, 0, sizeof(VarDecl));
    varDecl->this_BType = grammar_BType_Analyzer();
    varDecl->VarDefs.push_back(grammar_VarDef_Analyzer());
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
    while (strcmp(now_couple.content, ",") == 0) {
        getNextTokin();
        varDecl->VarDefs.push_back(grammar_VarDef_Analyzer());
//            if (res != 1) {
//                printf("error!\n");
//                return -1;
//            }
    }
    if (strcmp(now_couple.content, ";") != 0) {
        insertError(couples[place - 2].lineCount, "i");
            //return -1;
    }
    else {
        getNextTokin();
    }
    outfile << "<VarDecl>" << endl;
    return varDecl;
}

ConstInitVal * grammar_ConstInitVal_Analyzer() {
    ConstInitVal * ConstInit;
    ConstInit = (ConstInitVal *) malloc(sizeof(ConstInitVal));
    memset(ConstInit, 0, sizeof(ConstInitVal));
    if (strcmp(now_couple.content, "{") == 0) {
        ConstInit->flag = 1;
        getNextTokin();
        int res = 1;
        if (strcmp(now_couple.content, "}") != 0) {
            ConstInit->ConstInits.push_back(grammar_ConstInitVal_Analyzer());
//            if (res != 1) {
//                printf("error!\n");
//                return -1;
//            }
            while (strcmp(now_couple.content, ",") == 0) {
                getNextTokin();
                ConstInit->ConstInits.push_back(grammar_ConstInitVal_Analyzer());
//                if (res != 1) {
//                    printf("error!\n");
//                    return -1;
//                }
            }
            if (strcmp(now_couple.content, "}") != 0) {
                printf("error!\n");
                //return -1;
            }
            getNextTokin();
            outfile << "<ConstInitVal>" << endl;
            //return 1;
            return ConstInit;
        }
        else {
            getNextTokin();
            outfile << "<ConstInitVal>" << endl;
            //return 1;
            return ConstInit;
        }
    }
    else {
        int res = 1;
        ConstInit->flag = 0;
        ConstInit->this_ConstExp = grammar_ConstExp_Analyzer();
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
        outfile << "<ConstInitVal>" << endl;
        //return 1;
        return ConstInit;
    }
}

ConstExp * grammar_ConstExp_Analyzer() {
    ConstExp * constExp;
    constExp = (ConstExp *) malloc(sizeof(ConstExp));
    memset(constExp, 0, sizeof(ConstExp));
    constExp->this_AddExp = grammar_AddExp_Analyzer();
//    if (res != 1) {
//        printf("error!\n");
//        return -1;
//    }
    outfile << "<ConstExp>" << endl;
    //return 1;
    return constExp;
}

ConstDef * grammar_ConstDef_Analyzer() {
    ConstDef * constDef;
    constDef = (ConstDef *)malloc(sizeof(ConstDef));
    memset(constDef, 0, sizeof(ConstDef));
    constDef->this_Ident = grammar_Ident_Analyzer();
//    if (res != 1) {
//        printf("error!\n");
//        return -1;
//    }
    int dem = 0;
    while (strcmp(now_couple.content, "[") == 0) {
        getNextTokin();
        constDef->ConstExps.push_back(grammar_ConstExp_Analyzer());
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
        if (strcmp(now_couple.content, "]") != 0) {
            insertError(couples[place - 2].lineCount, "k");
            //printf("error!\n");
            //return -1;
        }
        else {
            getNextTokin();
        }
        dem ++;
    }
    constDef->dem = dem;
    if (strcmp(now_couple.content, "=") != 0) {
        printf("error!\n");
        //return -1;
    }
    getNextTokin();
    constDef->InitVal = grammar_ConstInitVal_Analyzer();
//    if (res != 1) {
//        printf("error!\n");
//        return -1;
//    }
    outfile << "<ConstDef>" << endl;
    //return 1;
    return constDef;
}

ConstDecl * grammar_ConstDecl_Analyzer() {
    ConstDecl * constDecl;
    constDecl = (ConstDecl *) malloc(sizeof(ConstDecl));
    memset(constDecl, 0, sizeof(ConstDecl));
    if (strcmp(now_couple.content, "const") != 0) {
        printf("error!\n");
        //return -1;
    }
    getNextTokin();
    constDecl->this_BType = grammar_BType_Analyzer();
//    if (res != 1) {
//        printf("error!\n");
//        return -1;
//    }
    constDecl->ConstDefs.push_back(grammar_ConstDef_Analyzer());
//    if (res != 1) {
//        printf("error!\n");
//        return -1;
//    }
    while (strcmp(now_couple.content, ",") == 0) {
        getNextTokin();
        constDecl->ConstDefs.push_back(grammar_ConstDef_Analyzer());
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
    }
    if (strcmp(now_couple.content, ";") != 0) {
        insertError(couples[place - 2].lineCount, "i");
        //printf("error!\n");
        //return -1;
    }
    else {
        getNextTokin();
    }
    outfile << "<ConstDecl>" << endl;
    //return 1;
    return constDecl;
}

Decl * grammar_Decl_Analyzer() {
    Decl * decl;
    decl = (Decl *) malloc(sizeof(Decl));
    memset(decl, 0, sizeof(Decl));
    if (strcmp("int", now_couple.content) == 0 &&
        strcmp(pre_couple.type, "IDENFR") == 0 &&
        (strcmp(pre_pre_couple.content, "(") != 0)) {
        decl->flag = 1;
        decl->this_VarDecl = grammar_VarDecl_Analyzer();
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
        //outfile << "<Decl>" <<endl;
        return decl;
    }
    else if (strcmp("const", now_couple.content) == 0) {
        decl->flag = 0;
        decl->this_ConstDecl = grammar_ConstDecl_Analyzer();
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
        //outfile << "<Decl>" << endl;
        return decl;
    }
    else {
        decl->flag = -1;
        return decl;
    }
}

FuncRParams * grammar_FuncRParams_Analyzer() {
    FuncRParams * funcRParams;
    funcRParams = (FuncRParams *) malloc(sizeof(FuncRParams));
    memset(funcRParams, 0, sizeof(FuncRParams));
    funcRParams->Exps.push_back(grammar_Exp_Analyzer());
//    if (res != 1) {
//        printf("error!\n");
//        return -1;
//    }
    while (strcmp(now_couple.content, ",") == 0) {
        getNextTokin();
        funcRParams->Exps.push_back(grammar_Exp_Analyzer());
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
    }
    funcRParams->count = funcRParams->Exps.size();
    outfile << "<FuncRParams>" << endl;
    //return 1;
    return funcRParams;
}

LVal * grammar_LVal_Analyzer() {
    LVal * lVal;
    lVal = (LVal *) malloc(sizeof(LVal));
    memset(lVal, 0, sizeof(LVal));
    lVal->this_Ident = grammar_Ident_Analyzer();
    int dem = 0;
    while (strcmp(now_couple.content, "[") == 0) {
        getNextTokin();
        lVal->Exps.push_back(grammar_Exp_Analyzer());
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
        if (strcmp(now_couple.content, "]") != 0) {
            insertError(couples[place - 2].lineCount, "k");
            //printf("error!\n");
            //return -1;
        }
        else {
            getNextTokin();
        }
        dem ++;
    }
    lVal->dem = dem;
    outfile << "<LVal>" << endl;
    //return 1;
    return lVal;
}

PrimaryExp * grammar_PrimaryExp_Analyzer() {
    PrimaryExp * primaryExp;
    primaryExp = (PrimaryExp *) malloc(sizeof(PrimaryExp));
    memset(primaryExp, 0, sizeof(PrimaryExp));
    if (strcmp(now_couple.content, "(") == 0) {
        primaryExp->indicator = 0;
        getNextTokin();
        primaryExp->this_exp = grammar_Exp_Analyzer();
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
        if (strcmp(now_couple.content, ")") != 0) {
            insertError(couples[place - 2].lineCount, "j");
            //printf("error!\n");
            //return -1;
        }
        else {
            getNextTokin();
        }
        outfile << "<PrimaryExp>" << endl;
        //return 1;
        return primaryExp;
    }
    else if (strcmp(now_couple.type, "INTCON") == 0) {
        primaryExp->indicator = 2;
        primaryExp->number = atoi(now_couple.content);
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
        getNextTokin();
        outfile << "<Number>" << endl;
        outfile << "<PrimaryExp>" << endl;
        //return 1;
        return primaryExp;
    }
    else {
        primaryExp->indicator = 1;
        primaryExp->this_lVal = grammar_LVal_Analyzer();
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
        outfile << "<PrimaryExp>" << endl;
        //return 1;
        return primaryExp;
    }
}

FuncFParam * grammar_FuncFParam_Analyzer() {
    FuncFParam * funcFParam;
    funcFParam = (FuncFParam *) malloc(sizeof(FuncFParam));
    memset(funcFParam, 0, sizeof(FuncFParam));
    funcFParam->this_BType = grammar_BType_Analyzer();
//    if (res != 1) {
//        printf("error!\n");
//        return -1;
//    }
    funcFParam->this_Ident = grammar_Ident_Analyzer();
//    if (res != 1) {
//        printf("error!\n");
//        return -1;
//    }
    int dem = 0;
    if (strcmp(now_couple.content, "[") == 0) {
        getNextTokin();
        if (strcmp(now_couple.content, "]") != 0) {
            insertError(couples[place - 2].lineCount, "k");
            //printf("error!\n");
            //return -1;
        }
        else {
            getNextTokin();
        }
        dem ++;
        while (strcmp(now_couple.content, "[") == 0) {
            getNextTokin();
            funcFParam->ConstExps.push_back(grammar_ConstExp_Analyzer());
//            if (res != 1) {
//                printf("error!\n");
//                return -1;
//            }
            if (strcmp(now_couple.content, "]") != 0) {
                insertError(couples[place - 2].lineCount, "k");
                //printf("error!\n");
                //return -1;
            }
            else {
                getNextTokin();
            }
            dem ++;
        }
    }
    funcFParam->dem = dem;
    outfile << "<FuncFParam>" << endl;
    return funcFParam;
}

FuncFParams * grammar_FuncFParams_Analyzer() {
    FuncFParams * funcFParams;
    funcFParams = (FuncFParams *) malloc(sizeof(FuncFParams));
    memset(funcFParams, 0, sizeof(FuncFParams));
    funcFParams->funcFs.push_back(grammar_FuncFParam_Analyzer());
//    if (res != 1) {
//        printf("error!\n");
//        return -1;
//    }
    while (strcmp(now_couple.content, ",") == 0) {
        getNextTokin();
        funcFParams->funcFs.push_back(grammar_FuncFParam_Analyzer());
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
    }
    outfile << "<FuncFParams>" << endl;
    //return 1;
    return funcFParams;
}

UnaryExp * grammar_UnaryExp_Analyzer() {
    UnaryExp * unaryExp;
    unaryExp = (UnaryExp *) malloc(sizeof(UnaryExp));
    memset(unaryExp, 0, sizeof(UnaryExp));
    if (strcmp(now_couple.type, "IDENFR") == 0 &&
        strcmp(pre_couple.content, "(") == 0) {
        unaryExp->flag = 1;
        unaryExp->this_Ident = grammar_Ident_Analyzer();
        getNextTokin();
        int res = 0;
        unaryExp->is_RParams = 0;
        if (strcmp(now_couple.content, ")") != 0 &&
            strcmp(now_couple.content, "{") != 0 &&
            strcmp(now_couple.content, "]") != 0) {
            unaryExp->is_RParams = 1;
            unaryExp->this_FuncR = grammar_FuncRParams_Analyzer();
//            if (res != 1) {
//                printf("error!\n");
//                return -1;
//            }
        }
        if (strcmp(now_couple.content, ")") != 0) {
            insertError(couples[place - 2].lineCount, "j");
            //printf("error!\n");
            //return -1;
        }
        else {
            getNextTokin();
        }
        outfile << "<UnaryExp>" << endl;
        //return 1;
        return unaryExp;
    }
    else if (strcmp(now_couple.content, "+") == 0 ||
             strcmp(now_couple.content, "!") == 0 ||
             strcmp(now_couple.content, "-") == 0) {
        unaryExp->flag = 2;
        if (strcmp(now_couple.content, "+") == 0) {
            unaryExp->this_UnaryOp = POSITIVE;
        }
        else if (strcmp(now_couple.content, "-") == 0) {
            unaryExp->this_UnaryOp = NEGATIVE;
        }
        else {
            unaryExp->this_UnaryOp = NOT;
        }
        getNextTokin();
        outfile << "<UnaryOp>" << endl;
        unaryExp->this_UnaryExp = grammar_UnaryExp_Analyzer();
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
        outfile << "<UnaryExp>" << endl;
        //return 1;
        return unaryExp;
    }
    else {
        unaryExp->flag = 0;
        unaryExp->PriExp = grammar_PrimaryExp_Analyzer();
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
        outfile << "<UnaryExp>" << endl;
        //return 1;
        return unaryExp;
    }
}

RelExp * grammar_RelExp_Analyzer() {
    RelExp * relExp;
    relExp = (RelExp *) malloc(sizeof(RelExp));
    memset(relExp, 0, sizeof(RelExp));
    int res = 1;
    relExp->AddExps.push_back(grammar_AddExp_Analyzer());
//    if (res != 1) {
//        printf("error!\n");
//        return -1;
//    }
    while ((strcmp(now_couple.content, ">") == 0) ||
           (strcmp(now_couple.content, "<") == 0) ||
           (strcmp(now_couple.content, ">=") == 0) ||
           (strcmp(now_couple.content, "<=") == 0)) {
        if (strcmp(now_couple.content, ">") == 0) {
            relExp->Syms.push_back(GT);
        }
        else if (strcmp(now_couple.content, "<") == 0) {
            relExp->Syms.push_back(LT);
        }
        else if (strcmp(now_couple.content, ">=") == 0) {
            relExp->Syms.push_back(GET);
        }
        else {
            relExp->Syms.push_back(LET);
        }
        outfile << "<RelExp>" << endl;
        getNextTokin();
        relExp->AddExps.push_back(grammar_AddExp_Analyzer());
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
    }
    outfile << "<RelExp>" << endl;
    //return 1;
    return relExp;
}

EqExp * grammar_EqExp_Analyzer() {
    EqExp * eqExp;
    eqExp = (EqExp *) malloc(sizeof(EqExp));
    memset(eqExp, 0, sizeof(EqExp));
    int res = 1;
    eqExp->RelExps.push_back(grammar_RelExp_Analyzer());
//    if (res != 1) {
//        printf("error!\n");
//        return -1;
//    }
    while ((strcmp(now_couple.content, "==") == 0) ||
           (strcmp(now_couple.content, "!=") == 0)) {
        if (strcmp(now_couple.content, "==") == 0) {
            eqExp->Syms.push_back(EQU);
        }
        else {
            eqExp->Syms.push_back(NEQ);
        }
        outfile << "<EqExp>" << endl;
        getNextTokin();
        eqExp->RelExps.push_back(grammar_RelExp_Analyzer());
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
    }
    outfile << "<EqExp>" << endl;
    //return 1;
    return eqExp;
}

LAndExp * grammar_LAndExp_Analyzer() {
    LAndExp * lAndExp;
    lAndExp = (LAndExp *) malloc(sizeof(LAndExp));
    memset(lAndExp, 0, sizeof(LAndExp));
    int res = 1;
    lAndExp->EqExps.push_back(grammar_EqExp_Analyzer());
//    if (res != 1) {
//        printf("error!\n");
//        return -1;
//    }
    while (strcmp(now_couple.content, "&&") == 0) {
        outfile << "<LAndExp>" << endl;
        getNextTokin();
        lAndExp->EqExps.push_back(grammar_EqExp_Analyzer());
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
    }
    outfile << "<LAndExp>" << endl;
    //return 1;
    return lAndExp;
}

LOrExp * grammar_LOrExp_Analyzer() {
    LOrExp * lOrExp;
    lOrExp = (LOrExp *) malloc(sizeof(LOrExp));
    memset(lOrExp, 0, sizeof(LOrExp));
    int res = 1;
    lOrExp->LAndExps.push_back(grammar_LAndExp_Analyzer());
//    if (res != 1) {
//        printf("error!\n");
//        return -1;
//    }
    while (strcmp(now_couple.content, "||") == 0) {
        outfile << "<LOrExp>" << endl;
        getNextTokin();
        lOrExp->LAndExps.push_back(grammar_LAndExp_Analyzer());
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
    }
    outfile << "<LOrExp>" << endl;
    //return 1;
    return lOrExp;
}

Cond * grammar_Cond_Analyzer() {
    Cond * cond;
    cond = (Cond *) malloc(sizeof(Cond));
    memset(cond, 0, sizeof(Cond));
    cond->this_LOrExp = grammar_LOrExp_Analyzer();
//    if (res != 1) {
//        printf("error!\n");
//        return -1;
//    }
    outfile << "<Cond>" << endl;
    //return 1;
    return cond;
}

MulExp * grammar_MulExp_Analyzer() {
    MulExp * mulExp;
    mulExp = (MulExp *) malloc(sizeof(MulExp));
    memset(mulExp, 0, sizeof(MulExp));
    mulExp->UnaryExps.push_back(grammar_UnaryExp_Analyzer());
//    if (res != 1) {
//        printf("error!\n");
//        return -1;
//    }
    while (strcmp(now_couple.content, "*") == 0 ||
           strcmp(now_couple.content, "/") == 0 ||
           strcmp(now_couple.content, "%") == 0 ) {
        if (strcmp(now_couple.content, "*") == 0) {
            mulExp->Syms.push_back(MULT);
        }
        else if (strcmp(now_couple.content, "/") == 0) {
            mulExp->Syms.push_back(DIV);
        }
        else {
            mulExp->Syms.push_back(MOD);
        }
        outfile << "<MulExp>" << endl;
        getNextTokin();
        mulExp->UnaryExps.push_back(grammar_UnaryExp_Analyzer());
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
    }
    outfile << "<MulExp>" << endl;
    //return 1;
    return mulExp;
}

AddExp * grammar_AddExp_Analyzer() {
    AddExp * addExp;
    addExp = (AddExp *) malloc(sizeof(AddExp));
    memset(addExp, 0, sizeof(AddExp));
    addExp->MulExps.push_back(grammar_MulExp_Analyzer());
//    if (res != 1) {
//        printf("error!\n");
//        return -1;
//    }
    while (strcmp(now_couple.content, "+") == 0 ||
           strcmp(now_couple.content, "-") == 0) {
        if (strcmp(now_couple.content, "+") == 0) {
            addExp->Syms.push_back(ADD);
        }
        else {
            addExp->Syms.push_back(SUB);
        }
        outfile << "<AddExp>" << endl;
        getNextTokin();
        addExp->MulExps.push_back(grammar_MulExp_Analyzer());
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
    }
    outfile << "<AddExp>" << endl;
    //return 1;
    return addExp;
}

Exp * grammar_Exp_Analyzer() {
    Exp * exp;
    exp = (Exp *) malloc(sizeof(Exp));
    memset(exp, 0, sizeof(Exp));
    exp->this_AddExp = grammar_AddExp_Analyzer();
//    if (res != 1) {
//        printf("error!\n");
//        return -1;
//    }
    outfile << "<Exp>" << endl;
    //return 1;
    return exp;
}

Stmt * grammar_Stmt_Analyzer() {
    Stmt * stmt;
    stmt = (Stmt *) malloc(sizeof(Stmt));
    memset(stmt, 0, sizeof(Stmt));
    stmt->lineCount =  now_couple.lineCount;
    if (strcmp(now_couple.type, "IDENFR") == 0 &&
        (strcmp(pre_couple.content, "=") == 0 ||
         strcmp(pre_couple.content, "[") == 0) &&
        (getEqualThere() == 1 ||
         strcmp(pre_couple.content, "=") == 0)) {
        //todo
        stmt->this_LVal = grammar_LVal_Analyzer();
//        if (res_1 != 1) {
//            printf("error!\n");
//            return -1;
//        }
        if (strcmp(now_couple.content, "=") != 0) {
            printf("error!\n");
            //return -1;
        }
        getNextTokin();
        if (strcmp(now_couple.content, "getint") == 0) {
            stmt->flag = 7;
            getNextTokin();
            if (strcmp(now_couple.content, "(") != 0) {
                printf("error!\n");
                //return -1;
            }
            getNextTokin();
            if (strcmp(now_couple.content, ")") != 0) {
                insertError(couples[place - 2].lineCount, "j");
                //printf("error!\n");
                //return -1;
            }
            else {
                getNextTokin();
            }
            if (strcmp(now_couple.content, ";") != 0) {
                insertError(couples[place - 2].lineCount, "i");
                //printf("error!\n");
                //return -1;
            }
            else {
                getNextTokin();
            }
            outfile << "<Stmt>" << endl;
            //return 1;
            return stmt;
        }
        else {
            stmt->flag = 0;
            stmt->assignExp = grammar_Exp_Analyzer();
//            if (res != 1) {
//                printf("error!\n");
//                return -1;
//            }
            if (strcmp(now_couple.content, ";") != 0) {
                insertError(couples[place - 2].lineCount, "i");
                //printf("error!\n");
                //return -1;
            }
            else {
                getNextTokin();
            }
            outfile << "<Stmt>" << endl;
            //return 1;
            return stmt;
        }
    }
    else if (strcmp(now_couple.content, "if") == 0) {
        stmt->flag = 3;
        getNextTokin();
        if (strcmp(now_couple.content, "(") != 0) {
            printf("error!\n");
            //return -1;
        }
        getNextTokin();
        stmt->this_Cond = grammar_Cond_Analyzer();
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
        if (strcmp(now_couple.content, ")") != 0) {
            insertError(couples[place - 2].lineCount, "j");
            //printf("error!\n");
            //return -1;
        }
        else {
            getNextTokin();
        }
        stmt->if_Stmt = grammar_Stmt_Analyzer();
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
        stmt->is_Else = 0;
        if (strcmp(now_couple.content, "else") == 0) {
            stmt->is_Else = 1;
            getNextTokin();
            stmt->else_Stmt = grammar_Stmt_Analyzer();
        }
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
        outfile << "<Stmt>" << endl;
        //return 1;
        return stmt;
    }
    else if (strcmp(now_couple.content, "while") == 0) {
        stmt->flag = 4;
        getNextTokin();
        if (strcmp(now_couple.content, "(") != 0) {
            printf("error!\n");
            //return -1;
        }
        getNextTokin();
        stmt->this_Cond = grammar_Cond_Analyzer();
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
        if (strcmp(now_couple.content, ")") != 0) {
            insertError(couples[place - 2].lineCount, "j");
            //printf("error!\n");
            //return -1;
        }
        else {
            getNextTokin();
        }
        stmt->while_Stmt = grammar_Stmt_Analyzer();
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
        outfile << "<Stmt>" << endl;
        //return 1;
        return stmt;
    }
    else if (strcmp(now_couple.content, "break") == 0) {
        stmt->flag = 5;
        getNextTokin();
        if (strcmp(now_couple.content, ";") != 0) {
            insertError(couples[place - 2].lineCount, "i");
            //printf("error!\n");
            //return -1;
        }
        else {
            getNextTokin();
        }
        outfile << "<Stmt>" << endl;
        //return 1;
        return stmt;
    }
    else if (strcmp(now_couple.content, "continue") == 0) {
        stmt->flag = 9;
        getNextTokin();
        if (strcmp(now_couple.content, ";") != 0) {
            insertError(couples[place - 2].lineCount, "i");
            //printf("error!\n");
            //return -1;
        }
        else {
            getNextTokin();
        }
        outfile << "<Stmt>" << endl;
        //return 1;
        return stmt;
    }
    else if (strcmp(now_couple.content, "return") == 0) {
        stmt->flag = 6;
        getNextTokin();
        int res = 1;
        stmt->is_Return_Exp = 0;
        if (strcmp(now_couple.content, ";") != 0) {
            if (strcmp(now_couple.content, "+") == 0 ||
                    strcmp(now_couple.content, "-") == 0 ||
                    strcmp(now_couple.content, "!") == 0 ||
                    strcmp(now_couple.content, "(") == 0 ||
                    strcmp(now_couple.type, "IDENFR") == 0 ||
                    strcmp(now_couple.type, "INTCON") == 0) {
                stmt->is_Return_Exp = 1;
                stmt->returnExp =  grammar_Exp_Analyzer();
//            if (res == -1) {
//                printf("error!\n");
//                return -1;
//            }
            }
            else {
                insertError(couples[place - 2].lineCount, "i");
            }
        }
        if (strcmp(now_couple.content, ";") != 0) {
            insertError(couples[place - 2].lineCount, "i");
            //printf("error!\n");
            //return -1;
        }
        else {
            getNextTokin();
        }
        outfile << "<Stmt>" << endl;
        //return 1;
        return stmt;
    }
    else if (strcmp(now_couple.content, "printf") == 0) {
        stmt->flag = 8;
        getNextTokin();
        int nowLine = now_couple.lineCount;
        if (strcmp(now_couple.content, "(") != 0) {
            printf("error!\n");
            //return -1;
        }
        getNextTokin();
        if (strcmp(now_couple.type, "STRCON") != 0) {
            printf("error!\n");
            //return -1;
        }
        stmt->formatString = now_couple.content;
        int strLine = now_couple.lineCount;
        if (Error_A_Analyzer(stmt->formatString) != 1) {
            insertError(strLine, "a");
        }
        getNextTokin();
        int res = 1;
        while (strcmp(now_couple.content, ",") == 0) {
            getNextTokin();
            stmt->Exps.push_back(grammar_Exp_Analyzer());
//            if (res != 1) {
//                printf("error!\n");
//                return -1;
//            }
        }
        if (!Error_L_Analyzer(stmt->formatString, stmt->Exps.size())) {
            insertError(nowLine, "l");
        }
        if (strcmp(now_couple.content, ")") != 0) {
            insertError(couples[place - 2].lineCount, "j");
            //printf("error!\n");
            //return -1;
        }
        else {
            getNextTokin();
        }
        if (strcmp(now_couple.content, ";") != 0) {
            insertError(couples[place - 2].lineCount, "i");
            //printf("error!\n");
            //return -1;
        }
        else {
            getNextTokin();
        }
        outfile << "<Stmt>" << endl;
        //return 1;
        return stmt;
    }
    else if (strcmp(now_couple.content, "{") == 0) {
        stmt->flag = 2;
        stmt->this_Block = grammar_Block_Analyzer();
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
        outfile << "<Stmt>" << endl;
        return stmt;
    }
    else {
        stmt->flag = 1;
        if (strcmp(now_couple.content, ";") != 0) {
            if (strcmp(now_couple.content, "+") == 0 ||
                    strcmp(now_couple.content, "-") == 0 ||
                    strcmp(now_couple.content, "!") == 0 ||
                    strcmp(now_couple.content, "(") == 0 ||
                    strcmp(now_couple.type, "IDENFR") == 0 ||
                    strcmp(now_couple.type, "INTCON") == 0) {
                stmt->oneExp= grammar_Exp_Analyzer();
//              if (res != 1) {
//                  printf("error!\n");
//                  return -1;
//              }
            }
            else {
                insertError(couples[place - 2].lineCount, "i");
            }
        }
        else {
            stmt->oneExp = nullptr;
        }
        if (strcmp(now_couple.content, ";") != 0) {
            insertError(couples[place - 2].lineCount, "i");
            //printf("error!\n");
            //return -1;
        }
        else {
            getNextTokin();
        }
        outfile << "<Stmt>" << endl;
        //return 1;
        return stmt;
    }
}

BlockItem * grammar_BlockItem_Analyzer(){
    BlockItem * blockItem;
    blockItem = (BlockItem *) malloc(sizeof(BlockItem));
    memset(blockItem, 0, sizeof(BlockItem));
    int res;
    if (strcmp(now_couple.content, "const") == 0 ||
        strcmp(now_couple.content, "int") == 0) {
        blockItem->flag = 0;
        blockItem->this_Decl = grammar_Decl_Analyzer();
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
        //outfile << "<BlockItem>" << endl;
        //return res;
        return blockItem;
    }
    else {
        blockItem->flag = 1;
        blockItem->this_Stmt = grammar_Stmt_Analyzer();
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
        //outfile << "<BlockItem>" << endl;
        //return res;
        return blockItem;
    }
}

Block * grammar_Block_Analyzer() {
    Block * block;
    block = (Block *) malloc(sizeof(Block));
    memset(block, 0, sizeof(Block));
    if (strcmp(now_couple.content, "{") != 0) {
        printf("error!\n");
//        return -1;
    }
    int res = 1;
    getNextTokin();
    block->flag = 1;
    if (strcmp(now_couple.content, "}") == 0) {
        block->flag = 0;
        block->lineCount = now_couple.lineCount;
        getNextTokin();
        outfile << "<Block>" << endl;
        return block;
    }
    while (strcmp(now_couple.content, "}") != 0) {
        block->BlockItems.push_back(grammar_BlockItem_Analyzer());
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
    }
    block->lineCount = now_couple.lineCount;
    getNextTokin();
    outfile << "<Block>" << endl;
    //return 1;
    return block;
}

fType grammar_FuncType_Analyzer() {
    if (strcmp(now_couple.content, "int") == 0) {
        getNextTokin();
        outfile << "<FuncType>" << endl;
        return INTTYPE;
    }
    else{
        getNextTokin();
        outfile << "<FuncType>" << endl;
        return VOIDTYPE;
    }
}

FuncDef * grammar_FuncDef_Analyzer() {
    FuncDef * funcDef;
    funcDef = (FuncDef *) malloc(sizeof(FuncDef));
    if ((strcmp(now_couple.content,"int") == 0 ||
         strcmp(now_couple.content, "void") == 0) &&
        strcmp(pre_couple.type, "IDENFR") == 0 &&
        strcmp(pre_pre_couple.content, "(") == 0) {
        funcDef->flag = 1;
        funcDef->funcType = grammar_FuncType_Analyzer();
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
        funcDef->this_Ident = grammar_Ident_Analyzer();
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
        if (strcmp(now_couple.content, "(") != 0) {
            printf("error!\n");
            //return -1;
        }
        getNextTokin();
        funcDef->isParm = 0;
        if (strcmp(now_couple.content, ")") != 0 &&
            strcmp(now_couple.content, "{") != 0) {
            funcDef->isParm = 1;
            funcDef->fParams = grammar_FuncFParams_Analyzer();
//            if (res != 1) {
//                printf("error!\n");
//                return -1;
//            }
        }
        if (strcmp(now_couple.content, ")") != 0) {
            insertError(couples[place - 2].lineCount, "j");
            //printf("error!\n");
            //return -1;
        }
        else {
            getNextTokin();
        }
        funcDef->this_Block = grammar_Block_Analyzer();
//        if (res != 1) {
//            printf("error!\n");
//            return -1;
//        }
        outfile << "<FuncDef>" << endl;
        //return 1;
        return funcDef;
    }
    else {
        funcDef->flag = -1;
        return funcDef;
    }
}

MainFuncDef * grammer_MainFuncDef_Analyzer() {
    MainFuncDef * mainFuncDef;
    mainFuncDef = (MainFuncDef *) malloc(sizeof(MainFuncDef));
    memset(mainFuncDef, 0, sizeof(MainFuncDef));
    if(strcmp(now_couple.content, "int") == 0) {
        getNextTokin();
        if (strcmp(now_couple.content, "main") == 0) {
            getNextTokin();
            if (strcmp(now_couple.content, "(") == 0) {
                getNextTokin();
                int fg = 0;
                if (strcmp(now_couple.content, ")") == 0) {
                    getNextTokin();
                }
                else {
                    fg = 1;
                    insertError(couples[place - 2].lineCount, "j");
                    //printf("error!\n");
                }
                mainFuncDef->this_Block = grammar_Block_Analyzer();
                outfile << "<MainFuncDef>" << endl;
//                if (fg == 1) {
//                    return nullptr;
//                }
                return mainFuncDef;
            }
            else {
                printf("error!\n");
                return nullptr;
            }
        }
        else {
            printf("error!\n");
            return nullptr;
        }
    }
    else {
        printf("error!\n");
        return nullptr;
    }
}

CompUnit * grammar_CompUnit_Analyzer() {
    CompUnit * compUnit;
    compUnit = (CompUnit *) malloc(sizeof(CompUnit));
    memset(compUnit, 0, sizeof(CompUnit));
    int res = 1;
    while (res != -1) {
        compUnit->Decls.push_back(grammar_Decl_Analyzer());
        if (compUnit->Decls[compUnit->Decls.size() - 1]->flag == -1) {
            compUnit->Decls.pop_back();
            res = -1;
        }
    }
    res = 1;
    while (res != -1) {
        compUnit->FuncDefs.push_back(grammar_FuncDef_Analyzer());
        if (compUnit->FuncDefs[compUnit->FuncDefs.size() - 1]->flag == -1) {
            compUnit->FuncDefs.pop_back();
            res = -1;
        }
    }
    compUnit->this_MainFuncDef = grammer_MainFuncDef_Analyzer();
//    if (res == -1) {
//        printf("error!\n");
//        return -1;
//    }
    outfile << "<CompUnit>" << endl;
    //return 1;
    return compUnit;
}

CompUnit *  grammar_Analazer() {
    getNextTokin();
    return grammar_CompUnit_Analyzer();
}