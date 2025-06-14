#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <stack>
#include <unordered_map>
#include <Windows.h>






bool debug = false;

using namespace std;

vector<vector<int>> give_moves(int b[]);

int move_number = 0;

vector<vector<int>> pass_moves;// {location,should_be_peice,new_passed possible_move} to pass a move from one player to other in speacial cases like an-pasant
vector<int> live_king_position = { 59,-1,4 }; //black then white
void get_Knight_moves(int t, vector<int>& ans, int peice);
void get_bishop_moves(int t, vector<int>& ans, int peice,bool is_for_king);
bool king_in_cheak_bool_only(int t, int peice,int active_player_local);




vector<int> white_peice_on_board({0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15});
vector<int> black_peice_on_board({ 63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48});


sf::Vector2u actualres = { 800,800 };
unsigned int fps = 60;
sf::Vector2f spos = { 0,0 };
sf::RectangleShape board[64]; //holds the rectanlge shapes for background board;
int white_facing_front = 0; //facing front means on lower side of sreen

int gloabal_last_loc = -1; //holds the last location selected to do next move;
bool is_selected = false; //cheaks if a peice is already selected;
vector<int> danger_white; //keeps peice that are atacking white king
vector<int> danger_black;	//keeps peice that are atacking black king
vector<vector<int>> danger_net({ danger_black,{},danger_white }); //0 for black 2 for white as -1+1 is 0 and 1+1 is 2
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
sf::Vector2i getcord(int n) {
	int y = n / 8;
	int x = n % 8;
	sf::Vector2i cord(x, y);
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
	sf::Vector2i a = getcord(t);
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
			
				ans.push_back(getnumber(s[i].first, s[i].second));
			
		}
	}
}
void get_bishop_moves(int t, vector<int>& ans, int peice, bool is_for_king) {
	sf::Vector2i a = getcord(t);
	vector<pair<int, int>> s;
	sf::Vector2i b(a);

	if (b.x != 7 && b.y != 7) {
		do {
			b.x++;b.y++;
			if (b.x > 7 || b.y > 7) {
				break;
			}
			s.push_back({ b.x,b.y });
		} while (peice_board[getnumber(b.x, b.y)] == 0 || (peice_board[getnumber(b.x,b.y)]==active_player*16 && is_for_king));
	}

	b = a;
	if (b.x != 0 && b.y != 7)
	{
		do {
			b.x--; b.y++;
			if ((b.x < 0 || b.y > 7)) {
				break;
			}
			s.push_back({ b.x,b.y });
		} while (peice_board[getnumber(b.x, b.y)] == 0 || (peice_board[getnumber(b.x, b.y)] == active_player * 16 && is_for_king));
	}
	b = a;
	if (b.x != 7 && b.y != 0)
	{
		
		do {
			b.x++; b.y--;
			if ((b.y < 0 || b.x > 7)) {
				break;
			}
			s.push_back({ b.x,b.y });
		} while (peice_board[getnumber(b.x, b.y)] == 0 || (peice_board[getnumber(b.x, b.y)] == active_player * 16 && is_for_king));
	}
	b = a;
	if (b.x != 0 && b.y != 0)
	{
		do {
			b.x--;b.y--;
			if ((b.y < 0 || b.x < 0)) {
				break;
			}
			s.push_back({ b.x,b.y });
		} while (peice_board[getnumber(b.x, b.y)] == 0 || (peice_board[getnumber(b.x, b.y)] == active_player * 16 && is_for_king));
	}
	for (int i = 0;i < s.size();i++) {
		ans.push_back(getnumber(s[i].first, s[i].second));
	}

}
void get_pawn_moves_white(int t, vector<int>& ans, int peice,bool atack_only) {
	sf::Vector2i a = getcord(t);
	vector<pair<int, int>> f;
	vector<pair<int, int>> s;
	if (white_facing_front == 1) {
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
				f.push_back({ a.x,a.y + 2 });
			}
		}
		s.push_back({ a.x + 1,a.y + 1 });
		s.push_back({ a.x - 1,a.y + 1 });
	}
	if (atack_only) {
		for (int i = 0;i < s.size();i++) {
			ans.push_back(getnumber(s[i].first, s[i].second));
		}
	}else
	{
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

}
void get_pawn_moves_black(int t, vector<int>& ans, int peice,bool atack_only) {
	sf::Vector2i a = getcord(t);
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
				f.push_back({ a.x,a.y + -2 });
			}
		}
		s.push_back({ a.x + 1,a.y + 1 });
		s.push_back({ a.x - 1,a.y + 1 });
	}
	if (atack_only) {
		for (int i = 0;i < s.size();i++) {
			ans.push_back(getnumber(s[i].first, s[i].second));
		}
	}else
	{
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
}
void get_rook_moves(int t, vector<int>& ans, int peice, bool is_for_king) {
	sf::Vector2i a = getcord(t);
	vector<pair<int, int>> s;
	sf::Vector2i b=a;

	b = a;
	if (b.y != 7)
	{
		do {
			
			b.y++;
			if (b.y > 7) {
				break;
			}
			s.push_back({ b.x, b.y });

		} while (peice_board[getnumber(b.x, b.y)] == 0 || (peice_board[getnumber(b.x, b.y)] == active_player * 16 && is_for_king));
	}
	b = a;
	if (b.y != 0)
	{
		do {
			
			b.y--;
			if (b.y < 0){
				break;
			}
			s.push_back({ b.x, b.y });


		} while (peice_board[getnumber(b.x, b.y)] == 0 || (peice_board[getnumber(b.x, b.y)] == active_player * 16 && is_for_king));
	}

	b = a;
	if (b.x != 7)
	{
		do {
		
			b.x++;
			if (b.x > 7) {
				break;
			}
			s.push_back({ b.x, b.y });
		} while (peice_board[getnumber(b.x, b.y)] == 0 || (peice_board[getnumber(b.x, b.y)] == active_player * 16 && is_for_king));
	}
	b = a;
	if (b.x != 0)
	{
		do {
			
			b.x--;
			if (b.x < 0) {
				break;
			}
			s.push_back({ b.x, b.y });
		} while (peice_board[getnumber(b.x, b.y)] == 0 || (peice_board[getnumber(b.x, b.y)] == active_player * 16 && is_for_king));
	}

	for (int i = 0;i < s.size();i++) {
		ans.push_back(getnumber(s[i].first, s[i].second));
	}


}
void get_king_moves(int t, vector<int>& ans, int peice ) {
	vector < pair<int, int>> s;
	sf::Vector2i pos = getcord(t);
	s.push_back({ pos.x + 1,pos.y + 1 });
	s.push_back({ pos.x + 1,pos.y });
	s.push_back({ pos.x + 1,pos.y - 1 });
	s.push_back({ pos.x,pos.y + 1 });
	s.push_back({ pos.x,pos.y - 1 });
	s.push_back({ pos.x - 1,pos.y + 1 });
	s.push_back({ pos.x - 1,pos.y });
	s.push_back({ pos.x - 1,pos.y - 1 });
	for (int i = 0;i < s.size();i++) {
		if (s[i].first < 0 || s[i].first > 7 || s[i].second < 0 || s[i].second > 7) {
			continue;
		}
		else {
			ans.push_back(getnumber(s[i].first, s[i].second));
		}
	}
	if (castle_righs[active_player + 1][0] == true) {
		if (castle_righs[active_player + 1][1] == true) {
			int l = t-1;
			while ((l % 8) != 0) {
				if(peice_board[l] != 0 || king_in_cheak_bool_only(l,active_player*16,active_player)){
				break;
				}
				l--;
			}
			if (l % 8 == 0 && t % 8 == 4) {
				ans.push_back(l + 2);
			}
			else if(l % 8 == 0 && t % 8 == 3){
				ans.push_back(l + 1);
			}
		}
		if (castle_righs[active_player + 1][2] == true) {
			int l = t+1;
			while ((l % 8) != 7) {
				if (peice_board[l] != 0 || king_in_cheak_bool_only(l, active_player * 16,active_player)){
					break;
				}
				l++;
			}
			if (l % 8 == 7 && t % 8 ==4){
				ans.push_back(l - 1);
			}else if (l % 8 == 7 && t % 8 == 3) {
				ans.push_back(l - 2);
			}
		}
	}

}
void get_queen_moves(int t, vector<int>& ans, int peice) {
	get_rook_moves(t, ans, peice,false);
	get_bishop_moves(t, ans, peice, false);
}


