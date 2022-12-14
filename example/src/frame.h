#ifndef FRAME_H
#define FRAME_H

#include <cstdint>
#include <vector>

#include "vector2.h"
#include "circle.h"

class Frame
{
  public:
    struct Attributes
    {
      bool axes;
      bool grid;
      uint32_t gridstep;
      
    };

    Frame(uint32_t width,
          uint32_t height,
          uint32_t background = 0xFFFFFFFF,
          Attributes attr = {false, false, 0});

    Frame(uint32_t width,
          uint32_t height,
          const std::vector<Circle<uint32_t>>& circles,
          uint32_t background = 0xFFFFFFFF,
          Attributes attr = {false, false, 0});

    ~Frame();

    void MakeAxes();
    void MakeGrid();
    void DrawFrame(const std::vector<Circle<uint32_t>>& circles);
    void FillShape(uint32_t color = 0xFFFFFFFF);
    void DrawIntersectionBresenham(const std::vector<Circle<uint32_t>>& circles);
    void DrawIntersectionSinCos(const std::vector<Circle<uint32_t>>& circles);

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
    void DrawCircle(const Circle<uint32_t>& circle);
    void DrawCircleAxes(const Circle<uint32_t>& circle);
    void FloodFill8(Vector2<uint32_t> startpoint, uint32_t fillcolor, uint32_t stopcolor);
    void FloodFill8Stack(Vector2<uint32_t> startpoint, uint32_t fillcolor, uint32_t stopcolor);
    void CheckAndSetPixel(const Vector2<uint32_t>&, const std::vector<Circle<uint32_t>>& circles );
    
    Vector2<uint32_t>* FindIntersection(const std::vector<Circle<uint32_t>>& circles);

    uint32_t* data;
    uint32_t  width;
    uint32_t  height;

    Attributes attr;
};

#endif