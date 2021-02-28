// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件

#ifndef PCH_H
#define PCH_H

#include<cstdio>
#include<iostream>
#include<algorithm>
#include<graphics.h>
#include<conio.h>
#include<string>
#include<cstring>
#include <time.h>
#include"pch.cpp"

struct  x_y
{
	int x;
	int y;
};

struct moveto
{
	x_y fro, to;
};
int make_map(int tim);
void run();
void run_AI();
void make_pieces();
void map_detail(int x, int y);
void disapp(int x, int y);
x_y chang_coordinate(int x_, int y_);
void highlight(x_y now);
void cancel_highlight(x_y now);
bool check(x_y k1, x_y k2);
void Move_to_cemetery_Red(int Chess_type, int die_num);
void Move_to_cemetery_Black(int Chess_type, int die_num);
void Startup(void);
void HumanMove(x_y Chess, x_y Target);
int ResponseMove(void);

// TODO: 添加要在此处预编译的标头
using namespace std;
class pieces
{
public:
	LPCTSTR   Name;
	int Black_or_Red; // 1 -> red, 0->black

	void app(int x, int y)//在横坐标第x个纵坐标第y个生成棋子
	{
		setlinecolor(RGB(0, 0, 0));
		setlinestyle(PS_SOLID, 2);
		int  gridx = 44, gridy = 45;
		fillcircle(10 + x * gridx, 10 + gridy * y, 20);

		LOGFONT f;
		gettextstyle(&f);						// 获取当前字体设置
		f.lfHeight = 24;						// 设置字体高度为 24
		_tcscpy_s(f.lfFaceName, _T("楷体"));		// 设置字体为“楷体”
		f.lfQuality = ANTIALIASED_QUALITY;		// 设置输出效果为抗锯齿  
		settextstyle(&f);

		if (Black_or_Red) settextcolor(RED);
		else  settextcolor(RGB(0, 0, 0));

		RECT r = { 10 + x * gridx - 20, 10 + gridy * y - 20,10 + x * gridx + 20, 10 + gridy * y + 20 };
		drawtext(LPCTSTR(Name), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

};



#endif   //PCH_H
