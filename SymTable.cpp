//
// Created by ZIMUQIN on 2021/11/12.
//

#include <iostream>
#include <fstream>
#include <cstring>
#include <utility>
#include<vector>
#include "def.h"
#include "SymTable.h"
#include <map>

using namespace std;

void FuncSymTable::getFuncSymTable(std::string name, std::map<std::string, symbol> symbolTable) {
    this->name = name;
    this->symbolTable = symbolTable;
}

void SymTable::get_Function_SymTable(const std::string& str, const symbol& sym) {
    map<string, symbol> newFuncSymTB;
    vector<param>::iterator it;
    vector<param> p = symbolTable.find(to_string(sym.layer) + sym.name + "f")->second.params;
    if (p.empty()) {
        FuncSymTable * funcSymTable;
        funcSymTable = (FuncSymTable *) malloc(sizeof(FuncSymTable));
        memset(funcSymTable, 0, sizeof(FuncSymTable));
        funcSymTable->getFuncSymTable(str, newFuncSymTB);
        this->FunctionSymTable.insert(map<string, FuncSymTable *>::value_type(str, funcSymTable));
        return;
    }
    for (it = p.begin(); it != p.end(); it++) {
        symbol symPara;
        symPara.name = it->name;
        symPara.dem = it->dem;
        symPara.layer = it->layer;
        symPara.type = it->type;
        symPara.position = it->position;
        symPara.dimension1 = it->dimension1;
        symPara.dimension2 = it->dimension2;
        if (newFuncSymTB.find(to_string(symPara.layer) + symPara.name + "v") == newFuncSymTB.end()) {
            newFuncSymTB.insert(map<string, symbol>::value_type(to_string(symPara.layer) + symPara.name + "v", symPara));
            //insertfile << symPara.layer << " " << symPara.name << " " << symPara.position << endl;
        }
    }
    FuncSymTable * funcSymTable;
    funcSymTable = (FuncSymTable *) malloc(sizeof(FuncSymTable));
    memset(funcSymTable, 0, sizeof(FuncSymTable));
    funcSymTable->getFuncSymTable(str, newFuncSymTB);
    this->FunctionSymTable.insert(map<string, FuncSymTable *>::value_type(str, funcSymTable));
//    map<string, symbol>::iterator itt;
//    map<string, symbol> nss = FunctionSymTable.find(str)->second->symbolTable;
//    for (itt = nss.begin(); itt != nss.end(); itt ++) {
//        cout << itt->first << endl;
//    }
}

void SymTable::insert_Var_to_SymTable(symbol sym) {
    if (symbolTable.find(to_string(sym.layer) + sym.name + "v") == symbolTable.end()) {
        symbolTable.insert(map<string, symbol>::value_type(to_string(sym.layer) + sym.name + "v", sym));
        //insertfile << sym.layer << " " << sym.name << " " << sym.position << endl;
    }
}

int SymTable::search_In_SymTable(std::string name, int layer) {
    int layNum = layer;
    while (layNum >= 0) {
        if (symbolTable.find(to_string(layNum) + name + "v") != symbolTable.end()) {
            return symbolTable.find(to_string(layNum) + name + "v")->second.toFrameReg;
        }
        layNum --;
    }
    return -1;
}

bool SymTable::isParam(std::string name, int layer) {
    int layNum = layer;
    while (layNum >= 0) {
        if (symbolTable.find(to_string(layNum) + name + "v") != symbolTable.end()) {
            return symbolTable.find(to_string(layNum) + name + "v")->second.isParam;
        }
        layNum --;
    }
    return false;
}

int SymTable::searchDem(std::string name, int layer) {
    int layNum = layer;
    while (layNum >= 0) {
        if (symbolTable.find(to_string(layNum) + name + "v") != symbolTable.end()) {
            return symbolTable.find(to_string(layNum) + name + "v")->second.dem;
        }
        layNum --;
    }
    return -1;
}

bool SymTable::isGlobal(std::string name, int layer) {
    int layNum = layer;
    while (layNum >= 0) {
        if (symbolTable.find(to_string(layNum) + name + "v") != symbolTable.end()) {
            if (layNum == 0) {
                return true;
            }
            else {
                return false;
            }
        }
        layNum --;
    }
    return false;
}

bool SymTable::isZeroDem(std::string name, int layer) {
    int layNum = layer;
    while (layNum >= 0) {
        if (symbolTable.find(to_string(layNum) + name + "v") != symbolTable.end()) {
            if (symbolTable.find(to_string(layNum) + name + "v")->second.dem == 0) {
                return true;
            }
            else {
                return false;
            }
        }
        layNum --;
    }
    return false;
}

int SymTable::getDimension2(std::string name, int layer) {
    int layNum = layer;
    while (layNum >= 0) {
        if (symbolTable.find(to_string(layNum) + name + "v") != symbolTable.end()) {
            return stoi(symbolTable.find(to_string(layNum) + name + "v")->second.dimension2);
        }
        layNum --;
    }
    return -1;
}

int SymTable::getFuncParamSize(std::string name) {
    return symbolTable.find(to_string(0) + name + "f")->second.params.size();
}