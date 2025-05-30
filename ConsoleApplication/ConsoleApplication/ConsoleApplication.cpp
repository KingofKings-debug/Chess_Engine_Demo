#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <unordered_map>
#include <Windows.h>
using namespace std;

sf::Vector2u actualres = {800,800};
unsigned int fps = 60;
sf::Vector2f spos = { 0,0 };
sf::RectangleShape board[64];


sf::Vector2i lastloc = { -1,-1 };
sf::Color lastcolr;
bool is_selected = false;

/*	peices are refered to as numbers
*	white player has numbers > 0
*	black player has numbers < 0 
*	Peice table-:
*	Pawn 1,2,3,4,5,6,7,8
*	Rook 9,10
*	Knight 11,12
*	Bishop 13,14
*	Queen 15
*	king 16
*/
unordered_map<int,string> mypmap;


int peice_baord[64]; //holds information of which peice using numbers


//run on start to set map of images
void setup_piece_map(){

	int back_row[8] = { 9,11,13,15,16,14,12,10 };
	vector<string> b_row({ "r.png","n.png","b.png","k.png","q.png","b.png","n.png","r.png" });
	for (int i = 8;i < 16;i++) {
		peice_baord[i] = i-7;
		peice_baord[i + 40] = (-1) * (i - 7);
		mypmap[i-7] = "white\\p.png";
		mypmap[(-1) * (i - 7)] = "black\\p.png";

	}
	for (int i = 0;i < 8;i++) {
		peice_baord[i] = back_row[i];
		mypmap[back_row[i]]= "white\\"+b_row[i];
	}
	for (int i = 56;i < 64;i++) {
		peice_baord[i] = (-1)*back_row[7 - (i -56)];
		mypmap[(-1) * back_row[7 - (i -56)]] = "black\\"+b_row[7 - (i - 56)];
	}
	for (int i = 24;i < 48;i++) {
		peice_baord[i] = 0;
	}
}



//converts the num[0,67] into {x,y} from [0,7] range;
sf::Vector2u getcord(int n) {
	int y = n / 8;
	int x = n % 8;
	sf::Vector2u cord(x,y);
	return cord;
}

int getnumber(int x, int y) {
	return ((y* 8) + x);
}

int get_number_from_localpos(int x, int y) {
	return (((y - 40) / 100) * 8 + ((x - 10) / 100));
}

void setup_board(){
	bool waswhite = false;
	
	sf::Vector2f size = { 100.0f,100.0f };

	for (int i = 0;i < 8;i++) {
		for (int j = 0;j < 8;j++) {
			sf::RectangleShape rec;
			rec.setPosition(spos);
			rec.setSize(size);
			rec.setOutlineColor(sf::Color::Black);
			rec.setOutlineThickness(2.0f);
			if (waswhite) {
				rec.setFillColor(sf::Color::Blue);
			}
			else {
				rec.setFillColor(sf::Color::White);
			}
			waswhite = !waswhite;
			spos.x += 100;
			board[getnumber(j,i)] = rec;
		}
		waswhite =!waswhite;
		spos.x = 0;
		spos.y += 100;
		
	}
}

void set_texture(sf::RenderTexture& texture){
	for (int i = 0;i < 64;i++){
		texture.draw(board[i]);
	}
}

void on_left_press(sf::RenderWindow &window,sf::RenderTexture &texture){
	
	sf::Vector2i local_pos = sf::Mouse::getPosition() - window.getPosition();
	if (local_pos.x < 0 || local_pos.y < 0 || local_pos.x > 800 || local_pos.y > 800) {
		return;
	}
			cout << get_number_from_localpos(local_pos.x, local_pos.y) << endl;
			Sleep(20);
			board[get_number_from_localpos(local_pos.x, local_pos.y)].setFillColor(sf::Color::Green);
			set_texture(texture);
}

sf::Texture Piece_tex() {
	sf::RenderTexture peice_borad_tex({800,800});
	for (int i = 0;i < 64;i++) {
		if (peice_baord[i] == 0) {
			continue;
		}
		sf::Texture p;
		string path = "C:\\Users\\ad999\\source\\repos\\ConsoleShowCase\\ConsoleApp\\ConsoleApplication\\ConsoleApplication\\" + mypmap[peice_baord[i]];
		if (!p.loadFromFile(path)) {
			exit(1);
		}
		sf::Sprite s(p);
		s.setScale({ 1.5,1.5 });
		sf::Angle a = sf::degrees(180);
		s.setRotation(a);
		sf::Vector2u loc = getcord(i);
		sf::Vector2f l;

		l.x = ((float)(loc.x) * 100)+100;
		l.y = 900 - ((float)(loc.y + 1) * 100);
		s.setPosition(l);
		peice_borad_tex.draw(s);
	}
	sf::Texture te = peice_borad_tex.getTexture();
	return te;
}
int main() {
	sf::RenderWindow window(sf::VideoMode(actualres), "Chess Sim");
	window.setFramerateLimit(fps);
	sf::RenderTexture texture(actualres);
	texture.clear();

	setup_board();
	set_texture(texture);
	setup_piece_map();
	sf::Texture ptex = Piece_tex();


	while (window.isOpen()){	
		while (const optional event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>())
				window.close();
		}
		set_texture(texture);
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
			on_left_press(window, texture);
			
		}

		texture.display();
		sf::Texture tex = texture.getTexture();
		sf::Sprite sp(tex);
		sf::Sprite spt(ptex);
		window.clear();
		window.draw(sp);
		window.draw(spt);
		
		
		
		window.display();
		
	}
	return 0;
}