//
// Created by ZIMUQIN on 2021/11/7.
//
#include <iostream>
#include <fstream>
#include <cstring>
#include<vector>
#include "astree.h"
#include "SymTable.h"
#include "MipsGenerator.h"
#include <map>

#define MIPS_DATA_LENGTH 600
using namespace std;
extern vector<IR> IRs;
extern int tNum;
extern map<string, symbol> symbolTable;
int layer_num = 0;
int mark[100] = {0};
extern ofstream insertfile;
extern SymTable symTable;
extern MipsGenerator mipsGenerator;
int global;
int label_num = 0;
vector<string> whileBeginLabels;
vector<string> whileOutLabels;
vector<int> whileLabs;

char buffer[MIPS_DATA_LENGTH];

string generate_new_label() {
    return "new_label" + to_string(label_num ++);
}

bool isNumber(std::string str) {
    return isdigit(str[0]) || (str[0] == '+') || (str[0] == '-');
}

int checkIfConst(int layNum, string name) {
    while (layNum >= 0) {
        if (symTable.symbolTable.find(to_string(layNum) + name + "v") != symTable.symbolTable.end()) {
            if (symTable.symbolTable.find(to_string(layNum) + name + "v")->second.type == CONSTANT) {
                return 1;
            }
            else {
                return 0;
            }
        }
        layNum --;
    }
    return 0;
}

void insert_To_Table(symbol sym, string type) {
    if (symTable.symbolTable.find(to_string(layer_num) + sym.name + type) == symTable.symbolTable.end()) {
        symTable.symbolTable.insert(map<string, symbol>::value_type(to_string(layer_num) + sym.name + type, sym));
        insertfile << sym.layer << " " << sym.name << " " << sym.position << endl;
    }
    //cout << sym.name << " " << sym.dimension1 << " " << sym.dimension2 << endl;
}

//insert f params in the functions
void insert_Params(string name, int nowLayer) {
    vector<param>::iterator it;
    vector<param> p = symTable.symbolTable.find(to_string(nowLayer) + name + "f")->second.params;
    symTable.symbolTable.find(to_string(nowLayer) + name + "f")->second.validPara = 0;
    for (it = p.begin(); it != p.end(); it++) {
        symbol sym;
        sym.name = it->name;
        sym.dem = it->dem;
        sym.layer = it->layer;
        sym.type = it->type;
        sym.position = it->position;
        sym.dimension1 = it->dimension1;
        sym.dimension2 = it->dimension2;
        if (symTable.symbolTable.find(to_string(sym.layer) + sym.name + "v") == symTable.symbolTable.end()) {
            symTable.symbolTable.insert(map<string, symbol>::value_type(to_string(sym.layer) + sym.name + "v", sym));
            insertfile << sym.layer << " " << sym.name << " " << sym.position << endl;
            symTable.symbolTable.find(to_string(nowLayer) + name + "f")->second.validPara ++;
        }
    }
//    map<string, symbol>::iterator itw;
//    for (itw = symTable.symbolTable.begin(); itw != symTable.symbolTable.end(); itw ++) {
//        insertfile << "yyy" << itw->first << itw->second.layer << " " << itw->second.name << " " << itw->second.position << endl;
//    }
}

void remove_Layer(int layNum, int ss) {
    //insertfile << mark[layNum] << " " << layNum << " " << symTable.symbolTable.size() << endl;
    int pre_size = mark[layNum] - ss;
    while (symTable.symbolTable.size() > pre_size) {
        //cout << symTable.symbolTable.begin()->first << symTable.symbolTable.begin()->second.layer << endl;
        insertfile << "delete " << (--symTable.symbolTable.end())->second.layer << " " << (--symTable.symbolTable.end())->second.name << " " << (--symTable.symbolTable.end())->second.position << endl;
        symTable.symbolTable.erase(--symTable.symbolTable.end());
    }
}

int checkIdent(int flag, int layNum, string name) {
    string addIn;
    if (flag == 1) {
        addIn = "f";
    }
    else {
        addIn = "v";
    }
    while (layNum >= 0) {
        if (symTable.symbolTable.find(to_string(layNum) + name + addIn) != symTable.symbolTable.end()) {
            return layNum;
        }
        layNum --;
    }
    return -1;
}

void UnaryOp::Analyze_Unary_Op() const {
    if (UnaryOp == NEGATIVE) {
        printf("ye");
    }
    else if (UnaryOp == POSITIVE) {
        printf("no");
    }
    else {
        printf("in");
    }
}

