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
        printf("文件%s没有被扫描完全！\n",fileName);
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
    // if(Args::showChar){
    //     showChar(ch);TODO:可能还没有写的函数
    // }
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

Lexer::Lexer (Scanner & sc):scanner(sc){    //const类型和引用类型必须在构造函数前赋值初始化
    token   = NULL;           //词法记号为空
    chInput = ' ';            //初始化为空格
}

Lexer::~Lexer(){
    if(!token){              //删除已经记录的词法记号变量的内存，防止内存溢出
        delete token;
    }
}

/*
	封装的扫描方法,处理预读字符
*/
bool Lexer::scan(char need)
{
	chInput = scanner.scan();       //扫描出字符
	if(need){
		if(chInput != need){        //与预期不吻合
            return false;
        }
		chInput = scanner.scan();   //与预期吻合，扫描下一个
		return true;
	}
	return true;
}

//打印词法错误
#define LEXERROR(code) Error::lexError(code)

/*
	有穷自动机匹配，词法记号解析
*/
Token* Lexer::tokenize(){
    for(;chInput != -1;){        //过滤掉无效的词法记号，只保留正常词法记号或者NULL
        Token* t = NULL;        //初始化Token指针
        /*****************************
            不要问为什么这里不用scan（）
            初始化那个鬼东西的chInput，
            因为会在else的case -1处处理
        *****************************/
        while(chInput==' '||chInput=='\n'||chInput=='\t'){  //忽略空白符
            scan();
        }
        //标识符，关键字
        if((chInput >= 'a' && chInput <= 'z') || (chInput >= 'A' && chInput <= 'Z') || chInput == '_'){
            string name = "";
            do{
                name.push_back(chInput);    //添加字符到末尾
                printf("This char is : %c\n",chInput);
                scan();
            }while((chInput >= 'a' && chInput <= 'z') || (chInput >= 'A' && chInput <= 'Z') || chInput == '_' || chInput >= '0' && chInput <= '9');
            //匹配结束
            Tag tag = keywords.getTag(name);
            if(tag == ID){  //标识符
                t = new Id(name);
            }
            else{           //关键字(后期专门写一个关键字类！！！)
                t = new Token(tag);
            }
        }
        //数字
        else if(chInput >= '0' && chInput <= '9'){
            int value = 0;
            if(chInput != '0'){   //十进制
                do{
                    value = value*10 + chInput - '0';
                    scan();
                }while(chInput >= '0' && chInput <= '9');
            }
            else{   //判断二进制还是十六进制还是八进制
                //TODO:未能实现八进制数,扫描时有BUG
                scan();
                if(chInput == 'x' || chInput == 'X'){   //十六进制 
                    scan();
                    if((chInput>='0'&&chInput<='9')||(chInput>='A'&&chInput<='F')||(chInput>='a'&&chInput<='f')){
						do{
							value = value*16 + chInput;
							if(chInput>='0'&&chInput<='9')value -= '0';
							else if(chInput>='A'&&chInput<='F')value += 10-'A';
							else if(chInput>='a'&&chInput<='f')value += 10-'a';							
							scan();
						}while((chInput>='0'&&chInput<='9')||(chInput>='A'&&chInput<='F')||(chInput>='a'&&chInput<='f'));
					}     
                    else{
                        //TODO:这个函数未被实现
						LEXERROR(NUM_HEX_TYPE);//0x后无数据或者乱写数据
						t = new Token(ERR);
					}        
                }
                else if(chInput == 'b' || chInput == 'B'){  //二进制数
                    scan();
					if(chInput>='0' && chInput<='1'){
						do{
							value=value*2 + chInput-'0';
							scan();
						}while(chInput >= '0' && chInput <= '1');
					}
					else{
						LEXERROR(NUM_BIN_TYPE);//0b后无数据或者乱写数据
						t = new Token(ERR);
					}
                }
            }
            if(!t){
                t = new Num(value);
            }
        }
        //字符串
        else if(chInput == '"'){
            string str = "";
            while(!scan('"')){  //字符串右双引号
                if(chInput == '\\'){    //遇到转义符
                    scan();
                    if(chInput=='n')            str.push_back('\n');    
					else if(chInput == '\\')    str.push_back('\\');    
					else if(chInput == 't')     str.push_back('\t');
					else if(chInput == '"')     str.push_back('"');
					else if(chInput == '0')     str.push_back('\0');
					else if(chInput == '\n');                           //什么也不做，字符串换行
					else if(chInput == -1){
						LEXERROR(STR_NO_R_QUTION);                      //字符串没有右双引号
						t = new Token(ERR);
						break;
					}
                    //TODO:在C++中会对\A,\a这样的提出WARNING
					else str.push_back(chInput);
                }
                else if(chInput == '\n'|| chInput ==-1){                //换行或者文件结束
					LEXERROR(STR_NO_R_QUTION);
					t = new Token(ERR);
					break;
				}
				else
					str.push_back(chInput);
            }
            if(!t){
                t = new Str(str);
            }
        }
        else if(chInput == '\''){   //字符，终于到你了
            char c;
            scan();
            if(chInput == '\\'){    //又是转义符
                scan();
                    if(chInput=='n')            c = '\n' ;
					else if(chInput == '\\')    c = '\\' ;
					else if(chInput == 't')     c = '\t' ;
					else if(chInput == '0')     c = '\0' ;
					else if(chInput == '\'')    c = '\'' ;                      
					else if(chInput == -1 || chInput == '\n'){          //文件结束 换行
						LEXERROR(CHAR_NO_R_QUTION);
						t = new Token(ERR);
					}
                    //TODO:在C类语言会对\A,\a这样的提出WARNING
					else c = chInput;
            }
            else if(chInput == -1 || chInput == '\n'){      //文件结束 换行
                LEXERROR(CHAR_NO_R_QUTION);
                t = new Token(ERR);
            }
            else if(chInput == '\''){       //字符为空
                LEXERROR(CHAR_NO_DATA);
                t = new Token(ERR);
                scan();             //把单引号读过
            }
            else{
                c = chInput;
                scan();
            }
            if(!t){
				if(scan('\'')){             //匹配右侧引号,读掉引号
					t = new Char(c);
				}
				else{
					LEXERROR(CHAR_NO_R_QUTION);
					t = new Token(ERR);
				}
			}
        }
        else{       //界符
            switch (chInput)    //TODO:没有处理’#‘，也就是宏定义
            {
            case '+':
                t = new Token(scan('+')?INC:ADD);break;
            case '-':   //TODO:未能够解决负数的问题
                t = new Token(('-')?DEC:SUB);break;
            case '*':
                t = new Token(MUL);
                scan();
                break;
            case '/':
                scan();
                if(chInput == '/'){ //单行注释
                    while(chInput != '\n' && chInput != -1)
							scan();     //行 文件不结束
					t = new Token(ERR);
                }
                else if(chInput == '*'){ //多行注释,不允许嵌套注释
                    while(!scan(-1)){    //一直扫描
                        if(chInput == '*'){
                            if(scan('/')){
                                break;
                            }
                        }
                    }
                    if(chInput == -1){  //没有被正常注释
                        LEXERROR(COMMENT_NO_END);
                    }
                    t = new Token(ERR);
                }
                else    
                    t = new Token(DIV);
                break;
            case '%':
                t = new Token(MOD);
                scan();
                break;
            case '>':
                t = new Token(scan('=')?GE:GT);
                scan();
                break;
            case '<':
                t = new Token(scan('=')?LE:LT);
                scan();
                break;  
            case '=':
                t = new Token(scan('=')?EQU:ASSIGN);
                scan();
                break;
            case '&':
                t = new Token(scan('&')?AND:LEA);
                scan();
                break;
            case '|':
                t = new Token(scan('|')?OR:ERR);    //因为没有位运算，所以 | 就是错的
                if(t->tag == ERR){
                    LEXERROR(OR_NO_PAIR);               //||只有一个
                }
                scan();
                break;
            case '!':
				t = new Token(scan('=')?NEQU:NOT);
                scan();
                break;
			case ',':
				t = new Token(COMMA);
                scan();
                break;
			case ':':
				t = new Token(COLON);
                scan();
                break;
			case ';':
				t = new Token(SEMICON);
                scan();
                break;
			case '(':
				t = new Token(LPAREN);
                scan();
                break;
			case ')':
				t = new Token(RPAREN);
                scan();
                break;
			case '[':
				t = new Token(LBRACK);
                scan();
                break;
			case ']':
				t = new Token(RBRACK);
                scan();
                break;
			case '{':
				t = new Token(LBRACE);
                scan();
                break;
			case '}':
				t = new Token(RBRACE);
                scan();
                break;
            case -1:
                scan();            //没错，就是这里了，第一次还不知道有什么用，真的是脑残，不过十分佩服初始化第一个字符的这种方法
                break;
            default:
                t = new Token(ERR);//错误的词法记号
				LEXERROR(TOKEN_NO_EXIST);
				scan();
                break;
            }
        }
        //词法记号内存管理
		if(token)delete token;
		token = t;                           //强制记录
		if(token && token->tag!=ERR)      //有效,直接返回
			return token;
		else
			continue;                                   //否则一直扫描直到结束
    }
    //文件结束
	if(token)delete token;
	return token = new Token(END);
}

