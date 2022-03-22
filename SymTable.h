//
// Created by ZIMUQIN on 2021/11/12.
//

#ifndef MIPSGENERATOR_SYMTABLE_H
#define MIPSGENERATOR_SYMTABLE_H
#include "def.h"
#include <map>
#include <string>

class FuncSymTable {
public:
    std::string name;
    std::map<std::string, symbol> symbolTable;
    void getFuncSymTable(std::string name, std::map<std::string, symbol> symbolTable);
};

class SymTable {
public:
    std::map<std::string, symbol> symbolTable;
    std::map<std::string, FuncSymTable *> FunctionSymTable;

    void get_Function_SymTable(const std::string&, const symbol& sym);
    void insert_Var_to_SymTable(symbol sym);
    int search_In_SymTable(std::string name, int layer);
    bool isParam(std::string name, int layer);
    int searchDem(std::string name, int layer);
    bool isGlobal(std::string name, int layer);
    bool isZeroDem(std::string name, int layer);
    int getDimension2(std::string name, int layer);
    int getFuncParamSize(std::string name);
    //void insert_FuncParam_to_Table()
};
#endif //MIPSGENERATOR_SYMTABLE_H