string LVal::generate_LVal_IR() {
    int demTrue = symTable.searchDem(this_Ident->ident, layer_num);
//    printf("%s\n", this_Ident->ident.c_str());
//    printf("%d\n", demTrue);
//    printf("%d\n", dem);
    if (demTrue != dem) {
        if (dem == 0) {
            return this_Ident->ident;
        }
        else {
            return this_Ident->ident + "[" + Exps[0]->generate_Exp_IR() + "]";
        }
    }
    if (dem == 0) {
        return this_Ident->ident;
    }
    else if (dem == 1) {
        string exp = Exps[0]->generate_Exp_IR();
        IR newIR;
        newIR.instruction = GETARR;
        string res = "#t" + to_string(tNum);
        tNum ++;
        newIR.reg1 = res;
        newIR.reg2 = this_Ident->ident;
        newIR.reg3 = exp;
        IRs.push_back(newIR);
        return res;
    }
    else {
        int lay = checkIdent(0, layer_num, this->this_Ident->ident);
        string dimension2;
        if (lay != -1) {
            symbol sym = symTable.symbolTable.find(to_string(lay) + this_Ident->ident + "v")->second;
            dimension2 = sym.dimension2;
        }
        IR newIR;
        newIR.instruction = MULTI;
        string res = "#t" + to_string(tNum);
        newIR.reg1 = res;
        tNum ++;
        newIR.reg2 = dimension2;
        newIR.reg3 = Exps[0]->generate_Exp_IR();
        IRs.push_back(newIR);
        IR newIR_1;
        newIR_1.instruction = ADDIN;
        newIR_1.reg1 = res;
        newIR_1.reg2 = res;
        newIR_1.reg3 = Exps[1]->generate_Exp_IR();
        IRs.push_back(newIR_1);
        IR newIR_2;
        newIR_2.instruction = GETARR;
        string result = "#t" + to_string(tNum);
        tNum ++;
        newIR_2.reg1 = result;
        newIR_2.reg2 = this_Ident->ident;
        newIR_2.reg3 = res;
        IRs.push_back(newIR_2);
        return result;
    }
}

string LVal::solve_LVal_Res() {
    if (dem == 0) {
        int lay = checkIdent(0, layer_num, this_Ident->ident);
        return symTable.symbolTable.find(to_string(lay) + this_Ident->ident + "v")->second.value[0];
    }
    else if (dem == 1) {
        int lay = checkIdent(0, layer_num, this_Ident->ident);
        string exp = Exps[0]->this_AddExp->solve_AddExp_Res();
        return symTable.symbolTable.find(to_string(lay) + this_Ident->ident + "v")->second.value[stoi(exp)];
    }
    else {
        int lay = checkIdent(0, layer_num, this->this_Ident->ident);
        string dimension2;
        if (lay != -1) {
            symbol sym = symTable.symbolTable.find(to_string(lay) + this_Ident->ident + "v")->second;
            dimension2 = sym.dimension2;
        }

        string mem1 = Exps[0]->this_AddExp->solve_AddExp_Res();
        int add1 = stoi(mem1) * stoi(dimension2);
        add1 = add1 + stoi(Exps[1]->this_AddExp->solve_AddExp_Res());
        return symTable.symbolTable.find(to_string(lay) + this_Ident->ident + "v")->second.value[add1];
    }
}

string UnaryExp::generate_UnaryExp_IR() {
    if (flag == 0) {
        if (this->PriExp->indicator == 1 && checkIfConst(layer_num, this->PriExp->this_lVal->this_Ident->ident)) {
            return this->PriExp->solve_PrimaryExp_Res();
        }
        return this->PriExp->generate_PrimaryExp_IR();
    }
    else if (flag == 1) {
        IR newIR_use;
        newIR_use.instruction = BEFOREPUSH;
        IRs.push_back(newIR_use);
        if (is_RParams) {
            this_FuncR->generate_FuncRParams_IR();
        }
        IR newIR;
        newIR.instruction = CALL;
        newIR.reg1 = this_Ident->ident;
        IRs.push_back(newIR);
        IR newIR_2;
        newIR_2.instruction = ENDCALL;
        IRs.push_back(newIR_2);
        string ret = "#t" + to_string(tNum);
        tNum ++;
        IR newIR_1;
        newIR_1.instruction = ASSIGN;
        newIR_1.reg1 = ret;
        newIR_1.reg2 = "#RET";
        IRs.push_back(newIR_1);
        return ret;
    }
    else {
        string ret = "#t" + to_string(tNum);
        tNum ++;
        IR newIR;
        if (this_UnaryOp == NOT) {
            newIR.instruction = NOTI;
            newIR.reg1 = ret;
            newIR.reg2 = this_UnaryExp->generate_UnaryExp_IR();
            IRs.push_back(newIR);
            return ret;
        }
        else if (this_UnaryOp == POSITIVE) {
            return this_UnaryExp->generate_UnaryExp_IR();
        }
        else {
            newIR.instruction = MULTI;
            newIR.reg1 = ret;
            newIR.reg2 = "-1";
            newIR.reg3 = this->this_UnaryExp->generate_UnaryExp_IR();
            IRs.push_back(newIR);
            return ret;
        }
    }
}

string UnaryExp::solve_UnaryExp_Res() {
    if (flag == 0) {
        return this->PriExp->solve_PrimaryExp_Res();
    }
    else if (flag == 1) {
        return "";
    }
    else {
        if (this_UnaryOp == NOT) {
            return "";
        }
        else if (this_UnaryOp == POSITIVE) {
            return this_UnaryExp->solve_UnaryExp_Res();
        }
        else {
            int number = stoi(this_UnaryExp->solve_UnaryExp_Res());
            number = number * -1;
            return to_string(number);
        }
    }
}

