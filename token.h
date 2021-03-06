#pragma once
#include "common.h"

/*******************************************************************************
                                   词法记号
*******************************************************************************/
class Token{
public:
    Tag tag;        //内部标签，表明是什么样子的东东。是关键字还是标识符还是运算符等等
    Token(Tag t);   //初始化
    virtual string toString();
    virtual ~Token();
};


/*******************************************************************************
                                   标识符
*******************************************************************************/
class Id:public Token{
public:
	string name;                    //标识符（自定义的变量名？）
	Id (string n);
	virtual string toString();
};

/*******************************************************************************
                                   字符串
*******************************************************************************/
class Str:public Token{
public:
    string str;
    Str(string s);
    virtual string toString();
};

/*******************************************************************************
                                   数字
*******************************************************************************/

class Num:public Token
{
public:
	int val;
	Num (int v);
	virtual string toString();
};

/*******************************************************************************
                                   字符
*******************************************************************************/

class Char:public Token
{
public:
	char ch;
	Char (char c);
	virtual string toString();
};