bool king_in_cheak_bool_only(int t, int peice,int active_player_local){
	vector<int> knights;
	vector<int> bishop;
	vector<int> rook;
	vector<int> pawn;
	//vector<int> king;
	//get_king_moves(t, king, peice_board[t]);
	get_Knight_moves(t, knights, peice_board[t]);
	get_bishop_moves(t, bishop, peice_board[t],true);
	get_rook_moves(t, rook, peice_board[t],true);

	/*for (auto i : king) {
		if (peice_board[i] == -1*active_player_local*16) {
			cout << "a";
			return true;
		}
	}*/
	if (active_player_local == 1) {
		get_pawn_moves_white(t, pawn, peice_board[t],true);
		for (auto i : pawn) {
			if (peice_board[i] >= (-1) * active_player_local * abs(8) && peice_board[i] <= (-1) * active_player_local * abs(1)) {
				
				return true;
			}
		}
	}
	else {
		get_pawn_moves_black(t, pawn, peice_board[t], true);
		for (auto i : pawn) {
			if (peice_board[i] <= (-1) * active_player_local * abs(8) && peice_board[i] >= (-1) * active_player_local * abs(1)) {
				return true;
			}
		}
	}
	
	//vector<int> danger;
	for (auto i : knights) {
		if (peice_board[i] == (-1) * active_player_local * abs(11) || peice_board[i] == (-1) * active_player_local * abs(12)) {
			
			return true;
		}
	}
	for (int i : bishop) {
		if ((peice_board[i]) == (-1) * active_player_local * abs(13) || (peice_board[i]) == (-1) * active_player_local * abs(14) || (peice_board[i]) == (-1) * active_player_local * abs(15)) {
		
			return true;
		}
		if (peice_board[i] == -1 * active_player_local * 16 && abs(getcord(i).x - getcord(t).x) <= 1 && abs(getcord(i).y - getcord(t).y) <= 1) {
			return true;
		}

	}
	for (int i : rook) {
		if ((peice_board[i]) == (-1) * active_player_local * abs(9) || (peice_board[i]) == (-1) * active_player_local * abs(10) || (peice_board[i]) == (-1) * active_player_local * abs(15)) {
			
			return true;
		}
		if (peice_board[i] == -1 * active_player_local * 16 && abs(getcord(i).x - getcord(t).x) <= 1 && abs(getcord(i).y - getcord(t).y) <= 1) {
			return true;
		}
	}
	return false;
}

