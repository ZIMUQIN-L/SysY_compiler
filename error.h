//
// Created by ZIMUQIN on 2021/12/4.
//

#ifndef ERRORHANDLERUPD_ERROR_H
#define ERRORHANDLERUPD_ERROR_H
void insertError(int lineCount, std::string type);
int Error_A_Analyzer(std::string StrCon);
int Error_H_Analyzer(int layNum, std::string name);
int Error_L_Analyzer(std::string strCon, int expNumber);
int checkErrorIdent(int flag, int layNum, std::string name);
struct myError {
    std::string type;
    int lineCount;
};
#endif //ERRORHANDLERUPD_ERROR_H
