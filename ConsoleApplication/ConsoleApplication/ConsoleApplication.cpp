#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <unordered_map>
#include <Windows.h>
using namespace std;

void get_Knight_moves(int t, vector<int>& ans, int peice);
void get_bishop_moves(int t, vector<int>& ans, int peice);
bool king_in_cheak_bool_only(int t, int peice);


sf::Vector2u actualres = { 800,800 };
unsigned int fps = 60;
sf::Vector2f spos = { 0,0 };
sf::RectangleShape board[64]; //holds the rectanlge shapes for background board;
int white_facing_front = 0; //facing front means on lower side of sreen

int lastloc = -1; //holds the last location selected to do next move;
bool is_selected = false; //cheaks if a peice is already selected;
vector<int> danger_white; //keeps peice that are atacking white king
vector<int> danger_black;	//keeps peice that are atacking black king
vector<vector<int>> danger_net({ danger_black,{},danger_white });
int active_player = -1; //-1 for black +1 for white changes on turn
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

unordered_map<int, string> mypmap;	//holdes peice and its path to draw on texrue


int peice_board[64]; //holds information of which peice using numbers


//run on start to set map of images
void setup_piece_map() {

	int back_row[8] = { 9,11,13,15,16,14,12,10 };
	vector<string> b_row({ "r.png","n.png","b.png","q.png","k.png","b.png","n.png","r.png" });
	for (int i = 8;i < 16;i++) {
		peice_board[i] = i - 7;
		peice_board[i + 40] = (-1) * (i - 7);
		mypmap[i - 7] = "white\\p.png";
		mypmap[(-1) * (i - 7)] = "black\\p.png";

	}
	for (int i = 0;i < 8;i++) {
		peice_board[i] = back_row[i];
		mypmap[back_row[i]] = "white\\" + b_row[i];
	}
	for (int i = 56;i < 64;i++) {
		peice_board[i] = (-1) * back_row[7 - (i - 56)];
		mypmap[(-1) * back_row[7 - (i - 56)]] = "black\\" + b_row[7 - (i - 56)];
	}
	for (int i = 24;i < 48;i++) {
		peice_board[i] = 0;
	}
}


//converts the num[0,67] into {x,y} from [0,7] range;
sf::Vector2u getcord(int n) {
	int y = n / 8;
	int x = n % 8;
	sf::Vector2u cord(x, y);
	return cord;
}

//returns a number[0,63] from x and y codinates[0,7];
int getnumber(int x, int y) {
	return ((y * 8) + x);
}

//returns a number[0,63] from x and y codintes in window pixels[0,800]
int get_number_from_localpos(int x, int y) {
	return (((y - 40) / 100) * 8 + ((x - 10) / 100));
}

//setup initial board with 2 color sqares in 'board[64]'
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



