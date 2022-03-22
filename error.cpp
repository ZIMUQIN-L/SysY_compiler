//
// Created by ZIMUQIN on 2021/12/4.
//

#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include <map>
#include "def.h"
#include "grammar.h"
#include "error.h"
#include "astree.h"
using namespace std;

extern map<int, myError> Errors;
map<string, symbol> mySymbolTable;
vector<vector<param>> funcParams;
int lay_Num = 0;
int marker[100] = {0};
int inWhile = 0;
int funcFlag = 0; //0: intFunc, 1: voidFunc
int isRet = 0;

void insert_Error_Table(symbol sym, string type) {
    if (mySymbolTable.find(to_string(lay_Num) + sym.name + type) == mySymbolTable.end()) {
        mySymbolTable.insert(map<string, symbol>::value_type(to_string(lay_Num) + sym.name + type, sym));
        //insertfile << sym.layer << " " << sym.name << " " << sym.position << endl;
    }
    else {
        insertError(sym.position, "b");
    }
}

//insert f params in the functions
void insert_ErrorParams(string name, int nowLayer) {
    vector<param>::iterator it;
    vector<param> p = mySymbolTable.find(to_string(nowLayer) + name + "f")->second.params;
    mySymbolTable.find(to_string(nowLayer) + name + "f")->second.validPara = 0;
    for (it = p.begin(); it != p.end(); it++) {
        symbol sym;
        sym.name = it->name;
        sym.dem = it->dem;
        sym.layer = it->layer;
        sym.type = it->type;
        sym.position = it->position;
        if (mySymbolTable.find(to_string(sym.layer) + sym.name + "v") == mySymbolTable.end()) {
            mySymbolTable.insert(map<string, symbol>::value_type(to_string(sym.layer) + sym.name + "v", sym));
            //insertfile << sym.layer << " " << sym.name << " " << sym.position << endl;
            mySymbolTable.find(to_string(nowLayer) + name + "f")->second.validPara ++;
        }
        else {
            insertError(sym.position, "b");
        }
    }
}

void remove_ErrorLayer(int layNum, int ss) {
    //insertfile << mark[layNum] << " " << layNum << " " << symbolTable.size() << endl;
    int pre_size = marker[layNum] - ss;
    while (mySymbolTable.size() > pre_size) {
        //cout << symbolTable.begin()->first << symbolTable.begin()->second.layer << endl;
        //insertfile << "delete " << (--symbolTable.end())->second.layer << " " << (--symbolTable.end())->second.name << " " << (--symbolTable.end())->second.position << endl;
        mySymbolTable.erase(--mySymbolTable.end());
    }
}


void insertError(int lineCount, string type) {
    myError error;
    error.type = type;
    error.lineCount = lineCount;
    Errors.insert(map<int, myError>::value_type (lineCount, error));
}

int checkErrorIdent(int flag, int layNum, std::string name) {
    string addIn;
    if (flag == 1) {
        addIn = "f";
    }
    else {
        addIn = "v";
    }
    while (layNum >= 0) {
        if (mySymbolTable.find(to_string(layNum) + name + addIn) != mySymbolTable.end()) {
            return 1;
        }
        layNum --;
    }
    return 0;
}

symbol getSymbol(int flag, int layNum, string name) {
    string addIn;
    if (flag == 1) {
        addIn = "f";
    }
    else {
        addIn = "v";
    }
    while (layNum >= 0) {
        if (mySymbolTable.find(to_string(layNum) + name + addIn) != mySymbolTable.end()) {
            return mySymbolTable.find(to_string(layNum) + name + addIn)->second;
        }
        layNum--;
    }
    symbol symbol1;
    return symbol1;
}

int Error_A_Analyzer(std::string StrCon) {
    int i = 1;
    while (i < StrCon.size()) {
        if (StrCon[i] == '"') {
            if (i == StrCon.size() - 1) {
                return 1;
            }
            else {
                return 0;
            }
        }
        else if (StrCon[i] == 92) {
            i ++;
            if (StrCon[i] != 'n') {
                return 0;
            }
            i ++;
            continue;
        }
        else if (StrCon[i] == 32 || StrCon[i] == 33 || (StrCon[i] >= 40 && StrCon[i] <= 126)) {
            i++;
            continue;
        }
        else if (StrCon[i] == '%') {
            i++;
            if (StrCon[i] != 'd') {
                return 0;
            }
            i++;
            continue;
        }
        else {
            return 0;
        }
    }
    return 1;
}

