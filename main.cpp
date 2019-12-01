#include "compiler.h"
#include "error.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc,char *argv[])
{
    char* file = argv[1];
    cout<<"处理的文件是："<<file<<endl;
    Compiler compiler;
    compiler.lexerTest(file);
    int error = Error::getErrorNum();
	int warn  = Error::getWarnNum();
    cout<<"词法分析完成：错误="<<error<<",警告="<<warn<<"."<<endl;
    return 0;
}