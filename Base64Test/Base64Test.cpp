// Base64Test.cpp : �������̨Ӧ�ó������ڵ㡣
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
  
    memcpy(pIn,"���",5); 
  
    string strout;  
  
    cout<<(char*)pIn<<endl;  
    if(CBase64::Encode(pIn, 4, strout))  
        cout<<strout<<endl;  
    else  
        cout<<"����ʧ��"<<endl;  
  
    string stroIn = strout;  
    cout<<stroIn<<endl;  
  
    memset(pOut,0,sizeof(pOut));  
  
    if(CBase64::Decode(stroIn, pOut, &len))  
    {  
        cout<<(char *)pOut<<endl;  
        cout<<"len="<<strlen((char *)pOut)<<endl;  
    }  
    else  
        cout<<"����ʧ��"<<endl;  
  
    cout<<len<<endl;  
  
    return 0;  
}