//funtions to get moveset of peice;
void get_Knight_moves(int t, vector<int>& ans, int peice) {
	sf::Vector2u a = getcord(t);
	vector<pair<int, int>> s;
	s.push_back({ a.x + 2,a.y + 1 });
	s.push_back({ a.x - 2,a.y + 1 });
	s.push_back({ a.x + 1,a.y + 2 });
	s.push_back({ a.x - 1,a.y + 2 });
	s.push_back({ a.x + 1,a.y - 2 });
	s.push_back({ a.x - 1,a.y - 2 });
	s.push_back({ a.x + 2,a.y - 1 });
	s.push_back({ a.x - 2,a.y - 1 });
	for (int i = 0;i < s.size();i++) {
		if (s[i].first < 0 || s[i].first > 7 || s[i].second < 0 || s[i].second > 7) {
			continue;
		}
		else {
			if (peice_board[getnumber(s[i].first, s[i].second)] * peice <= 0) {
				ans.push_back(getnumber(s[i].first, s[i].second));
			}
		}
	}
}
void get_bishop_moves(int t, vector<int>& ans, int peice) {
	sf::Vector2u a = getcord(t);
	vector<pair<int, int>> s;
	sf::Vector2u b(a);

	if (b.x != 7 && b.y != 7) {
		while (peice_board[getnumber(b.x, b.y)] == 0 || peice_board[getnumber(b.x, b.y)] == peice) {
			if (peice_board[getnumber(b.x, b.y)] != peice) {
				s.push_back({ b.x,b.y });
			}
			b.x++;b.y++;

			if ((b.x == 7 || b.y == 7) && (peice_board[getnumber(b.x, b.y)] * peice) <= 0) {
				s.push_back({ b.x,b.y });
				break;
			}
			if ((peice_board[getnumber(b.x, b.y)] * peice) < 0) {
				s.push_back({ b.x,b.y });
				break;
			}
		}
	}

	b = a;
	if (b.x != 0 && b.y != 7)
	{

		while (peice_board[getnumber(b.x, b.y)] == 0 || peice_board[getnumber(b.x, b.y)] == peice) {
			if (peice_board[getnumber(b.x, b.y)] != peice) {
				s.push_back({ b.x,b.y });
			}
			b.x--; b.y++;
			if ((b.x == 0 || b.y == 7) && (peice_board[getnumber(b.x, b.y)] * peice) <= 0) {
				s.push_back({ b.x,b.y });
				break;
			}
			if ((peice_board[getnumber(b.x, b.y)] * peice) < 0) {
				s.push_back({ b.x,b.y });
				break;
			}
		}
	}
	b = a;
	if (b.x != 7 && b.y != 0)
	{

		while (peice_board[getnumber(b.x, b.y)] == 0 || peice_board[getnumber(b.x, b.y)] == peice) {
			if (peice_board[getnumber(b.x, b.y)] != peice) {
				s.push_back({ b.x,b.y });
			}
			b.x++; b.y--;
			if ((b.x == 7 || b.y == 0) && (peice_board[getnumber(b.x, b.y)] * peice) <= 0) {
				s.push_back({ b.x,b.y });
				break;
			}
			if ((peice_board[getnumber(b.x, b.y)] * peice) < 0) {
				s.push_back({ b.x,b.y });
				break;
			}
		}
	}
	b = a;
	if (b.x != 0 && b.y != 0)
	{

		while (peice_board[getnumber(b.x, b.y)] == 0 || peice_board[getnumber(b.x, b.y)] == peice) {
			if (peice_board[getnumber(b.x, b.y)] != peice) {
				s.push_back({ b.x,b.y });
			}
			b.x--;b.y--;
			if ((b.x == 0 || b.y == 0) && (peice_board[getnumber(b.x, b.y)] * peice) <= 0) {
				s.push_back({ b.x,b.y });
				break;
			}
			if ((peice_board[getnumber(b.x, b.y)] * peice) < 0) {
				s.push_back({ b.x,b.y });
				break;
			}
		}
	}
	for (int i = 0;i < s.size();i++) {
		ans.push_back(getnumber(s[i].first, s[i].second));
	}

}
void get_pawn_moves_white(int t, vector<int>& ans, int peice) {
	sf::Vector2u a = getcord(t);
	vector<pair<int, int>> f;
	vector<pair<int, int>> s;
	if (white_facing_front == 1){
		f.push_back({ a.x,a.y-1});
		if (a.y == 6) {
			if(peice_board[getnumber(a.x, a.y - 1)] == 0)
			{
				f.push_back({ a.x,a.y - 2 });
			}
		}
		s.push_back({ a.x + 1,a.y-1 });
		s.push_back({ a.x - 1,a.y-1 });
	}
	else {
		f.push_back({ a.x,a.y + 1 });
		if (a.y == 1) {
			if (peice_board[getnumber(a.x, a.y + 1)] == 0)
			{
				f.push_back({ a.x,a.y + 2 });
			}
		}
		s.push_back({ a.x + 1,a.y + 1 });
		s.push_back({ a.x - 1,a.y + 1 });
	}
	for (int i = 0;i < f.size();i++) {
		if (f[i].first < 0 || f[i].first > 7 || f[i].second < 0 || f[i].second > 7) {
			continue;
		}
		else {
			if (peice_board[getnumber(f[i].first, f[i].second)] == 0) {
				ans.push_back(getnumber(f[i].first, f[i].second));
			}
		}
	}
	for (int i = 0;i < s.size();i++) {
		if (s[i].first < 0 || s[i].first > 7 || s[i].second < 0 || s[i].second > 7) {
			continue;
		}
		else {
			if (peice_board[getnumber(s[i].first, s[i].second)]*peice<0) {
				ans.push_back(getnumber(s[i].first, s[i].second));
			}
		}
	}
	
}
void get_pawn_moves_black(int t, vector<int>& ans, int peice) {
	sf::Vector2u a = getcord(t);
	vector<pair<int, int>> f;
	vector<pair<int, int>> s;
	if (white_facing_front == 0) {
		f.push_back({ a.x,a.y - 1 });
		if (a.y == 6) {
			if (peice_board[getnumber(a.x, a.y - 1)] == 0)
			{
				f.push_back({ a.x,a.y - 2 });
			}
		}
		s.push_back({ a.x + 1,a.y - 1 });
		s.push_back({ a.x - 1,a.y - 1 });
	}
	else {
		f.push_back({ a.x,a.y + 1 });
		if (a.y == 1) {
			if (peice_board[getnumber(a.x, a.y + 1)] == 0)
			{
				f.push_back({ a.x,a.y+- 2 });
			}
		}
		s.push_back({ a.x + 1,a.y + 1 });
		s.push_back({ a.x - 1,a.y + 1 });
	}
	for (int i = 0;i < f.size();i++) {
		if (f[i].first < 0 || f[i].first > 7 || f[i].second < 0 || f[i].second > 7) {
			continue;
		}
		else {
			if (peice_board[getnumber(f[i].first, f[i].second)] == 0) {
				ans.push_back(getnumber(f[i].first, f[i].second));
			}
		}
	}
	for (int i = 0;i < s.size();i++) {
		if (s[i].first < 0 || s[i].first > 7 || s[i].second < 0 || s[i].second > 7) {
			continue;
		}
		else {
			if (peice_board[getnumber(s[i].first, s[i].second)] * peice < 0) {
				ans.push_back(getnumber(s[i].first, s[i].second));
			}
		}
	}
}
void get_rook_moves(int t, vector<int>& ans, int peice) {
	sf::Vector2u a = getcord(t);
	vector<pair<int, int>> s;
	sf::Vector2u b(a);

	b = a;
	if(b.y!=7)
	{
		while (peice_board[getnumber(b.x, b.y)] == 0 || peice_board[getnumber(b.x, b.y)] == peice) {
			if (peice_board[getnumber(b.x, b.y)] != peice) {
				s.push_back({ b.x,b.y });
			}
			b.y++;

			if ((b.y == 7) && (peice_board[getnumber(b.x, b.y)] * peice) <= 0) {
				s.push_back({ b.x,b.y });
				break;
			}
			if ((peice_board[getnumber(b.x, b.y)] * peice) < 0) {
				s.push_back({ b.x,b.y });
				break;
			}
		}
	}
	b = a;
	if(b.y!=0)
	{
		while (peice_board[getnumber(b.x, b.y)] == 0 || peice_board[getnumber(b.x, b.y)] == peice) {
			if (peice_board[getnumber(b.x, b.y)] != peice) {
				s.push_back({ b.x,b.y });
			}
			b.y--;

			if ((b.y == 0) && (peice_board[getnumber(b.x, b.y)] * peice) <= 0) {
				s.push_back({ b.x,b.y });
				break;
			}
			if ((peice_board[getnumber(b.x, b.y)] * peice) < 0) {
				s.push_back({ b.x,b.y });
				break;
			}
		}
	}
	b = a;
	if(b.x!=7)
	{
		while (peice_board[getnumber(b.x, b.y)] == 0 || peice_board[getnumber(b.x, b.y)] == peice) {
			if (peice_board[getnumber(b.x, b.y)] != peice) {
				s.push_back({ b.x,b.y });
			}
			b.x++;

			if ((b.x == 7) && (peice_board[getnumber(b.x, b.y)] * peice) <= 0) {
				s.push_back({ b.x,b.y });
				break;
			}
			if ((peice_board[getnumber(b.x, b.y)] * peice) < 0) {
				s.push_back({ b.x,b.y });
				break;
			}
		}
	}
	b = a;
	if(b.x!=0)
	{
		while (peice_board[getnumber(b.x, b.y)] == 0 || peice_board[getnumber(b.x, b.y)] == peice) {
			if (peice_board[getnumber(b.x, b.y)] != peice) {
				s.push_back({ b.x,b.y });
			}
			b.x--;

			if ((b.x == 0) && (peice_board[getnumber(b.x, b.y)] * peice) <= 0) {
				s.push_back({ b.x,b.y });
				break;
			}
			if ((peice_board[getnumber(b.x, b.y)] * peice) < 0) {
				s.push_back({ b.x,b.y });
				break;
			}
		}
	}
	for (int i = 0;i < s.size();i++) {
		ans.push_back(getnumber(s[i].first, s[i].second));
	}


}
void get_king_moves(int t,vector<int>& ans,int peice) {
	vector < pair<int, int>> s;
	sf::Vector2u pos = getcord(t);
	s.push_back({ pos.x+1,pos.y+1 });
	s.push_back({ pos.x+1,pos.y });
	s.push_back({ pos.x+1,pos.y-1 });
	s.push_back({ pos.x,pos.y+1 });
	s.push_back({ pos.x,pos.y-1 });
	s.push_back({ pos.x-1,pos.y+1 });
	s.push_back({ pos.x-1,pos.y });
	s.push_back({ pos.x-1,pos.y-1 });
	for (int i = 0;i < s.size();i++) {
		if (s[i].first < 0 || s[i].first > 7 || s[i].second < 0 || s[i].second > 7) {
			continue;
		}
		else {
			if (peice_board[getnumber(s[i].first, s[i].second)] * peice <= 0) {
				if (!king_in_cheak_bool_only(getnumber(s[i].first, s[i].second),16)) {
					ans.push_back(getnumber(s[i].first, s[i].second));
				}
			}
		}
	}


}
void get_queen_moves(int t, vector<int>& ans, int peice) {
	get_rook_moves(t, ans, peice);
	get_bishop_moves(t, ans, peice);
}


