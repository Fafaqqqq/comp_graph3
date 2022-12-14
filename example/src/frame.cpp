#include "frame.h"
#include "circle.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <vector>
#include <iostream>

#define pi 3.1415

Frame::Frame(uint32_t width, uint32_t height, uint32_t background, Attributes attr)
: data(new uint32_t[width * height]), width(width), height(height), attr(attr)
{
  for (uint32_t i = 0; i < this->height; ++i)
	{
		for (uint32_t j = 0; j < this->width; ++j)
		{
			*(data + j + i * this->width) = background;
		}
	}

	// if (this->attr.axes)
	// {
	// 	MakeAxes();
	// }

	// if (this->attr.grid)
	// {
	// 	MakeGrid();
	// }
}

Frame::Frame(uint32_t width,
						 uint32_t height,
						 const std::vector<Circle<uint32_t>>& circles,
						 uint32_t background,
						 Frame::Attributes attr)
: Frame(width, height, background, attr)
{
	if (circles.size())
	{
		for (uint32_t i = 0; i < circles.size(); i++)
		{
			DrawCircle(circles[i]);
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

void Frame::DrawCircle(const Circle<uint32_t>& circle) {
	int x = 0;
	int y = circle.radius;
	int delta = 1 - 2 * circle.radius;
	int error = 0;
	while(y >= 0)
	{
		SetPixel(circle.centre.x + x, circle.centre.y + y);
		SetPixel(circle.centre.x + x, circle.centre.y - y);
		SetPixel(circle.centre.x - x, circle.centre.y + y);
		SetPixel(circle.centre.x - x, circle.centre.y - y);
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

void Frame::DrawCircleAxes(const Circle<uint32_t>& circle)
{
	for (double i = 0; i < 2 * pi; i += 0.005)
	{
		double x = circle.radius * cos(i);
		double y = circle.radius * sin(i);

		SetPixel(circle.centre.x + (uint32_t)floor(x), circle.centre.y + (uint32_t)floor(y));
	}
}

void Frame::DrawFrame(const std::vector<Circle<uint32_t>>& circles)
{
	DrawIntersectionBresenham(circles);
	// DrawIntersectionSinCos(circles);

	// for (auto & circle : circles)
	// {
	// 	DrawCircle(circle);
	// }


	auto res = FindIntersection(circles);

	if (res)
	{
		FloodFill8(*res, 0xEBE39EFF, 0);
	}
}

void Frame::DrawIntersectionBresenham(const std::vector<Circle<uint32_t>>& circles)
{
	for (uint32_t i = 0; i < circles.size(); i++)
	{
		uint32_t intersect_count = 0;
		int x = 0;
		int y = circles[i].radius;
		int delta = 1 - 2 * circles[i].radius;
		int error = 0;

		while(y >= 0)
		{
			CheckAndSetPixel({circles[i].centre.x + x, circles[i].centre.y - y}, circles);
			CheckAndSetPixel({circles[i].centre.x + x, circles[i].centre.y + y}, circles);
			CheckAndSetPixel({circles[i].centre.x - x, circles[i].centre.y - y}, circles);
			CheckAndSetPixel({circles[i].centre.x - x, circles[i].centre.y + y}, circles);

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
}

void Frame::CheckAndSetPixel(const Vector2<uint32_t>& point, const std::vector<Circle<uint32_t>>& circles)
{
	uint32_t intersect_count = 0;
	double x_ = point.x - 1;
	double y_ = point.y - 1;

	for (uint32_t j = 0; j < circles.size(); j++)
	{
		double x_ = circles[j].centre.x > point.x ? point.x + 2 : point.x - 2;
		double y_ = circles[j].centre.y > point.y ? point.y + 2 : point.y - 2;

		if (pow((x_ - circles[j].centre.x), 2) + pow((y_ - circles[j].centre.y), 2) <= pow(circles[j].radius, 2))
		{
			intersect_count++;
		}
	}
	
	if (intersect_count == 3)
	{
		SetPixel(point.x, point.y);
	}
}

void Frame::DrawIntersectionSinCos(const std::vector<Circle<uint32_t>>& circles)
{
	for (uint32_t i = 0; i < circles.size(); i++)
	{
		for (double angle = 0; angle < 2 * pi; angle += 0.005)
		{
			uint32_t intersect_count = 0;

			double x = circles[i].radius * cos(angle) + circles[i].centre.x;
			double y = circles[i].radius * sin(angle) + circles[i].centre.y;

			double x_ = (circles[i].radius - 2) * cos(angle) + circles[i].centre.x;
			double y_ = (circles[i].radius - 2) * sin(angle) + circles[i].centre.y;
			
			for (uint32_t j = 0; j < circles.size(); j++)
			{
				if (pow((x_ - circles[j].centre.x), 2) + pow((y_ - circles[j].centre.y), 2) <= pow(circles[j].radius, 2))
				{
					intersect_count++;
				}
			}

			if (intersect_count >= 3)
			{
				SetPixel((uint32_t)floor(x),(uint32_t)floor(y));
			}
		}
	}
}

void Frame::FloodFill8(Vector2<uint32_t> point, uint32_t fillcolor, uint32_t stopcolor)
{
	if(point.x >= 0 && point.x < width &&
	   point.y >= 0 && point.y < height && 
		 data[point.x + point.y * width] != fillcolor && 
		 data[point.x + point.y * width] != stopcolor)
	{
		data[point.x + point.y * width] = fillcolor;
		
		FloodFill8({point.x + 1, point.y}, fillcolor, stopcolor);
		FloodFill8({point.x - 1, point.y}, fillcolor, stopcolor);
		FloodFill8({point.x, point.y + 1}, fillcolor, stopcolor); 
		FloodFill8({point.x, point.y - 1}, fillcolor, stopcolor);

		// FloodFill8({point.x + 1, point.y + 1}, fillcolor, stopcolor); 
		// FloodFill8({point.x - 1, point.y - 1}, fillcolor, stopcolor); 
		// FloodFill8({point.x - 1, point.y + 1}, fillcolor, stopcolor); 
		// FloodFill8({point.x + 1, point.y - 1}, fillcolor, stopcolor); 
	}
}

Vector2<uint32_t>* Frame::FindIntersection(const std::vector<Circle<uint32_t>>& circles)
{
	int count = 50;

	for (double angle = 0; angle < 2 * pi; angle += 0.005)
	{
		uint32_t intersect_count = 0;
		
		double x = (circles[0].radius - 10) * cos(angle) + circles[0].centre.x;
		double y = (circles[0].radius - 10) * sin(angle) + circles[0].centre.y;
		
		for (uint32_t j = 1; j < circles.size(); j++)
		{
			if (pow((x - circles[j].centre.x), 2) + pow((y - circles[j].centre.y), 2) <= pow(circles[j].radius, 2))
			{
				intersect_count++;
			}
		}

		if (intersect_count == 2 && !count--)
		{
			return new Vector2<uint32_t>{(uint32_t)x, (uint32_t)y};
		}
	}

	return nullptr;
}

void Frame::FloodFill8Stack(Vector2<uint32_t> startpoint, uint32_t fillcolor, uint32_t stopcolor)
{
	uint32_t* data_ptr = data;
	
	std::vector<uint32_t*> stack(width * height);

	auto** stack_ptr = stack.data();
	*stack_ptr++ = data_ptr + startpoint.x + startpoint.y*width;
	while(stack.data() != stack_ptr)
	{
		uint32_t* top = *(--stack_ptr);
		size_t off = size_t(top-data);
		// printf("filling %lu %lu\n", off%width, off/width);
		*top = stopcolor;
		// uint32_t off = -width;
		if(*(top - width) != stopcolor)
			*stack_ptr++ = top-width;
		if(top[+width] != stopcolor)
			*stack_ptr++ = top+width;
		if(top[-1] != stopcolor)
			*stack_ptr++ = top-1;
		if(top[+1] != stopcolor)
			*stack_ptr++ = top+1;
	}

}
