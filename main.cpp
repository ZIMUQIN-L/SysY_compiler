#include <iostream>
#include <fstream>
#include <string.h>
#include<vector>
#include <map>
#include "def.h"
#include "lexical.h"
#include "grammar.h"
#include "SymTable.h"
#include "MipsGenerator.h"
#include "registerManager.h"
#include "error.h"
using namespace std;

vector<couple> couples;
vector<IR> IRs;
ifstream infile;
ofstream outfile;
int tNum = 0;
map<string, symbol> symbolTable;
int layer_Num = 0;
ofstream insertfile;
ofstream quadfile;
ofstream mipsfile;
ofstream errorfile;
class SymTable symTable;
class MipsGenerator mipsGenerator;
class regManager regManage;
map<int, myError> Errors;

int main() {
    IRs.reserve(10000);
    infile.open("testfile.txt", ios::in | ios::binary);
    outfile.open("output.txt", ios::out);
    insertfile.open("insert.txt", ios::out);
    quadfile.open("quad.txt", ios::out);
    mipsfile.open("mips.txt", ios::out);
    errorfile.open("error.txt", ios::out);
    lexcialAnalasy();
//    vector<couple>::iterator it;
//    for(it = couples.begin(); it != couples.end(); it++) {
//        outfile << it->type << " " << it->content << endl;
//    }
    vector<string> instr;
    instr.push_back("ADD");
    instr.push_back("PUSH");
    instr.push_back("CALL");
    instr.push_back("VAR");
    instr.push_back("ARR");
    instr.push_back("ASSIGN");
    instr.push_back("SUB");
    instr.push_back("MULT");
    instr.push_back("DIV");
    instr.push_back("MOD");
    instr.push_back("NOT");
    instr.push_back("FUNC");
    instr.push_back("PARA");
    instr.push_back("INTFUNCTION");
    instr.push_back("VOIDFUNCTION");
    instr.push_back("ENDFUNCTION");
    instr.push_back("PRINTVAR");
    instr.push_back("PRINTSTR");
    instr.push_back("LBRACE");
    instr.push_back("RBRACE");
    instr.push_back("GETARR");
    instr.push_back("GETINT");
    instr.push_back("ASSIGNARR");
    instr.push_back("ENDCALL");
    instr.push_back("BEFOREPUSH");
    instr.push_back("LABEL");
    instr.push_back("GOTO");
    instr.push_back("RETURN");
    instr.push_back("CONST");
    instr.push_back("CMP");
    instr.push_back("BNE");
    instr.push_back("BGT");
    instr.push_back("BLT");
    instr.push_back("BGE");
    instr.push_back("BLE");
    instr.push_back("INT");
    instr.push_back("VOID");
    instr.push_back("BEQ");
    instr.push_back("LEAVEBRACE");
    CompUnit * comp;
    comp = grammar_Analazer();
    comp->error_Analyzer();
    map<int, myError>::iterator it;
    if (!Errors.empty()) {
        for (it = Errors.begin(); it != Errors.end(); it++) {
            errorfile << it->first << " " << it->second.type << endl;
        }
    }
    else {
        comp->generate_CompUnit_IR();
        //mipsGenerator.IRs = IRs;
        regManage.buildManager();
        mipsGenerator.regManage = regManage;
        //mipsGenerator.symTable = symTable;
        mipsGenerator.MipsCodeGenerator();
        mipsGenerator.getAscii();
        mipsGenerator.MipsTextGenerator();
    }
    //vector<IR>().swap(IRs);

    //comp->generate_CompUnit_IR();
    vector<IR>::iterator its;
    for (its = IRs.begin(); its != IRs.end(); its ++) {
        quadfile << instr[its->instruction] << " " << its->reg1 << " " << its->reg2 << " " << its->reg3 << endl;
    }

//    mipsGenerator.IRs = IRs;
//    regManage.buildManager();
//    mipsGenerator.regManage = regManage;
//    mipsGenerator.symTable = symTable;
//
//    mipsGenerator.MipsCodeGenerator();
//    mipsGenerator.getAscii();
//    mipsGenerator.MipsTextGenerator();
//    map<string, symbol>::iterator itt;
//    map<string, symbol> nss = symTable.FunctionSymTable.find("f2")->second->symbolTable;
//    for (itt = nss.begin(); itt != nss.end(); itt ++) {
//        cout << itt->first << endl;
//    }
}
