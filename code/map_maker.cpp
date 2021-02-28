#include"pch.h"


using namespace std;

const int  shiftingx = 150;
const int  shiftingy = 100;
const int  wide = 372;
const int  length = 425;
const int gridx = (wide - 20) / 8;
const int gridy = (length - 20) / 9;

void map_detail(int x, int y)//卒和炮开始位置的格点细节
{

	if (x != wide - 10)
	{
		line(x + 4, y + 12, x + 4, y + 4);
		line(x + 4, y + 4, x + 12, y + 3);
		line(x + 4, y - 12, x + 4, y - 4);
		line(x + 4, y - 4, x + 12, y - 4);
	}

	if (x != 10)
	{
		line(x - 4, y + 12, x - 4, y + 4);
		line(x - 4, y + 4, x - 12, y + 3);
		line(x - 4, y - 12, x - 4, y - 4);
		line(x - 4, y - 4, x - 12, y - 4);
	}
}
int make_map(int tim)
{

	if (!tim) initgraph(1080, 640);// 初始化720x640的绘图屏幕
	setorigin(150, 100);//设置坐标原点
	setbkcolor(RGB(255, 255, 255));
	cleardevice();
	//画外边框
	setlinecolor(RGB(0, 0, 0));
	setlinestyle(PS_SOLID, 5);
	rectangle(0, 0, wide, length);
	setfillcolor(RGB(255, 255, 255));
	fillrectangle(0, 0, wide, length);
	//画内边框
	setlinestyle(PS_SOLID, 2);
	rectangle(10, 10, wide - 10, length - 10);
	//画网格
	for (int i = 1; i <= 7; i++)
	{
		line(10 + i * gridx, 10, 10 + i * gridx, length - 10);
	}
	setbkmode(TRANSPARENT);
	fillrectangle(10, 10 + 4 * gridy, wide - 10, 10 + 5 * gridy);
	for (int i = 1; i <= 8; i++)
	{
		line(10, 10 + i * gridy, wide - 10, 10 + i * gridy);
	}
	//输入汉字
	LOGFONT f;
	gettextstyle(&f);						// 获取当前字体设置
	f.lfHeight = 32;						// 设置字体高度为 32
	_tcscpy_s(f.lfFaceName, _T("隶书"));		// 设置字体为“隶书”
	f.lfQuality = ANTIALIASED_QUALITY;		// 设置输出效果为抗锯齿  
	settextstyle(&f);

	settextcolor(RGB(0, 0, 0));
	for (int i = 1; i <= 5; i++)
	{
		RECT r = { 10 + 44 - 5, 10 + 4 * gridy, 10 + 44 * 3 + 5, 10 + 5 * gridy };
		drawtext(_T("楚  河"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		RECT c = { 10 + 5 * 44 - 5, 10 + 4 * gridy, 10 + 44 * 7 + 5, 10 + 5 * gridy };
		drawtext(_T("汉  界"), &c, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	//细节补充
	line(10 + 3 * gridx, 10, 10 + 5 * gridx, 10 + 2 * gridy);
	line(10 + 5 * gridx, 10, 10 + 3 * gridx, 10 + 2 * gridy);
	line(10 + 3 * gridx, 10 + 7 * gridy, 10 + 5 * gridx, 10 + 2 * gridy + 7 * gridy);
	line(10 + 5 * gridx, 10 + 7 * gridy, 10 + 3 * gridx, 10 + 2 * gridy + 7 * gridy);

	for (int i = 0;i < 5; i++)
	{
		map_detail(10 + 2 * i * gridx, 10 + gridy * 3);
		map_detail(10 + 2 * i * gridx, 10 + gridy * 6);
	}
	map_detail(10 + gridx, 10 + gridy * 2);
	map_detail(10 + 7 * gridx, 10 + gridy * 2);
	map_detail(10 + gridx, 10 + gridy * 7);
	map_detail(10 + 7 * gridx, 10 + gridy * 7);

	IMAGE OP;
	loadimage(&OP, _T("timg3.jpg"),149,66);	

	f.lfHeight = 24;						// 设置字体高度为 32
	_tcscpy_s(f.lfFaceName, _T("楷书"));		// 设置字体为“楷书”
	f.lfQuality = ANTIALIASED_QUALITY;		// 设置输出效果为抗锯齿  
	settextstyle(&f);

	putimage(470, 180, &OP);
	RECT r = { 470, 180 + 66, 470 + 150, 180 + 66 + 40 };
	for (int i = 1; i <= 5; i++) drawtext(_T("双人对战"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	putimage(490+150, 180, &OP);
	r = { 490 + 150, 180 + 66, 490 + 150 + 150, 180 + 66 + 40 };
	for (int i = 1; i <= 5; i++) drawtext(_T("AI对战"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	int ok = 0;
	while (!ok)
	{
		MOUSEMSG cnt = GetMouseMsg();
		if (cnt.mkLButton)
		{
			if (cnt.x >= 470 + shiftingx && cnt.x <= 470 + 149 + shiftingx && cnt.y >= 180 + shiftingy && cnt.y <= shiftingy + 180 + 66)
				ok = 1;
			if (cnt.x >= 490 + 150 + shiftingx && cnt.x <= 490 + 150 + 149 + shiftingx && cnt.y >= 180 + shiftingy && cnt.y <= shiftingy + 180 + 66)
				ok = 2;
		}
	}

	clearrectangle(470, 180, 490 + 150 + 150, 180 + 66 + 40);
	return ok;
}