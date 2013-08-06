/*
created by crazyboysk @ SWPU in 2013 for "Code to Change"
mail:xjzdy3000@163.com
VC++6.0  windows XP 
*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define end 1
#define noend 2

typedef struct//学习单位和计算用
{
  char mete[10];//单位
	double num;//数值
}unit,*punit;

//读取初始化给定的单位换算公式
int read_formula(char *buf1,struct _iobuf *file1,punit un1)
{
	char *str;
	int line=0;
	while(fgets(buf1,50,file1))//按行读取
	{		
		str = strtok(buf1, " ");//首位1
		if(!strcmp(str,"\n"))//碰到空行则给定换算公式结束
			break;
		str = strtok(NULL, " ");//单位
		strcpy(un1[line].mete,str);
		strtok(NULL, " ");//=
		str = strtok(NULL, " ");//换算值
		un1[line].num=atof(str);
		line++;
	}//单位换算学习结束
	return line;
}

//获取需要计算的每个运算式中字符串个数以便计算有多少数参与运算
int get_string(char *buf1)
{
	int len=0;
	for(int i=0;i<(int)strlen(buf1);i++)
	{
		if(buf1[i]==' ')
			len++;
		else if(buf1[i]=='+'||buf1[i]=='-')
			i++;
		else if(buf1[i]=='\n')
		{
			len++;
			break;
		}
	}
	return len;
}

//读取每个运算数并将其换算成米以便进行下一步计算
void read_convert(char *buf1,punit un1,punit un2,struct _iobuf *file2,int line,int i,int mark)
{
	int cmpresult[50],tmp,cmp,j,k;
	char *str;
	memset(cmpresult, 0, 50); 
	str = strtok(buf1, " ");//数字
	un2[i].num=atof(str);
	if(mark==1)
		str = strtok(NULL, "\n");//单位(用于算式结尾)
	else if(mark==2)
		str = strtok(NULL, " ");//单位(用于算式中间)
	strcpy(un2[i].mete,str);	
	for(k=0;k<line;k++)//比较所有可以用于换算的单位
	{
		for(j=0;j<(int)strlen(un2[i].mete);j++)//求出每个已知单位与所求单位的相似度
			if(un1[k].mete[j]==un2[i].mete[j])
				cmpresult[k]=cmpresult[k]+1;
	}
	tmp=0;
	cmp=0;
	for(k=0;k<line;k++)//查找最大相似度的那个单位
	{
		if(cmpresult[k]>=tmp)
		{
			tmp=cmpresult[k];
			cmp=k;
		}
	}
	un2[i].num*=un1[cmp].num;//用相似度最大的单位进行单位换算
}

//计算多项式的值
void calculate(char *buf1,struct _iobuf *file1,struct _iobuf *file2,punit un1,punit un2,int line)
{
	int len;//记录每行除去运算符后的字符串
	char buf2[50],*str;
	int i;
	while(fgets(buf1,50,file1))
	{
		len=get_string(buf1);
		char opr1,opr2;//运算符
		double result;
		i=0;
		switch(len)//字符串个数确定进行几次运算
		{
		case 2://两个字符串则只是简单的单位替换
			{
				read_convert(buf1,un1,un2,file2,line,i,end);
				memset(buf2, 0, 50); 
				sprintf(buf2, "%0.2f m\n", un2[i].num);
				fputs(buf2,file2);
				break;
			}
		case 4://四个字符串则表示两个数参与运算
			{
				read_convert(buf1,un1,un2,file2,line,i,noend);//获取第一个数字并写入结构体相应位置
				i++;
				str = strtok(NULL, " ");//运算符
				opr1=str[0];
				read_convert(NULL,un1,un2,file2,line,i,end);//获取第二个数字并写入结构体相应位置
				if(opr1=='+')
					result=un2[i-1].num+un2[i].num;
				else if(opr1=='-')
					result=un2[i-1].num-un2[i].num;
				memset(buf2, 0, 50); 
				sprintf(buf2, "%0.2f m\n", result);
				fputs(buf2,file2);
				break;
			}
		case 5://最后一行因为没有换行等特殊标志因此会得到少一个字符串的情况
		case 6://六个字符串则表示三个数参与运算
			{
				read_convert(buf1,un1,un2,file2,line,i,noend);//获取第一个数字并写入结构体相应位置
				i++;
				str = strtok(NULL, " ");//运算符1
				opr1=str[0];
				read_convert(NULL,un1,un2,file2,line,i,noend);//获取第二个数字并写入结构体相应位置
				if(opr1=='+')
					result=un2[i-1].num+un2[i].num;
				else if(opr1=='-')
					result=un2[i-1].num-un2[i].num;
				str = strtok(NULL, " ");//运算符2
				opr2=str[0];
				i++;
				read_convert(NULL,un1,un2,file2,line,i,end);//获取第三个数字并写入结构体相应位置
				if(opr2=='+')
					result+=un2[i].num;
				else if(opr2=='-')
					result-=un2[i].num;
				memset(buf2, 0, 50); 
				sprintf(buf2, "%0.2f m\n", result);
				fputs(buf2,file2);
				break;
			}
			}//switch
	}//while
}

void main()
{
	FILE *file1=fopen("input.txt","r");
	FILE *file2=fopen("output.txt","w");
	fputs("xjzdy3000@163.com\n\n",file2);
	char buf1[50];
	int line=0;//换算单位的个数及计算用的结构体个数
	
    punit un1=(punit)malloc(10*sizeof(unit));//保存单位换算用,程序暂时只支持10种单位换算,实际只用到6种
	punit un2=(punit)malloc(5*sizeof(unit));//保存临时运算使用,程序暂时只支持5个数据参与运算,实际只用到3个
	memset(un1,0,sizeof(unit)*10);
	memset(un2,0,sizeof(unit)*5);

	line=read_formula(buf1,file1,un1);
	calculate(buf1,file1,file2,un1,un2,line);
	
	fclose(file1);
	fclose(file2);
}
