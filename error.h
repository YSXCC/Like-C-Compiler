#pragma once
#include "common.h"

/*
    错误类，就是用来报错的
*/
class Error{
private:
    static Scanner * scanner;   //使用当前扫描器
public:
    //构造和初始化
    Error(Scanner * sc);

    static int errorNum;        //错误个数
    static int warnNum;         //警告个数

    //外部接口
    static int getErrorNum();
    static int getWarnNum();

    //错误接口
    static void lexError(int code);     //打印词法错误
};