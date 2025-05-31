#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <unordered_map>
#include <Windows.h>
using namespace std;

sf::Vector2u actualres = { 800,800 };
unsigned int fps = 60;
sf::Vector2f spos = { 0,0 };
sf::RectangleShape board[64];


int lastloc = -1;
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
unordered_map<int, string> mypmap;


int peice_baord[64]; //holds information of which peice using numbers


//run on start to set map of images
void setup_piece_map() {

	int back_row[8] = { 9,11,13,15,16,14,12,10 };
	vector<string> b_row({ "r.png","n.png","b.png","k.png","q.png","b.png","n.png","r.png" });
	for (int i = 8;i < 16;i++) {
		peice_baord[i] = i - 7;
		peice_baord[i + 40] = (-1) * (i - 7);
		mypmap[i - 7] = "white\\p.png";
		mypmap[(-1) * (i - 7)] = "black\\p.png";

	}
	for (int i = 0;i < 8;i++) {
		peice_baord[i] = back_row[i];
		mypmap[back_row[i]] = "white\\" + b_row[i];
	}
	for (int i = 56;i < 64;i++) {
		peice_baord[i] = (-1) * back_row[7 - (i - 56)];
		mypmap[(-1) * back_row[7 - (i - 56)]] = "black\\" + b_row[7 - (i - 56)];
	}
	for (int i = 24;i < 48;i++) {
		peice_baord[i] = 0;
	}
}



//converts the num[0,67] into {x,y} from [0,7] range;
sf::Vector2u getcord(int n) {
	int y = n / 8;
	int x = n % 8;
	sf::Vector2u cord(x, y);
	return cord;
}

int getnumber(int x, int y) {
	return ((y * 8) + x);
}

int get_number_from_localpos(int x, int y) {
	return (((y - 40) / 100) * 8 + ((x - 10) / 100));
}

void setup_board() {
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
			board[getnumber(j, i)] = rec;
		}
		waswhite = !waswhite;
		spos.x = 0;
		spos.y += 100;

	}
}

void set_texture(sf::RenderTexture& texture) {
	for (int i = 0;i < 64;i++) {
		texture.draw(board[i]);
	}
}
vector<int> getmoves(int t,vector<int> &ans) {
	int peice = peice_baord[t];
	if (peice <= 8 && peice >= 1) {
		if (peice_baord[t + 7] < 0) {
			ans.push_back(t + 7);
		}
		if (peice_baord[t + 9] < 0) {
			ans.push_back(t + 7);
		}
		if (peice_baord[t + 8] == 0) {
			ans.push_back(t + 8);
		}
	}

	if (peice <= -1 && peice >= -8) {
		if (peice_baord[t - 7] > 0) {
			ans.push_back(t - 7);
		}
		if (peice_baord[t - 9] > 0) {
			ans.push_back(t - 7);
		}
		if (peice_baord[t - 8] == 0) {
			ans.push_back(t - 8);
		}
	}
	if (abs(peice) == abs(12) || abs(peice) == abs(11)) {
		sf::Vector2u a = getcord(t);
		vector<pair<int,int>> s;
		s.push_back({ a.x+2,a.y+1 });
		s.push_back({ a.x-2,a.y+1 });
		s.push_back({ a.x+1,a.y+2 });
		s.push_back({ a.x-1,a.y+2 });
		s.push_back({ a.x+1,a.y-2 });
		s.push_back({ a.x-1,a.y-2 });
		s.push_back({ a.x+2,a.y-1 });
		s.push_back({ a.x-2,a.y-1 });
		for (int i = 0;i < s.size();i++) {
			if (s[i].first < 0 || s[i].first > 7 || s[i].second < 0 || s[i].second > 7) {
				continue;
			}
			else {
				if (peice_baord[getnumber(s[i].first, s[i].second)] * peice <= 0) {
					ans.push_back(getnumber(s[i].first, s[i].second));
				}
			}
		}	
	}
	if (abs(peice) == abs(13) || abs(peice) == abs(14)) {
		sf::Vector2u a = getcord(t);
		vector<pair<int, int>> s;
		sf::Vector2u b(a);

		if(b.x!=7 && b.y!=7){
			++b.x;++b.y;
			while (peice_baord[getnumber(b.x, b.y)] == 0) {
				s.push_back({ b.x++,b.y++ });
				if ((b.x > 7 || b.y > 7 )) {
					break;
				}
				if ((peice_baord[getnumber(b.x, b.y)] * peice) < 0) {
					s.push_back({ b.x,b.y });
					break;
				}
			}
		}
		
		b = a;
		if(b.x!=0 && b.y!=7)
		{
			b.x--; b.y++;
			while (peice_baord[getnumber(b.x, b.y)] == 0) {
				s.push_back({ b.x--,b.y++ });
				if ((b.x < 0 || b.y > 7 )) {
					break;
				}
				if ((peice_baord[getnumber(b.x, b.y)] * peice) < 0) {
					s.push_back({ b.x,b.y });
					break;
				}
			}
		}
		b = a;
		if (b.x != 7 && b.y != 0)
		{
			b.x++; b.y--;
			while (peice_baord[getnumber(b.x, b.y)] == 0) {
				s.push_back({ b.x++,b.y-- });
				if ((b.x > 7 || b.y < 0)) {
					break;
				}
				if ((peice_baord[getnumber(b.x, b.y)] * peice) < 0) {
					s.push_back({ b.x,b.y });
					break;
				}
			}
		}
		b = a;
		if (b.x != 0 && b.y != 0)
		{
			b.x--; b.y--;
			while (peice_baord[getnumber(b.x, b.y)] == 0) {
				s.push_back({ b.x--,b.y-- });
				if ((b.x < 0 || b.y < 0)) {
					break;
				}
				if ((peice_baord[getnumber(b.x, b.y)] * peice) < 0) {
					s.push_back({ b.x,b.y });
					break;
				}
			}
		}
		for (int i = 0;i < s.size();i++) {
			ans.push_back(getnumber(s[i].first, s[i].second));;
		}
	}
	for (auto i : ans) {
		cout << i << " ";
	}
	return ans;
}


