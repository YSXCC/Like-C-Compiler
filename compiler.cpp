#include "compiler.h"
#include "lexer.h"
#include "error.h"

/*
	编译参数初始化
*/
bool Args::showChar=false;
bool Args::showToken=false;

void Compiler::lexerTest(char * file){
    //准备
	Scanner scanner(file);  //扫描器
	Error error(&scanner);  //错误处理
	Lexer lexer(scanner);   //词法分析器
    lexer.tokenize();
}