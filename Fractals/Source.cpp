#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <Windows.h>
#include <complex>
#include <chrono>
#include <thread>

#define width 256
#define height 256

auto pixels = new sf::RectangleShape[width][height];

//std::vector<sf::Color> cols = { sf::Color::Color(255,0,0), sf::Color::Color(126,126,0), sf::Color::Color(0,255,0), sf::Color::Color(0,126,126), sf::Color::Color(0,0,255), sf::Color::Color(126,0,126) };
std::vector<sf::Color> cols = { };


long double zoom = 2;
long double xcoord = -0.7611251856176778352;
long double ycoord = -0.084749953259429494645;

long double itterations = 100;

//double xcoord = -2;
//double ycoord = -1;

int threads = 1;

void rendPixel(int x, int y) {

	std::complex<long double> point(((long double)x - width/2) / width * zoom + xcoord, ((long double)y - height / 2) / height * zoom - ycoord);
	std::complex<long double> z(0, 0);

	sf::RectangleShape rect;
	rect.setSize({ 1, 1 });
	rect.setPosition(x, y);

	long double count = 0;

	while (std::abs(z) < 20 && count < (itterations)) {
		z = z * z + point /** std::complex<double>(0, count / 10)*/;
		count += 1;
	}

	rect.setFillColor(cols[(int)count % cols.size()]);
	rect.setOutlineColor(sf::Color::Black);
	rect.setOutlineThickness(0.1f);

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

	std::cout << "Booting" << "\n";

	std::system("CLS");

	unsigned int n = std::thread::hardware_concurrency();
	std::cout <<"Concurrent threads supported by your machine: " << n << "\n";

	std::cout << "Enter Desired Threads: ";

	std::cin >> threads;

	if ((int)threads <= 0) {
		threads = 1;
	}

	std::system("CLS");

	//Make Window
	sf::RenderWindow window(sf::VideoMode(width, height), "Fractals By Keane Carotenuto");

	////Setting coordinates of mid screen, and the amount to render outwards from that point
	//float xcoord = 0.0f;
	//float ycoord = 0.0f;
	//float scale = 1.5f;

	////from -2 to 2 is 4, so we divide that by the view port size, to get how much we need to increment by for each pixel
	//for (float y = ycoord - scale; y < ycoord + scale; y += ((2 * scale) / height)) {
	//	for (float x = xcoord - scale; x < xcoord + scale; x += ((2 * scale) / width)) {
	//		//Make the point
	//		std::complex<float> point(x, y);
	//		//Set z to 0 at first
	//		std::complex<float> z(0, 0);

	//		//Make the "pixel"
	//		sf::RectangleShape rect;
	//		rect.setSize({ 1, 1 });
	//		//Set it to the right position on screen (move into positives, and scale position by screen size)
	//		rect.setPosition((x - (xcoord - scale)) * (width / (2 * scale)), (y - (ycoord - scale)) * (height / (2 * scale)));

	//		//Init count
	//		long double count = 0;

	//		//Repeat until gets out of bounds, or reaches over 100 tries
	//		while (std::abs(z) < 20 && count < (100)) {
	//			z = z * z + point /** std::complex<double>(0, count / 10)*/;
	//			count += 1;
	//		}

	//		//Set the colour to red, with brightness of how many tries
	//		rect.setFillColor(sf::Color::Color(count, 0, 0));

	//		//Draw pixel
	//		window.draw(rect);
	//	}
	//}
	//
	////Display pixel
	//window.display();

	////Wait for user before quit
	//int wait;
	//std::cin >> wait;

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

			auto start = std::chrono::high_resolution_clock::now();

			

			for (int y = 0; y < threads; y++) {
					std::thread* t_thread = new std::thread(rendSquare, 0, width, y * height / threads, (y + 1) * height / threads);

					//std::cout << "X1: " << x * width / hSplit << ",   X2: " << (x + 1) * width / hSplit << ",   Y1: " << y * height / vSplit << ",   Y2: " << (y + 1) * height / vSplit << std::endl;

					ts.push_back(t_thread);
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

			auto finish = std::chrono::high_resolution_clock::now();

			window.display();
			update = false;

			COORD pos = { 0, 0 };
			HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleCursorPosition(output, pos);

			std::chrono::duration<double> elapsed = finish - start;

			std::cout	<<						   "Scale: " << zoom
						<< "                      \nTime: " << elapsed.count() 
						<< "                      \nCoords:  (" << (long double)xcoord << ", " << (long double)ycoord << ")                      " 
						<< "                      \nItterations: " << itterations << "                      " 
						<< "                      \nThreads Used: " << threads << "                      ";
		}

		bool spin = false;

		if (spin){
			cols.push_back(cols[0]);
			cols.erase(cols.begin());
			update = true;
		}

		if (autoDepth) {
			itterations++;
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

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
				itterations = 1000;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::M)) {
				itterations += 10;
				update = true;

				if (itterations < 10) itterations = 10;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
				itterations -= 10;
				update = true;

				if (itterations < 10) itterations = 10;
			}
		}
	}

	return 0;
}