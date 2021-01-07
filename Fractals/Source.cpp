#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <Windows.h>
#include <complex>
#include <thread> 

#define width 512
#define height 512

auto pixels = new sf::RectangleShape[width][height];

//std::vector<sf::Color> cols = { sf::Color::Color(255,0,0), sf::Color::Color(126,126,0), sf::Color::Color(0,255,0), sf::Color::Color(0,126,126), sf::Color::Color(0,0,255), sf::Color::Color(126,0,126) };
std::vector<sf::Color> cols = { };


long double zoom = 2;
long double xcoord = -0.7611251856176778352;
long double ycoord = -0.084749953259429494645;

long double depth = 50;

time_t sinceLast = 0;

//double xcoord = -2;
//double ycoord = -1;

void rendPixel(int x, int y) {

	std::complex<long double> point(((long double)x - width/2) / width * zoom + xcoord, ((long double)y - height / 2) / height * zoom - ycoord);
	std::complex<long double> z(0, 0);

	sf::RectangleShape rect;
	rect.setSize({ 1, 1 });
	rect.setPosition(x, y);

	long double count = 0;

	while (std::abs(z) < 20 && count < (depth)) {
		z = z * z + point /** std::complex<double>(0, count / 10)*/;
		count += 1;
	}


	rect.setFillColor(cols[(int)count % cols.size()]);

	pixels[x][y] = rect;
}

void rendSquare(int x1, int x2, int y1, int y2) {
	for (int y = y1; y < y2; y++) {
		for (int x = x1; x < x2; x++) {
			rendPixel(x, y);
		}
	}
}

int main() {

	std::cout.precision(20);

	std::cout << "Booting" << std::endl;

	sf::RenderWindow window(sf::VideoMode(width, height), "Fractals By Keane Carotenuto");

	std::vector<std::thread*> ts;
	ts.clear();

	time_t time = std::time(0);

	bool update = true;
	bool autoDepth = false;

	int r = 125;
	int g = 125;
	int b = 125;

	while (b < 255) {
		cols.push_back(sf::Color::Color(r, g, b));

		if (r < 255) {
			r++;
		}
		else if (g < 255) {
			g++;
		}
		else if (b < 255) {
			b++;
		}
	}

	while (b >125) {
		cols.push_back(sf::Color::Color(r, g, b));

		if (r > 125) {
			r--;
		}
		else if (g > 125) {
			g--;
		}
		else if (b > 125) {
			b--;
		}
	}

	while (window.isOpen() == true)
	{

		if (update == true) {

			int vSplit = 2;
			int hSplit = 2;

			for (int y = 0; y < vSplit; y++) {
				for (int x = 0; x < hSplit; x++) {
					std::thread* t_thread = new std::thread(rendSquare, x * width / hSplit, (x + 1) * width / hSplit, y * height / vSplit, (y + 1) * height / vSplit);

					//std::cout << "X1: " << x * width / hSplit << ",   X2: " << (x + 1) * width / hSplit << ",   Y1: " << y * height / vSplit << ",   Y2: " << (y + 1) * height / vSplit << std::endl;

					ts.push_back(t_thread);
				}
			}

			for (std::thread* _t : ts) {
				_t->join();
			}

			for (std::thread* _t : ts) {
				delete _t;
			}

			ts.clear();



			window.clear();

			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					window.draw(pixels[x][y]);
				}
			}

			window.display();
			update = false;

			COORD pos = { 0, 0 };
			HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleCursorPosition(output, pos);

			sinceLast = std::time(0) - sinceLast;

			std::cout << "Scale: " << zoom << "                  \nTime: " << sinceLast << "                 \nCoords:  (" << (long double)xcoord << ", " << (long double)ycoord << ")                    " << "                 \nDepth: " << depth << "                      ";
		}

		bool spin = false;

		if (spin){
			cols.push_back(cols[0]);
			cols.erase(cols.begin());
			update = true;
		}

		if (autoDepth) {
			depth++;
			update = true;
		}

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
				xcoord += zoom / 10;
				update = true;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
				xcoord -= zoom / 10;
				update = true;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
				ycoord += zoom / 10;
				update = true;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
				ycoord -= zoom / 10;
				update = true;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
				zoom *= 0.8;
				update = true;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
				zoom /= 0.8;
				update = true;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
				autoDepth = !autoDepth;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Add)) {
				depth += 10;
				update = true;

				if (depth < 10) depth = 10;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract)) {
				depth -= 10;
				update = true;

				if (depth < 10) depth = 10;
			}
		}
	}

	return 0;
}