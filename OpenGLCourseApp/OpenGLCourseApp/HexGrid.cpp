#include "HexGrid.h"

HexGrid::HexGrid()
{
	numberOfRings = 5;
	tileSpacing = 1;
}

HexGrid::HexGrid(int setRings, float spacing)
{
	numberOfRings = setRings;
	tileSpacing = spacing;
}

std::vector<glm::vec3> HexGrid::CreateHexGrid()
{
	std::vector<glm::vec3> hexCoords;

	for (size_t q = -numberOfRings; q < numberOfRings; q++)
	{
		for (size_t r = -numberOfRings; r < numberOfRings; r++)
		{
			for (size_t s = -numberOfRings; s < numberOfRings; s++)
			{
				bool isValid = q + r + s == 0;
				if (isValid) {
					glm::vec3 pos = GetWorldPosFromCoords(q, r, s);
					hexCoords.push_back(pos);
				}
			}
		}
	}

	return std::vector<glm::vec3>();
}


glm::vec3 HexGrid::GetWorldPosFromCoords(int q, int r, int s)
{
	return glm::vec3();
}

HexGrid::~HexGrid()
{
}
