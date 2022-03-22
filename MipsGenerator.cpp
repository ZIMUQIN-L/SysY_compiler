//
// Created by ZIMUQIN on 2021/11/12.
//

#include <iostream>
#include <fstream>
#include <cstring>
#include<vector>
#include "MipsGenerator.h"
#include <map>
#include <algorithm>
#include <regex>
#include "def.h"
#define MIPS_TEXT_LENGTH 1000

using namespace std;
extern ofstream insertfile;
extern ofstream mipsfile;
extern SymTable symTable;
extern vector<IR> IRs;

void MipsGenerator::store_Used_Reg() {
    //TODO
    //it can be replaced by sw $reg, i($reg)
    int i;
    int getSp = 0;
    char * buffer;
    buffer = (char *) malloc(1000 * sizeof(char));
    bFuncReg bfReg1;
    bfReg1.regName = "fp";
    bfReg1.varName = "fp";
    bfReg1.frame = stackRegister;
    sprintf(buffer, "sw $fp, %d($sp)\n", -1 * getSp);
    //MipsTextCode.emplace_back(buffer);
    mipsfile << buffer;
    //sprintf(buffer, "addi $sp, $sp, -4\n");
    //MipsTextCode.emplace_back(buffer);
    //mipsfile << buffer;
    getSp = getSp + 4;
    stackRegister = stackRegister + 4;
    storeBeforeFunc.emplace_back(bfReg1);
    for (i = 0; i < regManage.regs.size(); i ++) {
        if (regManage.regs[i].isUsing) {
            bFuncReg bfReg;
            bfReg.regName = regManage.regs[i].registerName;
            bfReg.varName = regManage.regs[i].varName;
            bfReg.frame = stackRegister;
            sprintf(buffer, "sw $%s, %d($sp)\n", bfReg.regName.c_str(), -1 * getSp);
            //MipsTextCode.emplace_back(buffer);
            mipsfile << buffer;
            //sprintf(buffer, "addi $sp, $sp, -4\n");
            //MipsTextCode.emplace_back(buffer);
            //mipsfile << buffer;
            getSp = getSp + 4;
            stackRegister = stackRegister + 4;
            storeBeforeFunc.emplace_back(bfReg);
            regManage.regs[i].isUsing = false;
        }
    }
    bFuncReg bfReg;
    bfReg.regName = "ra";
    bfReg.varName = "ra";
    bfReg.frame = stackRegister;
    sprintf(buffer, "sw $ra, %d($sp)\n", -1 * getSp);
    //MipsTextCode.emplace_back(buffer);
    mipsfile << buffer;
    getSp = getSp + 4;
    sprintf(buffer, "addi $sp, $sp, %d\n", -1 * getSp);
    //MipsTextCode.emplace_back(buffer);
    mipsfile << buffer;
    stackRegister = stackRegister + 4;
    storeBeforeFunc.emplace_back(bfReg);
}

void MipsGenerator::load_Used_Reg() {
    //todo
    //it can be replaced just like store
    char * buffer;
    int getSp = 4;
    buffer = (char *) malloc(1000 * sizeof(char));
    int i;
    for (i = 0; i < regManage.regs.size(); i ++) {
        regManage.regs[i].isUsing = false;
    }
    //sprintf(buffer, "addiu $sp, $sp, 4\n");
    //MipsTextCode.emplace_back(buffer);
    //mipsfile << buffer;
    sprintf(buffer, "lw $ra, %d($sp)\n", getSp);
    //MipsTextCode.emplace_back(buffer);
    stackRegister = stackRegister - 4;
    mipsfile << buffer;
    storeBeforeFunc.pop_back();
    i = storeBeforeFunc.size() - 1;
    while (storeBeforeFunc[i].regName != "fp") {
        //sprintf(buffer, "addiu $sp, $sp, 4\n");
        //MipsTextCode.emplace_back(buffer);
        //mipsfile << buffer;
        getSp = getSp + 4;
        sprintf(buffer, "lw $%s, %d($sp)\n", storeBeforeFunc[i].regName.c_str(), getSp);
        //MipsTextCode.emplace_back(buffer);
        mipsfile << buffer;
        int regPlace = regManage.findReg(storeBeforeFunc[i].regName);
        regManage.regs[regPlace].varName = storeBeforeFunc[i].varName;
        regManage.regs[regPlace].isUsing = true;
        stackRegister = stackRegister - 4;
        storeBeforeFunc.pop_back();
        i = storeBeforeFunc.size() - 1;
    }
    getSp = getSp + 4;
    sprintf(buffer, "addiu $sp, $sp, %d\n", getSp);
    //MipsTextCode.emplace_back(buffer);
    mipsfile << buffer;
    sprintf(buffer, "lw $fp, 0($sp)\n");
    //MipsTextCode.emplace_back(buffer);
    mipsfile << buffer;
    stackRegister = stackRegister - 4;
    storeBeforeFunc.pop_back();
}

void MipsGenerator::insert_Func_Symbol(std::string funcName) {
    int removeParamsNum = 0;
    vector<param>::iterator it;
    vector<param> p = symTable.symbolTable.find(to_string(0) + funcName + "f")->second.params;
    if (p.empty()) {
        printf("add %d\n", removeParamsNum);
        toRemoveParamsNumber.push_back(removeParamsNum);
        return;
    }
    int funcFrame = 0;
    for (it = p.begin(); it != p.end(); it++) {
        symbol symPara;
        symPara.name = it->name;
        symPara.dem = it->dem;
        symPara.toFrameReg = funcFrame;
        funcFrame += 4;
        symPara.layer = layer_num + 1;
        symPara.type = it->type;
        symPara.position = it->position;
        symPara.dimension1 = it->dimension1;
        symPara.dimension2 = it->dimension2;
        symPara.isParam = true;
        if (symTable.symbolTable.find(to_string(symPara.layer) + symPara.name + "v") == symTable.symbolTable.end()) {
            removeParamsNum ++;
            symTable.symbolTable.insert(map<string, symbol>::value_type(to_string(symPara.layer) + symPara.name + "v", symPara));
            //insertfile << symPara.layer << " " << symPara.name << " " << symPara.position << endl;
        }
    }
    toRemoveParamsNumber.push_back(removeParamsNum);
    printf("add %d\n", removeParamsNum);
}

void MipsGenerator::remove_Layer(int layNum, int ss) {
    //insertfile << mark[layNum] << " " << layNum << " " << symTable.symbolTable.size() << endl;
    int pre_size = mark[layNum] - ss;
    while (symTable.symbolTable.size() > pre_size) {
        //cout << symTable.symbolTable.begin()->first << symTable.symbolTable.begin()->second.layer << endl;
        insertfile << "delete in " << (--symTable.symbolTable.end())->second.layer << " " << (--symTable.symbolTable.end())->second.name << " " << (--symTable.symbolTable.end())->second.toFrameReg << endl;
        symTable.symbolTable.erase(--symTable.symbolTable.end());
    }
}

void MipsGenerator::MipsDataGenerator() {
    int i;
    cout << ".data" << endl;
    for (i = 0; i < MipsDataCode.size(); i ++) {
        cout << MipsDataCode[i];
    }
}

bool MipsGenerator::isNum(std::string str) {
    return isdigit(str[0]) || (str[0] == '+') || (str[0] == '-');
}

bool MipsGenerator::isTempReg(std::string str) {
    return str[0] == '#';
}

