//
// Created by ZIMUQIN on 2021/11/13.
//

#include <iostream>
#include <fstream>
#include <cstring>
#include<vector>
#include "astree.h"
#include "SymTable.h"
#include "MipsGenerator.h"
#include <map>
#include "registerManager.h"

using namespace std;

void regManager::buildManager() {
    reg reg_t0;
    reg_t0.registerName = "t0";
    regs.push_back(reg_t0);
    reg reg_t1;
    reg_t1.registerName = "t1";
    regs.push_back(reg_t1);
    reg reg_t2;
    reg_t2.registerName = "t2";
    regs.push_back(reg_t2);
    reg reg_t3;
    reg_t3.registerName = "t3";
    regs.push_back(reg_t3);
    reg reg_t4;
    reg_t4.registerName = "t4";
    regs.push_back(reg_t4);
    reg reg_t5;
    reg_t5.registerName = "t5";
    regs.push_back(reg_t5);
    reg reg_t6;
    reg_t6.registerName = "t6";
    regs.push_back(reg_t6);
    reg reg_t7;
    reg_t7.registerName = "t7";
    regs.push_back(reg_t7);
    reg reg_s0;
    reg_t7.registerName = "s0";
    regs.push_back(reg_t7);
    reg reg_s1;
    reg_t7.registerName = "s1";
    regs.push_back(reg_t7);
    reg reg_s2;
    reg_t7.registerName = "s2";
    regs.push_back(reg_t7);
    regMap.push_back(0);
    regMap.push_back(1);
    regMap.push_back(2);
    regMap.push_back(3);
    regMap.push_back(4);
    regMap.push_back(5);
    regMap.push_back(6);
    regMap.push_back(7);
    regMap.push_back(8);
    regMap.push_back(9);
    regMap.push_back(10);
}

int regManager::findSpareReg() {
    int i;
    for (i = 0; i < regs.size(); i ++) {
        if (!regs[i].isUsing) {
            return i;
        }
    }
    return -1;
}

int regManager::isInUsage(std::string name) {
    int i;
    for (i = 0; i < regs.size(); i ++) {
        if (regs[i].varName == name && regs[i].isUsing) {
            return i;
        }
    }
    return -1;
}

int regManager::findReg(std::string regName) {
    int i;
    for (i = 0; i < regs.size(); i ++) {
        if (regs[i].registerName == regName) {
            return i;
        }
    }
    return -1;
}