bool king_in_cheak_bool_only(int t, int peice) {
	vector<int> knights;
	vector<int> bishop;
	vector<int> rook;
	vector<int> pawn;

	get_Knight_moves(t, knights, peice_board[t]);
	get_bishop_moves(t, bishop, peice_board[t]);
	get_rook_moves(t, rook, peice_board[t]);
	//vector<int> danger;
	for (auto i : knights) {
		if (peice_board[i] == (-1)*active_player*abs(11) || peice_board[i] == (-1) * active_player * abs(12)) {
			return true;
		}
	}
	for (int i : bishop) {
		if ((peice_board[i]) == (-1) * active_player * abs(13) || (peice_board[i]) == (-1) * active_player * abs(14) || (peice_board[i]) == (-1) * active_player * abs(15)){
			return true;
		}
	}
	for (int i : rook) {
		if ((peice_board[i]) == (-1) * active_player * abs(9) || (peice_board[i]) == (-1) * active_player * abs(10) || (peice_board[i]) == (-1) * active_player * abs(15)){
			return true;
		}
	}
	return false;
}

bool king_in_cheak(int t, int peice, vector<int>& danger) {
	vector<int> knights;
	vector<int> bishop;
	vector<int> rook;
	vector<int> pawn;

	get_Knight_moves(t, knights, peice);
	get_bishop_moves(t, bishop, peice);
	get_rook_moves(t, rook, peice);
	//vector<int> danger;
	for (auto i : knights) {
		if (abs(peice_board[i]) == abs(11) || abs(peice_board[i]) == abs(12)) {
			danger.push_back(i);
		}
	}
	for (int i : bishop) {
		if (abs(peice_board[i]) == abs(13) || abs(peice_board[i]) == abs(14) || abs(peice_board[i]) == 15) {
			danger.push_back(i);
		}
	}
	for (int i : rook) {
		if (abs(peice_board[i]) == abs(9) || abs(peice_board[i]) == abs(10) || abs(peice_board[i]) == 15) {
			danger.push_back(i);
		}
	}
	if (danger.size() == 0) {
		return true;
	}
	return false;

}



