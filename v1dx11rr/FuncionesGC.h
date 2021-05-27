#pragma once
#include <stdlib.h>

float PosXRndm(int min, int max)
{
	if (min < 0 && max > 0)
	{
		int ayuda = min * -1;

		int x = rand() % (max + ayuda);

		return x - ayuda;
	}

	if (min < 0 && max < 0)
	{
		int ayuda = min * -1;
		int ayuda2 = ayuda + max;

		int x = rand() % ayuda2;

		return x - ayuda;
	}
	if (min >= 0 && max >= 0)
	{
		float randomNum = (rand() % (max - min)) + min;

		return randomNum;
	}


}

float PosZRndm(int min, int max)
{
	if (min < 0 && max > 0)
	{
		int ayuda = min * -1;

		int x = rand() % (max + ayuda);

		return x - ayuda;
	}

	if (min < 0 && max < 0)
	{
		int ayuda = min * -1;
		int ayuda2 = ayuda + max;

		int x = rand() % ayuda2;

		return x - ayuda;
	}

	if (min >= 0 && max >= 0)
	{
		float randomNum = (rand() % (max - min)) + min;

		return randomNum;
	}

}


float clamp(float n, float lower, float upper)
{
	if (n > lower && n < upper)
		return n;
	if (n > upper)
		return upper;
	if (n < lower)
		return lower;
}

bool CheckCollision(float CamX, float CamY, float modXMin, float modXMax, float modYMin, float modYMax)
{
	if ((CamX > modXMin && CamX < modXMax) && (CamY > modYMin && CamY < modYMax))
	{
		return true;
	}
	return false;
}