string MulExp::generate_MulExp_IR() {
    int i;
    string register2 = UnaryExps[0]->generate_UnaryExp_IR();
    for (i = 0; i < Syms.size(); i ++) {
        IR newIR;
        if (Syms[i] == MULT) {
            newIR.instruction = MULTI;
        }
        else if (Syms[i] == DIV){
            newIR.instruction = DIVI;
        }
        else {
            newIR.instruction = MODI;
        }
        newIR.reg2 = register2;
        newIR.reg3 = UnaryExps[i + 1]->generate_UnaryExp_IR();
        if (isNumber(newIR.reg2) && isNumber(newIR.reg3)) {
            if (Syms[i] == MULT) {
                register2 = to_string(stoi(newIR.reg2) * stoi(newIR.reg3));
            }
            else if (Syms[i] == DIV){
                register2 = to_string(stoi(newIR.reg2) / stoi(newIR.reg3));
            }
            else {
                register2 = to_string(stoi(newIR.reg2) % stoi(newIR.reg3));
            }
        }
        else if (isNumber(newIR.reg2) && newIR.reg2 == "1" && Syms[i] == MULT) {
            register2 = newIR.reg3;
        }
        else if (isNumber(newIR.reg3) && newIR.reg3 == "1" && (Syms[i] == MULT || Syms[i] == DIV)) {
            register2 = newIR.reg2;
        }
        else if (isNumber(newIR.reg2) && newIR.reg2 == "0" && Syms[i] == DIV) {
            register2 = "0";
        }
        else if (isNumber(newIR.reg2) && newIR.reg2 == "0" && Syms[i] == MULT) {
            register2 = "0";
        }
        else if (isNumber(newIR.reg3) && newIR.reg3 == "0" && Syms[i] == MULT) {
            register2 = "0";
        }
        else {
            register2 = "#t" + to_string(tNum);
            tNum ++;
            newIR.reg1 = register2;
            IRs.push_back(newIR);
        }
    }
    return register2;
}

string MulExp::solve_MulExp_Res() {
    int i;
    int number1 = stoi(UnaryExps[0]->solve_UnaryExp_Res());
    int number2;
    for (i = 0; i < Syms.size(); i ++) {
        number2 = stoi(UnaryExps[i + 1]->solve_UnaryExp_Res());
        if (Syms[i] == MULT) {
            number1 = number1 * number2;
        }
        else if (Syms[i] == DIV){
            number1 = number1 / number2;
        }
        else {
            number1 = number1 % number2;
        }
    }
    return to_string(number1);
}

string AddExp::generate_AddExp_IR() {
    int i;
    string register2 = MulExps[0]->generate_MulExp_IR();
    for (i = 0; i < Syms.size(); i ++) {
        IR newIR;
        if (Syms[i] == ADD) {
            newIR.instruction = ADDIN;
        }
        else {
            newIR.instruction = SUBU;
        }
        newIR.reg2 = register2;
        newIR.reg3 = MulExps[i + 1]->generate_MulExp_IR();
        if (isNumber(newIR.reg2) && isNumber(newIR.reg3)) {
            if (Syms[i] == ADD) {
                register2 = to_string(stoi(newIR.reg2) + stoi(newIR.reg3));
            }
            else {
                register2 = to_string(stoi(newIR.reg2) - stoi(newIR.reg3));
            }
        }
        else {
            register2 = "#t" + to_string(tNum);
            tNum++;
            newIR.reg1 = register2;
            IRs.push_back(newIR);
        }
    }
    return register2;
}

string AddExp::solve_AddExp_Res() {
    int i;
    string register1 = MulExps[0]->solve_MulExp_Res();
    string register2;
    int number1 = stoi(register1);
    int number2;
    for (i = 0; i < Syms.size(); i ++) {
        register2 = MulExps[i + 1]->solve_MulExp_Res();
        number2 = stoi(register2);
        if (Syms[i] == ADD) {
            number1 = number1 + number2;
        }
        else {
            number1 = number1 - number2;
        }
    }
    return to_string(number1);
}

string ConstExp::generate_ConstExp_IR() {
    return this_AddExp->generate_AddExp_IR();
}

string ConstExp::solve_ConstExp_Res() {
    return this_AddExp->solve_AddExp_Res();
}

string Exp::generate_Exp_IR() {
    return this_AddExp->generate_AddExp_IR();
}

void FuncRParams::generate_FuncRParams_IR() {
    int j;
    for (j = 0; j < Exps.size(); j ++) {
        string para;
        para = Exps[j]->generate_Exp_IR();
        IR newIR;
        newIR.instruction = PUSH;
        newIR.reg1 = para;
        IRs.push_back(newIR);
    }
}

string PrimaryExp::generate_PrimaryExp_IR() {
    IR newIR;
    if (this->indicator == 2) {
        return to_string(this->number);
    }
    else if (this->indicator == 0) {
        return this->this_exp->generate_Exp_IR();
    }
    else {
        return this->this_lVal->generate_LVal_IR();
    }
}

string PrimaryExp::solve_PrimaryExp_Res() {
    if (this->indicator == 2) {
        return to_string(this->number);
    }
    else if (this->indicator == 0) {
        return this->this_exp->this_AddExp->solve_AddExp_Res();
    }
    else {
        return this->this_lVal->solve_LVal_Res();
    }
}

