#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <Windows.h>
using namespace std;


constexpr float nums = 1 / 8;

struct board_total_info {
	vector<int> board;
	vector<vector<bool>> castle_rights;
	int white_facing;
	vector<int> live_king_position;
	int active_player;
};

void getmoves(int t, vector<int>& p_moves, struct board_total_info &game);
bool handle_move(int lastloc, int newloc, vector<int>& p_moves, struct board_total_info& game);
sf::Vector2i getcord(int n);
int getnumber(int x, int y);
int get_number_from_localpos(int x, int y);
vector<vector<int>> ai_move_handler(struct board_total_info &game);








bool debug = false;




int move_number = 0;


vector<int> live_king_position = { 59,-1,4 }; //black then white





vector<int> white_peice_on_board({0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15});
vector<int> black_peice_on_board({ 63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48});


sf::Vector2u actualres = { 800,800 };
unsigned int fps = 60;
sf::Vector2f spos = { 0,0 };
sf::RectangleShape board[64]; //holds the rectanlge shapes for background board;
int white_facing_front = 0; //facing front means on lower side of sreen

int gloabal_last_loc = -1; //holds the last location selected to do next move;
bool is_selected = false; //cheaks if a peice is already selected;


int active_player = 1; //-1 for black +1 for white changes on turn
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

vector<bool>  blackcastle({ true,true,true });
vector<bool>  whitecastle({ true,true,true });

vector<vector<bool>> castle_righs({ blackcastle, {}, whitecastle });
unordered_map<int, string> mypmap;	//holdes peice and its path to draw on texrue

vector<int> peice_board_global(64); //holds information of which peice using numbers


//run on start to set map of images
void setup_piece_map(vector<int> &peice_board) {

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



//draws board texture
void set_texture(sf::RenderTexture& texture) {
	for (int i = 0;i < 64;i++) {
		texture.draw(board[i]);
	}
}


//seperates and return the moves acording to the peice


//sets up and update middile texture for indicating possible moves and selected peice
void setup_mid(sf::RenderTexture& midtex, int l, vector<int>& pmoves, bool reset, vector<int> &peice_board) {
	if (reset) {
		midtex.clear();
		return;
	}
	sf::RectangleShape a;
	if(debug){
		cout << (l % 8) * 100 << " " << (l / 8) * 100 << " " << l << endl;
	}

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






void final_steps( vector<int>& p_moves, sf::RenderTexture& midlletex,struct board_total_info &game) {
	move_number++;
	is_selected = false;
	gloabal_last_loc = -1;
	midlletex.clear(sf::Color::Transparent);
	p_moves.clear();
	
}


void ai_move_doer(int from_loc, int to_loc, struct board_total_info& game);

//is called on selection of sqare on board
void on_left_press(sf::RenderWindow& window, sf::RenderTexture& texture, vector<int>& p_moves, sf::RenderTexture& midlletex, bool& to_draw, struct board_total_info &game) {

	if (game.active_player == 1){
		vector<vector<int>> moves=ai_move_handler(game);
		ai_move_doer(moves[0][0], moves[0][1], game);
		final_steps(p_moves, midlletex,game);
		return;
	}
	sf::Vector2i local_pos = sf::Mouse::getPosition() - window.getPosition();
	if(debug)
	{
		cout << "Active Player:" << active_player << endl;
	}
	if (local_pos.x < 0 || local_pos.y < 0 || local_pos.x > 800 || local_pos.y > 800) {
		return;
	}
	
	//a peice was already selected
	if (is_selected) {

		int newloc = get_number_from_localpos(local_pos.x, local_pos.y);
		
		
		if (handle_move(gloabal_last_loc, newloc, p_moves, game)) {
			final_steps(p_moves, midlletex,game);
		}
		else {
			gloabal_last_loc = -1;
			is_selected = false;
			midlletex.clear(sf::Color::Transparent);
			p_moves.clear();
			
		}
		
		
		
	}
	else {
		//a peice is selectec from empty position
		int selectedpos = get_number_from_localpos(local_pos.x, local_pos.y);
		if (game.board[selectedpos] * game.active_player > 0) {
			gloabal_last_loc = selectedpos;
			is_selected = true;
			to_draw = true;

			//get the moves of the peice and pass those to cheack if the king is in cheak and if the move allows us to move then set up mid accordingly
			getmoves(selectedpos, p_moves,game);
			for (auto i : p_moves) {
				cout << i << endl;
			}
			setup_mid(midlletex, selectedpos, p_moves, false,game.board);
		}
	}
	
}



//prepares and returns the top texure for every single peice from 'peice_board[64]'
sf::Texture Piece_tex(sf::RenderTexture& peice_borad_tex, vector<int> &peice_board) {
	//sf::RenderTexture peice_borad_tex(actualres);
	for (int i = 0;i < 64;i++) {
		if (peice_board[i] == 0) {
			continue;
		}
		sf::Texture p;
		string mypath = "C:\\Users\\ad999\\source\\repos\\ConsoleShowCase\\ConsoleApp\\ConsoleApplication\\ConsoleApplication\\"; //update this for your system(path to peice img)
		string path = mypath + mypmap[peice_board[i]];
		if (!p.loadFromFile(path)) {
			exit(1);
		}
		sf::Sprite s(p);
		s.setScale({ 1.5,1.5 });
		//	sf::Angle a = sf::degrees(180);
			//s.setRotation(a);
		sf::Vector2i loc = getcord(i);
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
	
	setup_piece_map(peice_board_global);
	

	struct board_total_info game;
	game.board = peice_board_global;
	game.castle_rights = castle_righs;
	game.live_king_position = live_king_position;
	game.white_facing = white_facing_front;
	game.active_player = active_player;

	
	sf::RenderTexture middletex(actualres);//middle texture for tile colors of selected and posible moves
	long long sum = 0;
	//done();

	vector<int> p_moves;
	

	bool to_draw = false;
	while (window.isOpen()) {
		while (const optional event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>())
				window.close();
		}

		set_texture(texture);
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
			on_left_press(window, texture, p_moves, middletex, to_draw,game);
			while (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
				Sleep(30);
			}
			peice_borad_tex.clear(sf::Color::Transparent);
		}

		texture.display();
		middletex.display();

		sf::Texture tex = texture.getTexture();
		sf::Texture t = middletex.getTexture();
		sf::Texture ptex = Piece_tex(peice_borad_tex,game.board);
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
