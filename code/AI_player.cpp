


#include "pch.h"

using namespace std;

extern int cnt_map[12][12];
// 棋盘范围
const int RANK_TOP = 3;
const int RANK_BOTTOM = 12;
const int FILE_LEFT = 3;
const int FILE_RIGHT = 11;

// 棋子编号
const int PIECE_KING = 0;
const int PIECE_ADVISOR = 1;
const int PIECE_BISHOP = 2;
const int PIECE_KNIGHT = 3;
const int PIECE_ROOK = 4;
const int PIECE_CANNON = 5;
const int PIECE_PAWN = 6;

// 其他常数
const int MAX_GEN_MOVES = 128; // 最大的生成走法数
const int LIMIT_DEPTH = 32;    // 最大的搜索深度
const int MATE_VALUE = 10000;  // 最高分值，即将死的分值
const int WIN_VALUE = MATE_VALUE - 100; // 搜索出胜负的分值界限，超出此值就说明已经搜索出杀棋了
const int ADVANCED_VALUE = 3;  // 先行权分值

// 判断棋子是否在棋盘中的数组
static const char ccInBoard[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// 判断棋子是否在九宫的数组
static const char ccInFort[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// 判断步长是否符合特定走法的数组，1=帅(将)，2=仕(士)，3=相(象)
static const char ccLegalSpan[512] = {
					   0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0
};

// 根据步长判断马是否蹩腿的数组
static const char ccKnightPin[512] = {
							  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,-16,  0,-16,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0, -1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0, -1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0, 16,  0, 16,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0
};

// 帅(将)的步长
static const char ccKingDelta[4] = { -16, -1, 1, 16 };
// 仕(士)的步长
static const char ccAdvisorDelta[4] = { -17, -15, 15, 17 };
// 马的步长，以帅(将)的步长作为马腿
static const char ccKnightDelta[4][2] = { {-33, -31}, {-18, 14}, {-14, 18}, {31, 33} };
// 马被将军的步长，以仕(士)的步长作为马腿
static const char ccKnightCheckDelta[4][2] = { {-33, -18}, {-31, -14}, {14, 31}, {18, 33} };

// 棋盘初始设置
static const int cucpcStartup[256] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0, 20, 19, 18, 17, 16, 17, 18, 19, 20,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0, 21,  0,  0,  0,  0,  0, 21,  0,  0,  0,  0,  0,
  0,  0,  0, 22,  0, 22,  0, 22,  0, 22,  0, 22,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0, 14,  0, 14,  0, 14,  0, 14,  0, 14,  0,  0,  0,  0,
  0,  0,  0,  0, 13,  0,  0,  0,  0,  0, 13,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0, 12, 11, 10,  9,  8,  9, 10, 11, 12,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// 子力位置价值表
static const int cucvlPiecePos[7][256] = {
  { // 帅(将)
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  2,  2,  2,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0, 11, 15, 11,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
  }, { // 仕(士)
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0, 20,  0, 20,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0, 23,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0, 20,  0, 20,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
  }, { // 相(象)
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0, 20,  0,  0,  0, 20,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0, 18,  0,  0,  0, 23,  0,  0,  0, 18,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0, 20,  0,  0,  0, 20,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
  }, { // 马
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0, 90, 90, 90, 96, 90, 96, 90, 90, 90,  0,  0,  0,  0,
	0,  0,  0, 90, 96,103, 97, 94, 97,103, 96, 90,  0,  0,  0,  0,
	0,  0,  0, 92, 98, 99,103, 99,103, 99, 98, 92,  0,  0,  0,  0,
	0,  0,  0, 93,108,100,107,100,107,100,108, 93,  0,  0,  0,  0,
	0,  0,  0, 90,100, 99,103,104,103, 99,100, 90,  0,  0,  0,  0,
	0,  0,  0, 90, 98,101,102,103,102,101, 98, 90,  0,  0,  0,  0,
	0,  0,  0, 92, 94, 98, 95, 98, 95, 98, 94, 92,  0,  0,  0,  0,
	0,  0,  0, 93, 92, 94, 95, 92, 95, 94, 92, 93,  0,  0,  0,  0,
	0,  0,  0, 85, 90, 92, 93, 78, 93, 92, 90, 85,  0,  0,  0,  0,
	0,  0,  0, 88, 85, 90, 88, 90, 88, 90, 85, 88,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
  }, { // 车
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,206,208,207,213,214,213,207,208,206,  0,  0,  0,  0,
	0,  0,  0,206,212,209,216,233,216,209,212,206,  0,  0,  0,  0,
	0,  0,  0,206,208,207,214,216,214,207,208,206,  0,  0,  0,  0,
	0,  0,  0,206,213,213,216,216,216,213,213,206,  0,  0,  0,  0,
	0,  0,  0,208,211,211,214,215,214,211,211,208,  0,  0,  0,  0,
	0,  0,  0,208,212,212,214,215,214,212,212,208,  0,  0,  0,  0,
	0,  0,  0,204,209,204,212,214,212,204,209,204,  0,  0,  0,  0,
	0,  0,  0,198,208,204,212,212,212,204,208,198,  0,  0,  0,  0,
	0,  0,  0,200,208,206,212,200,212,206,208,200,  0,  0,  0,  0,
	0,  0,  0,194,206,204,212,200,212,204,206,194,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
  }, { // 炮
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,100,100, 96, 91, 90, 91, 96,100,100,  0,  0,  0,  0,
	0,  0,  0, 98, 98, 96, 92, 89, 92, 96, 98, 98,  0,  0,  0,  0,
	0,  0,  0, 97, 97, 96, 91, 92, 91, 96, 97, 97,  0,  0,  0,  0,
	0,  0,  0, 96, 99, 99, 98,100, 98, 99, 99, 96,  0,  0,  0,  0,
	0,  0,  0, 96, 96, 96, 96,100, 96, 96, 96, 96,  0,  0,  0,  0,
	0,  0,  0, 95, 96, 99, 96,100, 96, 99, 96, 95,  0,  0,  0,  0,
	0,  0,  0, 96, 96, 96, 96, 96, 96, 96, 96, 96,  0,  0,  0,  0,
	0,  0,  0, 97, 96,100, 99,101, 99,100, 96, 97,  0,  0,  0,  0,
	0,  0,  0, 96, 97, 98, 98, 98, 98, 98, 97, 96,  0,  0,  0,  0,
	0,  0,  0, 96, 96, 97, 99, 99, 99, 97, 96, 96,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
  }, { // 兵(卒)
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  9,  9,  9, 11, 13, 11,  9,  9,  9,  0,  0,  0,  0,
	0,  0,  0, 19, 24, 34, 42, 44, 42, 34, 24, 19,  0,  0,  0,  0,
	0,  0,  0, 19, 24, 32, 37, 37, 37, 32, 24, 19,  0,  0,  0,  0,
	0,  0,  0, 19, 23, 27, 29, 30, 29, 27, 23, 19,  0,  0,  0,  0,
	0,  0,  0, 14, 18, 20, 27, 29, 27, 20, 18, 14,  0,  0,  0,  0,
	0,  0,  0,  7,  0, 13,  0, 16,  0, 13,  0,  7,  0,  0,  0,  0,
	0,  0,  0,  7,  0,  7,  0, 15,  0,  7,  0,  7,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
  }
};

// 判断棋子是否在棋盘中
inline bool IN_BOARD(int sq) {
	return ccInBoard[sq] != 0;
}

// 判断棋子是否在九宫中
inline bool IN_FORT(int sq) {
	return ccInFort[sq] != 0;
}

// 获得格子的横坐标
inline int RANK_Y(int sq) {
	return sq >> 4;
}

// 获得格子的纵坐标
inline int FILE_X(int sq) {
	return sq & 15;
}

// 根据纵坐标和横坐标获得格子
inline int COORD_XY(int x, int y) {
	return x + (y << 4);
}

// 翻转格子
inline int SQUARE_FLIP(int sq) {
	return 254 - sq;
}

// 纵坐标水平镜像
inline int FILE_FLIP(int x) {
	return 14 - x;
}

// 横坐标垂直镜像
inline int RANK_FLIP(int y) {
	return 15 - y;
}

// 格子水平镜像
inline int MIRROR_SQUARE(int sq) {
	return COORD_XY(FILE_FLIP(FILE_X(sq)), RANK_Y(sq));
}

// 格子水平镜像
inline int SQUARE_FORWARD(int sq, int sd) {
	return sq - 16 + (sd << 5);
}

// 走法是否符合帅(将)的步长
inline bool KING_SPAN(int sqSrc, int sqDst) {
	return ccLegalSpan[sqDst - sqSrc + 256] == 1;
}

// 走法是否符合仕(士)的步长
inline bool ADVISOR_SPAN(int sqSrc, int sqDst) {
	return ccLegalSpan[sqDst - sqSrc + 256] == 2;
}

// 走法是否符合相(象)的步长
inline bool BISHOP_SPAN(int sqSrc, int sqDst) {
	return ccLegalSpan[sqDst - sqSrc + 256] == 3;
}

// 相(象)眼的位置
inline int BISHOP_PIN(int sqSrc, int sqDst) {
	return (sqSrc + sqDst) >> 1;
}

// 马腿的位置
inline int KNIGHT_PIN(int sqSrc, int sqDst) {
	return sqSrc + ccKnightPin[sqDst - sqSrc + 256];
}

// 是否未过河
inline bool HOME_HALF(int sq, int sd) {
	return (sq & 0x80) != (sd << 7);
}

// 是否已过河
inline bool AWAY_HALF(int sq, int sd) {
	return (sq & 0x80) == (sd << 7);
}

// 是否在河的同一边
inline bool SAME_HALF(int sqSrc, int sqDst) {
	return ((sqSrc ^ sqDst) & 0x80) == 0;
}

// 是否在同一行
inline bool SAME_RANK(int sqSrc, int sqDst) {
	return ((sqSrc ^ sqDst) & 0xf0) == 0;
}

// 是否在同一列
inline bool SAME_FILE(int sqSrc, int sqDst) {
	return ((sqSrc ^ sqDst) & 0x0f) == 0;
}

// 获得红黑标记(红子是8，黑子是16)
inline int SIDE_TAG(int sd) {
	return 8 + (sd << 3);
}

// 获得对方红黑标记
inline int OPP_SIDE_TAG(int sd) {
	return 16 - (sd << 3);
}

// 获得走法的起点
inline int SRC(int mv) {
	return mv & 255;
}

// 获得走法的终点
inline int DST(int mv) {
	return mv >> 8;
}

// 根据起点和终点获得走法
inline int MOVE(int sqSrc, int sqDst) {
	return sqSrc + sqDst * 256;
}

// 走法水平镜像
inline int MIRROR_MOVE(int mv) {
	return MOVE(MIRROR_SQUARE(SRC(mv)), MIRROR_SQUARE(DST(mv)));
}

// 局面结构
struct PositionStruct {
	int sdPlayer;                   // 轮到谁走，0=红方，1=黑方
	int ucpcSquares[256];          // 棋盘上的棋子
	int vlWhite, vlBlack;           // 红、黑双方的子力价值
	int nDistance;                  // 距离根节点的步数

	void Startup(void);             // 初始化棋盘
	void ChangeSide(void) {         // 交换走子方
		sdPlayer = 1 - sdPlayer;
	}
	void AddPiece(int sq, int pc) { // 在棋盘上放一枚棋子
		ucpcSquares[sq] = pc;
		// 红方加分，黑方(注意"cucvlPiecePos"取值要颠倒)减分
		if (pc < 16) {
			vlWhite += cucvlPiecePos[pc - 8][sq];
		}
		else {
			vlBlack += cucvlPiecePos[pc - 16][SQUARE_FLIP(sq)];
		}
	}
	void DelPiece(int sq, int pc) { // 从棋盘上拿走一枚棋子
		ucpcSquares[sq] = 0;
		// 红方减分，黑方(注意"cucvlPiecePos"取值要颠倒)加分
		if (pc < 16) {
			vlWhite -= cucvlPiecePos[pc - 8][sq];
		}
		else {
			vlBlack -= cucvlPiecePos[pc - 16][SQUARE_FLIP(sq)];
		}
	}
	int Evaluate(void) const {      // 局面评价函数
		return (sdPlayer == 0 ? vlWhite - vlBlack : vlBlack - vlWhite) + ADVANCED_VALUE;
	}
	int MovePiece(int mv);                      // 搬一步棋的棋子
	void UndoMovePiece(int mv, int pcCaptured); // 撤消搬一步棋的棋子
	bool MakeMove(int mv, int &pcCaptured);     // 走一步棋
	void UndoMakeMove(int mv, int pcCaptured) { // 撤消走一步棋
		nDistance--;
		ChangeSide();
		UndoMovePiece(mv, pcCaptured);
	}
	int GenerateMoves(int *mvs) const;          // 生成所有走法
	bool LegalMove(int mv) const;               // 判断走法是否合理
	bool Checked(void) const;                   // 判断是否被将军
	bool IsMate(void);                          // 判断是否被杀
};

// 初始化棋盘
void PositionStruct::Startup(void) {
	int sq, pc;
	sdPlayer = vlWhite = vlBlack = nDistance = 0;
	//memset(ucpcSquares, 0, sizeof(ucpcSquares));
	for (sq = 0; sq < 256; sq++) {
		pc = cucpcStartup[sq];
		if (pc != 0) {
			AddPiece(sq, pc);
		}
	}
}

// 搬一步棋的棋子
int PositionStruct::MovePiece(int mv) {
	int sqSrc, sqDst, pc, pcCaptured;
	sqSrc = SRC(mv);
	sqDst = DST(mv);
	pcCaptured = ucpcSquares[sqDst];
	if (pcCaptured != 0) {
		DelPiece(sqDst, pcCaptured);
	}
	pc = ucpcSquares[sqSrc];
	DelPiece(sqSrc, pc);
	AddPiece(sqDst, pc);
	return pcCaptured;
}

// 撤消搬一步棋的棋子
void PositionStruct::UndoMovePiece(int mv, int pcCaptured) {
	int sqSrc, sqDst, pc;
	sqSrc = SRC(mv);
	sqDst = DST(mv);
	pc = ucpcSquares[sqDst];
	DelPiece(sqDst, pc);
	AddPiece(sqSrc, pc);
	if (pcCaptured != 0) {
		AddPiece(sqDst, pcCaptured);
	}
}

// 走一步棋
bool PositionStruct::MakeMove(int mv, int &pcCaptured) {
	bool pd = pcCaptured != 1e9 + 7 ? true : false;
	pcCaptured = MovePiece(mv);
	if (Checked()&& pd) {
		UndoMovePiece(mv, pcCaptured);
		return false;
	}
	ChangeSide();
	nDistance++;
	return true;
}

// 生成所有走法
int PositionStruct::GenerateMoves(int *mvs) const {
	int i, j, nGenMoves, nDelta, sqSrc, sqDst;
	int pcSelfSide, pcOppSide, pcSrc, pcDst;
	// 生成所有走法，需要经过以下几个步骤：

	nGenMoves = 0;
	pcSelfSide = SIDE_TAG(sdPlayer);
	pcOppSide = OPP_SIDE_TAG(sdPlayer);
	for (sqSrc = 0; sqSrc < 256; sqSrc++) {

		// 1. 找到一个本方棋子，再做以下判断：
		pcSrc = ucpcSquares[sqSrc];
		if ((pcSrc & pcSelfSide) == 0) {
			continue;
		}

		// 2. 根据棋子确定走法
		switch (pcSrc - pcSelfSide) {
		case PIECE_KING:
			for (i = 0; i < 4; i++) {
				sqDst = sqSrc + ccKingDelta[i];
				if (!IN_FORT(sqDst)) {
					continue;
				}
				pcDst = ucpcSquares[sqDst];
				if ((pcDst & pcSelfSide) == 0) {
					mvs[nGenMoves] = MOVE(sqSrc, sqDst);
					nGenMoves++;
				}
			}
			break;
		case PIECE_ADVISOR:
			for (i = 0; i < 4; i++) {
				sqDst = sqSrc + ccAdvisorDelta[i];
				if (!IN_FORT(sqDst)) {
					continue;
				}
				pcDst = ucpcSquares[sqDst];
				if ((pcDst & pcSelfSide) == 0) {
					mvs[nGenMoves] = MOVE(sqSrc, sqDst);
					nGenMoves++;
				}
			}
			break;
		case PIECE_BISHOP:
			for (i = 0; i < 4; i++) {
				sqDst = sqSrc + ccAdvisorDelta[i];
				if (!(IN_BOARD(sqDst) && HOME_HALF(sqDst, sdPlayer) && ucpcSquares[sqDst] == 0)) {
					continue;
				}
				sqDst += ccAdvisorDelta[i];
				pcDst = ucpcSquares[sqDst];
				if ((pcDst & pcSelfSide) == 0) {
					mvs[nGenMoves] = MOVE(sqSrc, sqDst);
					nGenMoves++;
				}
			}
			break;
		case PIECE_KNIGHT:
			for (i = 0; i < 4; i++) {
				sqDst = sqSrc + ccKingDelta[i];
				if (ucpcSquares[sqDst] != 0) {
					continue;
				}
				for (j = 0; j < 2; j++) {
					sqDst = sqSrc + ccKnightDelta[i][j];
					if (!IN_BOARD(sqDst)) {
						continue;
					}
					pcDst = ucpcSquares[sqDst];
					if ((pcDst & pcSelfSide) == 0) {
						mvs[nGenMoves] = MOVE(sqSrc, sqDst);
						nGenMoves++;
					}
				}
			}
			break;
		case PIECE_ROOK:
			for (i = 0; i < 4; i++) {
				nDelta = ccKingDelta[i];
				sqDst = sqSrc + nDelta;
				while (IN_BOARD(sqDst)) {
					pcDst = ucpcSquares[sqDst];
					if (pcDst == 0) {
						mvs[nGenMoves] = MOVE(sqSrc, sqDst);
						nGenMoves++;
					}
					else {
						if ((pcDst & pcOppSide) != 0) {
							mvs[nGenMoves] = MOVE(sqSrc, sqDst);
							nGenMoves++;
						}
						break;
					}
					sqDst += nDelta;
				}
			}
			break;
		case PIECE_CANNON:
			for (i = 0; i < 4; i++) {
				nDelta = ccKingDelta[i];
				sqDst = sqSrc + nDelta;
				while (IN_BOARD(sqDst)) {
					pcDst = ucpcSquares[sqDst];
					if (pcDst == 0) {
						mvs[nGenMoves] = MOVE(sqSrc, sqDst);
						nGenMoves++;
					}
					else {
						break;
					}
					sqDst += nDelta;
				}
				sqDst += nDelta;
				while (IN_BOARD(sqDst)) {
					pcDst = ucpcSquares[sqDst];
					if (pcDst != 0) {
						if ((pcDst & pcOppSide) != 0) {
							mvs[nGenMoves] = MOVE(sqSrc, sqDst);
							nGenMoves++;
						}
						break;
					}
					sqDst += nDelta;
				}
			}
			break;
		case PIECE_PAWN:
			sqDst = SQUARE_FORWARD(sqSrc, sdPlayer);
			if (IN_BOARD(sqDst)) {
				pcDst = ucpcSquares[sqDst];
				if ((pcDst & pcSelfSide) == 0) {
					mvs[nGenMoves] = MOVE(sqSrc, sqDst);
					nGenMoves++;
				}
			}
			if (AWAY_HALF(sqSrc, sdPlayer)) {
				for (nDelta = -1; nDelta <= 1; nDelta += 2) {
					sqDst = sqSrc + nDelta;
					if (IN_BOARD(sqDst)) {
						pcDst = ucpcSquares[sqDst];
						if ((pcDst & pcSelfSide) == 0) {
							mvs[nGenMoves] = MOVE(sqSrc, sqDst);
							nGenMoves++;
						}
					}
				}
			}
			break;
		}
	}
	return nGenMoves;
}

// 判断走法是否合理
bool PositionStruct::LegalMove(int mv) const {
	int sqSrc, sqDst, sqPin;
	int pcSelfSide, pcSrc, pcDst, nDelta;
	// 判断走法是否合法，需要经过以下的判断过程：

	// 1. 判断起始格是否有自己的棋子
	sqSrc = SRC(mv);
	pcSrc = ucpcSquares[sqSrc];
	pcSelfSide = SIDE_TAG(sdPlayer);
	if ((pcSrc & pcSelfSide) == 0) {
		return false;
	}

	// 2. 判断目标格是否有自己的棋子
	sqDst = DST(mv);
	pcDst = ucpcSquares[sqDst];
	if ((pcDst & pcSelfSide) != 0) {
		return false;
	}

	// 3. 根据棋子的类型检查走法是否合理
	switch (pcSrc - pcSelfSide) {
	case PIECE_KING:
		return IN_FORT(sqDst) && KING_SPAN(sqSrc, sqDst);
	case PIECE_ADVISOR:
		return IN_FORT(sqDst) && ADVISOR_SPAN(sqSrc, sqDst);
	case PIECE_BISHOP:
		return SAME_HALF(sqSrc, sqDst) && BISHOP_SPAN(sqSrc, sqDst) &&
			ucpcSquares[BISHOP_PIN(sqSrc, sqDst)] == 0;
	case PIECE_KNIGHT:
		sqPin = KNIGHT_PIN(sqSrc, sqDst);
		return sqPin != sqSrc && ucpcSquares[sqPin] == 0;
	case PIECE_ROOK:
	case PIECE_CANNON:
		if (SAME_RANK(sqSrc, sqDst)) {
			nDelta = (sqDst < sqSrc ? -1 : 1);
		}
		else if (SAME_FILE(sqSrc, sqDst)) {
			nDelta = (sqDst < sqSrc ? -16 : 16);
		}
		else {
			return false;
		}
		sqPin = sqSrc + nDelta;
		while (sqPin != sqDst && ucpcSquares[sqPin] == 0) {
			sqPin += nDelta;
		}
		if (sqPin == sqDst) {
			return pcDst == 0 || pcSrc - pcSelfSide == PIECE_ROOK;
		}
		else if (pcDst != 0 && pcSrc - pcSelfSide == PIECE_CANNON) {
			sqPin += nDelta;
			while (sqPin != sqDst && ucpcSquares[sqPin] == 0) {
				sqPin += nDelta;
			}
			return sqPin == sqDst;
		}
		else {
			return false;
		}
	case PIECE_PAWN:
		if (AWAY_HALF(sqDst, sdPlayer) && (sqDst == sqSrc - 1 || sqDst == sqSrc + 1)) {
			return true;
		}
		return sqDst == SQUARE_FORWARD(sqSrc, sdPlayer);
	default:
		return false;
	}
}

// 判断是否被将军
bool PositionStruct::Checked() const {
	int i, j, sqSrc, sqDst;
	int pcSelfSide, pcOppSide, pcDst, nDelta;
	pcSelfSide = SIDE_TAG(sdPlayer);
	pcOppSide = OPP_SIDE_TAG(sdPlayer);
	// 找到棋盘上的帅(将)，再做以下判断：

	for (sqSrc = 0; sqSrc < 256; sqSrc++) {
		if (ucpcSquares[sqSrc] != pcSelfSide + PIECE_KING) {
			continue;
		}

		// 1. 判断是否被对方的兵(卒)将军
		if (ucpcSquares[SQUARE_FORWARD(sqSrc, sdPlayer)] == pcOppSide + PIECE_PAWN) {
			return true;
		}
		for (nDelta = -1; nDelta <= 1; nDelta += 2) {
			if (ucpcSquares[sqSrc + nDelta] == pcOppSide + PIECE_PAWN) {
				return true;
			}
		}

		// 2. 判断是否被对方的马将军(以仕(士)的步长当作马腿)
		for (i = 0; i < 4; i++) {
			if (ucpcSquares[sqSrc + ccAdvisorDelta[i]] != 0) {
				continue;
			}
			for (j = 0; j < 2; j++) {
				pcDst = ucpcSquares[sqSrc + ccKnightCheckDelta[i][j]];
				if (pcDst == pcOppSide + PIECE_KNIGHT) {
					return true;
				}
			}
		}

		// 3. 判断是否被对方的车或炮将军(包括将帅对脸)
		for (i = 0; i < 4; i++) {
			nDelta = ccKingDelta[i];
			sqDst = sqSrc + nDelta;
			while (IN_BOARD(sqDst)) {
				pcDst = ucpcSquares[sqDst];
				if (pcDst != 0) {
					if (pcDst == pcOppSide + PIECE_ROOK || pcDst == pcOppSide + PIECE_KING) {
						return true;
					}
					break;
				}
				sqDst += nDelta;
			}
			sqDst += nDelta;
			while (IN_BOARD(sqDst)) {
				int pcDst = ucpcSquares[sqDst];
				if (pcDst != 0) {
					if (pcDst == pcOppSide + PIECE_CANNON) {
						return true;
					}
					break;
				}
				sqDst += nDelta;
			}
		}
		return false;
	}
	return false;
}

// 判断是否被杀
bool PositionStruct::IsMate(void) {
	int i, nGenMoveNum, pcCaptured;
	int mvs[MAX_GEN_MOVES];

	nGenMoveNum = GenerateMoves(mvs);
	for (i = 0; i < nGenMoveNum; i++) {
		pcCaptured = MovePiece(mvs[i]);
		if (!Checked()) {
			UndoMovePiece(mvs[i], pcCaptured);
			return false;
		}
		else {
			UndoMovePiece(mvs[i], pcCaptured);
		}
	}
	return true;
}

static PositionStruct pos; // 局面实例

// 与图形界面有关的全局变量
static struct {

	int sqSelected, mvLast;                       // 选中的格子，上一步棋
	bool bFlipped;                                // 是否翻转棋盘
} Xqwl;

// 与搜索有关的全局变量
static struct {
	int mvResult;             // 电脑走的棋
	int nHistoryTable[65536]; // 历史表
} Search;

// "qsort"按历史表排序的比较函数
static int CompareHistory(const void *lpmv1, const void *lpmv2) {
	return Search.nHistoryTable[*(int *)lpmv2] - Search.nHistoryTable[*(int *)lpmv1];
}

// 超出边界(Fail-Soft)的Alpha-Beta搜索过程
static int SearchFull(int vlAlpha, int vlBeta, int nDepth) {
	int i, nGenMoves, pcCaptured;
	int vl, vlBest, mvBest;
	int mvs[MAX_GEN_MOVES];
	// 一个Alpha-Beta完全搜索分为以下几个阶段

	// 1. 到达水平线，则返回局面评价值
	if (nDepth == 0) {
		return pos.Evaluate();
	}

	// 2. 初始化最佳值和最佳走法
	vlBest = -MATE_VALUE; // 这样可以知道，是否一个走法都没走过(杀棋)
	mvBest = 0;           // 这样可以知道，是否搜索到了Beta走法或PV走法，以便保存到历史表

	// 3. 生成全部走法，并根据历史表排序
	nGenMoves = pos.GenerateMoves(mvs);
	qsort(mvs, nGenMoves, sizeof(int), CompareHistory);

	// 4. 逐一走这些走法，并进行递归
	for (i = 0; i < nGenMoves; i++) {
		if (pos.MakeMove(mvs[i], pcCaptured)) {
			vl = -SearchFull(-vlBeta, -vlAlpha, nDepth - 1);
			pos.UndoMakeMove(mvs[i], pcCaptured);

			// 5. 进行Alpha-Beta大小判断和截断
			if (vl > vlBest) {    // 找到最佳值(但不能确定是Alpha、PV还是Beta走法)
				vlBest = vl;        // "vlBest"就是目前要返回的最佳值，可能超出Alpha-Beta边界
				if (vl >= vlBeta) { // 找到一个Beta走法
					mvBest = mvs[i];  // Beta走法要保存到历史表
					break;            // Beta截断
				}
				if (vl > vlAlpha) { // 找到一个PV走法
					mvBest = mvs[i];  // PV走法要保存到历史表
					vlAlpha = vl;     // 缩小Alpha-Beta边界
				}
			}
		}
	}

	// 5. 所有走法都搜索完了，把最佳走法(不能是Alpha走法)保存到历史表，返回最佳值
	if (vlBest == -MATE_VALUE) {
		// 如果是杀棋，就根据杀棋步数给出评价
		return pos.nDistance - MATE_VALUE;
	}
	if (mvBest != 0) {
		// 如果不是Alpha走法，就将最佳走法保存到历史表
		Search.nHistoryTable[mvBest] += nDepth * nDepth;
		if (pos.nDistance == 0) {
			// 搜索根节点时，总是有一个最佳走法(因为全窗口搜索不会超出边界)，将这个走法保存下来
			Search.mvResult = mvBest;
		}
	}
	return vlBest;
}

// 迭代加深搜索过程
static void SearchMain(void) {
	int i, t, vl;

	// 初始化
	memset(Search.nHistoryTable, 0, 65536 * sizeof(int)); // 清空历史表
	t = clock();       // 初始化定时器
	pos.nDistance = 0; // 初始步数

	int nGenMoves, pcCaptured;
	int mvs[MAX_GEN_MOVES];
	nGenMoves = pos.GenerateMoves(mvs);
	qsort(mvs, nGenMoves, sizeof(int), CompareHistory);

	for (i = 0; i < nGenMoves; i++) 
	{
		int sqSrc, sqDst, pc, pcCaptured, sd;
			sqSrc = SRC(mvs[i]);
			sqDst = DST(mvs[i]);
			pcCaptured = pos.ucpcSquares[sqDst];

			if (pcCaptured == 8) {
				Search.mvResult = mvs[i];
				return;
			}
	}

	// 迭代加深过程
	for (i = 1; i <= LIMIT_DEPTH; i++) {
		vl = SearchFull(-MATE_VALUE, MATE_VALUE, i);
		// 搜索到杀棋，就终止搜索
		if (vl > WIN_VALUE || vl < -WIN_VALUE) {
			break;
		}
		// 超过一秒，就终止搜索
		if (clock() - t > CLOCKS_PER_SEC) {
			break;
		}
	}
}


// 初始化棋局
void Startup(void) {
	pos.Startup();
	Xqwl.sqSelected = Xqwl.mvLast = 0;
}

int Chang_x_y(x_y sd)
{
	return 3+sd.x + (sd.y + 3) * 16;
}

void HumanMove(x_y Chess, x_y Target)
{
	int pc, mv, sq = Chang_x_y(Target);

	pc = 1e9 + 7;

		mv = MOVE(Chang_x_y(Chess), sq);

		pos.MakeMove(mv, pc);

}

// 电脑回应一步棋
int ResponseMove(void) {
	int pcCaptured = 1e9+7;
	// 电脑走一步棋
	SearchMain(); 
	pos.MakeMove(Search.mvResult, pcCaptured);
	// 清除上一步棋的选择标记

	// 把电脑走的棋标记出来
	Xqwl.mvLast = Search.mvResult;

	return Search.mvResult;
}