void VarDef::generate_VarDef_IR() {
    IR newIR;
    symbol sym;
    sym.dem = this->dem;
    sym.name = this_Ident->ident;
    sym.position = this_Ident->lineCount;
    sym.type = INTEGER;
    sym.layer = layer_num;
    int space;
    if (dem == 0) {
        newIR.instruction = VAR;
        newIR.reg1 = this->this_Ident->ident;
        newIR.global = global;
        space = 1;
        IRs.push_back(newIR);
        if (this->flag_Init == 1) {
            IR newIR_1;
            string ret = this->this_InitVal->this_Exp->generate_Exp_IR();
            sym.value.push_back(ret);
            newIR_1.instruction = ASSIGN;
            newIR_1.reg1 = this->this_Ident->ident;
            newIR_1.reg2 = ret;
            IRs.push_back(newIR_1);
        }
    }
    else if (dem == 1) {
        newIR.instruction = ARR;
        string dem1 = ConstExps[0]->solve_ConstExp_Res();
        sym.dimension1 = dem1;
        space = stoi(dem1);
        newIR.reg1 = this_Ident->ident;
        newIR.reg2 = "[" + dem1 + "]";
        newIR.global = global;
        IRs.push_back(newIR);
        if (flag_Init == 1) {
            int i;
            for (i = 0; i < this_InitVal->Inits.size(); i ++) {
                IR newIR_1;
                string ret = this_InitVal->Inits[i]->this_Exp->generate_Exp_IR();
                sym.value.push_back(ret);
                newIR_1.instruction = ASSIGNARR;
                newIR_1.reg1 = this->this_Ident->ident;
                newIR_1.reg2 = to_string(i);
                newIR_1.reg3 = ret;
                IRs.push_back(newIR_1);
            }
        }
    }
    else {
        newIR.instruction = ARR;
        string dem1 = ConstExps[0]->solve_ConstExp_Res();
        string dem2 = ConstExps[1]->solve_ConstExp_Res();
        sym.dimension1 = dem1;
        sym.dimension2 = dem2;
        space = stoi(dem1) * stoi(dem2);
        newIR.global = global;
        newIR.reg1 = this_Ident->ident;
        newIR.reg2 = "[" + dem1 + "]" + "[" + dem2 + "]";
        IRs.push_back(newIR);
        if (flag_Init == 1) {
            int i, j;
            int m = 0;
            for (i = 0; i < this_InitVal->Inits.size(); i ++) {
                for (j = 0; j < this_InitVal->Inits[i]->Inits.size(); j ++) {
                    IR newIR_1;
                    string ret = this_InitVal->Inits[i]->Inits[j]->this_Exp->generate_Exp_IR();
                    sym.value.push_back(ret);
                    newIR_1.instruction = ASSIGNARR;
                    newIR_1.reg1 = this->this_Ident->ident;
                    newIR_1.reg2 = to_string(m);
                    newIR_1.reg3 = ret;
                    IRs.push_back(newIR_1);
                    m ++;
                }
            }
        }
    }
    if (global == 1) {
        sprintf(buffer, "%s: .space %d\n", sym.name.c_str(), space * 4);
        mipsGenerator.MipsDataCode.emplace_back(buffer);
    }
    sym.layer = layer_num;
    insert_To_Table(sym, "v");

}

void ConstDef::generate_ConstDef_IR() {
    IR newIR;
    symbol sym;
    sym.dem = this->dem;
    sym.name = this_Ident->ident;
    sym.position = this_Ident->lineCount;
    sym.type = CONSTANT;
    sym.layer = layer_num;
    int space = 0;
    if (dem == 0) {
        newIR.instruction = VAR;
        newIR.reg1 = this->this_Ident->ident;
        newIR.global = global;
        IRs.push_back(newIR);
        IR newIR_1;
        string ret = this->InitVal->this_ConstExp->solve_ConstExp_Res();
        sym.value.push_back(ret);
        newIR_1.instruction = ASSIGN;
        newIR_1.reg1 = this->this_Ident->ident;
        newIR_1.reg2 = ret;
        IRs.push_back(newIR_1);
        space = 1;
    }
    else if (dem == 1) {
        newIR.instruction = ARR;
        string dem1 = ConstExps[0]->solve_ConstExp_Res();
        sym.dimension1 = dem1;
        space = stoi(dem1);
        newIR.reg1 = this_Ident->ident;
        newIR.reg2 = "[" + dem1 + "]";
        newIR.global = global;
        IRs.push_back(newIR);
        int i;
        for (i = 0; i < InitVal->ConstInits.size(); i ++) {
            IR newIR_1;
            string ret = InitVal->ConstInits[i]->this_ConstExp->solve_ConstExp_Res();
            sym.value.push_back(ret);
            newIR_1.instruction = ASSIGNARR;
            newIR_1.reg1 = this->this_Ident->ident;
            newIR_1.reg2 = to_string(i);
            newIR_1.reg3 = ret;
            IRs.push_back(newIR_1);
        }
    }
    else {
        newIR.instruction = ARR;
        string dem1 = ConstExps[0]->solve_ConstExp_Res();
        string dem2 = ConstExps[1]->solve_ConstExp_Res();
        sym.dimension1 = dem1;
        sym.dimension2 = dem2;
        space = stoi(dem1) * stoi(dem2);
        newIR.reg1 = this_Ident->ident;
        newIR.reg2 = "[" + dem1 + "]" + "[" + dem2 + "]";
        newIR.global = global;
        IRs.push_back(newIR);
        int i, j;
        int m = 0;
        for (i = 0; i < InitVal->ConstInits.size(); i ++) {
            for (j = 0; j < InitVal->ConstInits[i]->ConstInits.size(); j ++) {
                IR newIR_1;
                string ret = InitVal->ConstInits[i]->ConstInits[j]->this_ConstExp->solve_ConstExp_Res();
                newIR_1.instruction = ASSIGNARR;
                newIR_1.reg1 = this->this_Ident->ident;
                newIR_1.reg2 = to_string(m);
                newIR_1.reg3 = ret;
                sym.value.push_back(ret);
                IRs.push_back(newIR_1);
                m ++;
            }
        }
    }
    if (global == 1) {
        sprintf(buffer, "%s: .space %d\n", sym.name.c_str(), space * 4);
        mipsGenerator.MipsDataCode.emplace_back(buffer);
    }
    sym.layer = layer_num;
    insert_To_Table(sym, "v");

}