int Error_E_Analyzer(int layNum, string name, vector<vector<param>> params) {
    if ((params.end() - 1)->empty()) {
        return 1;
    }
    while (layNum >= 0) {
        if (mySymbolTable.find(to_string(layNum) + name + "f") != mySymbolTable.end()) {
            vector<param>:: iterator it;
            vector<param>:: iterator iter;
            iter = (params.end() - 1)->begin();
            for (it = mySymbolTable.find(to_string(layNum) + name + "f")->second.params.begin();
                 it != mySymbolTable.find(to_string(layNum) + name + "f")->second.params.end(); it ++) {
                if (it->dem != iter->dem || (iter->dem != 0 && iter->type == CONSTANT)) {
                    return 0;
                }
                iter ++;
            }
            return 1;
        }
        layNum --;
    }
    return 0;
}

int Error_H_Analyzer(int layNum, std::string name) {
    while (layNum >= 0) {
        if (mySymbolTable.find(to_string(layNum) + name + "v") != mySymbolTable.end()) {
            if (mySymbolTable.find(to_string(layNum) + name + "v")->second.type == CONSTANT) {
                return 0;
            }
            else {
                return 1;
            }
        }
        layNum --;
    }
    return 1;
}


int Error_L_Analyzer(std::string strCon, int expNumber) {
    int k = 0;
    int nub = 0;
    while ((k = strCon.find("%d", k)) != strCon.npos) {
        k ++;
        nub ++;
    }
    if (nub != expNumber) {
        return 0;
    }
    else {
        return 1;
    }
}

void LVal::lVal_Error_Handler() {
    if (!checkErrorIdent(0, lay_Num, this_Ident->ident)) {
        insertError(this_Ident->lineCount, "c");
    }
    if (!funcParams.empty()) {
        int dimens = getSymbol(0, lay_Num, this_Ident->ident).dem;
        dimens = dimens - this->dem;
        (-- (-- funcParams.end())->end())->dem = dimens;
    }
}

void Exp::exp_Error_Handler() {
    this_AddExp->addExp_Error_Handler();
}

void PrimaryExp::primary_Error_Handler() {
    if (indicator == 0) {
        this_exp->exp_Error_Handler();
    }
    else if (indicator == 1) {
        this_lVal->lVal_Error_Handler();
    }
}

void UnaryExp::unaryExp_Error_Handler() {
    if (flag == 1) {
        if (!checkErrorIdent(1, lay_Num, this_Ident->ident)) {
            insertError(this_Ident->lineCount, "c");
        }
        if (!funcParams.empty()) {
            int dimens = getSymbol(1, lay_Num, this_Ident->ident).dem;
            (-- (-- funcParams.end())->end())->dem = dimens;
        }
        symbol sym = getSymbol(1, lay_Num, this_Ident->ident);
        vector<param> realParams;
        funcParams.push_back(realParams);
        if (is_RParams) {
            for (auto & Exp : this_FuncR->Exps) {
                param newP;
                newP.dem = 0;
                (-- funcParams.end())->push_back(newP);
                Exp->exp_Error_Handler();
            }
            if (this_FuncR->Exps.size() != sym.params.size()) {
                insertError(this_Ident->lineCount, "d");
            }
            else {
                if (!Error_E_Analyzer(lay_Num, this_Ident->ident, funcParams)) {
                    insertError(this_Ident->lineCount, "e");
                }
            }
        }
        else {
            if (!sym.params.empty()) {
                insertError(this_Ident->lineCount, "d");
            }
        }
        funcParams.pop_back();
    }
    else if (flag == 0) {
        PriExp->primary_Error_Handler();
    }
    else {
        this_UnaryExp->unaryExp_Error_Handler();
    }
}

void MulExp::mulExp_Error_Handler() {
    for (auto & UnaryExp : UnaryExps) {
        UnaryExp->unaryExp_Error_Handler();
    }
}

void AddExp::addExp_Error_Handler() {
    for (auto & MulExp : MulExps) {
        MulExp->mulExp_Error_Handler();
    }
}

void RelExp::relExp_Error_Handler() {
    for (auto & AddExp : AddExps) {
        AddExp->addExp_Error_Handler();
    }
}

void EqExp::eqExp_Error_Handler() {
    for (auto & RelExp : RelExps) {
        RelExp->relExp_Error_Handler();
    }
}

void LAndExp::lAndExp_Error_Handler() {
    for (auto & EqExp : EqExps) {
        EqExp->eqExp_Error_Handler();
    }
}

void LOrExp::lOrExp_Error_Handler() {
    for (auto & LAndExp : LAndExps) {
        LAndExp->lAndExp_Error_Handler();
    }
}

void Cond::cond_Error_Handler() {
    this_LOrExp->lOrExp_Error_Handler();
}