//draws board texture
void set_texture(sf::RenderTexture& texture) {
	for (int i = 0;i < 64;i++) {
		texture.draw(board[i]);
	}
}

//seperates and return the moves acording to the peice
void getmoves(int t, vector<int>& ans) {
	int peice = peice_board[t];
	if (peice <= 8 && peice >= 1) {
		get_pawn_moves_white(t, ans, peice);
	}

	if (peice <= -1 && peice >= -8) {
		get_pawn_moves_black(t, ans, peice);
	}
	if (abs(peice) == abs(12) || abs(peice) == abs(11)) {
		get_Knight_moves(t, ans, peice);
	}
	if (abs(peice) == abs(13) || abs(peice) == abs(14)) {
		get_bishop_moves(t, ans, peice);
	}
	if (abs(peice) == abs(10) || abs(peice) == abs(9)) {
		get_rook_moves(t, ans, peice);
	}
	if (abs(peice) == abs(15)) {
		get_queen_moves(t, ans, peice);
	}
	if (abs(peice) == abs(16)) {
		get_king_moves(t, ans, peice);
	}


	for (auto i : ans) {
		cout << i << " ";
	}
	cout << "Peice:" << peice<<";";

}

//sets up and update middile texture for indicating possible moves and selected peice
void setup_mid(sf::RenderTexture& midtex, int l, vector<int>& pmoves, bool reset) {
	if (reset) {
		midtex.clear();
		return;
	}
	sf::RectangleShape a;
	cout << (l % 8) * 100 << " " << (l / 8) * 100 << " " << l << endl;

	a.setSize({ 100,100 });
	sf::Vector2f i((l % 8) * 100, (l / 8) * 100);
	a.setPosition(i);
	a.setOutlineColor(sf::Color::Black);
	a.setOutlineThickness(3.0);
	a.setFillColor(sf::Color::Green);
	midtex.draw(a);
	for (auto u : pmoves) {
		sf::RectangleShape b;
		b.setSize({ 100,100 });
		sf::Vector2f c((u % 8) * 100, (u / 8) * 100);
		b.setPosition(c);
		b.setFillColor(sf::Color::Yellow);
		b.setOutlineColor(sf::Color::Black);
		b.setOutlineThickness(1.0);
		midtex.draw(b);
	}
}

