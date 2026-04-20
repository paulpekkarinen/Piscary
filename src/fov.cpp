//Legend of Saladir - fov.cpp

//Refactored 25.2.2023 - 6.4.2026 Paul K. Pekkarinen

#include <cmath>
#include "caves.h"
#include "fov.h"
#include "gameview.h"

const int Fov::fov_multipliers[4][8]=
{
	{1, 0, 0, -1, -1, 0, 0, 1},
	{0, 1, -1, 0, 0, -1, 1, 0},
	{0, 1, 1, 0, 0, -1, -1, 0},
	{1, 0, 0, 1, -1, 0, 0, -1}
};

void Fov::Cast_Fov(int x, int y, int row,
	float start_slope, float end_slope, int xx, int xy, int yx, int yy)
{
	if (start_slope < end_slope) return;
    
	float next_start_slope = start_slope;
    Coord c;

	for (int i = row; i <= fov_radius; i++)
	{
        bool blocked = false;
		for (int dx = -i, dy = -i; dx <= 0; dx++)
		{
			float l_slope = (dx - 0.5f) / (dy + 0.5f);
			float r_slope = (dx + 0.5f) / (dy - 0.5f);
			
			if (start_slope < r_slope) continue;
			else
			{
				if (end_slope > l_slope) break;
			}

			const int sax = dx * xx + dy * xy;
			const int say = dx * yx + dy * yy;
			if ((sax < 0 && (int)std::abs(sax) > x) || (say < 0 && (int)std::abs(say) > y))
				continue;

			c.x=x+sax;
			c.y=y+say;

			//can check with level, because gameview is equal size
			if (lvl->Is_Outside(c))
				continue;

			gameview.Set_Fov(c);
			
			if (blocked)
			{
				if (lvl->Is_Blocking_Los(c))
				{
					next_start_slope = r_slope;
					continue;
				}
				else
				{
					blocked = false;
					start_slope = next_start_slope;
				}
			}
			else if (lvl->Is_Blocking_Los(c))
			{
				blocked = true;
				next_start_slope = r_slope;
				Cast_Fov(x, y, i + 1, start_slope, l_slope, xx, xy, yx, yy);
			}
		}
		if (blocked) break;
	}
}

void Fov::Calculate_Fov(const Coord &oc)
{
    for (int i = 0; i < 8; i++)
	{
        Cast_Fov(oc.x, oc.y, 1, 1.0, 0.0,
			fov_multipliers[0][i],
			fov_multipliers[1][i],
			fov_multipliers[2][i],
			fov_multipliers[3][i]);
    }

	//origin coordinate is not covered by shadowcasting routine
	gameview.Set_Fov(oc);
}

