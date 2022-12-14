#include <iostream>
#include <fstream>
#include <cstdint>
#include <sys/resource.h>

#include <SFML/Graphics.hpp>

#include "frame.h"

enum WindowSize
{
	kWidth  = 1200,
	kHeight = 800
};

int main(int argc, char **argv)
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;

	Frame frame(kWidth, kHeight, 0xFFFFFFFF, {true, true, 25});

	
	// frame.DrawFrame({
	// 	{kWidth / 2 - 20, kHeight / 2 + 30, 140},
	// 	{kWidth / 2 - 150, kHeight / 2 - 100, 120},
	// 	{kWidth / 2 + 20, kHeight / 2 - 120, 170}
	// });

	frame.DrawFrame({
		{kWidth / 2 - 100, kHeight / 2, 150},
		{kWidth / 2 + 100, kHeight / 2, 150},
		{kWidth / 2, kHeight / 2, 90}
	});

	// frame.DrawIntersectionBresenham({
	// 	{kWidth / 2 - 20, kHeight / 2 + 30, 140},
	// 	{kWidth / 2 - 150, kHeight / 2 - 100, 120},
	// 	{kWidth / 2 + 20, kHeight / 2 - 120, 170}
	// });

	// frame.DrawIntersectionSinCos({
	// 	{kWidth / 2 - 100, kHeight / 2, 150},
	// 	{kWidth / 2 - 100, kHeight / 2, 150},
	// 	{kWidth / 2, kHeight / 2, 100}
	// });
	// frame.MakeGrid();
	frame.MakeAxes();

	sf::Image image;

	image.create(frame.GetWidth(), frame.GetHeigh(), (uint8_t*)frame.GetData());

	sf::Texture texture;

	texture.loadFromImage(image);

	sf::Sprite sprite(texture);

	sf::RenderWindow window(sf::VideoMode(kWidth, kHeight), "Lab3", sf::Style::Default, settings);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		window.clear();
		window.draw(sprite);

		window.display();
	}

	return 0;
}
