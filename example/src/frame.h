#ifndef FRAME_H
#define FRAME_H

#include <cstdint>
#include <vector>

#include "vector2.h"
#include "circle.h"

class Frame
{
  public:
    enum DrawType
    {
      DRAW_A,
      DRAW_B
    };

    enum FillType
    {
      FILL_A,
      FILL_B
    };

    struct Attributes
    {
      bool axes{false};
      bool grid{false};
      bool drawcicles{false};
      uint32_t gridstep{25};
      uint32_t background{0xFFFFFFFF};
      FillType filltype{FILL_A};
      DrawType drawtype{DRAW_A};
    };

    Frame(uint32_t width,
          uint32_t height);

    Frame(uint32_t width,
          uint32_t height,
          Attributes attr);

    ~Frame();

    void Draw(const std::vector<Circle<uint32_t>>& circles, uint32_t fillcolor = 0xFFFFFFFF);

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
    void SetAxes();
    void SetGrid();
    void SetPixel(uint32_t x, uint32_t y);

    void DrawLine(Vector2<int32_t> begin, Vector2<int32_t> end);
    void DrawCircleBresenham(const Circle<uint32_t>& circle);
    void DrawCirclePolar(const Circle<uint32_t>& circle);

    void FloodFill8Recursive(Vector2<uint32_t> startpoint, uint32_t fillcolor, uint32_t stopcolor);
    void FloodFill8Stack(Vector2<uint32_t> startpoint, uint32_t fillcolor, uint32_t stopcolor);

    void DrawIntersectionBresenham(const std::vector<Circle<uint32_t>>& circles);
    void DrawIntersectionPolar(const std::vector<Circle<uint32_t>>& circles);

    void IsInsideIntersection(const Vector2<uint32_t>& pixel, const std::vector<Circle<uint32_t>>& circles);
    
    std::vector<Vector2<uint32_t>> FindIntersection(const std::vector<Circle<uint32_t>>& circles);

    uint32_t* data;
    uint32_t  width;
    uint32_t  height;

    Attributes attr;
};

#endif