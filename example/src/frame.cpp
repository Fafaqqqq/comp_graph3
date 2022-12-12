#include "frame.h"

#include <cmath>
#include <cstdio>

#define pi 3.1415

Frame::Frame(uint32_t width, uint32_t height, uint32_t background)
: data(new uint32_t[width * height]), width(width), height(height)
{
  for (uint32_t i = 0; i < this->height; ++i)
	{
		for (uint32_t j = 0; j < this->width; ++j)
		{
			*(data + j + i * this->width) = background;
		}
	}
}

Frame::~Frame()
{
  if (data)
  {
    delete[] data;
  }
}

void Frame::SetPixel(uint32_t x, uint32_t y)
{
  *(data + x + y * width) = 0;
}

void Frame::DrawLine(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
  const int deltaX = abs(x2 - x1);
  const int deltaY = abs(y2 - y1);
  const int signX = x1 < x2 ? 1 : -1;
  const int signY = y1 < y2 ? 1 : -1;
  int error = deltaX - deltaY;

  SetPixel(x2, y2);

  while (x1 != x2 || y1 != y2)
  {
    SetPixel(x1, y1);
    int error2 = error * 2;
    if (error2 > -deltaY)
    {
      error -= deltaY;
      x1 += signX;
    }
    if (error2 < deltaX)
    {
      error += deltaX;
      y1 += signY;
    }
  }
}

void Frame::MakeAxes()
{
//------- Отрисовка координатных осей -------
  DrawLine(50, 50, 50, height - 50);
  DrawLine(width - 50, 50, width - 50, height - 50);
  
  DrawLine(50, 50, width - 50, 50);
  DrawLine(50, height - 50, width - 50, height - 50);
//-------------------------------------------

//------- Отрисовка букв X и Y -------
  DrawLine(width / 2 - 5, 15, width / 2 + 5, 40);
  DrawLine(width / 2 + 5, 15, width / 2 - 5, 40);

  DrawLine(35, height / 2 - 12, 25, height / 2 + 13);
  DrawLine(25, height / 2 - 12, 30, height / 2);
//------------------------------------
}

void Frame::MakeGrid()
{
  for (uint32_t x = 50; x < width - 50; x += 25)
  {
    DrawLine(x, 50, x, height - 50);
  }

  for (uint32_t y = 50; y < height - 50; y += 25)
  {
    DrawLine(50, y, width - 50, y);
  }
}

void Frame::DrawCircle(uint32_t x0, uint32_t y0, uint32_t radius) {
	int x = 0;
	int y = radius;
	int delta = 1 - 2 * radius;
	int error = 0;
	while(y >= 0) {
		SetPixel(x0 + x, y0 + y);
		SetPixel(x0 + x, y0 - y);
		SetPixel(x0 - x, y0 + y);
		SetPixel(x0 - x, y0 - y);
		error = 2 * (delta + y) - 1;
		if(delta < 0 && error <= 0) {
			++x;
			delta += 2 * x + 1;
			continue;
		}
		error = 2 * (delta - x) - 1;
		if(delta > 0 && error > 0) {
			--y;
			delta += 1 - 2 * y;
			continue;
		}
		++x;
		delta += 2 * (x - y);
		--y;
	}
}

void Frame::DrawFrame()
{
  DrawCircle(width / 2 - 20, height / 2 + 30, 140);
	DrawCircle(width / 2 - 20, height / 2 + 30, 141);
	DrawCircle(width / 2 - 20, height / 2 + 30, 142);
	DrawCircle(width / 2 - 20, height / 2 + 30, 143);

  DrawCircle(width / 2 - 150, height / 2 - 100, 120);
	DrawCircle(width / 2 - 150, height / 2 - 100, 121);
	DrawCircle(width / 2 - 150, height / 2 - 100, 122);
	DrawCircle(width / 2 - 150, height / 2 - 100, 123);

  DrawCircleAxes(width / 2 + 20, height / 2 - 120, 170);
	DrawCircleAxes(width / 2 + 20, height / 2 - 120, 171);
	DrawCircleAxes(width / 2 + 20, height / 2 - 120, 172);
	DrawCircleAxes(width / 2 + 20, height / 2 - 120, 173);

	auto res = FindIntersection();

	FloodFill8(res.x, res.y, 0xEBE39EFF, 0);
}

void Frame::DrawCircleAxes(uint32_t x0, uint32_t y0, uint32_t radius)
{
	for (double i = 0; i < 2 * pi; i += 0.005)
	{
		double x = radius * cos(i);
		double y = radius * sin(i);

		SetPixel(x0 + (uint32_t)floor(x), y0 + (uint32_t)floor(y));
	}
}


void Frame::FloodFill8(const uint32_t& x, const uint32_t& y, const uint32_t& fillcolor, const uint32_t& stopcolor)
{
	if(x >= 0 && x < width && y >= 0 && y < height && data[x + y * width] != fillcolor && data[x + y * width] != stopcolor)
	{
		data[x + y * width] = fillcolor;
		
		FloodFill8(x + 1, y, fillcolor, stopcolor);
		FloodFill8(x - 1, y, fillcolor, stopcolor);
		FloodFill8(x, y + 1, fillcolor, stopcolor); 
		FloodFill8(x, y - 1, fillcolor, stopcolor);

		FloodFill8(x + 1, y + 1, fillcolor, stopcolor); 
		FloodFill8(x - 1, y - 1, fillcolor, stopcolor); 
		FloodFill8(x - 1, y + 1, fillcolor, stopcolor); 
		FloodFill8(x + 1, y - 1, fillcolor, stopcolor); 
	}
}

Vector2<int32_t> Frame::FindIntersection()
{
	uint32_t x0 = width / 2 - 20;
	uint32_t y0 = height / 2 + 30;
	uint32_t r0 = 140;

	uint32_t x1 = width / 2 - 150;
	uint32_t y1 = height / 2 - 100;
	uint32_t r1 = 120;

	uint32_t x2 = width / 2 + 20;
	uint32_t y2 = height / 2 - 120;
	uint32_t r2 = 170;

	int count = 50;

	for (double i = 0.; i < 2 * pi; i += 0.005)
	{
		double x = (r0 - 10) * cos(i) + x0;
		double y = (r0 - 10) * sin(i) + y0;

		if (pow((x - x1), 2) + pow((y - y1), 2) < pow(r1, 2) &&
				pow((x - x2), 2) + pow((y - y2), 2) < pow(r2, 2))
		{
			if (!count--)
			{
				return {x, y};
			}
		}
	}

	return {-1, -1};
}