//is called on selection of sqare on board
void on_left_press(sf::RenderWindow& window, sf::RenderTexture& texture, vector<int>& p_moves, sf::RenderTexture& midlletex, bool& to_draw) {
	sf::Vector2i local_pos = sf::Mouse::getPosition() - window.getPosition();
	if (local_pos.x < 0 || local_pos.y < 0 || local_pos.x > 800 || local_pos.y > 800) {
		return;
	}
	if (is_selected) {
		int newloc = get_number_from_localpos(local_pos.x, local_pos.y);
		if (newloc == lastloc) {
			//if player does not want to play this peice then this funtions cheaks that he has to first deselect the perivois paiece;
			lastloc = -1;
			is_selected = false;
			midlletex.clear(sf::Color::Transparent);
			p_moves.clear();

		}
		else if (count(p_moves.begin(), p_moves.end(), newloc) > 0) {
			//runs when the selected loc is availabe on p_moves indicating a succefull chance
			peice_board[newloc] = peice_board[lastloc];
			peice_board[lastloc] = 0;
			is_selected = false;
			lastloc = -1;
			midlletex.clear(sf::Color::Transparent);
			p_moves.clear();
			active_player *= (-1);
		}
	}
	else {

		int selectedpos = get_number_from_localpos(local_pos.x, local_pos.y);

		if (peice_board[selectedpos] * active_player > 0){
			lastloc = selectedpos;
			is_selected = true;
			to_draw = true;
			
			
			getmoves(selectedpos, p_moves);
			//if (king_in_cheak(selectedpos, active_player * 16, danger_net[active_player + 1])) { ; }
			setup_mid(midlletex, selectedpos, p_moves, false);

		}
	}
}


//prepares and returns the top texure for every single peice from 'peice_board[64]'
sf::Texture Piece_tex(sf::RenderTexture& peice_borad_tex) {
	//sf::RenderTexture peice_borad_tex(actualres);
	for (int i = 0;i < 64;i++) {
		if (peice_board[i] == 0) {
			continue;
		}
		sf::Texture p;
		string path = "C:\\Users\\ad999\\source\\repos\\ConsoleShowCase\\ConsoleApp\\ConsoleApplication\\ConsoleApplication\\" + mypmap[peice_board[i]];
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

//main funtion which contains the mainloop of the game
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
	peice_board[28] = 9;

	vector<int> p_moves;


	bool to_draw = false;
	while (window.isOpen()) {
		while (const optional event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>())
				window.close();
		}

		set_texture(texture);
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {

			on_left_press(window, texture, p_moves, middletex, to_draw);
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