void VarDecl::generate_VarDecl_IR() {
    int i = 0;
    if (this->VarDefs.empty()) {
        return;
    }
    for (i = 0; i < VarDefs.size(); i ++) {
        VarDefs[i]->generate_VarDef_IR();
    }
}

void ConstDecl::generate_ConstDecl_IR() {
    int i = 0;
    if (this->ConstDefs.empty()) {
        return;
    }
    for (i = 0; i < ConstDefs.size(); i ++) {
        ConstDefs[i]->generate_ConstDef_IR();
    }
}

void Decl::generate_Decl_IR() {
    if (this->flag == 1) {
        this->this_VarDecl->generate_VarDecl_IR();
    }
    else {
        this->this_ConstDecl->generate_ConstDecl_IR();
    }
}

param FuncFParam::generate_FuncFParam_IR() {
    param newParam;
    IR newIR;
    newIR.instruction = PARA;
    newParam.name = this_Ident->ident;
    newParam.layer = layer_num + 1;
    newParam.dem = dem;
    newParam.position = this_Ident->lineCount;
    if (dem == 0) {
        newIR.reg1 = "int";
        newIR.reg2 = this->this_Ident->ident;
        IRs.push_back(newIR);
        newParam.type = INTEGER;
        return newParam;
    }
    else if (dem == 1) {
        newIR.reg1 = "arr";
        newParam.dimension1 = "";
        newParam.type = INTEGER;
        newIR.reg2 = this_Ident->ident + "[" + "]";
        IRs.push_back(newIR);
        return newParam;
    }
    else {
        string dem2 = ConstExps[0]->solve_ConstExp_Res();
        newParam.dimension1 = "";
        newParam.dimension2 = dem2;
        newIR.reg1 = "arr";
        newIR.reg2 = this_Ident->ident + "[" + "]" + "[" + dem2 + "]";
        IRs.push_back(newIR);
        return newParam;
    }
}

void FuncDef::generate_FuncDef_IR() {
    IR newIR;
    symbol sym;
    if (funcType == INTTYPE) {
        sym.name = this_Ident->ident;
        sym.layer = layer_num;
        sym.position = this_Ident->lineCount;
        sym.dem = 0;
        sym.validPara = 0;
        sym.type = INT_FUNCTION;
        newIR.instruction = INTFUNCTION;
        newIR.reg1 = this_Ident->ident;
        IRs.push_back(newIR);
        if (isParm == 1) {
            int i;
            for (i = 0; i < fParams->funcFs.size(); i ++) {
                sym.params.push_back(fParams->funcFs[i]->generate_FuncFParam_IR());
            }
        }
        insert_To_Table(sym, "f");
        insert_Params(sym.name, sym.layer);
        this_Block->generate_Block_IR();
        symTable.get_Function_SymTable(sym.name, sym);
        remove_Layer(layer_num + 1, symTable.symbolTable.find(to_string(sym.layer) + sym.name + "f")->second.validPara);
        IR newIR_1;
        newIR_1.instruction = ENDFUNCTION;
        IRs.push_back(newIR_1);
    }
    else {
        sym.name = this_Ident->ident;
        sym.layer = layer_num;
        sym.position = this_Ident->lineCount;
        sym.dem = 0;
        sym.validPara = 0;
        sym.type = VOID_FUNCTION;
        newIR.instruction = VOIDFUNCTION;
        newIR.reg1 = this_Ident->ident;
        IRs.push_back(newIR);
        if (isParm == 1) {
            int i;
            for (i = 0; i < fParams->funcFs.size(); i ++) {
                sym.params.push_back(fParams->funcFs[i]->generate_FuncFParam_IR());
            }
        }
        insert_To_Table(sym, "f");
        insert_Params(sym.name, sym.layer);
        this_Block->generate_Block_IR();
        symTable.get_Function_SymTable(sym.name, sym);
        remove_Layer(layer_num + 1, symTable.symbolTable.find(to_string(sym.layer) + sym.name + "f")->second.validPara);
        IR newIR_1;
        newIR_1.instruction = ENDFUNCTION;
        IRs.push_back(newIR_1);
    }
}

