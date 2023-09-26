#pragma once
#include <glm/glm.hpp>
#include <vector>

struct Orientation
{
	float f0, f1, f2, f3, b0, b1, b2, b3, start_angle;

	Orientation(float f0, float f1, float f2, float f3, float b0, float b1, float b2, float b3, float start_angle) {
		f0 = f0;
		f1 = f1;
		f2 = f2;
		f3 = f3;
		b0 = b0; 
		b1 = b1;
		b2 = b2;
		b3 = b3;
		start_angle = start_angle;
	}
};

struct Hex
{
	int q, r, s;
};


struct Layout
{
	Orientation orientation; 
	float size;
	glm::vec3 origin;
};


class HexGrid
{
public:
	HexGrid();
	HexGrid(int setRings, float spacing);

	std::vector<glm::vec3>  CreateHexGrid();
	glm::vec3 GetWorldPosFromCoords(int q, int r, int s);
	
	~HexGrid();

private:
	int numberOfRings;
	float tileSpacing;

	Orientation* layout_pointy = new Orientation(sqrt(3.0), sqrt(3.0) / 2.0, 0.0, 3.0 / 2.0, sqrt(3.0) / 3.0, -1.0 / 3.0, 0.0, 2.0 / 3.0, 0.5);
	Orientation* layout_flat = new Orientation(3.0 / 2.0, 0.0, sqrt(3.0) / 2.0, sqrt(3.0), 2.0 / 3.0, 0.0, -1.0 / 3.0, sqrt(3.0) / 3.0, 0.0);
};