void king_in_cheak(int t, int peice,int active_player_local,vector<int> &pmoves){
	vector<int> knights;
	vector<int> bishop;
	vector<int> rook;
	vector<int> pawn;
	vector<int> danger;
	
	get_Knight_moves(t, knights, peice_board[t]);
	get_bishop_moves(t, bishop, peice_board[t], true);
	get_rook_moves(t, rook, peice_board[t], true);

	
	if (active_player_local == 1) {
		get_pawn_moves_white(t, pawn, peice_board[t], true);
		for (auto i : pawn) {
			if (peice_board[i] >= (-1) * active_player_local * abs(8) && peice_board[i] <= (-1) * active_player_local * abs(1)) {
				danger.push_back(i);
			}
		}
	}
	else {
		get_pawn_moves_black(t, pawn, peice_board[t], true);
		for (auto i : pawn) {
			if (peice_board[i] <= (-1) * active_player_local * abs(8) && peice_board[i] >= (-1) * active_player_local * abs(1)) {
				danger.push_back(i);
			}
		}
	}

	//vector<int> danger;
	for (auto i : knights) {
		if (peice_board[i] == (-1) * active_player_local * abs(11) || peice_board[i] == (-1) * active_player_local * abs(12)) {

			danger.push_back(i);
		}
	}
	for (int i : bishop) {
		if ((peice_board[i]) == (-1) * active_player_local * abs(13) || (peice_board[i]) == (-1) * active_player_local * abs(14) || (peice_board[i]) == (-1) * active_player_local * abs(15)) {

			danger.push_back(i);
		}
		if (peice_board[i] == -1 * active_player_local * 16 && abs(getcord(i).x - getcord(t).x) <= 1 && abs(getcord(i).y - getcord(t).y) <= 1) {
			danger.push_back(i);
		}

	}
	for (int i : rook) {
		if ((peice_board[i]) == (-1) * active_player_local * abs(9) || (peice_board[i]) == (-1) * active_player_local * abs(10) || (peice_board[i]) == (-1) * active_player_local * abs(15)) {
			danger.push_back(i);
		}
		if (peice_board[i] == -1 * active_player_local * 16 && abs(getcord(i).x - getcord(t).x) <= 1 && abs(getcord(i).y - getcord(t).y) <= 1) {
			danger.push_back(i);
		}
	}

	vector<int> local(pmoves);
	
		
	
	
	
	pmoves.clear();
	for (int i = 0;i < danger.size();i++){
		for (int j = 0;j < local.size();j++) {
			if (danger[i] == local[j]) {
				pmoves.push_back(danger[i]);
			}
		}
	}
}



//draws board texture
void set_texture(sf::RenderTexture& texture) {
	for (int i = 0;i < 64;i++) {
		texture.draw(board[i]);
	}
}

