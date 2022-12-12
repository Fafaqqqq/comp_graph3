#ifndef FRAME_H
#define FRAME_H

#include <cstdint>
#include <vector>

#include "vector2.h"

class Frame
{
  public:
    Frame(uint32_t width,
          uint32_t height,
          uint32_t background = 0xFFFFFFFF);

    Frame(uint32_t width,
          uint32_t height,
          const std::vector<Vector2<uint32_t>>& points,
          uint32_t background = 0xFFFFFFFF);

    ~Frame();

    void MakeAxes();
    void MakeGrid();
    void DrawFrame();
    void FillShape(uint32_t color = 0xFFFFFFFF);

    inline uint32_t* GetData()
    {
      return data;
    }

    inline uint32_t GetWidth()
    {
      return width;
    }

    inline uint32_t GetHeigh()
    {
      return height;
    }

  private:
    void SetPixel(uint32_t x, uint32_t y);
    void DrawLine(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);
    void DrawCircle(uint32_t x, uint32_t y, uint32_t radius);
    void DrawCircleAxes(uint32_t x, uint32_t y, uint32_t radius);
    void FloodFill8(const uint32_t& x, const uint32_t& y, const uint32_t& newColor, const uint32_t& oldColor);
    Vector2<int32_t> FindIntersection();

    uint32_t* data;
    uint32_t  width;
    uint32_t  height;
};

#endif