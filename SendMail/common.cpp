
#include "stdafx.h"
#include <stdio.h>
#include <time.h>
#include "common.h"
#include <ctime>
#include <iostream>

char* current_time()
{
    static char time_buf[128];
    time_t curr;
    struct tm *t;
    time(&curr);
    t = localtime(&curr);
	//localtime_s(t, &curr);
    sprintf(time_buf, "%4d.%02d.%02d %02d:%02d:%02d",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec);
    return time_buf;
}

int del_char(CStringA &str, char delChar)
{
	CString tmp;
	int len = str.GetLength();
	for(int i = 0; i < len; i++){
		if(str[i] == delChar) continue;
		tmp += str[i];
	}
	str = tmp;
	return 0;
}
int del_char(string &str, char delChar)
{
	string tmp;
	int len = str.size();
	for(int i = 0; i < len; i++){
		if(str[i] == delChar) continue;
		tmp += str[i];
	}
	str = tmp;
	return 0;
}

int add_char(string &str, char afterChar, char addChar)
{
	string tmp;
	int len = str.size();
	for(int i = 0; i < len; i++){
		tmp += str[i];
		if(str[i] == afterChar && i != len - 1){
			tmp += addChar;
		}	
	}
	str = tmp;
	return 0;
}

int add_char(CStringA &str, char afterChar, char addChar)
{
	CStringA tmp;
	int len = str.GetLength();
	for(int i = 0; i < len; i++){
		tmp += str[i];
		if(str[i] == afterChar && i != len - 1){
			tmp += addChar;
		}	
	}
	str = tmp;
	return 0;
}

int size2str(CStringA &str, int bytes)
{
	double size = (double)bytes;
	CStringA tmp = "B";
	if(bytes / 1024 > 0){
		tmp = "KB";
		size = size / 1024;
		if(bytes / 1024 / 1024 > 0){
			tmp = "MB";
			size = size / 1024;			
		}
	}
	str.Format("%.2lf%s", size, tmp.GetBuffer());
	return 0;
}


int del_chars(CStringA &str, char firstChar, char endChar)
{
	CStringA tmp;
	int len = str.GetLength();
	int flag = 0;//0,未找到开始的字符；1,找到了开始的字符；2,找到了结束的字符；
	for(int i = 0; i < len; i++){
		if(str[i] == firstChar){
			flag = 1;
		}else if(str[i] == endChar && flag == 1){
			flag = 2;
			i++;
		}
		if(flag == 0 || flag == 2){
			tmp += str[i];
		}
	}
	if(flag == 2){
		str = tmp;
	}
	if(flag == 1) return 1;//不存在endChar
	return 0;
}