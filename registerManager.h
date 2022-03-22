//
// Created by ZIMUQIN on 2021/11/13.
//

#ifndef MIPSGENERATOR_REGISTERMANAGER_H
#define MIPSGENERATOR_REGISTERMANAGER_H

struct reg {
    std::string registerName;
    std::string varName;
    bool isUsing = false;
};

class regManager {
public:
    std::vector<reg> regs;
    std::vector<int> regMap;

    void buildManager();
    int findSpareReg();
    int isInUsage(std::string name);
    int findReg(std::string regName);
};
#endif //MIPSGENERATOR_REGISTERMANAGER_H
