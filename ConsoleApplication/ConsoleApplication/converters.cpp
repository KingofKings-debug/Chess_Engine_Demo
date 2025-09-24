#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <SFML/Graphics.hpp>
#include <stack>

using namespace std;
//returns a number[0,63] from x and y codinates[0,7];
int getnumber(int x, int y) {
	return ((y * 8) + x);
}

//returns a number[0,63] from x and y codintes in window pixels[0,800]
int get_number_from_localpos(int x, int y) {
	return (((y - 40) / 100) * 8 + ((x - 10) / 100));
}
sf::Vector2i getcord(int n) {
	int y = n / 8;
	int x = n % 8;
	sf::Vector2i cord(x, y);
	return cord;
}

