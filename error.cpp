#include "error.h"
#include "token.h"
#include "lexer.h"

int Error::errorNum = 0;   //错误数
int Error::warnNum  = 0;   //警告错误数
Scanner* Error::scanner = NULL;

/*
	初始化错误个数
*/
Error::Error(Scanner * sc)
{
	scanner = sc;          //扫描器
}


/*
	打印词法错误
*/
void Error::lexError(int code)
{
	//词法错误信息串
	static const char *lexErrorTable[]=
	{
		"字符串丢失右引号",
		"二进制数没有实体数据",
		"十六进制数没有实体数据",
		"字符丢失右单引号",
		"不支持空字符",
		"错误的或运算符",
		"多行注释没有正常结束",
		"词法记号不存在"
	};
	errorNum++;
	printf("文件:%s<%d行,%d列> 词法错误 : %s.\n",scanner->getFile(),
		scanner->getLine(),scanner->getLine(),lexErrorTable[code]);
}

/*
	获取错误个数
*/
int Error::getErrorNum()
{
	return errorNum;
}

/*
	获取警告个数
*/
int Error::getWarnNum()
{
	return warnNum;
}

