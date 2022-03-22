//
// Created by ZIMUQIN on 2021/10/20.
//
//cmake_minimum_required(VERSION 3.20)
//project(grammar_Ana)
//
//set(CMAKE_CXX_STANDARD 14)
//
//add_executable(grammar_Ana main.cpp)
//add_library(lexical lexical.h lexical.cpp)
//add_library(grammar grammar.h grammar.cpp)
//target_link_libraries(grammar_Ana lexical)
//target_link_libraries(grammar_Ana grammar)
#ifndef COMPLIER_ERROR_DEF_H
#define COMPLIER_ERROR_DEF_H
#include "astree.h"
class couple
{
public:
    char content[1000];
    char type[100];
    int lineCount;
};

enum symbol_type {
    VOID_FUNCTION, INT_FUNCTION, INTEGER, CONSTANT
};

struct param
{
public:
    symbol_type type;
    std::string name;
    int position;
    int layer;
    int dem;
    std::string dimension1;
    std::string dimension2;
    int sz[5];
};

struct symbol
{
public:
    std::string name;
    //show up line
    int position;
    //the layer of the ident
    int layer;
    symbol_type type;
    int dem;
    //数组每层大小
    int sz[5];
    std::vector<param> params;
    std::string dimension1;
    std::string dimension2;
    std::vector<std::string> value;
    //数组实际有效输入参数
    int validPara;
    int toFrameReg;
    bool isParam = false;
};

enum BackType {
    NORETURN, EXP_RETURN, SEM_RETURN
};

class func_Back {
public:
    symbol_type funcType;
    BackType backType;
    int layNow;
};
#endif //COMPLIER_ERROR_DEF_H
