#include"pch.h"


using namespace std;

void main()
{
	int tim = 0;
	while (1)
	{
		if (make_map(tim++) == 1)
			run();
		else
			run_AI();
		cleardevice();
	}
}