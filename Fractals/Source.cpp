#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <Windows.h>
#include <complex>
#include <chrono>
#include <thread>

#define width 256
#define height 256

//Array of pixels
auto pixels = new sf::RectangleShape[width][height];

std::vector<sf::Color> cols = { sf::Color::Color(255,0,0), sf::Color::Color(126,126,0), sf::Color::Color(0,255,0), sf::Color::Color(0,126,126), sf::Color::Color(0,0,255), sf::Color::Color(126,0,126) };
//std::vector<sf::Color> cols = { };

//Scale and Coords
long double zoom = 2;
long double xcoord = -0.7611251856176778352;
long double ycoord = -0.084749953259429494645;

//How many innterations to do (max)
long double itterations = 100;

//Amount of threads to use
int threads = 1;

//Renders a single pixel
void rendPixel(int x, int y) {

	//Create point and z
	std::complex<long double> point(((long double)x - width/2) / width * zoom + xcoord, ((long double)y - height / 2) / height * zoom - ycoord);
	std::complex<long double> z(0, 0);

	//Make pixel
	sf::RectangleShape rect;
	rect.setSize({ 1, 1 });
	rect.setPosition(x, y);

	long double count = 0;

	//Do the fractal stuff
	while (std::abs(z) < 20 && count < (itterations)) {
		z = z * z + point /** std::complex<long double>(0, count / 10)*/;
		count += 1;
	}

	//Colour pixel
	rect.setFillColor(cols[(int)count % cols.size()]);
	rect.setOutlineColor(sf::Color::Black);
	rect.setOutlineThickness(0.1f);

	//Save pixel to array
	pixels[x][y] = rect;
}

//Renders area, by rendering pixels
void rendSquare(int x1, int x2, int y1, int y2) {
	for (int y = y1; y < y2; y++) {
		for (int x = x1; x < x2; x++) {
			rendPixel(x, y);
		}
	}
}

int main() {

	//decimal precision for cout
	std::cout.precision(20);

	std::cout << "Booting" << "\n";

	std::system("CLS");

	//Check how many threads and ask user
	unsigned int n = std::thread::hardware_concurrency();
	std::cout <<"Concurrent threads supported by your machine: " << n << "\n";

	std::cout << "Enter Desired Threads: ";

	std::cin >> threads;

	//I bad answer, set to 1
	if ((int)threads <= 0) {
		threads = 1;
	}

	std::system("CLS");

	//Make Window
	sf::RenderWindow window(sf::VideoMode(width, height), "Fractals By Keane Carotenuto");

	std::vector<std::thread*> ts;
	ts.clear();

	time_t time = std::time(0);

	bool update = true;
	bool autoDepth = false;

	//Make cool gradients for colours
	/*int r = 125;
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
	}*/

	//Main Loop
	while (window.isOpen() == true)
	{
		//if needs to be updated, update
		if (update == true) {

			//Log start time
			auto start = std::chrono::high_resolution_clock::now();

			//Split task into threads
			for (int y = 0; y < threads; y++) {
					std::thread* t_thread = new std::thread(rendSquare, 0, width, y * height / threads, (y + 1) * height / threads);

					//std::cout << "X1: " << x * width / hSplit << ",   X2: " << (x + 1) * width / hSplit << ",   Y1: " << y * height / vSplit << ",   Y2: " << (y + 1) * height / vSplit << std::endl;

					ts.push_back(t_thread);
			}

			//Wait for all threads to be done
			for (std::thread* _t : ts) {
				_t->join();
			}

			//Clear threads
			for (std::thread* _t : ts) {
				delete _t;
			}
			ts.clear();

			//Clear Screen
			window.clear();

			//Draw pixels
			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					window.draw(pixels[x][y]);
				}
			}

			//Log Finish time
			auto finish = std::chrono::high_resolution_clock::now();

			//Display
			window.display();
			update = false;

			//Calc time taken
			std::chrono::duration<double> elapsed = finish - start;

			system("CLS");

			//Make sure cursor is at top
			COORD pos = { 0, 0 };
			HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleCursorPosition(output, pos);

			//Display info
			std::cout	<< "Scale: " << zoom
						<< "\nTime: " << elapsed.count() 
						<< "\nCoords:  (" << (long double)xcoord << ", " << (long double)ycoord << ")" 
						<< "\nItterations: " << itterations 
						<< "\nThreads Used: " << threads;
		}

		//Makes colours go woooo
		bool spin = false;
		if (spin){
			cols.push_back(cols[0]);
			cols.erase(cols.begin());
			update = true;
		}

		//Makes the program continuously increase itterations
		if (autoDepth) {
			itterations++;
			update = true;
		}

		//React to user input, zoom and stuff
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