#include"pch.h"

const int  shiftingx = 150;
const int  shiftingy = 100;
const int  wide = 372;
const int  length = 425;
const int gridx = (wide - 20) / 8;
const int gridy = (length - 20) / 9;

using namespace std;

extern int cnt_map[12][12];//用编号模拟当前棋盘
extern pieces P[30];//棋子

void run_AI()
{
	make_pieces();

	for (int i = 0; i <= 8;i++)
		for (int j = 0; j <= 9; j++)
			cnt_map[i][j] = 0;

	for (int i = 0;i < 5; i++)//初始化棋盘落兵
	{
		P[14].app(i * 2, 3);
		cnt_map[i * 2][3] = 14;

		P[7].app(i * 2, 6);
		cnt_map[i * 2][6] = 7;
		/*fillcircle(10 + 2 * i * gridx, 10 + gridy * 3, 20);
		fillcircle(10 + 2 * i * gridx, 10 + gridy * 6, 20);

		settextcolor(RGB(0, 0, 0));
		RECT r = { 10 + 2 * i * gridx - 22, 10 + gridy * 3 - 22,10 + 2 * i * gridx + 22, 10 + gridy * 3 + 22 };
		drawtext(_T("兵"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		settextcolor(RED);
		RECT c = { 10 + 2 * i * gridx - 22, 10 + gridy * 6 - 22,10 + 2 * i * gridx + 22, 10 + gridy * 6 + 22 };
		drawtext(_T("兵"), &c, DT_CENTER | DT_VCENTER | DT_SINGLELINE);*/
	}

	//落炮
	cnt_map[1][2] = cnt_map[7][2] = 1;
	P[1].app(1, 2); P[1].app(7, 2);

	cnt_map[1][7] = cnt_map[7][7] = 8;
	P[8].app(1, 7); P[8].app(7, 7);

	/*/落車
	cnt_map[0][0] = cnt_map[8][0] = 2;
	P[2].app(0, 0); P[2].app(8, 0);

	cnt_map[0][9] = cnt_map[8][9] = 9;
	P[9].app(0, 9); P[9].app(8, 9);

	//落马
	cnt_map[1][0] = cnt_map[7][0] = 3;
	P[3].app(1, 0); P[3].app(7, 0);

	cnt_map[1][9] = cnt_map[7][9] = 10;
	P[10].app(1, 9); P[10].app(7, 9);

	//落象
	cnt_map[2][0] = cnt_map[6][0] = 4;
	P[4].app(2, 0); P[4].app(6, 0);

	cnt_map[1][9] = cnt_map[7][9] = 10;
	P[10].app(1, 9); P[10].app(7, 9);*/

	for (int i = 2; i <= 6;i++)//优化，快捷落 車马象士将
	{
		cnt_map[i - 2][0] = cnt_map[10 - i][0] = i;
		P[i].app(i - 2, 0); P[i].app(10 - i, 0);

		cnt_map[i - 2][9] = cnt_map[10 - i][9] = i + 7;
		P[i + 7].app(i - 2, 9); P[i + 7].app(10 - i, 9);
	}

	int win = 1;//2->黑方赢 3->红方赢
	int who = 1; //1->黑方动子 0->红方动子
	x_y Chess, Target;//被选中的棋子和移动的目的地
	x_y Last_Chess, Last_Target;
	int die_Red = 0, die_Black = 0;
	Last_Chess.x = -1;//判断是第一次进行选子
	Startup();

	while (win == 1)
	{
		int AI_Move;
		who = 1 - who;

		int Next = 1;//是否进行下一步
		while (Next)
		{
			clearrectangle(-80, length / 2 - 30, -10, length / 2 + 30);
			RECT r = { -80, length / 2 - 30, -10, length / 2 + 30 };
			settextcolor(RGB(0, 0, 0));
			if (who)
			{
				drawtext(_T("黑方"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
			else
			{
				drawtext(_T("红方"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}

			Chess.x = Target.x = -1;//-1表示没有选定棋子
			if (!who)
			{
				MOUSEMSG cnt = GetMouseMsg();

				if (cnt.mkLButton)
				{
					//消除痕迹
					if (Last_Chess.x != -1)
					{
						cancel_highlight(Last_Target);
						disapp(Last_Chess.x, Last_Chess.y);
					}

					x_y cnt_x = chang_coordinate(cnt.x, cnt.y);//当前鼠标的坐标值

					//输出当前鼠标坐标，若不在棋盘则输出666 666；
					clearrectangle(80, -80, 300, -20);
					TCHAR s[20];
					_stprintf_s(s, _T("%d"), cnt_x.x);
					outtextxy(100, -50, s);
					_stprintf_s(s, _T("%d"), cnt_x.y);
					outtextxy(200, -50, s);

					if (cnt_x.x == 666) continue;

					if (who == 1)
						if (cnt_map[cnt_x.x][cnt_x.y] && cnt_map[cnt_x.x][cnt_x.y] % 14 <= 6)
						{
							highlight(cnt_x);
							Chess = cnt_x;
						}

					if (who == 0)
						if (cnt_map[cnt_x.x][cnt_x.y] && cnt_map[cnt_x.x][cnt_x.y] % 14 > 6)
						{
							highlight(cnt_x);
							Chess = cnt_x;
						}

					if (Chess.x == -1) continue;
					MOUSEMSG Target_ = GetMouseMsg();
					while (1)
					{
						if (Target_.mkLButton)
						{
							Target = chang_coordinate(Target_.x, Target_.y);//当前鼠标的坐标值

							settextcolor(RGB(0, 0, 0));
							clearrectangle(80, -80, 300, -20);
							TCHAR s[20];
							_stprintf_s(s, _T("%d"), Target.x);
							outtextxy(100, -50, s);
							_stprintf_s(s, _T("%d"), Target.y);
							outtextxy(200, -50, s);

							if (who == 1)//切换选中的棋子
								if (cnt_map[Target.x][Target.y] && cnt_map[Target.x][Target.y] % 14 <= 6)
								{
									cancel_highlight(Chess);
									highlight(Target);
									Chess = Target;
									Target_ = GetMouseMsg();
									continue;
								}

							if (who == 0)//切换选中的棋子 
								if (cnt_map[Target.x][Target.y] && cnt_map[Target.x][Target.y] % 14 > 6)
								{
									cancel_highlight(Chess);
									highlight(Target);
									Chess = Target;
									Target_ = GetMouseMsg();
									continue;
								}

							if (!check(Chess, Target))
							{
								cancel_highlight(Chess);
							}
							else Next = 0;
							break;
						}
						Target_ = GetMouseMsg();
					}
				}
			}
			else
			{
				cancel_highlight(Last_Target);
				disapp(Last_Chess.x, Last_Chess.y);
				settextcolor(RGB(0, 0, 0));
				clearrectangle(80, -80, 300, -20);
				RECT r = { 80, -80, 300, -20 };
				drawtext(_T("运算中。。"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				AI_Move = ResponseMove(); 

				clearrectangle(80, -80, 300, -20);
				Next = 0;
			}
		}

		if (!who)
		{
			HumanMove(Chess, Target);
		}
		else
		{
			int k1 = AI_Move & 255;
			Chess.x = k1 % 16 - 3; Chess.y = k1 / 16 - 3;
			
			int k2 = AI_Move >> 8;
			Target.x = k2 % 16 - 3; Target.y = k2 / 16 - 3;
		}

		//判断是否获胜
		if (cnt_map[Target.x][Target.y] == 6) win = 3;
		if (cnt_map[Target.x][Target.y] == 13) win = 2;

		Last_Chess = Chess; Last_Target = Target;
		//动子
		P[cnt_map[Chess.x][Chess.y]].app(Target.x, Target.y);

		if (cnt_map[Target.x][Target.y] && cnt_map[Target.x][Target.y] % 14 <= 6)
			Move_to_cemetery_Black(cnt_map[Target.x][Target.y], die_Black++);
		if (cnt_map[Target.x][Target.y] && cnt_map[Target.x][Target.y] % 14 > 6)
			Move_to_cemetery_Red(cnt_map[Target.x][Target.y], die_Red++);

		cnt_map[Target.x][Target.y] = cnt_map[Chess.x][Chess.y];
		disapp(Chess.x, Chess.y);
		//留下痕迹
		highlight(Last_Chess);
		highlight(Last_Target);

	}

	LOGFONT f;
	gettextstyle(&f);						// 获取当前字体设置
	f.lfHeight = 48;						// 设置字体高度为 48
	_tcscpy_s(f.lfFaceName, _T("楷体"));		// 设置字体为“楷体”
	f.lfQuality = ANTIALIASED_QUALITY;		// 设置输出效果为抗锯齿  
	settextstyle(&f);

	fillrectangle(44 + 22 + 10, 10 + 3.5 * gridy, 10 + 44 * 6.5 + 5, 10 + 5.5 * gridy);
	RECT r = { 44 + 22 + 10, 10 + 3.5 * gridy, 10 + 44 * 6.5 + 5, 10 + 5.5 * gridy };
	if (win == 2)
	{
		settextcolor(RGB(0, 0, 0));
		drawtext(_T("黑 方 胜"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	else
	{
		settextcolor(RED);
		drawtext(_T("红 方 胜"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	MOUSEMSG End = GetMouseMsg();
	while (1)
	{
		if (End.mkLButton)
			break;
		End = GetMouseMsg();
	}

}