//seperates and return the moves acording to the peice
void getmoves(int t, vector<int>& p_moves,int active_players) {
	int peice = peice_board[t];
	vector<int> ans;
	if (peice <= 8 && peice >= 1) {
		get_pawn_moves_white(t, ans, peice,false);
		if (pass_moves.size() != 0) {
			for (int i = 0;i < pass_moves.size();i++) {
				if (pass_moves[i][0] == t && (active_players * pass_moves[i][1]) > 0) {
				
					ans.push_back(pass_moves[i][2]);
				}
			}
		}
	}

	if (peice <= -1 && peice >= -8) {
		get_pawn_moves_black(t, ans, peice, false);
		if (pass_moves.size() != 0) {
			for (int i = 0;i < pass_moves.size();i++) {
				if (pass_moves[i][0] == t && (active_players* pass_moves[i][1]) > 0) {
				
					ans.push_back(pass_moves[i][2]);
				}
			}
		}
	}
	if (abs(peice) == abs(12) || abs(peice) == abs(11)) {
		get_Knight_moves(t, ans, peice);
	}
	if (abs(peice) == abs(13) || abs(peice) == abs(14)) {
		get_bishop_moves(t, ans, peice,false);
	}
	if (abs(peice) == abs(10) || abs(peice) == abs(9)) {
		get_rook_moves(t, ans, peice,false);
	}
	if (abs(peice) == abs(15)) {
		get_queen_moves(t, ans, peice);
	}
	if (abs(peice) == abs(16)){
		get_king_moves(t, ans, peice);
		for (int i = 0;i < ans.size();i++) {
			if (peice_board[ans[i]] * peice <= 0 && !king_in_cheak_bool_only(ans[i],peice,active_players)) {
				p_moves.push_back(ans[i]);
			}
		}
	}else{
		for (int i = 0;i < ans.size();i++) {
			if (peice_board[ans[i]] * peice <= 0) {
				p_moves.push_back(ans[i]);

			}
		}
	}


	if(debug){
		for (auto i : ans) {
			cout << i << " ";
		}
		cout << "Peice:" << peice << ";";
	}

}

