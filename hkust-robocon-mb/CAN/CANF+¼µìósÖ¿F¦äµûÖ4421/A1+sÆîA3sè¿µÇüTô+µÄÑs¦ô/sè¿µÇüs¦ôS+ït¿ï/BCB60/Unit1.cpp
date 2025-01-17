//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
   //以下是调用DLL声明
#include <windows.h>
#include <Sysdefs.h>
HINSTANCE LM_DLL;
typedef int (__stdcall *P_Init_can)(unsigned char com_NUM,unsigned char Model,unsigned int CanBaudRate,unsigned char SET_ID_TYPE,unsigned char FILTER_MODE,unsigned char RXF[],unsigned char RXM[]);
typedef	int	(__stdcall *P_Can_send)(unsigned char IDbuff[],unsigned char Databuff[],unsigned char FreamType,unsigned char Bytes);
typedef	int	(__stdcall *P_Can_receive)(unsigned char IDbuff[],unsigned char Databuff[],unsigned char *FreamType,unsigned char *Bytes);
typedef	int	(__stdcall *P_Quit_can)(void);
P_Init_can Init_can;
P_Can_send Can_send;
P_Can_receive Can_receive;
P_Quit_can Quit_can;
//以上是调用DLL声明


#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender)
{
    //这里是动态加载 DLL函数例子。
    LM_DLL = LoadLibrary("LM_USB.dll"); //动态方式加载dll

    if (LM_DLL != NULL)
    {
        Init_can = (P_Init_can)GetProcAddress(LM_DLL, "Init_can");
        Can_send = (P_Can_send)GetProcAddress(LM_DLL, "Can_send");
        Can_receive = (P_Can_receive)GetProcAddress(LM_DLL, "Can_receive");
        Quit_can = (P_Quit_can)GetProcAddress(LM_DLL, "Quit_can");
        Memo1->Lines->Append("加载DLL成功");
    }
    else Memo1->Lines->Append("加载DLL失败");
    //这里是动态加载 DLL函数例子。
    SendMessage(Memo1->Handle, WM_VSCROLL, MAKELONG(SB_BOTTOM, 0), 0);//显示最后一行
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button2Click(TObject *Sender)
{
    unsigned int ret;
    unsigned char RXF[4],RXM[4];
    RXF[0] = 0;
    RXF[1] = 0;
    RXF[2] = 0;
    RXF[3] = 0;
    RXM[0] = 0;
    RXM[1] = 0;
    RXM[2] = 0;
    RXM[3] = 0;
    ret = Init_can(0,1,20,0,0,RXF,RXM); //调用连接A1+设备函数例子
    //ret 返回值 0：正常  1：已连接设备  2：无应答（端口有效无应答）3：无可用的串口（3-30）
    if (ret==0) Memo1->Lines->Append("连接设备成功");
    else Memo1->Lines->Append("连接设备失败，返回值"+IntToStr(ret));
    SendMessage(Memo1->Handle, WM_VSCROLL, MAKELONG(SB_BOTTOM, 0), 0);//显示最后一行
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button3Click(TObject *Sender)
{
    unsigned int ret;
    unsigned char IDbuff[4],Databuff[8],FreamType,Bytes;
    IDbuff[0] = 0;
    IDbuff[1] = 0;
    Databuff[0] = 1;
    Databuff[1] = 2;
    Databuff[2] = 3;
    Databuff[3] = 4;
    Databuff[4] = 5;
    Databuff[5] = 6;
    Databuff[6] = 7;
    Databuff[7] = 8;
    FreamType = 0;//标准数据帧
    Bytes = 8; //本帧8个字节
    ret = Can_send(IDbuff,Databuff,FreamType,Bytes); //调用发送函数例子
    Memo1->Lines->Append("发送:"+IntToStr(IDbuff[0])+"  "+IntToStr(IDbuff[1])+"    "+IntToStr(Databuff[0])+"  "+IntToStr(Databuff[1])+"  "+IntToStr(Databuff[2])+"  "+IntToStr(Databuff[3])+"  "+IntToStr(Databuff[4])+"  "+IntToStr(Databuff[5])+"  "+IntToStr(Databuff[6])+"  "+IntToStr(Databuff[7]));
    SendMessage(Memo1->Handle, WM_VSCROLL, MAKELONG(SB_BOTTOM, 0), 0);//显示最后一行
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button4Click(TObject *Sender)
{
    unsigned int ret;
    unsigned char no_data;
    unsigned char IDbuff[4],Databuff[8],FreamType,Bytes;
    no_data = 1;
    do
    {
        ret = Can_receive(IDbuff,Databuff,&FreamType,&Bytes);//调用接收函数例子
	if (ret==1)
	{
	    Memo1->Lines->Append("接收:"+IntToStr(IDbuff[0])+"  "+IntToStr(IDbuff[1])+"    "+IntToStr(Databuff[0])+"  "+IntToStr(Databuff[1])+"  "+IntToStr(Databuff[2])+"  "+IntToStr(Databuff[3])+"  "+IntToStr(Databuff[4])+"  "+IntToStr(Databuff[5])+"  "+IntToStr(Databuff[6])+"  "+IntToStr(Databuff[7]));
	    no_data = 0;
	}
    } while (ret==1);
    if (no_data ==1) Memo1->Lines->Append("接收: 无数据");
    SendMessage(Memo1->Handle, WM_VSCROLL, MAKELONG(SB_BOTTOM, 0), 0);//显示最后一行
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button5Click(TObject *Sender)
{
    unsigned int ret;
    ret = Quit_can(); //调用释放函数例子
    Memo1->Lines->Append("释放设备连接");
    SendMessage(Memo1->Handle, WM_VSCROLL, MAKELONG(SB_BOTTOM, 0), 0);//显示最后一行
}


 //---------------------------------------------------------------------------