string RelExp::generate_RelExp_IR() {
    if (AddExps.size() == 1) {
        return AddExps[0]->generate_AddExp_IR();
    }
    else {
        string opA = AddExps[0]->generate_AddExp_IR();
        string opB;
        for (int i = 1; i < AddExps.size(); ++i) {
            opB = AddExps[i]->generate_AddExp_IR();
            IR newIR;
            if (Syms[i - 1] == GT) {
                newIR.instruction = BGT;
            }
            else if (Syms[i - 1] == LT) {
                newIR.instruction = BLT;
            }
            else if (Syms[i - 1] == GET) {
                newIR.instruction = BGE;
            }
            else {
                newIR.instruction = BLE;
            }
            if (isNumber(opA) && isNumber(opB)) {
                int then = 0;
                if (Syms[i - 1] == GT) {
                    then = stoi(opA) > stoi(opB)? 1:0;
                }
                else if (Syms[i - 1] == LT) {
                    then = stoi(opA) < stoi(opB)? 1:0;
                }
                else if (Syms[i - 1] == GET) {
                    then = stoi(opA) >= stoi(opB)? 1:0;
                }
                else {
                    then = stoi(opA) <= stoi(opB)? 1:0;
                }
                opA = to_string(then);
            }
            else {
                string res = "#t" + to_string(tNum);
                tNum ++;
                newIR.reg1 = res;
                newIR.reg2 = opA;
                newIR.reg3 = opB;
                IRs.push_back(newIR);
                opA = res;
            }

        }
        return opA;
    }
}

string EqExp::generate_EqExp_IR(std::string begin_label, std::string out_label) {
    if (RelExps.size() == 1) {
        return RelExps[0]->generate_RelExp_IR();
    }
    else {
        string opA = RelExps[0]->generate_RelExp_IR();
        string opB;
        for (int i = 1; i < RelExps.size(); i ++) {
            opB = RelExps[i]->generate_RelExp_IR();
            IR newIR;
            if (i != RelExps.size() - 1) {
                if (Syms[i - 1] == EQU) {
                    newIR.instruction = BEQ;
                } else {
                    newIR.instruction = BNE;
                }
                if (isNumber(opA) && isNumber(opB)) {
                    int then = 0;
                    if (Syms[i - 1] == EQU) {
                        then = stoi(opA) == stoi(opB) ? 1 : 0;
                    } else {
                        then = stoi(opA) != stoi(opB) ? 1 : 0;
                    }
                    opA = to_string(then);
                } else {
                    string res = "#t" + to_string(tNum);
                    tNum++;
                    newIR.reg2 = opA;
                    newIR.reg3 = opB;
                    newIR.reg1 = res;
                    IRs.push_back(newIR);
                    opA = res;
                }
            }
            else {
                newIR.instruction = CMP;
                if (Syms[i - 1] == EQU) {
                    newIR.global = true;
                } else {
                    newIR.global = false;
                }
                if (isNumber(opA) && isNumber(opB)) {
                    int then = 0;
                    if (Syms[i - 1] == EQU) {
                        then = stoi(opA) == stoi(opB) ? 1 : 0;
                    }
                    else {
                        then = stoi(opA) != stoi(opB) ? 1 : 0;
                    }
                    if (then == 0) {
                        newIR.instruction = GOTO;
                        newIR.reg1 = out_label;
                        IRs.push_back(newIR);
                    }
                    opA = "$$hasGOTO";
                }
                else {
                    newIR.reg1 = opA;
                    newIR.reg2 = opB;
                    newIR.reg3 = out_label;
                    IRs.push_back(newIR);
                    opA = "$$hasGOTO";
                }
            }
        }
        return opA;
    }
//    if (RelExps.size() == 1) {
//        string res = RelExps[0]->generate_RelExp_IR();
//        IR newIR;
//        newIR.instruction = BEQ;
//        newIR.reg1 = res;
//        newIR.reg2 = to_string(0);
//        newIR.reg3 = out_label;
//        IRs.push_back(newIR);
//    }
//    else {
//        string opA = RelExps[0]->generate_RelExp_IR();
//        string opB;
//        for (int i = 1; i < RelExps.size(); i ++) {
//            opB = RelExps[i]->generate_RelExp_IR();
//            IR newIR;
//            if (Syms[i - 1] == EQU) {
//                newIR.instruction = BNE;
//                newIR.reg1 = opA;
//                newIR.reg2 = opB;
//                newIR.reg3 = out_label;
//            } else {
//                newIR.instruction = BEQ;
//                newIR.reg1 = opA;
//                newIR.reg2 = opB;
//                newIR.reg3 = out_label;
//            }
//            IRs.push_back(newIR);
//            opA = to_string(1);
//        }
//    }
}

