// Base64Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Base64.h"    
#include <iostream> 

using namespace std;  

int _tmain(int argc, _TCHAR* argv[])
{
	unsigned long len = 10;  
  
    unsigned char pIn[100];  
    unsigned char pOut[100];  
  
    memcpy(pIn,"你好",5); 
  
    string strout;  
  
    cout<<(char*)pIn<<endl;  
    if(CBase64::Encode(pIn, 4, strout))  
        cout<<strout<<endl;  
    else  
        cout<<"加密失败"<<endl;  
  
    string stroIn = strout;  
    cout<<stroIn<<endl;  
  
    memset(pOut,0,sizeof(pOut));  
  
    if(CBase64::Decode(stroIn, pOut, &len))  
    {  
        cout<<(char *)pOut<<endl;  
        cout<<"len="<<strlen((char *)pOut)<<endl;  
    }  
    else  
        cout<<"解密失败"<<endl;  
  
    cout<<len<<endl;  
  
    return 0;  
}

