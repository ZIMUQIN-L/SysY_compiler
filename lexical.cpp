//
// Created by ZIMUQIN on 2021/10/20.
//
#include <iostream>
#include <fstream>
#include <string.h>
#include<vector>
#include "def.h"
using namespace std;

extern vector<couple> couples;
extern ifstream infile;
char tokin;
char nxt;
char ident[256];
int changed = 1;
int line = 1;

char getNextChar() {
    char a;
    if ((a = infile.get()) != EOF) {
        //outfile << "read" << infile.tellg() << endl;
        //outfile << "read " << a << endl;
        //infile_1.get();
        if (a == '\n') {
            line ++;
        }
        return a;
    }
    else {
        return EOF;
    }
}

void lexcialAnalasy() {
    while (tokin != EOF) {
        couple nowCp;
        if (tokin == '"') {
            memset(ident, 0, sizeof ident);
            int i = 0;
            ident[i++] = tokin;
            tokin = getNextChar();
            while (tokin != '"') {
                ident[i++] = tokin;
                tokin = getNextChar();
            }
            ident[i] = tokin;
            strcpy(nowCp.content, ident);
            char trans[] = "STRCON";
            strcpy(nowCp.type, trans);
            tokin = getNextChar();
            changed = 1;
            nowCp.lineCount = line;
            couples.push_back(nowCp);
            continue;
        }
        else if (tokin == '!') {
            nxt = getNextChar();
            if (nxt == '=') {
                tokin = getNextChar();
                char trans[] = "NEQ";
                strcpy(nowCp.type, trans);
                char trans1[] = "!=";
                strcpy(nowCp.content ,trans1);
                changed = 1;
                nowCp.lineCount = line;
                couples.push_back(nowCp);
                continue;
            }
            else {
                tokin = nxt;
                char trans[] = "NOT";
                strcpy(nowCp.type, trans);
                char trans1[] = "!";
                strcpy(nowCp.content ,trans1);
                changed = 1;
                nowCp.lineCount = line;
                couples.push_back(nowCp);
                continue;
            }
        }
        else if (tokin == '&') {
            nxt = getNextChar();
            if (nxt == '&') {
                tokin = getNextChar();
                char trans[] = "AND";
                strcpy(nowCp.type, trans);
                char trans1[] = "&&";
                strcpy(nowCp.content ,trans1);
                changed = 1;
                nowCp.lineCount = line;
                couples.push_back(nowCp);
                continue;
            }
            else {
                //todo
                //wrong matter
            }
        }
        else if (tokin == '|') {
            nxt = getNextChar();
            if (nxt == '|') {
                tokin = getNextChar();
                char trans[] = "OR";
                strcpy(nowCp.type, trans);
                char trans1[] = "||";
                strcpy(nowCp.content ,trans1);
                changed = 1;
                nowCp.lineCount = line;
                couples.push_back(nowCp);
                continue;
            }
            else {
                //todo
                //wrong format
            }
        }
        else if (tokin == '+') {
            tokin = getNextChar();
            char trans[] = "PLUS";
            strcpy(nowCp.type, trans);
            char trans1[] = "+";
            strcpy(nowCp.content ,trans1);
            changed = 1;
            nowCp.lineCount = line;
            couples.push_back(nowCp);
            continue;
        }
        else if (tokin == '-') {
            tokin = getNextChar();
            char trans[] = "MINU";
            strcpy(nowCp.type, trans);
            char trans1[] = "-";
            strcpy(nowCp.content, trans1);
            changed = 1;
            nowCp.lineCount = line;
            couples.push_back(nowCp);
            continue;
        }
        else if (tokin == '*') {
            tokin = getNextChar();
            char trans[] = "MULT";
            strcpy(nowCp.type, trans);
            char trans1[] = "*";
            strcpy(nowCp.content, trans1);
            changed = 1;
            nowCp.lineCount = line;
            couples.push_back(nowCp);
            continue;
        }
        else if (tokin == '/') {
            nxt = getNextChar();
            if (nxt == '*') {
                char a, b;
                a = getNextChar();
                b = getNextChar();
                while (!(a == '*' && b == '/')) {
                    a = b;
                    b = getNextChar();
                }
                //notes
                tokin = getNextChar();
                continue;
            }
            else if (nxt == '/') {
                tokin = getNextChar();
                while (tokin != '\n') {
                    if (tokin == EOF) {
                        break;
                    }
                    tokin = getNextChar();
                }
                if (tokin == EOF) {
                    continue;
                }
                tokin = getNextChar();
                continue;
            }
            else {
                tokin = nxt;
                char trans[] = "DIV";
                strcpy(nowCp.type, trans);
                char trans1[] = "/";
                strcpy(nowCp.content, trans1);
                changed = 1;
                nowCp.lineCount = line;
                couples.push_back(nowCp);
                continue;
            }
        }
        else if (tokin == '%') {
            tokin = getNextChar();
            char trans[] = "MOD";
            strcpy(nowCp.type, trans);
            char trans1[] = "%";
            strcpy(nowCp.content, trans1);
            changed = 1;
            nowCp.lineCount = line;
            couples.push_back(nowCp);
            continue;
        }
        else if (tokin == '<') {
            nxt = getNextChar();
            if (nxt == '=') {
                tokin = getNextChar();
                char trans[] = "LEQ";
                strcpy(nowCp.type, trans);
                char trans1[] = "<=";
                strcpy(nowCp.content, trans1);
                changed = 1;
                nowCp.lineCount = line;
                couples.push_back(nowCp);
                continue;
            }
            else {
                tokin = nxt;
                char trans[] = "LSS";
                strcpy(nowCp.type, trans);
                char trans1[] = "<";
                strcpy(nowCp.content, trans1);
                changed = 1;
                nowCp.lineCount = line;
                couples.push_back(nowCp);
                continue;
            }
        }
        else if (tokin == '>') {
            nxt = getNextChar();
            if (nxt == '=') {
                tokin = getNextChar();
                char trans[] = "GEQ";
                strcpy(nowCp.type, trans);
                char trans1[] = ">=";
                strcpy(nowCp.content, trans1);
                changed = 1;
                nowCp.lineCount = line;
                couples.push_back(nowCp);
                continue;
            }
            else {
                tokin = nxt;
                char trans[] = "GRE";
                strcpy(nowCp.type, trans);
                char trans1[] = ">";
                strcpy(nowCp.content, trans1);
                changed = 1;
                nowCp.lineCount = line;
                couples.push_back(nowCp);
                continue;
            }
        }
        else if (tokin == '=') {
            nxt = getNextChar();
            if (nxt == '=') {
                tokin = getNextChar();
                char trans[] = "EQL";
                strcpy(nowCp.type, trans);
                char trans1[] = "==";
                strcpy(nowCp.content, trans1);
                changed = 1;
                nowCp.lineCount = line;
                couples.push_back(nowCp);
                continue;
            }
            else {
                tokin = nxt;
                char trans[] = "ASSIGN";
                strcpy(nowCp.type, trans);
                char trans1[] = "=";
                strcpy(nowCp.content, trans1);
                changed = 1;
                nowCp.lineCount = line;
                couples.push_back(nowCp);
                continue;
            }
        }
        else if (tokin == ';') {
            tokin = getNextChar();
            char trans[] = "SEMICN";
            strcpy(nowCp.type, trans);
            char trans1[] = ";";
            strcpy(nowCp.content, trans1);
            changed = 1;
            nowCp.lineCount = line;
            couples.push_back(nowCp);
            continue;
        }
        else if (tokin == ',') {
            tokin = getNextChar();
            char trans[] = "COMMA";
            strcpy(nowCp.type, trans);
            char trans1[] = ",";
            strcpy(nowCp.content, trans1);
            changed = 1;
            nowCp.lineCount = line;
            couples.push_back(nowCp);
            continue;
        }
        else if (tokin == '(') {
            tokin = getNextChar();
            char trans[] = "LPARENT";
            strcpy(nowCp.type, trans);
            char trans1[] = "(";
            strcpy(nowCp.content, trans1);
            changed = 1;
            nowCp.lineCount = line;
            couples.push_back(nowCp);
            continue;
        }
        else if (tokin == ')') {
            tokin = getNextChar();
            char trans[] = "RPARENT";
            strcpy(nowCp.type, trans);
            char trans1[] = ")";
            strcpy(nowCp.content, trans1);
            changed = 1;
            nowCp.lineCount = line;
            couples.push_back(nowCp);
            continue;
        }
        else if (tokin == '[') {
            tokin = getNextChar();
            char trans[] = "LBRACK";
            strcpy(nowCp.type, trans);
            char trans1[] = "[";
            strcpy(nowCp.content, trans1);
            changed = 1;
            nowCp.lineCount = line;
            couples.push_back(nowCp);
            continue;
        }
        else if (tokin == ']') {
            tokin = getNextChar();
            char trans[] = "RBRACK";
            strcpy(nowCp.type, trans);
            char trans1[] = "]";
            strcpy(nowCp.content, trans1);
            changed = 1;
            nowCp.lineCount = line;
            couples.push_back(nowCp);
            continue;
        }
        else if (tokin == '{') {
            tokin = getNextChar();
            char trans[] = "LBRACE";
            strcpy(nowCp.type, trans);
            char trans1[] = "{";
            strcpy(nowCp.content, trans1);
            changed = 1;
            nowCp.lineCount = line;
            couples.push_back(nowCp);
            continue;
        }
        else if (tokin == '}') {
            tokin = getNextChar();
            char trans[] = "RBRACE";
            strcpy(nowCp.type, trans);
            char trans1[] = "}";
            strcpy(nowCp.content, trans1);
            changed = 1;
            nowCp.lineCount = line;
            couples.push_back(nowCp);
            continue;
        }
        else if ('0' <= tokin && tokin <= '9') {
            memset(ident, 0, sizeof ident);
            int i = 0;
            ident[i++] = tokin;
            tokin = getNextChar();
            while('0' <= tokin && tokin <= '9') {
                ident[i++] = tokin;
                tokin = getNextChar();
            }
            char trans[] = "INTCON";
            strcpy(nowCp.type, trans);
            strcpy(nowCp.content, ident);
            changed = 1;
            nowCp.lineCount = line;
            couples.push_back(nowCp);
            continue;
        }
        else if (tokin == '_' || ('a' <= tokin && tokin <= 'z')
                 || ('A' <= tokin && tokin <= 'Z')) {
            memset(ident, 0, sizeof ident);
            int i = 0;
            ident[i++] = tokin;
            tokin = getNextChar();
            while (tokin == '_' || ('a' <= tokin && tokin <= 'z')
                   || ('A' <= tokin && tokin <= 'Z')
                   || ('0' <= tokin && tokin <= '9')) {
                ident[i++] = tokin;
                tokin = getNextChar();
            }
            if (strcmp(ident, "const") == 0) {
                char trans[] = "CONSTTK";
                strcpy(nowCp.type, trans);
                strcpy(nowCp.content, ident);
                changed = 1;
            }
            else if (strcmp(ident, "main") == 0) {
                char trans[] = "MAINTK";
                strcpy(nowCp.type, trans);
                strcpy(nowCp.content, ident);
                changed = 1;
            }
            else if (strcmp(ident, "int") == 0) {
                char trans[] = "INTTK";
                strcpy(nowCp.type, trans);
                strcpy(nowCp.content, ident);
                changed = 1;
            }
            else if (strcmp(ident, "break") == 0) {
                char trans[] = "BREAKTK";
                strcpy(nowCp.type, trans);
                strcpy(nowCp.content, ident);
                changed = 1;
            }
            else if (strcmp(ident, "continue") == 0) {
                char trans[] = "CONTINUETK";
                strcpy(nowCp.type, trans);
                strcpy(nowCp.content, ident);
                changed = 1;
            }
            else if (strcmp(ident, "if") == 0) {
                char trans[] = "IFTK";
                strcpy(nowCp.type, trans);
                strcpy(nowCp.content, ident);
                changed = 1;
            }
            else if (strcmp(ident, "else") == 0) {
                char trans[] = "ELSETK";
                strcpy(nowCp.type, trans);
                strcpy(nowCp.content, ident);
                changed = 1;
            }
            else if (strcmp(ident, "while") == 0) {
                char trans[] = "WHILETK";
                strcpy(nowCp.type, trans);
                strcpy(nowCp.content, ident);
                changed = 1;
            }
            else if (strcmp(ident, "getint") == 0) {
                char trans[] = "GETINTTK";
                strcpy(nowCp.type, trans);
                strcpy(nowCp.content, ident);
                changed = 1;
            }
            else if (strcmp(ident, "printf") == 0) {
                char trans[] = "PRINTFTK";
                strcpy(nowCp.type, trans);
                strcpy(nowCp.content, ident);
                changed = 1;
            }
            else if (strcmp(ident, "return") == 0) {
                char trans[] = "RETURNTK";
                strcpy(nowCp.type, trans);
                strcpy(nowCp.content, ident);
                changed = 1;
            }
            else if (strcmp(ident, "void") == 0) {
                char trans[] = "VOIDTK";
                strcpy(nowCp.type, trans);
                strcpy(nowCp.content, ident);
                changed = 1;
            }
            else {
                char trans[] = "IDENFR";
                strcpy(nowCp.type, trans);
                strcpy(nowCp.content, ident);
                changed = 1;
            }
            nowCp.lineCount = line;
            couples.push_back(nowCp);
            continue;
        }
        tokin = getNextChar();
    }
}