int MipsGenerator::getUseReg() {
    int pos = regManage.findSpareReg();
    if (pos != -1) {
        vector<int>::iterator it;
        for (it = regManage.regMap.begin(); * it != pos; it ++) {

        }
        regManage.regMap.erase(it);
        regManage.regMap.push_back(pos);
        return pos;
    }
    else {
        int useIn = regManage.regMap[0];
        regManage.regMap.erase(regManage.regMap.begin());
        regManage.regMap.push_back(useIn);
        char * buffer;
        buffer = (char *) malloc(1000 * sizeof(char));
        if (regManage.regs[useIn].varName[0] == '#') {
            symbol sym;
            sym.name = regManage.regs[useIn].varName;
            sym.layer = layer_num;
            sym.dem = 0;
            sym.type = INTEGER;
            sprintf(buffer, "sw $%s, 0($sp)\n", regManage.regs[useIn].registerName.c_str());
            //MipsTextCode.emplace_back(buffer);
            mipsfile << buffer;
            sprintf(buffer, "addi $sp, $sp, -4\n");
            //MipsTextCode.emplace_back(buffer);
            mipsfile << buffer;
            sym.toFrameReg = stackRegister;
            stackRegister = stackRegister + 4;
            symTable.insert_Var_to_SymTable(sym);
            regManage.regs[useIn].isUsing = false;
        }
        else {
            if (symTable.isParam(regManage.regs[useIn].varName, layer_num)) {
                int frame = symTable.search_In_SymTable(regManage.regs[useIn].varName, layer_num);
                sprintf(buffer, "sw $%s, %d($fp)\n", regManage.regs[useIn].registerName.c_str(), -1 * frame);
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                regManage.regs[useIn].isUsing = false;
            }
            else {
                int frame = symTable.search_In_SymTable(regManage.regs[useIn].varName, layer_num);
                sprintf(buffer, "sw $%s, %d($fp)\n", regManage.regs[useIn].registerName.c_str(), -1 * frame);
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                regManage.regs[useIn].isUsing = false;
            }
        }
        vector<int>::iterator it;
        for (it = regManage.regMap.begin(); * it != useIn; it ++) {

        }
        regManage.regMap.erase(it);
        regManage.regMap.push_back(useIn);
        return useIn;
    }
}

int MipsGenerator::moveToReg(std::string string1) {
    char * buffer;
    buffer = (char *) malloc(1000 * sizeof(char));
    int pos = getUseReg();
    sprintf(buffer, "li $%s, %s\n", regManage.regs[pos].registerName.c_str(), string1.c_str());
    //MipsTextCode.emplace_back(buffer);
    mipsfile << buffer;
    regManage.regs[pos].isUsing = true;
    return pos;
}

