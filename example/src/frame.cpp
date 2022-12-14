#include "frame.h"
#include "circle.h"

#include <cmath>
#include <cstdint>
#include <vector>
#include <iostream>
#include <chrono>

#define pi 3.1415

Frame::Frame(uint32_t width, uint32_t height)
: data(new uint32_t[width * height]), width(width), height(height)
{
	for (uint32_t i = 0; i < this->height; ++i)
	{
		for (uint32_t j = 0; j < this->width; ++j)
		{
			*(data + j + i * this->width) = attr.background;
		}
	}
}

Frame::Frame(uint32_t width, uint32_t height, Attributes attr)
: Frame(width, height)
{
	this->attr = attr;
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

void Frame::DrawLine(Vector2<int32_t> begin, Vector2<int32_t> end)
{
  const int32_t deltaX = abs(end.x- begin.x);
  const int32_t deltaY = abs(end.y- begin.y);
  const int32_t signX = begin.x < end.x ? 1 : -1;
  const int32_t signY = begin.y < end.y ? 1 : -1;
  int32_t error = deltaX - deltaY;

  SetPixel(end.x, end.y);

  while (begin.x != end.x || begin.y != end.y)
  {
    SetPixel(begin.x, begin.y);
    int32_t error2 = error * 2;
    if (error2 > -deltaY)
    {
      error -= deltaY;
      begin.x += signX;
    }
    if (error2 < deltaX)
    {
      error += deltaX;
      begin.y += signY;
    }
  }
}

void Frame::SetAxes()
{
//------- Отрисовка координатных осей -------
  DrawLine({50, 50}, {50, height - 50});
  DrawLine({width - 50, 50}, {width - 50, height - 50});
  
  DrawLine({50, 50}, {width - 50, 50});
  DrawLine({50, height - 50}, {width - 50, height - 50});
//-------------------------------------------

//------- Отрисовка букв X и Y -------
  DrawLine({width / 2 - 5, 15}, {width / 2 + 5, 40});
  DrawLine({width / 2 + 5, 15}, {width / 2 - 5, 40});

  DrawLine({35, height / 2 - 12}, {25, height / 2 + 13});
  DrawLine({25, height / 2 - 12}, {30, height / 2});
//------------------------------------
}

void Frame::SetGrid()
{
  for (uint32_t x = 50; x < width - 50; x += 25)
  {
    DrawLine({x, 50}, {x, height - 50});
  }

  for (uint32_t y = 50; y < height - 50; y += 25)
  {
    DrawLine({50, y}, {width - 50, y});
  }
}

void Frame::DrawCircleBresenham(const Circle<uint32_t>& circle) {
	int32_t x = 0;
	int32_t y = circle.radius;
	int32_t delta = 1 - 2 * circle.radius;
	int32_t error = 0;

	while(y >= 0)
	{
		SetPixel(circle.centre.x + x, circle.centre.y + y);
		SetPixel(circle.centre.x + x, circle.centre.y - y);
		SetPixel(circle.centre.x - x, circle.centre.y + y);
		SetPixel(circle.centre.x - x, circle.centre.y - y);

		error = 2 * (delta + y) - 1;
		if(delta < 0 && error <= 0)
		{
			++x;
			delta += 2 * x + 1;
			continue;
		}

		error = 2 * (delta - x) - 1;
		if(delta > 0 && error > 0)
		{
			--y;
			delta += 1 - 2 * y;
			continue;
		}

		++x;
		delta += 2 * (x - y);
		--y;
	}
}

void Frame::DrawCirclePolar(const Circle<uint32_t>& circle)
{
	for (double i = 0; i < 2 * pi; i += 0.005)
	{
		double x = circle.radius * cos(i);
		double y = circle.radius * sin(i);

		SetPixel(circle.centre.x + (uint32_t)floor(x), circle.centre.y + (uint32_t)floor(y));
	}
}

void Frame::Draw(const std::vector<Circle<uint32_t>>& circles, uint32_t fillcolor)
{
	if (attr.drawcicles)
	{
		for (uint32_t i = 0; i < circles.size(); i++)
		{
			auto begin = std::chrono::steady_clock::now();
			if (attr.drawtype == DRAW_A)
			{
				DrawCirclePolar(circles[i]);
				auto end = std::chrono::steady_clock::now();
				auto elapsed_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

				printf("[Draw type - A, circle - %d ]: time -> %.4fms\n", i, elapsed_ms.count() / 1000000.);
			}
			else
			{
				auto begin = std::chrono::steady_clock::now();
				DrawCircleBresenham(circles[i]);
				auto end = std::chrono::steady_clock::now();
				auto elapsed_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

				printf("[Draw type - B, circle - %d ]: time -> %.4fms\n", i, elapsed_ms.count() / 1000000.);
			}
		}
	}
	else 
	{
		if (attr.drawtype == DRAW_A)
		{
			auto begin = std::chrono::steady_clock::now();
			DrawIntersectionPolar(circles);
			auto end = std::chrono::steady_clock::now();
			auto elapsed_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

			printf("[Draw type A]: time -> %.4fms\n", elapsed_ms.count() / 1000000.);
		}
		else
		{
			auto begin = std::chrono::steady_clock::now();
			DrawIntersectionBresenham(circles);
			auto end = std::chrono::steady_clock::now();
			auto elapsed_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

			printf("[Draw type B]: time -> %.4fms\n", elapsed_ms.count() / 1000000.);
		}
	}

	auto intersected_points = std::move(FindIntersection(circles));

	for (auto& point : intersected_points)
	{
		if (attr.filltype == FILL_A)
		{
			auto begin = std::chrono::steady_clock::now();
			FloodFill8Recursive(point, fillcolor, 0);
			auto end = std::chrono::steady_clock::now();
			auto elapsed_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

			printf("[Fill type A]: time -> %.4fms\n", elapsed_ms.count() / 1000000.);
		}
		else
		{
			auto begin = std::chrono::steady_clock::now();
			FloodFill8Stack(point, fillcolor, 0);
			auto end = std::chrono::steady_clock::now();
			auto elapsed_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

			printf("[Fill type B]: time -> %.4fms\n", elapsed_ms.count() / 1000000.);
		}
	}
	
	if (attr.axes)
	{
		SetAxes();
	}

	if (attr.grid)
	{
		SetGrid();
	}
}

void Frame::DrawIntersectionBresenham(const std::vector<Circle<uint32_t>>& circles)
{
	for (uint32_t i = 0; i < circles.size(); i++)
	{
		int32_t intersect_count = 0;
		int32_t x = 0;
		int32_t y = circles[i].radius;
		int32_t delta = 1 - 2 * circles[i].radius;
		int32_t error = 0;

		while(y >= 0)
		{
			IsInsideIntersection({circles[i].centre.x + x, circles[i].centre.y - y}, circles);
			IsInsideIntersection({circles[i].centre.x + x, circles[i].centre.y + y}, circles);
			IsInsideIntersection({circles[i].centre.x - x, circles[i].centre.y - y}, circles);
			IsInsideIntersection({circles[i].centre.x - x, circles[i].centre.y + y}, circles);

			error = 2 * (delta + y) - 1;
			if(delta < 0 && error <= 0)
			{
				++x;
				delta += 2 * x + 1;
				continue;
			}

			error = 2 * (delta - x) - 1;
			if(delta > 0 && error > 0)
			{
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

void Frame::IsInsideIntersection(const Vector2<uint32_t>& pixel, const std::vector<Circle<uint32_t>>& circles)
{
	uint32_t intersect_count = 0;

	for (uint32_t j = 0; j < circles.size(); j++)
	{
		double x_ = circles[j].centre.x > pixel.x ? pixel.x + 2 : pixel.x - 2;
		double y_ = circles[j].centre.y > pixel.y ? pixel.y + 2 : pixel.y - 2;

		if (pow((x_ - circles[j].centre.x), 2) + pow((y_ - circles[j].centre.y), 2) <= pow(circles[j].radius, 2))
		{
			intersect_count++;
		}
	}
	
	if (intersect_count == 3)
	{
		SetPixel(pixel.x, pixel.y);
	}
}

void Frame::DrawIntersectionPolar(const std::vector<Circle<uint32_t>>& circles)
{
	for (uint32_t i = 0; i < circles.size(); i++)
	{
		for (double angle = 0; angle < 2 * pi; angle += 0.005)
		{
			uint32_t intersect_count = 0;

			double x = circles[i].radius * cos(angle) + circles[i].centre.x;
			double y = circles[i].radius * sin(angle) + circles[i].centre.y;

			double x_ = (circles[i].radius - 1) * cos(angle) + circles[i].centre.x;
			double y_ = (circles[i].radius - 1) * sin(angle) + circles[i].centre.y;
			
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

void Frame::FloodFill8Recursive(Vector2<uint32_t> point, uint32_t fillcolor, uint32_t stopcolor)
{
	if(point.x >= 0 && point.x < width &&
	   point.y >= 0 && point.y < height && 
		 data[point.x + point.y * width] != fillcolor && 
		 data[point.x + point.y * width] != stopcolor)
	{
		data[point.x + point.y * width] = fillcolor;
		
		FloodFill8Recursive({point.x + 1, point.y}, fillcolor, stopcolor);
		FloodFill8Recursive({point.x - 1, point.y}, fillcolor, stopcolor);
		FloodFill8Recursive({point.x, point.y + 1}, fillcolor, stopcolor); 
		FloodFill8Recursive({point.x, point.y - 1}, fillcolor, stopcolor);

		// FloodFill8({point.x + 1, point.y + 1}, fillcolor, stopcolor); 
		// FloodFill8({point.x - 1, point.y - 1}, fillcolor, stopcolor); 
		// FloodFill8({point.x - 1, point.y + 1}, fillcolor, stopcolor); 
		// FloodFill8({point.x + 1, point.y - 1}, fillcolor, stopcolor); 
	}
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

		*top = fillcolor;

		if(*(top - width) != stopcolor && *(top - width) == attr.background)
		{
			*stack_ptr++ = top-width;
		}

		if(top[+width] != stopcolor && top[+width] == attr.background)
		{
			*stack_ptr++ = top+width;
		}

		if(top[-1] != stopcolor && top[-1] == attr.background)
		{
			*stack_ptr++ = top-1;
		}

		if(top[+1] != stopcolor && top[+1] == attr.background)
		{
			*stack_ptr++ = top+1;
		}
	}
}

std::vector<Vector2<uint32_t>> Frame::FindIntersection(const std::vector<Circle<uint32_t>>& circles)
{
	uint32_t count = 50;
	std::vector<Vector2<uint32_t>> intersected_points;

	for (double angle = 0; angle < 2 * pi; angle += 0.005)
	{
		uint32_t intersect_count = 0;
		
		double x = (circles[0].radius - 1) * cos(angle) + circles[0].centre.x;
		double y = (circles[0].radius - 1) * sin(angle) + circles[0].centre.y;
		
		for (uint32_t j = 1; j < circles.size(); j++)
		{
			if (pow((x - circles[j].centre.x), 2) + pow((y - circles[j].centre.y), 2) <= pow(circles[j].radius, 2))
			{
				intersect_count++;
			}
		}

		if (intersect_count == 2 && !count--)
		{
			intersected_points.push_back({(uint32_t)x, (uint32_t)y});
		}
	}

	return std::move(intersected_points);
}