void LAndExp::generate_LAndExp_IR(std::string begin_label, std::string out_label) {
    for (auto & EqExp : EqExps) {
        string res = EqExp->generate_EqExp_IR(begin_label, out_label);
        if (res == "$$hasGOTO") {
            continue;
        }
        if (isNumber(res)) {
            int then = 0;
            then = stoi(res) == 0 ? 1 : 0;
            if (then == 1) {
                IR newIR_1;
                newIR_1.instruction = GOTO;
                newIR_1.reg1 = out_label;
                IRs.push_back(newIR_1);
            }
        }
        else {
            IR newIR_1;
            newIR_1.instruction = CMP;
            newIR_1.reg1 = res;
            newIR_1.reg2 = out_label;
            newIR_1.reg3 = "$$none";
            IRs.push_back(newIR_1);
        }
    }
    IR newIR;
    newIR.instruction = GOTO;
    newIR.reg1 = begin_label;
    IRs.push_back(newIR);
}

void LOrExp::generate_LOrExp_IR(std::string if_label, std::string else_label) {
    if (this->LAndExps.size() == 1) {
        this->LAndExps[0]->generate_LAndExp_IR(if_label, else_label);
    }
    else {
        for (int j = 0; j < LAndExps.size() - 1; j++) {
            string newLab = generate_new_label();
            this->LAndExps[j]->generate_LAndExp_IR(if_label, newLab);
            IR newIR;
            newIR.instruction = LABEL;
            newIR.reg1 = newLab;
            IRs.push_back(newIR);
        }
        this->LAndExps[LAndExps.size() - 1]->generate_LAndExp_IR(if_label, else_label);
    }
}

void Cond::generate_Cond_IR(std::string if_label, std::string else_label) {
    this_LOrExp->generate_LOrExp_IR(if_label, else_label);
}

void Stmt::generate_Stmt_IR() {
    IR newIR;
    if (flag == 0) {
        string lval = this_LVal->generate_LVal_IR();
        if (this_LVal->dem == 0) {
            newIR.instruction = ASSIGN;
            newIR.reg1 = lval;
            string exp = this->assignExp->generate_Exp_IR();
            newIR.reg2 = exp;
            IRs.push_back(newIR);
        }
        else {
            IR oldIR;
            newIR.instruction = ASSIGNARR;
            newIR.reg1 = this_LVal->this_Ident->ident;
            newIR.reg2 = (-- IRs.end())->reg3;
            IRs.pop_back();
            string exp = this->assignExp->generate_Exp_IR();
            newIR.reg3 = exp;
            IRs.push_back(newIR);
        }
        return;
    }
    else if (flag == 1) {
        if (oneExp != nullptr) {
            oneExp->generate_Exp_IR();
        }
        return;
    }
    else if (flag == 2) {
        this_Block->generate_Block_IR();
    }
    else if (flag == 3) {
        string ifTodo = generate_new_label();
        string endIf = generate_new_label();
        string elseTodo;
        if (is_Else) {
            elseTodo = generate_new_label();
            this_Cond->generate_Cond_IR(ifTodo, elseTodo);
            newIR.instruction = LABEL;
            newIR.reg1 = ifTodo;
            IRs.push_back(newIR);
            if_Stmt->generate_Stmt_IR();
            IR newIR_3;
            newIR_3.instruction = GOTO;
            newIR_3.reg1 = endIf;
            IRs.push_back(newIR_3);
            IR newIR_1;
            newIR_1.instruction = LABEL;
            newIR_1.reg1 = elseTodo;
            IRs.push_back(newIR_1);
            else_Stmt->generate_Stmt_IR();
            IR newIR_2;
            newIR_2.instruction = LABEL;
            newIR_2.reg1 = endIf;
            IRs.push_back(newIR_2);
        }
        else {
            this_Cond->generate_Cond_IR(ifTodo, endIf);
            newIR.instruction = LABEL;
            newIR.reg1 = ifTodo;
            IRs.push_back(newIR);
            if_Stmt->generate_Stmt_IR();
            IR newIR_2;
            newIR_2.instruction = LABEL;
            newIR_2.reg1 = endIf;
            IRs.push_back(newIR_2);
        }
    }
    else if (flag == 4) {
        string beginLab = generate_new_label();
        string codeLab = generate_new_label();
        string outLab = generate_new_label();
        newIR.instruction = LABEL;
        newIR.reg1 = beginLab;
        IRs.push_back(newIR);
        whileBeginLabels.push_back(beginLab);
        whileLabs.push_back(layer_num);
        whileOutLabels.push_back(outLab);
        this_Cond->generate_Cond_IR(codeLab, outLab);
        IR newIR_3;
        newIR_3.instruction = LABEL;
        newIR_3.reg1 = codeLab;
        IRs.push_back(newIR_3);
        while_Stmt->generate_Stmt_IR();
        IR newIR_2;
        newIR_2.instruction = GOTO;
        newIR_2.reg1 = beginLab;
        IRs.push_back(newIR_2);
        whileBeginLabels.pop_back();
        whileOutLabels.pop_back();
        IR newIR_1;
        newIR_1.instruction = LABEL;
        newIR_1.reg1 = outLab;
        IRs.push_back(newIR_1);
        whileLabs.pop_back();
    }
    else if (flag == 5) {
        IR newIR_1;
        newIR_1.instruction = LEAVEBRACE;
        newIR_1.reg1 = to_string(layer_num - * (-- whileLabs.end()));
        IRs.push_back(newIR_1);
        newIR.instruction = GOTO;
        newIR.reg1 = * (-- whileOutLabels.end());
        IRs.push_back(newIR);
    }
    else if (flag == 6) {
        if (is_Return_Exp) {
            newIR.instruction = ASSIGN;
            newIR.reg1 = "#RET";
            newIR.reg2 = returnExp->generate_Exp_IR();
            IRs.push_back(newIR);
        }
        IR newIR_1;
        newIR_1.instruction = RETURN;
        IRs.push_back(newIR_1);
        return;
    }
    else if (flag == 7) {
        string lval = this_LVal->generate_LVal_IR();
        string getint = "#t" + to_string(tNum);
        tNum ++;
        IR newIR_1;
        newIR_1.instruction = GETINT;
        newIR_1.reg1 = getint;
        if (this_LVal->dem == 0) {
            newIR.instruction = ASSIGN;
            newIR.reg1 = lval;
            newIR.reg2 = getint;
            IRs.push_back(newIR_1);
            IRs.push_back(newIR);
        }
        else {
            IR oldIR;
            newIR.instruction = ASSIGNARR;
            newIR.reg1 = this_LVal->this_Ident->ident;
            newIR.reg2 = (-- IRs.end())->reg3;
            IRs.pop_back();
            newIR.reg3 = getint;
            IRs.push_back(newIR_1);
            IRs.push_back(newIR);
        }
        return;
    }
    else if (flag == 8) {
        int i = 1;
        int j = 0;
        while (i < formatString.size() - 1) {
            IR newIR_1;
            if (formatString[i] == '%' && formatString[i + 1] == 'd') {
                newIR_1.instruction = PRINTVAR;
                newIR_1.reg1 = Exps[j]->generate_Exp_IR();
                j ++;
                i = i + 2;
                IRs.push_back(newIR_1);
                continue;
            }
            if (formatString[i] == '\\' && formatString[i + 1] == 'n') {
                newIR_1.instruction = PRINTSTR;
                newIR_1.reg1 = "\\n";
                IRs.push_back(newIR_1);
                i = i + 2;
                continue;
            }
            newIR_1.instruction = PRINTSTR;
            newIR_1.reg1 = formatString[i];
            IRs.push_back(newIR_1);
            i = i + 1;
        }
    }
    else {
        IR newIR_1;
        newIR_1.instruction = LEAVEBRACE;
        newIR_1.reg1 = to_string(layer_num - * (-- whileLabs.end()));
        IRs.push_back(newIR_1);
        newIR.instruction = GOTO;
        newIR.reg1 = * (-- whileBeginLabels.end());
        IRs.push_back(newIR);
    }
}