void Stmt::stmt_Error_Handler() {
    isRet = 0;
    if (this->flag == 0) {
        this_LVal->lVal_Error_Handler();
        if (!Error_H_Analyzer(lay_Num, this_LVal->this_Ident->ident)) {
            insertError(this_LVal->this_Ident->lineCount, "h");
        }
        assignExp->exp_Error_Handler();
    }
    else if (flag == 1 && oneExp != nullptr) {
        oneExp->exp_Error_Handler();
    }
    else if (flag == 2) {
        this_Block->block_Error_Handler();
    }
    else if (flag == 3) {
        this_Cond->cond_Error_Handler();
        if_Stmt->stmt_Error_Handler();
        if (is_Else) {
            else_Stmt->stmt_Error_Handler();
        }
    }
    else if (flag == 4) {
        this_Cond->cond_Error_Handler();
        inWhile ++;
        while_Stmt->stmt_Error_Handler();
        inWhile --;
    }
    else if (flag == 5) {
        if (inWhile == 0) {
            insertError(this->lineCount, "m");
        }
    }
    else if (flag == 6) {
        if (funcFlag == 1 && is_Return_Exp) {
            //voidFunc
            insertError(this->lineCount, "f");
        }
        if (is_Return_Exp) {
            isRet = 1;
            returnExp->exp_Error_Handler();
        }
    }
    else if (flag == 7) {
        this_LVal->lVal_Error_Handler();
        if (!Error_H_Analyzer(lay_Num, this_LVal->this_Ident->ident)) {
            insertError(this_LVal->this_Ident->lineCount, "h");
        }
    }
    else if (flag == 8) {
        for (auto & Exp : Exps) {
            Exp->exp_Error_Handler();
        }
    }
    else if (flag == 9){
        if (inWhile == 0) {
            insertError(this->lineCount, "m");
        }
    }
    //else if ()
    //todo
}

void BlockItem::blockItem_Error_Handler() {
    if (flag == 0) {
        this_Decl->decl_Error_Handler();
    }
    else {
        this_Stmt->stmt_Error_Handler();
    }
}

void Block::block_Error_Handler() {
    lay_Num ++;
    marker[lay_Num] = mySymbolTable.size();
    if (this->flag == 1) {
        for (auto &BlockItem: this->BlockItems) {
            BlockItem->blockItem_Error_Handler();
        }
    }
    remove_ErrorLayer(lay_Num, 0);
    lay_Num --;
}

void FuncDef::funcDef_Error_Handler() {
    isRet = 0;
    symbol sym;
    sym.name = this_Ident->ident;
    sym.position = this_Ident->lineCount;
    sym.layer = lay_Num;
    if (funcType == INTTYPE) {
        sym.type = INT_FUNCTION;
        sym.dem = 0;
    }
    else {
        sym.type = VOID_FUNCTION;
        sym.dem = -1;
    }
    if (isParm) {
        for (auto & funcF : fParams->funcFs) {
            param newPara;
            newPara.name = funcF->this_Ident->ident;
            newPara.position = funcF->this_Ident->lineCount;
            newPara.layer = lay_Num + 1;
            newPara.dem = funcF->dem;
            sym.params.push_back(newPara);
        }
    }
    if (funcType == INTTYPE) {
        funcFlag = 0;
    }
    else {
        funcFlag = 1;
    }
    insert_Error_Table(sym, "f");
    insert_ErrorParams(sym.name, lay_Num);
    this->this_Block->block_Error_Handler();
    if (funcType == INTTYPE) {
        if (!isRet) {
            insertError(this_Block->lineCount, "g");
        }

    }
    remove_ErrorLayer(lay_Num + 1, mySymbolTable.find(to_string(sym.layer) + sym.name + "f")->second.validPara);
}

void Decl::decl_Error_Handler() {
    if (flag == 0) {
        for (auto & ConstDef : this_ConstDecl->ConstDefs) {
            symbol sym;
            sym.name = ConstDef->this_Ident->ident;
            sym.dem = ConstDef->dem;
            sym.type = CONSTANT;
            sym.position = ConstDef->this_Ident->lineCount;
            sym.layer = lay_Num;
            insert_Error_Table(sym, "v");
        }
    }
    else {
        for (auto & VarDef : this_VarDecl->VarDefs) {
            symbol sym;
            sym.name = VarDef->this_Ident->ident;
            sym.dem = VarDef->dem;
            sym.position = VarDef->this_Ident->lineCount;
            sym.layer = lay_Num;
            sym.type = INTEGER;
            insert_Error_Table(sym, "v");
        }
    }
}

void MainFuncDef::mainFuncDef_Error_Handler() {
    isRet = 0;
    funcFlag = 0;
    this_Block->block_Error_Handler();
    if (!isRet) {
        insertError(this_Block->lineCount, "g");
    }

}

void CompUnit::error_Analyzer() {
    int i = 0;
    if (!this->Decls.empty()) {
        for (i = 0; i < this->Decls.size(); i ++) {
            this->Decls[i]->decl_Error_Handler();
        }
    }
    if (!this->FuncDefs.empty()) {
        for (i = 0; i < this->FuncDefs.size(); i ++) {
            this->FuncDefs[i]->funcDef_Error_Handler();
        }
    }
    this_MainFuncDef->mainFuncDef_Error_Handler();
}
