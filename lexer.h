#pragma once
#include "common.h"
#include <ext/hash_map>         //linux环境
using namespace __gnu_cxx;

/*******************************************************************************
                                   扫描器
*******************************************************************************/
class Scanner{
private:
    //文件名
    char * fileName;
    //文件指针
    FILE * file;

    //内部状态
    static const int BUFFERLENTH = 80;    //缓冲区长度设置为80
    char line[BUFFERLENTH];
    int lineLenth;      //当前行长度
    int readPosition;   //当前读取的位置
    char lastChar;      //上一个字符，用于判断换行位置

    //读取状态
    int lineNum;        //记录行号
    int colNum;         //记录列号

    void showChar();    //显示字符

public:

    //构造函数和析构函数
    Scanner(char* name);
    ~Scanner();

    //扫描
    int scan();     //基于字符缓冲区的读取算法，扫描完成后关闭文件

    //外部接口
	char* getFile();    //获取文件名
	int getLine();      //获取行号
	int getCol();       //获取列号
};


/*******************************************************************************
                                   关键字表
*******************************************************************************/
class Keywords{
    //hash函数
    struct string_hash{
        size_t operator()(const string& str) const{
            return __stl_hash_string(str.c_str());
        }
    };
};