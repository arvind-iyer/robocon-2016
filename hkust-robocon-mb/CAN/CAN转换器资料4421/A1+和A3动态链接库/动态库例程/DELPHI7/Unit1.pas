unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls;

type

  TForm1 = class(TForm)
    Button1: TButton;
    Button2: TButton;
    Button3: TButton;
    Button4: TButton;
    Button5: TButton;
    Memo1: TMemo;
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure Button3Click(Sender: TObject);
    procedure Button5Click(Sender: TObject);
    procedure Button4Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;
//以下是调用DLL声明
TYPE
    P_Init_can = function(com_NUM:BYTE;Model:BYTE;CanBaudRate:WORD;SET_ID_TYPE:BYTE;FILTER_MODE:BYTE;RXF:pointer;RXM:pointer):integer;stdcall;
    P_Quit_can = function():integer;stdcall;
    P_Can_send = function(IDbuff:pointer;Databuff:pointer;FreamType:BYTE;Bytes:BYTE):integer;stdcall;
    P_Can_receive = function(IDbuff:pointer;Databuff:pointer;FreamType:pointer;Bytes:pointer):integer;stdcall;
VAR
    LM_DLL:Thandle;
    Init_can : P_Init_can;
    Quit_can : P_Quit_can;
    Can_send : P_Can_send;
    Can_receive : P_Can_receive;
//以上是调用DLL声明

implementation

{$R *.dfm}

procedure TForm1.Button1Click(Sender: TObject);
begin
    //这里是动态加载 DLL函数例子。
    LM_DLL := LoadLibrary('LM_USB.dll'); //动态方式加载dll
    if (LM_DLL <> NULL) then
    begin
	      @Init_can := GetProcAddress(LM_DLL, 'Init_can');
        @Quit_can := GetProcAddress(LM_DLL, 'Quit_can');
        @Can_send := GetProcAddress(LM_DLL, 'Can_send');
        @Can_receive := GetProcAddress(LM_DLL, 'Can_receive');
        Memo1.Lines.Append('加载DLL成功');
    end
	  else
    begin
	       Memo1.Lines.Append('加载DLL失败');
    end;
    //这里是动态加载 DLL函数例子。
    Memo1.SelLength := Length(Memo1.Text);
end;

procedure TForm1.Button2Click(Sender: TObject);
var ret:integer;
    RXF,RXM:array [0..3] of BYTE;
begin
    //端口号,型号,CAN波特率,ID排列方法,屏蔽滤波状态,滤波寄存器,屏蔽寄存器
    ret:=Init_can(0,1,20,0,0,@RXF,@RXM); //初始化函数演示
    if (ret=0) then Memo1.Lines.Append('连接设备成功')
    else Memo1.Lines.Append('连接设备失败，返回值'+inttostr(ret));
    Memo1.SelLength := Length(Memo1.Text);
end;

procedure TForm1.Button3Click(Sender: TObject);
var
    IDbuff:array[0..3] of byte;
    Databuff:array [0..7] of byte;
    ret,i:integer;
    str:string;
begin
    IDbuff[0]:=0;
    IDbuff[1]:=0;
    Databuff[0]:=1;
    Databuff[1]:=2;
    Databuff[2]:=3;
    Databuff[3]:=4;
    Databuff[4]:=5;
    Databuff[5]:=6;
    Databuff[6]:=7;
    Databuff[7]:=8;
    ret:= Can_send(@IDbuff,@Databuff,0,8);//发送函数演示
    Memo1.Lines.Append('已发送 '+IntTostr(IDbuff[0])+'  '+IntTostr(IDbuff[1])+'    '+IntTostr(Databuff[0])+'  '+IntTostr(Databuff[1])+'  '+IntTostr(Databuff[2])+'  '+IntTostr(Databuff[3])+'  '+IntTostr(Databuff[4])+'  '+IntTostr(Databuff[5])+'  '+IntTostr(Databuff[6])+'  '+IntTostr(Databuff[7]));
    Memo1.SelLength := Length(Memo1.Text);
end;

procedure TForm1.Button5Click(Sender: TObject);
var ret:integer;
begin
    ret := Quit_can();  //释放函数演示。退出程序或需重设参数需设用此函数。
    Memo1.Lines.Append('释放设备连接');
    Memo1.SelLength := Length(Memo1.Text);
end;

procedure TForm1.Button4Click(Sender: TObject);
var ret:integer;
    no_data:integer;
    IDbuff:array[0..3] of byte;
    Databuff:array [0..7] of byte;
    FreamType:BYTE;
    Bytes:BYTE;
begin
  	no_data := 1;
		repeat
			  ret := Can_receive(@IDbuff,@Databuff,@FreamType,@Bytes);//调用接收函数例子
				if (ret=1) then
				begin
					 Memo1.Lines.Append('接收:'+IntTostr(IDbuff[0])+'  '+IntTostr(IDbuff[1])+'    '+IntTostr(Databuff[0])+'  '+IntTostr(Databuff[1])+'  '+IntTostr(Databuff[2])+'  '+IntTostr(Databuff[3])+'  '+IntTostr(Databuff[4])+'  '+IntTostr(Databuff[5])+'  '+IntTostr(Databuff[6])+'  '+IntTostr(Databuff[7]));
					 no_data := 0;
				end;
    until (ret<>1);
		if (no_data =1) then Memo1.Lines.Append('接收: 无数据');
    Memo1.SelLength := Length(Memo1.Text);
end;

end.
