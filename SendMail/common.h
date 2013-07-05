#ifndef _COMMON_H_
#define _COMMON_H_
#include "stdafx.h"
#include <iostream>
#include <string>
using namespace std;

struct MailItem{
	string receiver;
	string name;
	string title;
	string text;
	string appdenix;
};

char* current_time();

int add_char(string &str, char afterChar, char addChar);
int add_char(CStringA &str, char afterChar, char addChar);
int del_char(CStringA &str, char delChar);
int del_char(string &str, char delChar);
int del_chars(CStringA &str, char firstChar, char endChar);
int size2str(CStringA &str, int bytes);

#endif