//
// Created by ZIMUQIN on 2021/11/12.
//

#ifndef MIPSGENERATOR_MIPSGENERATOR_H
#define MIPSGENERATOR_MIPSGENERATOR_H
#include "astree.h"
#include "SymTable.h"
#include "registerManager.h"

struct storedReg {
    std::string name;
    int number;
};

struct bFuncReg {
    std::string regName;
    std::string varName;
    int frame;
};
class MipsGenerator {
public:
    int mark[100] = {0};
    //std::vector<IR> IRs;
    std::vector<std::string> MipsDataCode;
    std::vector<std::string> MipsAsciizCode;
    //std::vector<std::string> MipsTextCode;
    std::vector<storedReg> storedRegs;
    std::vector<int> toRemoveParamsNumber;
    std::vector<bFuncReg> storeBeforeFunc;
    std::vector<storedReg> funcParams;
    std::vector<int> frameReg;
    std::vector<int> toPushFuncStackFrame;
    std::vector<int> whileLab;
    //SymTable symTable;
    regManager regManage;
    int layer_num = 0;
    int frameRegister = 0;
    int stackRegister = 0;
    void insert_Func_Symbol(std::string funcName);
    void store_Used_Reg();
    void load_Used_Reg();
    void remove_Layer(int layNum, int ss);
    void MipsDataGenerator();
    void MipsTextGenerator();
    void MipsCodeGenerator();
    bool isNum(std::string str);
    bool isTempReg(std::string str);
    void getAscii();
    int getUseReg();
    int moveToReg(std::string string1);
    int loadToRegister(std::string name, std::string certitude, bool isARR);
    void storeToAddress(std::string name, std::string certitude, std::string assignNumber);
};
#endif //MIPSGENERATOR_MIPSGENERATOR_H