int MipsGenerator::loadToRegister(std::string name, std::string certitude, bool isARR) {
//    char * buffer;
//    buffer = (char *) malloc(1000 * sizeof(char));
    char buffer[1000];
    memset(buffer, 0, sizeof(char) * 1000);
    int layNum = layer_num;
    if (!isARR) {
        int i = regManage.isInUsage(name);
        if (i != -1) {
            return i;
        }
    }
    int spareReg;
    int my_load_flag = 0;
    if (certitude == "0") {
        my_load_flag = 1;
    }
    else if (isNum(certitude)) {
        spareReg = getUseReg();
        regManage.regs[spareReg].isUsing = true;
        regManage.regs[spareReg].varName = "#temp";
        sprintf(buffer, "li $%s, %d\n", regManage.regs[spareReg].registerName.c_str(), stoi(certitude) * 4);
        //MipsTextCode.emplace_back(buffer);
        mipsfile << buffer;
    }
    else {
        int regFlag = loadToRegister(certitude, to_string(0), false);
        spareReg = getUseReg();
        sprintf(buffer, "move $%s, $%s\n", regManage.regs[spareReg].registerName.c_str(),
                regManage.regs[regFlag].registerName.c_str());
        regManage.regs[regFlag].isUsing = false;
        //MipsTextCode.emplace_back(buffer);
        mipsfile << buffer;
        sprintf(buffer, "addu $%s, $%s, $%s\n", regManage.regs[spareReg].registerName.c_str(),
                regManage.regs[spareReg].registerName.c_str(),
                regManage.regs[spareReg].registerName.c_str());
        //MipsTextCode.emplace_back(buffer);
        mipsfile << buffer;
        sprintf(buffer, "addu $%s, $%s, $%s\n", regManage.regs[spareReg].registerName.c_str(),
                regManage.regs[spareReg].registerName.c_str(),
                regManage.regs[spareReg].registerName.c_str());
        //MipsTextCode.emplace_back(buffer);
        mipsfile << buffer;
        regManage.regs[spareReg].isUsing = true;
        regManage.regs[spareReg].varName = "#temp";
    }
    if (symTable.isParam(name, layNum)) {
        if (symTable.isZeroDem(name, layNum)) {
            int frame = symTable.search_In_SymTable(name, layNum);
            int toLoad = getUseReg();
            sprintf(buffer, "lw $%s, %d($fp)\n", regManage.regs[toLoad].registerName.c_str(), -1 * frame);
            //MipsTextCode.emplace_back(buffer);
            mipsfile << buffer;
            if (!my_load_flag) {
                regManage.regs[spareReg].isUsing = false;
            }
            regManage.regs[toLoad].isUsing = true;
            regManage.regs[toLoad].varName = name;
            return toLoad;
        }
        else {
            int toLoad = getUseReg();
            int frame = symTable.search_In_SymTable(name, layNum);
            sprintf(buffer, "lw $t8, %d($fp)\n", -1 * frame);
            //MipsTextCode.emplace_back(buffer);
            mipsfile << buffer;
            if (!my_load_flag) {
                sprintf(buffer, "addu $t8, $t8, $%s\n", regManage.regs[spareReg].registerName.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                regManage.regs[spareReg].isUsing = false;
            }
            sprintf(buffer, "lw $%s, 0($t8)\n", regManage.regs[toLoad].registerName.c_str());
            //MipsTextCode.emplace_back(buffer);
            mipsfile << buffer;
            regManage.regs[toLoad].isUsing = true;
            regManage.regs[toLoad].varName = name;
            return toLoad;
        }
    }
    while (layNum >= 0) {
        if (symTable.symbolTable.find(to_string(layNum) + name + "v") != symTable.symbolTable.end()) {
            if (symTable.symbolTable.find(to_string(layNum) + name + "v")->second.layer == 0) {
                int toLoad = getUseReg();
                sprintf(buffer, "la $%s, %s\n", regManage.regs[toLoad].registerName.c_str(), name.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                if (!my_load_flag) {
                    sprintf(buffer, "addu $%s, $%s, $%s\n", regManage.regs[toLoad].registerName.c_str(),
                            regManage.regs[toLoad].registerName.c_str(),
                            regManage.regs[spareReg].registerName.c_str());
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    regManage.regs[spareReg].isUsing = false;
                }
                sprintf(buffer, "lw $%s, 0($%s)\n", regManage.regs[toLoad].registerName.c_str(),
                        regManage.regs[toLoad].registerName.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                regManage.regs[toLoad].isUsing = true;
                regManage.regs[toLoad].varName = name;
                return toLoad;
            }
            else {
                int frame = symTable.symbolTable.find(to_string(layNum) + name + "v")->second.toFrameReg;
                int toLoad = getUseReg();
                if (!my_load_flag) {
                    sprintf(buffer, "subiu $%s, $%s, %d\n", regManage.regs[spareReg].registerName.c_str(),
                            regManage.regs[spareReg].registerName.c_str(), frame);
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    sprintf(buffer, "addu $%s, $%s, $fp\n", regManage.regs[spareReg].registerName.c_str(),
                            regManage.regs[spareReg].registerName.c_str());
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    sprintf(buffer, "lw $%s, 0($%s)\n",
                            regManage.regs[toLoad].registerName.c_str(),
                            regManage.regs[spareReg].registerName.c_str());
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    regManage.regs[spareReg].isUsing = false;
                }
                else {
                    sprintf(buffer, "subiu $%s, $fp, %d\n", regManage.regs[toLoad].registerName.c_str(),
                            frame);
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    sprintf(buffer, "lw $%s, 0($%s)\n",
                            regManage.regs[toLoad].registerName.c_str(),
                            regManage.regs[toLoad].registerName.c_str());
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                }
                regManage.regs[toLoad].isUsing = true;
                regManage.regs[toLoad].varName = name;
                return toLoad;
            }
        }
        layNum --;
    }
    return -1;
}

void MipsGenerator::storeToAddress(std::string name, std::string certitude, std::string assignNumber) {
    char * buffer;
    buffer = (char *) malloc(1000 * sizeof(char));
    int tempReg;
    if (isNum(assignNumber)) {
        tempReg = getUseReg();
        sprintf(buffer, "li $%s, %s\n", regManage.regs[tempReg].registerName.c_str(), assignNumber.c_str());
        //MipsTextCode.emplace_back(buffer);
        mipsfile << buffer;
    }
    else {
        tempReg = regManage.isInUsage(assignNumber);
        if (tempReg == -1) {
            tempReg = loadToRegister(assignNumber, to_string(0), false);
        }
    }
    regManage.regs[tempReg].isUsing = true;
    regManage.regs[tempReg].varName = "#tempUsingReg";
    int spareReg;
    int my_load_flag = 0;
    if (certitude == "0") {
        my_load_flag = 1;
    }
    else if (isNum(certitude)) {
        spareReg = getUseReg();
        regManage.regs[spareReg].isUsing = true;
        regManage.regs[spareReg].varName = "#temp";
        sprintf(buffer, "li $%s, %d\n", regManage.regs[spareReg].registerName.c_str(), stoi(certitude) * 4);
        //MipsTextCode.emplace_back(buffer);
        mipsfile << buffer;
    }
    else {
        int regFlag = loadToRegister(certitude, to_string(0), false);
        spareReg = getUseReg();
        sprintf(buffer, "move $%s, $%s\n", regManage.regs[spareReg].registerName.c_str(),
                regManage.regs[regFlag].registerName.c_str());
        regManage.regs[regFlag].isUsing = false;
        //MipsTextCode.emplace_back(buffer);
        mipsfile << buffer;
        sprintf(buffer, "addu $%s, $%s, $%s\n", regManage.regs[spareReg].registerName.c_str(),
                regManage.regs[spareReg].registerName.c_str(),
                regManage.regs[spareReg].registerName.c_str());
        //MipsTextCode.emplace_back(buffer);
        mipsfile << buffer;
        sprintf(buffer, "addu $%s, $%s, $%s\n", regManage.regs[spareReg].registerName.c_str(),
                regManage.regs[spareReg].registerName.c_str(),
                regManage.regs[spareReg].registerName.c_str());
        //MipsTextCode.emplace_back(buffer);
        mipsfile << buffer;
        regManage.regs[spareReg].isUsing = true;
        regManage.regs[spareReg].varName = "#temp";
    }
    int layNum = layer_num;
    if (symTable.isParam(name, layNum)) {
        if (symTable.isZeroDem(name, layNum)) {
            int frame = symTable.search_In_SymTable(name, layNum);
            sprintf(buffer, "sw $%s, %d($fp)\n", regManage.regs[tempReg].registerName.c_str(), -1 * frame);
            //MipsTextCode.emplace_back(buffer);
            mipsfile << buffer;
            if (!my_load_flag) {
                regManage.regs[spareReg].isUsing = false;
            }
            regManage.regs[tempReg].isUsing = false;
            return;
        }
        else {
            int frame = symTable.search_In_SymTable(name, layNum);
            sprintf(buffer, "lw $t8, %d($fp)\n", -1 * frame);
            //MipsTextCode.emplace_back(buffer);
            mipsfile << buffer;
            if (!my_load_flag) {
                sprintf(buffer, "addu $t8, $t8, $%s\n", regManage.regs[spareReg].registerName.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                regManage.regs[spareReg].isUsing = false;
            }
            sprintf(buffer, "sw $%s, 0($t8)\n", regManage.regs[tempReg].registerName.c_str());
            //MipsTextCode.emplace_back(buffer);
            mipsfile << buffer;
            regManage.regs[tempReg].isUsing = false;
            return;
        }
    }
    while (layNum >= 0) {
        if (symTable.symbolTable.find(to_string(layNum) + name + "v") != symTable.symbolTable.end()) {
            if (symTable.symbolTable.find(to_string(layNum) + name + "v")->second.layer == 0) {
                int pos = getUseReg();
                sprintf(buffer, "la $%s, %s\n", regManage.regs[pos].registerName.c_str(), name.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                if (!my_load_flag) {
                    sprintf(buffer, "addu $%s, $%s, $%s\n", regManage.regs[pos].registerName.c_str(),
                            regManage.regs[pos].registerName.c_str(),
                            regManage.regs[spareReg].registerName.c_str());

                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    regManage.regs[spareReg].isUsing = false;
                }
                sprintf(buffer, "sw $%s, 0($%s)\n", regManage.regs[tempReg].registerName.c_str(),
                        regManage.regs[pos].registerName.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                regManage.regs[pos].isUsing = false;
                regManage.regs[tempReg].isUsing = false;
                break;
            }
            else {
                int frame = symTable.symbolTable.find(to_string(layNum) + name + "v")->second.toFrameReg;
                if (!my_load_flag) {
                    sprintf(buffer, "subiu $%s, $%s, %d\n", regManage.regs[spareReg].registerName.c_str(),
                            regManage.regs[spareReg].registerName.c_str(), frame);
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    sprintf(buffer, "addu $%s, $%s, $fp\n", regManage.regs[spareReg].registerName.c_str(),
                            regManage.regs[spareReg].registerName.c_str());
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    sprintf(buffer, "sw $%s, 0($%s)\n",
                            regManage.regs[tempReg].registerName.c_str(),
                            regManage.regs[spareReg].registerName.c_str());
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    regManage.regs[spareReg].isUsing = false;
                }
                else {
                    sprintf(buffer, "subiu $t8, $fp, %d\n", frame);
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    sprintf(buffer, "sw $%s, 0($t8)\n",
                            regManage.regs[tempReg].registerName.c_str());
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                }
                regManage.regs[tempReg].isUsing = false;
                break;
            }
        }
        layNum --;
    }
    free(buffer);
}

void MipsGenerator::getAscii() {
    char * buffer;
    buffer = (char *) malloc(1000 * sizeof(char));
    int stringL = 0;
    int k = 0;
    for (k = 0; k < IRs.size(); k ++) {
        if (IRs[k].instruction == PRINTSTR) {
            string printStr;
            printStr = printStr.append(IRs[k].reg1);
            while (IRs[k + 1].instruction == PRINTSTR) {
                k ++;
                printStr = printStr.append(IRs[k].reg1);
            }
            string strName = "$Str" + to_string(stringL);
            stringL ++;
            sprintf(buffer, "%s: .asciiz \"%s\"\n", strName.c_str(), printStr.c_str());
            mipsfile << buffer;
        }
    }
    sprintf(buffer, "_end_main_:\n");
    //MipsTextCode.emplace_back(buffer);
    //mipsfile << buffer;
    mipsfile << ".text" << endl;
    free(buffer);
}

void MipsGenerator::MipsCodeGenerator() {
    char * buffer;
    buffer = (char *) malloc(1000 * sizeof(char));
    int i;
    mipsfile << ".data" << endl;
    if (!MipsDataCode.empty()) {
        for (i = 0; i < MipsDataCode.size(); i++) {
            mipsfile << MipsDataCode[i];
        }
    }
    if (!MipsAsciizCode.empty()) {
        for (i = 0; i < MipsAsciizCode.size(); i++) {
            mipsfile << MipsAsciizCode[i];
        }
    }
//    for (i = 0; i < MipsTextCode.size(); i ++) {
//        mipsfile << MipsTextCode[i];
//    }
//    mipsfile.close();
//    ifstream openMips;
//    openMips.open("mip1.txt", ios::in | ios::binary);
//    openMips.seekg(0,std::ios::beg);
//    string outStr;
//    char a;
//    cout << "eeee" << openMips.eof() << endl;
//    while ((a = openMips.get()) != EOF) {
//        printf("%c", a);
//        mips1 << a;
//    }
//    openMips.close();
    free(buffer);
}

void MipsGenerator::MipsTextGenerator() {
    //MipsTextCode.reserve(3000);
    char * buffer;
    buffer = (char *) malloc(1000 * sizeof(char));
    sprintf(buffer, "addu $fp, $sp, $0\n");
    //MipsTextCode.emplace_back(buffer);
    mipsfile << buffer;
    int stringLine = 0;
    int i = 0;
    for (i = 0; i < IRs.size(); i ++) {
        switch (IRs[i].instruction) {
            case ADDIN:
                if (isNum(IRs[i].reg2) && isNum(IRs[i].reg3)) {
                    int pos = getUseReg();
                    string rs = regManage.regs[pos].registerName;
                    sprintf(buffer, "li $%s, %s\n", rs.c_str(), IRs[i].reg3.c_str());
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    sprintf(buffer, "addiu $%s, $%s, %s\n", rs.c_str(), rs.c_str(), IRs[i].reg2.c_str());
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    regManage.regs[pos].varName = IRs[i].reg1;
                    regManage.regs[pos].isUsing = true;
                }
                else if (isNum(IRs[i].reg2)) {
                    int pos = getUseReg();
                    string rd = regManage.regs[pos].registerName;
                    int toLoad = loadToRegister(IRs[i].reg3, to_string(0), false);
                    sprintf(buffer, "addiu $%s, $%s, %s\n", regManage.regs[pos].registerName.c_str(),
                            regManage.regs[toLoad].registerName.c_str(), IRs[i].reg2.c_str());
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    regManage.regs[toLoad].isUsing = false;
                    regManage.regs[pos].varName = IRs[i].reg1;
                    regManage.regs[pos].isUsing = true;
                }
                else if (isNum(IRs[i].reg3)) {
                    int pos = getUseReg();
                    string rd = regManage.regs[pos].registerName;
                    int toLoad = loadToRegister(IRs[i].reg2, to_string(0), false);
                    sprintf(buffer, "addiu $%s, $%s, %s\n", regManage.regs[pos].registerName.c_str(),
                            regManage.regs[toLoad].registerName.c_str(), IRs[i].reg3.c_str());
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    regManage.regs[toLoad].isUsing = false;
                    regManage.regs[pos].varName = IRs[i].reg1;
                    regManage.regs[pos].isUsing = true;
                }
                else {
                    int pos = getUseReg();
                    string rd = regManage.regs[pos].registerName;
                    int toLoad1 = loadToRegister(IRs[i].reg2, to_string(0), false);
                    regManage.regs[toLoad1].isUsing = true;
                    regManage.regs[toLoad1].varName = IRs[i].reg2;
                    int toLoad2 = loadToRegister(IRs[i].reg3, to_string(0), false);
                    sprintf(buffer, "addu $%s, $%s, $%s\n", regManage.regs[pos].registerName.c_str(),
                            regManage.regs[toLoad1].registerName.c_str(),
                            regManage.regs[toLoad2].registerName.c_str());
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    regManage.regs[toLoad1].isUsing = false;
                    regManage.regs[toLoad2].isUsing = false;
                    regManage.regs[pos].varName = IRs[i].reg1;
                    regManage.regs[pos].isUsing = true;
                }
                break;
            case PUSH: {
                //if the param is a number
                if (isNum(IRs[i].reg1)) {
                    storedReg storeR;
                    sprintf(buffer, "li $t9, %s\n", IRs[i].reg1.c_str());
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    sprintf(buffer, "sw $t9, 0($sp)\n");
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    sprintf(buffer, "addi $sp, $sp, -4\n");
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    storeR.number = stackRegister;
                    storeR.name = IRs[i].reg1;
                    funcParams.push_back(storeR);
                    stackRegister = stackRegister + 4;
                }
                    //the param is a var
                else if (count(IRs[i].reg1.begin(), IRs[i].reg1.end(), '[') == 0) {
                    //if we need to take one dimension of the var
                    int usage = regManage.isInUsage(IRs[i].reg1);
                    if (IRs[i].reg1[0] == '#' && usage != -1) {
                        sprintf(buffer, "sw $%s, 0($sp)\n", regManage.regs[usage].registerName.c_str());
                        //MipsTextCode.emplace_back(buffer);
                        mipsfile << buffer;
                        sprintf(buffer, "addi $sp, $sp, -4\n");
                        //MipsTextCode.emplace_back(buffer);
                        mipsfile << buffer;
                        storedReg storeR;
                        storeR.number = stackRegister;
                        storeR.name = IRs[i].reg1;
                        funcParams.push_back(storeR);
                        stackRegister = stackRegister + 4;
                        regManage.regs[i].isUsing = false;
                    }
                    else if (symTable.isParam(IRs[i].reg1, layer_num)) {
                        if (symTable.isZeroDem(IRs[i].reg1, layer_num)) {
                            int frame = symTable.search_In_SymTable(IRs[i].reg1, layer_num);
                            sprintf(buffer, "lw $t9, %d($fp)\n", -1 * frame);
                            //MipsTextCode.emplace_back(buffer);
                            mipsfile << buffer;
                            sprintf(buffer, "sw $t9, 0($sp)\n");
                            //MipsTextCode.emplace_back(buffer);
                            mipsfile << buffer;
                            sprintf(buffer, "addi $sp, $sp, -4\n");
                            //MipsTextCode.emplace_back(buffer);
                            mipsfile << buffer;
                            storedReg storeR;
                            storeR.number = stackRegister;
                            storeR.name = IRs[i].reg1;
                            funcParams.push_back(storeR);
                            stackRegister = stackRegister + 4;
                        }
                        else {
                            int frame = symTable.search_In_SymTable(IRs[i].reg1, layer_num);
                            sprintf(buffer, "lw $t9, %d($fp)\n", -1 * frame);
                            //MipsTextCode.emplace_back(buffer);
                            mipsfile << buffer;
                            sprintf(buffer, "sw $t9, 0($sp)\n");
                            //MipsTextCode.emplace_back(buffer);
                            mipsfile << buffer;
                            sprintf(buffer, "addi $sp, $sp, -4\n");
                            //MipsTextCode.emplace_back(buffer);
                            mipsfile << buffer;
                            storedReg storeR;
                            storeR.number = stackRegister;
                            storeR.name = IRs[i].reg1;
                            funcParams.push_back(storeR);
                            stackRegister = stackRegister + 4;
                        }
                    }
                    else if (symTable.isGlobal(IRs[i].reg1, layer_num)) {
                        //if it's a zero dem var, we just need to take the value
                        if (symTable.isZeroDem(IRs[i].reg1, layer_num)) {
                            sprintf(buffer, "la $t9, %s\n", IRs[i].reg1.c_str());
                            //MipsTextCode.emplace_back(buffer);
                            mipsfile << buffer;
                            sprintf(buffer, "lw $t9, 0($t9)\n");
                            //MipsTextCode.emplace_back(buffer);
                            mipsfile << buffer;
                            sprintf(buffer, "sw $t9, 0($sp)\n");
                            //MipsTextCode.emplace_back(buffer);
                            mipsfile << buffer;
                            sprintf(buffer, "addi $sp, $sp, -4\n");
                            //MipsTextCode.emplace_back(buffer);
                            mipsfile << buffer;
                            storedReg storeR;
                            storeR.number = stackRegister;
                            storeR.name = IRs[i].reg1;
                            funcParams.push_back(storeR);
                            stackRegister = stackRegister + 4;
                        }
                        else {
                            //if not a zero dem, we need to get the addr of the var
                            sprintf(buffer, "la $t9, %s\n", IRs[i].reg1.c_str());
                            //MipsTextCode.emplace_back(buffer);
                            mipsfile << buffer;
                            sprintf(buffer, "sw $t9, 0($sp)\n");
                            //MipsTextCode.emplace_back(buffer);
                            mipsfile << buffer;
                            sprintf(buffer, "addi $sp, $sp, -4\n");
                            //MipsTextCode.emplace_back(buffer);
                            mipsfile << buffer;
                            storedReg storeR;
                            storeR.number = stackRegister;
                            storeR.name = IRs[i].reg1;
                            funcParams.push_back(storeR);
                            stackRegister = stackRegister + 4;
                        }
                    }
                    else {
                        if (symTable.isZeroDem(IRs[i].reg1, layer_num)) {
                            int frame = symTable.search_In_SymTable(IRs[i].reg1, layer_num);
                            sprintf(buffer, "lw $t9, %d($fp)\n", -1 * frame);
                            //MipsTextCode.emplace_back(buffer);
                            mipsfile << buffer;
                            sprintf(buffer, "sw $t9, 0($sp)\n");
                            //MipsTextCode.emplace_back(buffer);
                            mipsfile << buffer;
                            sprintf(buffer, "addi $sp, $sp, -4\n");
                            //MipsTextCode.emplace_back(buffer);
                            mipsfile << buffer;
                            storedReg storeR;
                            storeR.number = stackRegister;
                            storeR.name = IRs[i].reg1;
                            funcParams.push_back(storeR);
                            stackRegister = stackRegister + 4;
                        }
                        else {
                            int frame = symTable.search_In_SymTable(IRs[i].reg1, layer_num);
                            sprintf(buffer, "subiu $t9, $fp, %d\n", frame);
                            //MipsTextCode.emplace_back(buffer);
                            mipsfile << buffer;
                            sprintf(buffer, "sw $t9, 0($sp)\n");
                            //MipsTextCode.emplace_back(buffer);
                            mipsfile << buffer;
                            sprintf(buffer, "addi $sp, $sp, -4\n");
                            //MipsTextCode.emplace_back(buffer);
                            mipsfile << buffer;
                            storedReg storeR;
                            storeR.number = stackRegister;
                            storeR.name = IRs[i].reg1;
                            funcParams.push_back(storeR);
                            stackRegister = stackRegister + 4;
                        }
                    }
                }
                else {
                    string varName;
                    string dimens;
                    string re = "(.*?)\\[(.*?)\\]";
                    regex pattern(re);
                    smatch results;
                    if (regex_match(IRs[i].reg1, results, pattern)) {
                        varName = results[1];
                        dimens = results[2];
                    }
                    int regUsing;
                    int fff = 0;
                    if (dimens == "0") {
                        fff = 1;
                    }
                    else if (isNum(dimens)) {
                        regUsing = getUseReg();
                        sprintf(buffer, "li $%s, %s\n", regManage.regs[regUsing].registerName.c_str(),
                                dimens.c_str());
                        //MipsTextCode.emplace_back(buffer);
                        mipsfile << buffer;
                    }
                    else {
                        regUsing = loadToRegister(dimens, to_string(0), false);
                    }
                    if (fff == 0) {
                        regManage.regs[regUsing].isUsing = true;
                        int dem2 = symTable.getDimension2(varName, layer_num);
                        sprintf(buffer, "li $t8, %d\n", dem2 * 4);
                        //MipsTextCode.emplace_back(buffer);
                        mipsfile << buffer;
                        sprintf(buffer, "mult $t8, $%s\n", regManage.regs[regUsing].registerName.c_str());
                        //MipsTextCode.emplace_back(buffer);
                        mipsfile << buffer;
                        sprintf(buffer, "mflo $t8\n");
                        //MipsTextCode.emplace_back(buffer);
                        mipsfile << buffer;
                        regManage.regs[regUsing].isUsing = false;
                    }
                    else {
                        sprintf(buffer, "move $t8, $0\n");
                        //MipsTextCode.emplace_back(buffer);
                        mipsfile << buffer;
                    }
                    if (symTable.isParam(varName, layer_num)) {
                        int frame = symTable.search_In_SymTable(varName, layer_num);
                        sprintf(buffer, "lw $t9, %d($fp)\n", -1 * frame);
                        //MipsTextCode.emplace_back(buffer);
                        mipsfile << buffer;
                        sprintf(buffer, "addu $t9, $t9, $t8\n");
                        //MipsTextCode.emplace_back(buffer);
                        mipsfile << buffer;
                        sprintf(buffer, "sw $t9, 0($sp)\n");
                        //MipsTextCode.emplace_back(buffer);
                        mipsfile << buffer;
                        sprintf(buffer, "addi $sp, $sp, -4\n");
                        //MipsTextCode.emplace_back(buffer);
                        mipsfile << buffer;
                        storedReg storeR;
                        storeR.number = stackRegister;
                        storeR.name = IRs[i].reg1;
                        funcParams.push_back(storeR);
                        stackRegister = stackRegister + 4;
                    }
                    else if (symTable.isGlobal(varName, layer_num)) {
                        sprintf(buffer, "la $t9, %s\n", varName.c_str());
                        //MipsTextCode.emplace_back(buffer);
                        mipsfile << buffer;
                        sprintf(buffer, "addu $t9, $t9, $t8\n");
                        //MipsTextCode.emplace_back(buffer);
                        mipsfile << buffer;
                        sprintf(buffer, "sw $t9, 0($sp)\n");
                        //MipsTextCode.emplace_back(buffer);
                        mipsfile << buffer;
                        sprintf(buffer, "addi $sp, $sp, -4\n");
                        //MipsTextCode.emplace_back(buffer);
                        mipsfile << buffer;
                        storedReg storeR;
                        storeR.number = stackRegister;
                        storeR.name = IRs[i].reg1;
                        funcParams.push_back(storeR);
                        stackRegister = stackRegister + 4;
                    }
                    else {
                        int frame = symTable.search_In_SymTable(varName, layer_num);
                        sprintf(buffer, "subiu $t9, $fp, %d\n", frame);
                        //MipsTextCode.emplace_back(buffer);
                        mipsfile << buffer;
                        sprintf(buffer, "addu $t9, $t9, $t8\n");
                        //MipsTextCode.emplace_back(buffer);
                        mipsfile << buffer;
                        sprintf(buffer, "sw $t9, 0($sp)\n");
                        //MipsTextCode.emplace_back(buffer);
                        mipsfile << buffer;
                        sprintf(buffer, "addi $sp, $sp, -4\n");
                        //MipsTextCode.emplace_back(buffer);
                        mipsfile << buffer;
                        storedReg storeR;
                        storeR.number = stackRegister;
                        storeR.name = IRs[i].reg1;
                        funcParams.push_back(storeR);
                        stackRegister = stackRegister + 4;
                    }
                }
            }
                break;
            case CALL: {
                if (IRs[i].reg1 == "main") {
                    sprintf(buffer, "jal _label_main\n");
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    sprintf(buffer, "j _end_main_\n");
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                }
                else {
                    //store_Used_Reg();
                    int j = 0;
                    frameReg.push_back(stackRegister);
//                    storedReg stR;
//                    stR.name = "sp";
//                    stR.number = stackRegister;
//                    storedRegs.push_back(stR);
                    stackRegister = 0;
                    int size = symTable.getFuncParamSize(IRs[i].reg1);
                    if (!funcParams.empty() && size != 0) {
                        for (j = funcParams.size() - size; j < funcParams.size(); j++) {
                            sprintf(buffer, "lw $t9, %d($fp)\n", -1 * funcParams[j].number);
                            //MipsTextCode.emplace_back(buffer);
                            mipsfile << buffer;
                            sprintf(buffer, "sw $t9, %d($sp)\n", -1 * stackRegister);
                            //MipsTextCode.emplace_back(buffer);
                            mipsfile << buffer;
                            //sprintf(buffer, "addi $sp, $sp, -4\n");
                            //MipsTextCode.emplace_back(buffer);
                            //mipsfile << buffer;
                            stackRegister = stackRegister + 4;
                        }
                        sprintf(buffer, "addi $sp, $sp, %d\n", -1 * stackRegister);
                        //MipsTextCode.emplace_back(buffer);
                        mipsfile << buffer;
                        for (j = 0; j < size; j ++) {
                            funcParams.pop_back();
                        }
                    }
                    sprintf(buffer, "addiu $fp, $sp, %d\n", stackRegister);
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    sprintf(buffer, "jal _label_%s\n", IRs[i].reg1.c_str());
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    insert_Func_Symbol(IRs[i].reg1);
                    layer_num++;
                    mark[layer_num] = symTable.symbolTable.size();

                }
            }
                break;
            case VAR:
                if (IRs[i].global == 1 && IRs[i + 1].instruction == ASSIGN) {
                }
                else if (IRs[i].global == 0) {
                    symbol sym;
                    sym.name = IRs[i].reg1;
                    sym.layer = layer_num;
                    sym.dem = 0;
                    sym.type = INTEGER;
                    sprintf(buffer, "addi $sp, $sp, -4\n");
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    sym.toFrameReg = stackRegister;
                    stackRegister = stackRegister + 4;
                    symTable.insert_Var_to_SymTable(sym);
                }
                break;
            case ARR:
                if (IRs[i].global == 1 && IRs[i + 1].instruction == ASSIGNARR) {
                }
                else if (IRs[i].global == 0) {
                    symbol sym;
                    sym.name = IRs[i].reg1;
                    sym.layer = layer_num;
                    string re = R"(\[(.*?)\]\[?(.*?)\]?)";
                    regex pattern(re);
                    smatch results;
                    int space;
                    if (count(IRs[i].reg2.begin(), IRs[i].reg2.end(), '[') == 1) {
                        sym.dem = 1;
                        if (regex_match(IRs[i].reg2, results, pattern)) {
                            sym.dimension1 = results[1];
                        }
                        space = stoi(sym.dimension1);
                    }
                    else {
                        sym.dem = 2;
                        if (regex_match(IRs[i].reg2, results, pattern)) {
                            sym.dimension1 = results[1];
                            sym.dimension2 = results[2];
                        }
                        space = stoi(sym.dimension1) * stoi(sym.dimension2);
                    }
                    sprintf(buffer, "addi $sp, $sp, -%d\n", space * 4);
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    stackRegister = stackRegister + 4 * space;
                    sym.toFrameReg = stackRegister - 4;
                    symTable.insert_Var_to_SymTable(sym);
                }
                break;
            case ASSIGN: {
                if (IRs[i].reg1 == "#RET") {
                    char buffer[MIPS_TEXT_LENGTH];
                    int tempReg;
                    if (isNum(IRs[i].reg2)) {
                        tempReg = getUseReg();
                        sprintf(buffer, "li $%s, %s\n", regManage.regs[tempReg].registerName.c_str(), IRs[i].reg2.c_str());
                        //MipsTextCode.emplace_back(buffer);
                        mipsfile << buffer;
                    }
                    else {
                        tempReg = regManage.isInUsage(IRs[i].reg2);
                        if (tempReg == -1) {
                            tempReg = loadToRegister(IRs[i].reg2, to_string(0), false);
                        }
                    }
                    sprintf(buffer, "move $v0, $%s\n", regManage.regs[tempReg].registerName.c_str());
                    regManage.regs[tempReg].isUsing = false;
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                }
                else if (IRs[i].reg1[0] == '#') {
                    int toLoad = getUseReg();
                    regManage.regs[toLoad].varName = IRs[i].reg1;
                    sprintf(buffer, "move $%s, $v0\n", regManage.regs[toLoad].registerName.c_str());
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    regManage.regs[toLoad].isUsing = true;
                }
                else {
                    storeToAddress(IRs[i].reg1, to_string(0), IRs[i].reg2);
                }
            }
                break;
            case SUBU:
                if (isNum(IRs[i].reg2) && isNum(IRs[i].reg3)) {
                    int pos = getUseReg();
                    string rs = regManage.regs[pos].registerName;
                    sprintf(buffer, "li $%s, %s\n", rs.c_str(), IRs[i].reg2.c_str());
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    sprintf(buffer, "subiu $%s, $%s, %s\n", rs.c_str(), rs.c_str(), IRs[i].reg3.c_str());
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    regManage.regs[pos].varName = IRs[i].reg1;
                    regManage.regs[pos].isUsing = true;
                }
                else if (isNum(IRs[i].reg2)) {
                    int pos = getUseReg();
                    string rd = regManage.regs[pos].registerName;
                    string rs = regManage.regs[pos].registerName;
                    int toLoad = loadToRegister(IRs[i].reg3, to_string(0), false);
                    sprintf(buffer, "li $%s, %s\n", rs.c_str(), IRs[i].reg2.c_str());
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    sprintf(buffer, "subu $%s, $%s, $%s\n", rd.c_str(),
                            rs.c_str(), regManage.regs[toLoad].registerName.c_str());
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    regManage.regs[toLoad].isUsing = false;
                    regManage.regs[pos].varName = IRs[i].reg1;
                    regManage.regs[pos].isUsing = true;
                }
                else if (isNum(IRs[i].reg3)) {
                    int pos = getUseReg();
                    string rd = regManage.regs[pos].registerName;
                    int toLoad = loadToRegister(IRs[i].reg2, to_string(0), false);
                    sprintf(buffer, "subiu $%s, $%s, %s\n", rd.c_str(),
                            regManage.regs[toLoad].registerName.c_str(), IRs[i].reg3.c_str());
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    regManage.regs[toLoad].isUsing = false;
                    regManage.regs[pos].varName = IRs[i].reg1;
                    regManage.regs[pos].isUsing = true;
                }
                else {
                    int pos = getUseReg();
                    string rd = regManage.regs[pos].registerName;
                    int toLoad1 = loadToRegister(IRs[i].reg2, to_string(0), false);
                    regManage.regs[toLoad1].isUsing = true;
                    regManage.regs[toLoad1].varName = IRs[i].reg2;
                    int toLoad2 = loadToRegister(IRs[i].reg3, to_string(0), false);
                    sprintf(buffer, "subu $%s, $%s, $%s\n", rd.c_str(),
                            regManage.regs[toLoad1].registerName.c_str(),
                            regManage.regs[toLoad2].registerName.c_str());
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    regManage.regs[toLoad1].isUsing = false;
                    regManage.regs[toLoad2].isUsing = false;
                    regManage.regs[pos].varName = IRs[i].reg1;
                    regManage.regs[pos].isUsing = true;
                }
                break;
            case MULTI: {
                int pos1, pos2;
                if (isNum(IRs[i].reg2)) {
                    pos1 = getUseReg();
                    string rs = regManage.regs[pos1].registerName;
                    sprintf(buffer, "li $%s, %s\n", rs.c_str(), IRs[i].reg2.c_str());
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    regManage.regs[pos1].varName = "#tempNum";
                } else {
                    pos1 = loadToRegister(IRs[i].reg2, to_string(0), false);
                    regManage.regs[pos1].varName = IRs[i].reg2;
                }
                regManage.regs[pos1].isUsing = true;
                if (isNum(IRs[i].reg3)) {
                    pos2 = getUseReg();
                    string rs = regManage.regs[pos2].registerName;
                    sprintf(buffer, "li $%s, %s\n", rs.c_str(), IRs[i].reg3.c_str());
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    regManage.regs[pos2].varName = "#tempNum";
                } else {
                    pos2 = loadToRegister(IRs[i].reg3, to_string(0), false);
                    regManage.regs[pos2].varName = IRs[i].reg2;
                }
                regManage.regs[pos2].isUsing = true;
                int pos = getUseReg();
                sprintf(buffer, "mult $%s, $%s\n", regManage.regs[pos1].registerName.c_str(),
                        regManage.regs[pos2].registerName.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                sprintf(buffer, "mflo $%s\n", regManage.regs[pos].registerName.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                regManage.regs[pos1].isUsing = false;
                regManage.regs[pos2].isUsing = false;
                regManage.regs[pos].isUsing = true;
                regManage.regs[pos].varName = IRs[i].reg1;
            }
                break;
            case DIVI: {
                int pos1, pos2;
                if (isNum(IRs[i].reg2)) {
                    pos1 = getUseReg();
                    string rs = regManage.regs[pos1].registerName;
                    sprintf(buffer, "li $%s, %s\n", rs.c_str(), IRs[i].reg2.c_str());
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                } else {
                    pos1 = loadToRegister(IRs[i].reg2, to_string(0), false);
                }
                regManage.regs[pos1].isUsing = true;
                if (isNum(IRs[i].reg3)) {
                    pos2 = getUseReg();
                    string rs = regManage.regs[pos2].registerName;
                    sprintf(buffer, "li $%s, %s\n", rs.c_str(), IRs[i].reg3.c_str());
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                } else {
                    pos2 = loadToRegister(IRs[i].reg3, to_string(0), false);
                }
                regManage.regs[pos2].isUsing = true;
                int pos = getUseReg();
                sprintf(buffer, "div $%s, $%s\n", regManage.regs[pos1].registerName.c_str(),
                        regManage.regs[pos2].registerName.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                sprintf(buffer, "mflo $%s\n", regManage.regs[pos].registerName.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                regManage.regs[pos1].isUsing = false;
                regManage.regs[pos2].isUsing = false;
                regManage.regs[pos].isUsing = true;
                regManage.regs[pos].varName = IRs[i].reg1;
            }
                break;
            case MODI: {
                int pos1, pos2;
                if (isNum(IRs[i].reg2)) {
                    pos1 = getUseReg();
                    string rs = regManage.regs[pos1].registerName;
                    sprintf(buffer, "li $%s, %s\n", rs.c_str(), IRs[i].reg2.c_str());
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                } else {
                    pos1 = loadToRegister(IRs[i].reg2, to_string(0), false);
                }
                regManage.regs[pos1].isUsing = true;
                if (isNum(IRs[i].reg3)) {
                    pos2 = getUseReg();
                    string rs = regManage.regs[pos2].registerName;
                    sprintf(buffer, "li $%s, %s\n", rs.c_str(), IRs[i].reg3.c_str());
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                } else {
                    pos2 = loadToRegister(IRs[i].reg3, to_string(0), false);
                }
                regManage.regs[pos2].isUsing = true;
                int pos = getUseReg();
                sprintf(buffer, "div $%s, $%s\n", regManage.regs[pos1].registerName.c_str(),
                        regManage.regs[pos2].registerName.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                sprintf(buffer, "mfhi $%s\n", regManage.regs[pos].registerName.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                regManage.regs[pos1].isUsing = false;
                regManage.regs[pos2].isUsing = false;
                regManage.regs[pos].isUsing = true;
                regManage.regs[pos].varName = IRs[i].reg1;
            }
                break;
            case NOTI: {
                //use SLTU
                int res = getUseReg();
                regManage.regs[res].isUsing = true;
                int pos;
                if (isNum(IRs[i].reg2)) {
                    pos = getUseReg();
                    sprintf(buffer, "li $%s, %s\n", regManage.regs[pos].registerName.c_str(),
                            IRs[i].reg2.c_str());
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    regManage.regs[pos].isUsing = true;
                }
                else {
                    pos = loadToRegister(IRs[i].reg2, to_string(0), false);
                    regManage.regs[pos].isUsing = true;
                }
                sprintf(buffer, "sltiu $%s, $%s, 1\n",
                        regManage.regs[res].registerName.c_str(),
                        regManage.regs[pos].registerName.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                regManage.regs[pos].isUsing = false;
                regManage.regs[res].varName = IRs[i].reg1;
            }
                break;
            case FUNC:
                break;
            case PARA:
                break;
            case INTFUNCTION:{
                sprintf(buffer, "_label_%s:\n", IRs[i].reg1.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                if (IRs[i].reg1 != "main") {
                    insert_Func_Symbol(IRs[i].reg1);
                    toPushFuncStackFrame.push_back(stackRegister);
                    stackRegister = 4 * (* (--toRemoveParamsNumber.end()));
                }
            }
                break;
            case VOIDFUNCTION: {
                sprintf(buffer, "_label_%s:\n", IRs[i].reg1.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                insert_Func_Symbol(IRs[i].reg1);
                toPushFuncStackFrame.push_back(stackRegister);
                stackRegister = 4 * (* (--toRemoveParamsNumber.end()));
            }
                break;
            case ENDFUNCTION: {
                int j;
                for (j = 0; j < regManage.regs.size(); j ++) {
                    regManage.regs[j].isUsing = false;
                }
                sprintf(buffer, "addiu $sp, $sp, %d\n", stackRegister);
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                stackRegister = (* (-- toPushFuncStackFrame.end()));
                toPushFuncStackFrame.pop_back();
                remove_Layer(layer_num + 1, * (--toRemoveParamsNumber.end()));
                printf("remove %d\n", * (--toRemoveParamsNumber.end()));
                toRemoveParamsNumber.pop_back();
                sprintf(buffer, "jr $ra\n");
                //MipsTextCode.emplace_back(buffer);
                for (auto & reg : regManage.regs) {
                    reg.isUsing = false;
                }
                mipsfile << buffer;
            }
                break;
            case PRINTVAR: {
                int pos;
                if (isNum(IRs[i].reg1)) {
                    pos = moveToReg(IRs[i].reg1);
                }
                else {
                    pos = loadToRegister(IRs[i].reg1, to_string(0), false);
                }
                sprintf(buffer, "move $a0, $%s\n", regManage.regs[pos].registerName.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                sprintf(buffer, "li $v0, 1\n");
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                sprintf(buffer, "syscall\n");
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                regManage.regs[pos].isUsing = false;
            }
                break;
            case PRINTSTR: {
                string printStr;
                printStr = printStr.append(IRs[i].reg1);
                while (IRs[i + 1].instruction == PRINTSTR) {
                    i ++;
                    printStr = printStr.append(IRs[i].reg1);
                }
                string strName = "$Str" + to_string(stringLine);
                stringLine ++;
                sprintf(buffer, "%s: .asciiz \"%s\"\n", strName.c_str(), printStr.c_str());
                MipsAsciizCode.emplace_back(buffer);
                sprintf(buffer, "la $a0, %s\n", strName.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                sprintf(buffer, "li $v0, 4\n");
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                sprintf(buffer, "syscall\n");
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
            }
                break;
            case RETURN: {
                sprintf(buffer, " addiu $sp, $sp, %d\n", stackRegister);
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                sprintf(buffer, "jr $ra\n");
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
            }
                break;
            case CONST:
                break;
            case CMP: {
                if (IRs[i].reg3 == "$$none") {
                    int pos1;
                    if (isNum(IRs[i].reg1)) {
                        pos1 = moveToReg(IRs[i].reg1);
                        regManage.regs[pos1].varName = "#tempNum";
                    } else {
                        pos1 = loadToRegister(IRs[i].reg1, to_string(0), false);
                        regManage.regs[pos1].varName = IRs[i].reg1;
                    }
                    regManage.regs[pos1].isUsing = true;
                    //sprintf(buffer, "li $t9, 0\n");
                    //MipsTextCode.emplace_back(buffer);
                    //mipsfile << buffer;
                    sprintf(buffer, "beq $%s, $0, %s\n",
                            regManage.regs[pos1].registerName.c_str(),
                            IRs[i].reg2.c_str());
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                    regManage.regs[pos1].isUsing = false;
                }
                else {
                    int pos1, pos2;
                    if (isNum(IRs[i].reg1)) {
                        pos1 = moveToReg(IRs[i].reg1);
                        regManage.regs[pos1].varName = "#tempNum";
                    } else {
                        pos1 = loadToRegister(IRs[i].reg1, to_string(0), false);
                        regManage.regs[pos1].varName = IRs[i].reg1;
                    }
                    regManage.regs[pos1].isUsing = true;
                    if (isNum(IRs[i].reg2)) {
                        pos2 = moveToReg(IRs[i].reg2);
                        regManage.regs[pos2].varName = "#tempNum";
                    }
                    else {
                        pos2 = loadToRegister(IRs[i].reg2, to_string(0), false);
                        regManage.regs[pos2].varName = IRs[i].reg2;
                    }
                    regManage.regs[pos2].isUsing = true;
                    if (IRs[i].global) {
                        sprintf(buffer, "bne $%s, $%s, %s\n", regManage.regs[pos1].registerName.c_str(),
                                regManage.regs[pos2].registerName.c_str(),
                                IRs[i].reg3.c_str());
                        mipsfile << buffer;
                        regManage.regs[pos1].isUsing = false;
                        regManage.regs[pos2].isUsing = false;
                    }
                    else {
                        sprintf(buffer, "beq $%s, $%s, %s\n", regManage.regs[pos1].registerName.c_str(),
                                regManage.regs[pos2].registerName.c_str(),
                                IRs[i].reg3.c_str());
                        mipsfile << buffer;
                        regManage.regs[pos1].isUsing = false;
                        regManage.regs[pos2].isUsing = false;
                    }
                }
            }
                break;
            case BNE: {
                //if not equal, jump
                int pos1, pos2;
                if (isNum(IRs[i].reg2)) {
                    pos1 = moveToReg(IRs[i].reg2);
                    regManage.regs[pos1].varName = "#tempNum";
                }
                else {
                    pos1 = loadToRegister(IRs[i].reg2, to_string(0), false);
                    regManage.regs[pos1].varName = IRs[i].reg2;
                }
                regManage.regs[pos1].isUsing = true;
                if (isNum(IRs[i].reg3)) {
                    pos2 = moveToReg(IRs[i].reg3);
                    regManage.regs[pos2].varName = "#tempNum";
                }
                else {
                    pos2 = loadToRegister(IRs[i].reg3, to_string(0), false);
                    regManage.regs[pos2].varName = IRs[i].reg3;
                }
                regManage.regs[pos2].isUsing = true;
                int res = getUseReg();
                sprintf(buffer, "xor $%s, $%s, $%s\n",
                        regManage.regs[res].registerName.c_str(),
                        regManage.regs[pos1].registerName.c_str(),
                        regManage.regs[pos2].registerName.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                sprintf(buffer, "sltiu $%s, $%s, 1\n",
                        regManage.regs[res].registerName.c_str(),
                        regManage.regs[res].registerName.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                sprintf(buffer, "sltiu $%s, $%s, 1\n",
                        regManage.regs[res].registerName.c_str(),
                        regManage.regs[res].registerName.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                regManage.regs[pos1].isUsing = false;
                regManage.regs[pos2].isUsing = false;
                regManage.regs[res].isUsing = true;
                regManage.regs[res].varName = IRs[i].reg1;
            }
                break;
            case BGT: {
                //slt
                int pos1, pos2;
                if (isNum(IRs[i].reg2)) {
                    pos1 = moveToReg(IRs[i].reg2);
                }
                else {
                    pos1 = loadToRegister(IRs[i].reg2, to_string(0), false);
                }
                regManage.regs[pos1].isUsing = true;
                if (isNum(IRs[i].reg3)) {
                    pos2 = moveToReg(IRs[i].reg3);
                }
                else {
                    pos2 = loadToRegister(IRs[i].reg3, to_string(0), false);
                }
                regManage.regs[pos2].isUsing = true;
                int res = getUseReg();
                regManage.regs[res].isUsing = true;
                sprintf(buffer, "slt $%s, $%s, $%s\n",
                        regManage.regs[res].registerName.c_str(),
                        regManage.regs[pos2].registerName.c_str(),
                        regManage.regs[pos1].registerName.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                regManage.regs[res].varName = IRs[i].reg1;
                regManage.regs[pos1].isUsing = false;
                regManage.regs[pos2].isUsing = false;
            }
                break;
            case BLT: {
                //slt
                int pos1, pos2;
                if (isNum(IRs[i].reg2)) {
                    pos1 = moveToReg(IRs[i].reg2);
                }
                else {
                    pos1 = loadToRegister(IRs[i].reg2, to_string(0), false);
                }
                regManage.regs[pos1].isUsing = true;
                if (isNum(IRs[i].reg3)) {
                    pos2 = moveToReg(IRs[i].reg3);
                }
                else {
                    pos2 = loadToRegister(IRs[i].reg3, to_string(0), false);
                }
                regManage.regs[pos2].isUsing = true;
                int res = getUseReg();
                regManage.regs[res].isUsing = true;
                sprintf(buffer, "slt $%s, $%s, $%s\n",
                        regManage.regs[res].registerName.c_str(),
                        regManage.regs[pos1].registerName.c_str(),
                        regManage.regs[pos2].registerName.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                regManage.regs[res].varName = IRs[i].reg1;
                regManage.regs[pos1].isUsing = false;
                regManage.regs[pos2].isUsing = false;
            }
                break;
            case BGE: {
                //slt
                int pos1, pos2;
                if (isNum(IRs[i].reg2)) {
                    pos1 = moveToReg(IRs[i].reg2);
                }
                else {
                    pos1 = loadToRegister(IRs[i].reg2, to_string(0), false);
                }
                regManage.regs[pos1].isUsing = true;
                if (isNum(IRs[i].reg3)) {
                    pos2 = moveToReg(IRs[i].reg3);
                }
                else {
                    pos2 = loadToRegister(IRs[i].reg3, to_string(0), false);
                }
                regManage.regs[pos2].isUsing = true;
                int res = getUseReg();
                regManage.regs[res].isUsing = true;
                sprintf(buffer, "slt $%s, $%s, $%s\n",
                        regManage.regs[res].registerName.c_str(),
                        regManage.regs[pos1].registerName.c_str(),
                        regManage.regs[pos2].registerName.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                sprintf(buffer, "xor $%s, $%s, 1\n",
                        regManage.regs[res].registerName.c_str(),
                        regManage.regs[res].registerName.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                regManage.regs[res].varName = IRs[i].reg1;
                regManage.regs[pos1].isUsing = false;
                regManage.regs[pos2].isUsing = false;
            }
                break;
            case BLE: {
                //slt
                int pos1, pos2;
                if (isNum(IRs[i].reg2)) {
                    pos1 = moveToReg(IRs[i].reg2);
                }
                else {
                    pos1 = loadToRegister(IRs[i].reg2, to_string(0), false);
                }
                regManage.regs[pos1].isUsing = true;
                if (isNum(IRs[i].reg3)) {
                    pos2 = moveToReg(IRs[i].reg3);
                }
                else {
                    pos2 = loadToRegister(IRs[i].reg3, to_string(0), false);
                }
                regManage.regs[pos2].isUsing = true;
                int res = getUseReg();
                regManage.regs[res].isUsing = true;
                sprintf(buffer, "slt $%s, $%s, $%s\n",
                        regManage.regs[res].registerName.c_str(),
                        regManage.regs[pos2].registerName.c_str(),
                        regManage.regs[pos1].registerName.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                sprintf(buffer, "xor $%s, $%s, 1\n",
                        regManage.regs[res].registerName.c_str(),
                        regManage.regs[res].registerName.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                regManage.regs[res].varName = IRs[i].reg1;
                regManage.regs[pos1].isUsing = false;
                regManage.regs[pos2].isUsing = false;
            }
                break;
            case GOTO: {
                //j
                sprintf(buffer, "j %s\n", IRs[i].reg1.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
            }
                break;
            case INT:
                break;
            case VOID:
                break;
            case LBRACE: {
                layer_num++;
                mark[layer_num] = symTable.symbolTable.size();
                storedReg stR;
                stR.name = "sp";
                stR.number = stackRegister;
                storedRegs.push_back(stR);
            }
                break;
            case RBRACE: {
                int number = (-- storedRegs.end())->number;
                sprintf(buffer, "addiu $sp, $sp, %d\n", (stackRegister - number));
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                stackRegister = number;
                storedRegs.pop_back();
                remove_Layer(layer_num, 0);
                layer_num --;
            }
                break;
            case GETARR: {
                int toLoad = loadToRegister(IRs[i].reg2, IRs[i].reg3, true);
                regManage.regs[toLoad].varName = IRs[i].reg1;
                regManage.regs[toLoad].isUsing = true;
            }
                break;
            case GETINT: {
                sprintf(buffer, "li $v0, 5\n");
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                sprintf(buffer, "syscall\n");
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                int pos = getUseReg();
                sprintf(buffer, "move $%s, $v0\n", regManage.regs[pos].registerName.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                regManage.regs[pos].isUsing = true;
                regManage.regs[pos].varName = IRs[i].reg1;
            }
                break;
            case ASSIGNARR: {
                storeToAddress(IRs[i].reg1, IRs[i].reg2, IRs[i].reg3);
            }
                break;
            case ENDCALL: {
                remove_Layer(layer_num, 0);
                layer_num --;
                stackRegister = 0;
                int number = * (--frameReg.end());
                frameReg.pop_back();
                number = number - * (--frameReg.end());
                sprintf(buffer, "addiu $sp, $fp, %d\n", number);
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                stackRegister = * (--frameReg.end());
                storedRegs.pop_back();
                frameReg.pop_back();
                load_Used_Reg();
                sprintf(buffer, "addiu $sp, $fp, %d\n", -1 * stackRegister);
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                remove_Layer(layer_num + 1, * (--toRemoveParamsNumber.end()));
                printf("remove %d\n", * (--toRemoveParamsNumber.end()));
                toRemoveParamsNumber.pop_back();
            }
                break;
            case BEFOREPUSH: {
                store_Used_Reg();
                storedReg stR;
                stR.name = "sp";
                stR.number = stackRegister;
                storedRegs.push_back(stR);
                frameReg.push_back(stackRegister);
            }
                break;
            case LABEL: {
                //generate label
                sprintf(buffer, "%s:\n", IRs[i].reg1.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
            }
                break;
            case BEQ: {
                //if beq, jump
                int pos1, pos2;
                if (isNum(IRs[i].reg2)) {
                    pos1 = moveToReg(IRs[i].reg2);
                    regManage.regs[pos1].varName = "#tempNum";
                }
                else {
                    pos1 = loadToRegister(IRs[i].reg2, to_string(0), false);
                    regManage.regs[pos1].varName = IRs[i].reg2;
                }
                regManage.regs[pos1].isUsing = true;
                if (isNum(IRs[i].reg3)) {
                    pos2 = moveToReg(IRs[i].reg3);
                    regManage.regs[pos2].varName = "#tempNum";
                }
                else {
                    pos2 = loadToRegister(IRs[i].reg3, to_string(0), false);
                    regManage.regs[pos2].varName = IRs[i].reg3;
                }
                regManage.regs[pos2].isUsing = true;
                int res = getUseReg();
                sprintf(buffer, "xor $%s, $%s, $%s\n",
                        regManage.regs[res].registerName.c_str(),
                        regManage.regs[pos1].registerName.c_str(),
                        regManage.regs[pos2].registerName.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                sprintf(buffer, "sltiu $%s, $%s, 1\n",
                        regManage.regs[res].registerName.c_str(),
                        regManage.regs[res].registerName.c_str());
                //MipsTextCode.emplace_back(buffer);
                mipsfile << buffer;
                regManage.regs[pos1].isUsing = false;
                regManage.regs[pos2].isUsing = false;
                regManage.regs[res].isUsing = true;
                regManage.regs[res].varName = IRs[i].reg1;
            }
                break;
            case LEAVEBRACE: {
                int siz = stoi(IRs[i].reg1);
                int number = 0;
                if (siz != 0) {
                    number = (storedRegs.end() - siz)->number;
                    sprintf(buffer, "addiu $sp, $sp, %d\n", stackRegister - number);
                    //MipsTextCode.emplace_back(buffer);
                    mipsfile << buffer;
                }
            }
                break;
        }
    }
    mipsfile << "_end_main_:\n";
    printf("1");
}