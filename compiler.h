#pragma once
#include "common.h"

/*
	编译器参数类
*/
class Args{
public:
	static bool showChar;       //显示字符
	static bool showToken;      //显示词法记号
	static bool showSymbol;     //显示符号表
};

/*
	编译器类
*/
class Compiler
{

public:
	//词法分析测试
	void lexerTest(char * file);
	//核心函数
	//void compile(char*file);//编译一个文件
};