//sets up and update middile texture for indicating possible moves and selected peice
void setup_mid(sf::RenderTexture& midtex, int l, vector<int>& pmoves, bool reset) {
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



void handle_king_move(int from_pos, int newloc, vector<int>& p_moves) {
	peice_board[newloc] = peice_board[from_pos];
	peice_board[from_pos] = 0;
	sf::Vector2i n = getcord(newloc);
	sf::Vector2i o = getcord(from_pos);
	if (o.y==n.y && abs(o.x-n.x)>1) {
		if (from_pos - newloc < 0) {
			peice_board[newloc - 1] = peice_board[getnumber(7,n.y)];
			peice_board[getnumber(7, n.y)] = 0;
		}
		else if (from_pos - newloc > 0) {
			peice_board[newloc + 1] = peice_board[getnumber(0,n.y)];
			peice_board[getnumber(0, n.y)] = 0;
		}
	}
	castle_righs[active_player + 1][0] = false;
}
void handle_pawn_move(int from_pos, int newloc, vector<int>& p_moves) {
	if (abs(getcord(from_pos).y - getcord(newloc).y)>1 && (getcord(from_pos).y==1 || getcord(from_pos).y==6)){
		vector<int> to_pass1;
		vector<int> to_pass2;
		if (from_pos % 8 != 7) {
			to_pass1.push_back(newloc + 1);
			to_pass1.push_back(peice_board[from_pos] * -1);
			to_pass1.push_back(from_pos - newloc < 0 ? from_pos + 8 : from_pos - 8);
			to_pass1.push_back(newloc);
			to_pass1.push_back(move_number+2);
			pass_moves.push_back(to_pass1);
		}
		if (from_pos % 8 != 0) {
			to_pass2.push_back(newloc - 1);
			to_pass2.push_back(peice_board[from_pos] * -1);
			to_pass2.push_back(from_pos - newloc < 0 ? from_pos + 8 : from_pos - 8);
			to_pass2.push_back(newloc);
			to_pass2.push_back(move_number+2);
			pass_moves.push_back(to_pass2);
		}
		
		
	}
	for (int i = 0;i < pass_moves.size();i++) {
		
		if (from_pos == pass_moves[i][0] && newloc == pass_moves[i][2]) {
			
			peice_board[pass_moves[i][3]] = 0;
		}
	}
	peice_board[newloc] = peice_board[from_pos];
	peice_board[from_pos] = 0;
}



void handle_move(int lastloc,int newloc,vector<int>& p_moves, sf::RenderTexture& midlletex) {
	

	if (newloc == lastloc) {
		//if player does not want to play this peice then this funtions cheaks that he has to first deselect the perivois paiece;
		lastloc = -1;
		is_selected = false;
		midlletex.clear(sf::Color::Transparent);
		p_moves.clear();

	}
	else if(count(p_moves.begin(), p_moves.end(), newloc) > 0){
		//runs when the selected loc is availabe on p_moves indicating a succefull 
		for (int i = 0;i < pass_moves.size();i++) {
			if (pass_moves[i][4] <= move_number) {
				pass_moves.erase(pass_moves.begin() + i);
				i--;
			}
		}
		if (abs(peice_board[lastloc]) <= 8) {
			handle_pawn_move(lastloc, newloc, p_moves);
		}else if (abs(peice_board[lastloc])!=16){

			if (abs(peice_board[lastloc]) == 9 || abs(peice_board[lastloc]) == 10) {
				castle_righs[active_player + 1][lastloc == 0 ? 1 : 2] = false;
			}
			peice_board[newloc] = peice_board[lastloc];
			peice_board[lastloc] = 0;
			

		}
		else if(abs(peice_board[lastloc]) == 16){
			handle_king_move(lastloc, newloc, p_moves);
			live_king_position[active_player + 1] = newloc;
			
		}
		
		
		//sets new loc with old peice ans sets old loc as empty(0)


		//if the king changes position then reflect it on its live_king_positon
		
		
		//cheak_both_kings(midlletex);

	}
}


void final_steps( vector<int>& p_moves, sf::RenderTexture& midlletex) {
	move_number++;
	is_selected = false;
	gloabal_last_loc = -1;
	midlletex.clear(sf::Color::Transparent);
	p_moves.clear();
	active_player *= (-1);
}
void is_move_legal(int t, vector<int>& p_moves,int u,int n,int m) {
	if (abs(peice_board[t]) == 16) {
		return;
	}
	vector<int> local(p_moves);
	p_moves.clear();
	for (auto i : local) {
		int delected_peice = peice_board[i];
		int moves_peice = peice_board[t];
		peice_board[i] = peice_board[t];
		peice_board[t] = 0;
		
		if (!king_in_cheak_bool_only(live_king_position[u], n,m)) {
			p_moves.push_back(i);
		}
		peice_board[t] = moves_peice;
		peice_board[i] = delected_peice;
	}
}

void cheak_king(int t,vector<int> &p_moves){
	if (king_in_cheak_bool_only(live_king_position[0], -16, -1)) {
		//king_in_cheak(live_king_position[0], -16, -1,p_moves);
	}
	
	if (king_in_cheak_bool_only(live_king_position[2], 16, +1)) {
		//king_in_cheak(live_king_position[2], 16, 1, p_moves);
	}
	if (active_player == -1) {
		is_move_legal(t, p_moves, 0, -16, -1);
	}
	else {
		is_move_legal(t, p_moves, 2, 16, 1);
	}
	
}

//is called on selection of sqare on board
void on_left_press(sf::RenderWindow& window, sf::RenderTexture& texture, vector<int>& p_moves, sf::RenderTexture& midlletex, bool& to_draw) {
	sf::Vector2i local_pos = sf::Mouse::getPosition() - window.getPosition();
	if(debug)
	{
		cout << "Active Player:" << active_player << endl;
	}
	if (local_pos.x < 0 || local_pos.y < 0 || local_pos.x > 800 || local_pos.y > 800) {
		return;
	}
	if (active_player == 1) {
		//vector holds moves from,to,p_moves
		vector<vector<int>> u=give_moves(peice_board);
		handle_move(u[0][0], u[0][1], u[1], midlletex);
		final_steps(u[1], midlletex);
		return;
	}
	
	if (is_selected) {
		int newloc = get_number_from_localpos(local_pos.x, local_pos.y);
		int n = gloabal_last_loc;
		handle_move(gloabal_last_loc, newloc,p_moves, midlletex);
		if (n != newloc) {
			final_steps(p_moves, midlletex);
		}
		n = -1;
		
		
	}
	else {
		//a peice is selectec from empty position
		int selectedpos = get_number_from_localpos(local_pos.x, local_pos.y);
		if (peice_board[selectedpos] * active_player > 0) {
			gloabal_last_loc = selectedpos;
			is_selected = true;
			to_draw = true;
			getmoves(selectedpos, p_moves,active_player);
			
			cheak_king(selectedpos,p_moves);
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
	
	setup_piece_map();
	//peice_board[23] = (-10);
	//peice_board[63] = (0);


	
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
