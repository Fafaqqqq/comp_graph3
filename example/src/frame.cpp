#include "frame.h"

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <vector>

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
	if (circles.size() == 0)
	{
		return;
	}

	for (uint32_t i = 0; i < circles.size() - 1; i++)
	{
		DrawCircle(circles[i]);
	}

	DrawCircleAxes(circles[circles.size() - 1]);

	auto res = FindIntersection();

	if (res)
	{
		FloodFill8Stack(*res, 0xEBE39EFF, 0);
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

Vector2<uint32_t>* Frame::FindIntersection()
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
				return new Vector2<uint32_t>{(uint32_t)x, (uint32_t)y};
			}
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
