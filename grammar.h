//
// Created by ZIMUQIN on 2021/10/20.
//
#include "astree.h"
#ifndef COMPLIER_ERROR_GRAMMAR_H
#define COMPLIER_ERROR_GRAMMAR_H
BType * grammar_BType_Analyzer();
Ident * grammar_Ident_Analyzer();
InitVal * grammar_InitVal_Analyzer();
VarDef * grammar_VarDef_Analyzer();
VarDecl * grammar_VarDecl_Analyzer();
ConstInitVal * grammar_ConstInitVal_Analyzer();
ConstExp * grammar_ConstExp_Analyzer();
ConstDef * grammar_ConstDef_Analyzer();
ConstDecl * grammar_ConstDecl_Analyzer();
Decl * grammar_Decl_Analyzer();
FuncRParams * grammar_FuncRParams_Analyzer();
LVal * grammar_LVal_Analyzer();
PrimaryExp * grammar_PrimaryExp_Analyzer();
FuncFParam * grammar_FuncFParam_Analyzer();
FuncFParams * grammar_FuncFParams_Analyzer();
UnaryExp * grammar_UnaryExp_Analyzer();
RelExp * grammar_RelExp_Analyzer();
EqExp * grammar_EqExp_Analyzer();
LAndExp * grammar_LAndExp_Analyzer();
LOrExp * grammar_LOrExp_Analyzer();
Cond * grammar_Cond_Analyzer();
MulExp * grammar_MulExp_Analyzer();
AddExp * grammar_AddExp_Analyzer();
Exp * grammar_Exp_Analyzer();
Stmt * grammar_Stmt_Analyzer();
BlockItem * grammar_BlockItem_Analyzer();
Block * grammar_Block_Analyzer();
fType grammar_FuncType_Analyzer();
FuncDef * grammar_FuncDef_Analyzer();
MainFuncDef * grammer_MainFuncDef_Analyzer();
CompUnit * grammar_CompUnit_Analyzer();
CompUnit * grammar_Analazer();
#endif //COMPLIER_ERROR_GRAMMAR_H