void BlockItem::generate_BlockItem_IR() {
    if (flag == 0) {
        this_Decl->generate_Decl_IR();
    }
    else {
        this_Stmt->generate_Stmt_IR();
    }
}

void Block::generate_Block_IR() {
    if (flag == 0) {
        layer_num ++;
        mark[layer_num] = symTable.symbolTable.size();
        IR newIR;
        newIR.instruction = LBRACE;
        IRs.push_back(newIR);
        remove_Layer(layer_num, 0);
        IR newIR_1;
        newIR_1.instruction = RBRACE;
        IRs.push_back(newIR_1);
        layer_num --;
        return;
    }
    else {
        layer_num ++;
        mark[layer_num] = symTable.symbolTable.size();
        IR newIR;
        newIR.instruction = LBRACE;
        IRs.push_back(newIR);
        int i;
        for (i = 0; i < BlockItems.size(); i ++) {
            BlockItems[i]->generate_BlockItem_IR();
        }
        remove_Layer(layer_num, 0);
        IR newIR_1;
        newIR_1.instruction = RBRACE;
        IRs.push_back(newIR_1);
        layer_num --;
    }
}

void MainFuncDef::generate_MainFuncDef_IR() {
    IR newIR;
    newIR.instruction = INTFUNCTION;
    newIR.reg1 = "main";
    IRs.push_back(newIR);
    this_Block->generate_Block_IR();
//    IR newIR_1;
//    newIR_1.instruction = ENDFUNCTION;
//    IRs.push_back(newIR_1);
}

void CompUnit::generate_CompUnit_IR() {
    int i = 0;
    global = 1;
    if (!this->Decls.empty()) {
        for (i = 0; i < this->Decls.size(); i ++) {
            Decls[i]->generate_Decl_IR();
        }
    }
    IR newIR;
    newIR.instruction = CALL;
    newIR.reg1 = "main";
    IRs.push_back(newIR);
    global = 0;
    if (!this->FuncDefs.empty()) {
        for (i = 0; i < this->FuncDefs.size(); i ++) {
            FuncDefs[i]->generate_FuncDef_IR();
        }
    }
    this_MainFuncDef->generate_MainFuncDef_IR();
    //printf("1");
}