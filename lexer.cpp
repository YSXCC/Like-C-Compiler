#include "lexer.h"
#include "token.h"
#include "error.h"
#include "compiler.h"

/*******************************************************************************
                                   扫描器
*******************************************************************************/
Scanner::Scanner(char* name){
	file = fopen(name,"r");
    if(!file){
        printf("文件%s打开失败！请检查文件名字和路径！\n",name);
        Error::errorNum++;          //错误数增加
    }
    fileName = name;

    //初始化扫描状态
    lineLenth = 0;
    readPosition = -1;
    lastChar = 0;
    lineNum = 1;
    colNum = 0;
}

Scanner::~Scanner(){
    if(file){
        printf("文件%s没有被扫描完全！\n");
        Error::warnNum++;
        fclose(file);
    }
}

/*
	显示字符
*/
void Scanner::showChar(char ch){
    if(ch==-1) printf("EOF");               //文件末尾
	else if(ch=='\n') printf("\\n");        //换行
	else if(ch=='\t') printf("\\t");        //TAB键
	else if(ch==' ')  printf("<blank>");    //空格
	else printf("%c",ch);	
	printf("\t\t<%d>\n",ch);                //不太清楚是什么
}

/*
	基于缓冲区的字符扫描算法,文件扫描接受后自动关闭文件
	缓冲区：使用fread读取效率更高，数据更集中
	字符：从缓冲区内索引获取
*/
int Scanner::scan(){
    if(!file){
        return -1;
    }
    if(readPosition == lineLenth - 1){      //缓冲区读取完毕
        lineLenth = fread(line,1,BUFFERLENTH,file);     //读取八十个字节到缓冲区数组，每次一个字节,返回BUFFERLENTH
        if(lineLenth == 0){ //没有数据
            lineLenth = 0;
            line[0] = -1;   //读取结束，返回文件标志-1
        }
        readPosition = -1;  //恢复读取line位置
    }
    readPosition++;         //移动读取line位置
    char ch = line[readPosition];       //获取当前字符
    if(lastChar == '\n'){
        lineNum++;      //行号加一
        colNum = 0;     //列号清空
    }
    if(ch == -1){       //文件结束标志
        fclose(file);
        file = NULL;
    }
    else if(ch != '\n'){//不是换行
        colNum++;
    }
    lastChar = ch;      //记录当前字符为上一个字符
    if(Args::showChar){
        showChar(ch);
    }
    return ch;
}

/*
	获取文件名
*/
char* Scanner::getFile(){
	return fileName;
}

/*
	获取行号
*/
int Scanner::getLine(){
	return lineNum;
}

/*
	获取列号
*/
int Scanner::getCol(){
	return colNum;
}

/*******************************************************************************
                                   关键字表
*******************************************************************************/

/*
	关键字列表初始化
*/
Keywords::Keywords(){

    //add keyword mapping here
	keywords["int"]=KW_INT;
	keywords["char"]=KW_CHAR;
	keywords["void"]=KW_VOID;
	keywords["extern"]=KW_EXTERN;
	keywords["if"]=KW_IF;
	keywords["else"]=KW_ELSE;
	keywords["switch"]=KW_SWITCH;
	keywords["case"]=KW_CASE;
	keywords["default"]=KW_DEFAULT;
	keywords["while"]=KW_WHILE;
	keywords["do"]=KW_DO;
	keywords["for"]=KW_FOR;
	keywords["break"]=KW_BREAK;
	keywords["continue"]=KW_CONTINUE;
	keywords["return"]=KW_RETURN;
}

/*
	测试是否是关键字
*/
Tag Keywords::getTag(string name){
    //如果找到就返回对应的关键字，没有找到就把它确定为标识符
	return keywords.find(name)!=keywords.end()?keywords[name]:ID;
}

/*******************************************************************************
                                   词法分析器
*******************************************************************************/
Keywords Lexer::keywords;

Lexer::Lexer (Scanner & sc)