void setup_mid(sf::RenderTexture& midtex,int l,vector<int> &pmoves,bool reset){
	if (reset) {
		midtex.clear();
		return;
	}
	sf::RectangleShape a;
	cout << (l % 8) * 100<<" "<< (l / 8) * 100<< " " <<l<< endl;
	
	a.setSize({ 100,100 });
	sf::Vector2f i( (l % 8) * 100, (l / 8) * 100);
	a.setPosition(i);
	a.setFillColor(sf::Color::Green);
	midtex.draw(a);
	for (auto u : pmoves){
		sf::RectangleShape b;
		b.setSize({ 100,100 });
		sf::Vector2f c((u % 8) * 100,(u / 8) * 100);
		b.setPosition(c);
		b.setFillColor(sf::Color::Yellow);
		midtex.draw(b);
	}
}
void on_left_press(sf::RenderWindow& window, sf::RenderTexture& texture,vector<int> &p_moves,sf::RenderTexture &midlletex,bool &to_draw) {
	sf::Vector2i local_pos = sf::Mouse::getPosition() - window.getPosition();
	if (local_pos.x < 0 || local_pos.y < 0 || local_pos.x > 800 || local_pos.y > 800) {
		return;
	}
	if (is_selected) {
		 int newloc= get_number_from_localpos(local_pos.x, local_pos.y);
		 if (newloc == lastloc) {
			 cout << "i am also here;";
			 lastloc = -1;
			 is_selected = false;
			 
			 midlletex.clear(sf::Color::Transparent);
			 p_moves.clear();
			 
		 }
		 else if (count(p_moves.begin(), p_moves.end(), newloc) > 0) {
			 
			 peice_baord[newloc] = peice_baord[lastloc];
			 peice_baord[lastloc] = 0;
			 is_selected = false;
			 lastloc = -1;
			 midlletex.clear(sf::Color::Transparent);
			 p_moves.clear();
		 }
	}
	else {
		int selectedpos = get_number_from_localpos(local_pos.x, local_pos.y);
		if (peice_baord[selectedpos] != 0) {
			lastloc = selectedpos;
			lastcolr = board[lastloc].getFillColor();
			is_selected = true;
			to_draw = true;
		//	cout << get_number_from_localpos(local_pos.x, local_pos.y) << endl;
		//	board[get_number_from_localpos(local_pos.x, local_pos.y)].setFillColor(sf::Color::Green);
		//	set_texture(texture);
			getmoves(selectedpos,p_moves);
			setup_mid(midlletex, selectedpos, p_moves, false);
			
		}
	}
}

sf::Texture Piece_tex(sf::RenderTexture &peice_borad_tex) {
	//sf::RenderTexture peice_borad_tex(actualres);
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
	//	sf::Angle a = sf::degrees(180);
		//s.setRotation(a);
		sf::Vector2u loc = getcord(i);
		sf::Vector2f l;

		l.x = ((float)(loc.x) * 100);
		l.y = ((float)(loc.y) * 100);
		s.setPosition(l);
		peice_borad_tex.draw(s);
	}
	sf::Texture te = peice_borad_tex.getTexture();
	peice_borad_tex.display();
	return te;
}
int main() {
	sf::RenderWindow window(sf::VideoMode(actualres), "Chess Sim");
	window.setFramerateLimit(fps);
	sf::RenderTexture texture(actualres);
	texture.clear();
	sf::RenderTexture peice_borad_tex(actualres);
	setup_board();
	set_texture(texture);
	setup_piece_map();

	sf::RenderTexture middletex(actualres);//middle texture for tile colors of selected and posible moves
	peice_baord[28] = 11;
	
	vector<int> p_moves;
	
	
	bool to_draw = false;
	while (window.isOpen()) {
		while (const optional event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>())
				window.close();
		}
		
		set_texture(texture);
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
			
			on_left_press(window, texture,p_moves,middletex,to_draw);
			while (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
				Sleep(30);
			}
			peice_borad_tex.clear(sf::Color::Transparent);
			

		}

		texture.display();
		middletex.display();
		
		sf::Texture tex = texture.getTexture();
		sf::Texture t = middletex.getTexture();
		sf::Texture ptex = Piece_tex(peice_borad_tex);
		sf::Sprite sp(tex);
		sf::Sprite spms(t);
		sf::Sprite spt(ptex);
		window.clear();
		window.draw(sp);
		if (to_draw) {
			window.draw(spms);
		}
		
		
		window.draw(spt);
		window.display();

	}
	return 0;
}