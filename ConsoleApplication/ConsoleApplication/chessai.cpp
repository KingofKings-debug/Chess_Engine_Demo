#include <stdio.h>
#include <stdlib.h>
#include <set>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <stack>
using namespace std;
using namespace std::chrono;
int global_depth = 7;
map<int, vector<int>> mymap;


struct board_total_info {
	vector<int> board;
	vector<vector<bool>> castle_rights;
	int white_facing;
	vector<int> live_king_position;
	int active_player;
};


int board_center_distance[] =
{ 5, 4, 4, 4, 4, 4, 4, 5,
4, 4, 3, 3, 3, 3, 4, 4,
4, 3, 2, 2, 2, 2, 3, 4,
4, 3, 2, 1, 1, 2, 3, 4,
4, 3, 2, 1, 1, 2, 3, 4,
4, 3, 2, 2, 2, 2, 3, 4,
4, 4, 3, 3, 3, 3, 4, 4,
5, 4, 4, 4, 4, 4, 4, 5 };





void getmoves(int t, vector<int>& p_moves, struct board_total_info &game);
int white_play(struct board_total_info &game, int depth,int alpha,int beta,map<vector<int>,int> &board_map);
int black_play(struct board_total_info &game, int depth, int alpha, int beta, map<vector<int>, int> &board_map);
void ai_move_doer(int from_loc, int to_loc, struct board_total_info& game);



int eval_only_peice(int side, vector<int> board) {
	int ans = 0;
	vector<int> values = { 1,1,1,1,1,1,1,1,5,5,3,3,3,3,9,100 };
	for (int i = 0;i < 64;i++) {
		if (board[i] != 0) {
			ans += values[abs(board[i])-1]*100*(board[i]<0?(-1):1);
			continue;
		}
		if (board[i] * side > 0) {
			if (abs(board[i]) <= 8 && abs(board[i]) > 8) {
				ans += 100;
			}
			if (abs(board[i]) == 11 || abs(board[i]) == 12 || abs(board[i]) == 13 || abs(board[i]) == 14) {
				ans += 300;
			}
			if ((abs(board[i]) == 9) || (abs(board[i]) == 10)) {
				ans += 500;
			}
			if (abs(board[i]) == 15) {
				ans += 900;
			}
			if (abs(board[i]) == 16) {
				ans += 1000;
			}
		}
	}
	
	return ans;
}





long long n = 0;
int cheack_move_value(vector<int> moves, vector<int> board) {
	vector<int> values = { 1,1,1,1,1,1,1,1,5,5,3,3,3,3,9,100 };
	int ans = 0;
	
	if (moves[3] != 0) {
		ans += 100;
		if (values[abs(moves[2]) - 1] < values[abs(moves[3]) - 1]) {
			ans += values[abs(moves[3]) - 1] - values[abs(moves[2]) - 1];
		}
	}
	return ans;
	
	
}
bool comp( const pair<vector<int>, int> &a,const pair<vector<int>, int> &b) {
	return true;
}
int white_play(struct board_total_info& game, int depth, int alpha, int beta, map<vector<int>, int>& board_map) {
	if (depth == 0) {
		n++;
		return eval_only_peice(1, game.board);
	}
	if (board_map.find(game.board) != board_map.end()) {
		return board_map[game.board];
	}
	vector<pair<vector<int>, int>> move_list;
	move_list.reserve(200);
	for (int i = 0;i < 64;i++) {
		if (game.board[i] > 0 ) {
			vector<int> peice_moves;
			getmoves(i, peice_moves, game);
			
			for (int j = 0;j < peice_moves.size();j++) {
				int val = cheack_move_value({ i,peice_moves[j],game.board[i],game.board[peice_moves[j]] }, game.board);
				move_list.push_back({ { i,peice_moves[j],game.board[i],game.board[peice_moves[j]]},val});
			}
		}
	}
	sort(move_list.begin(), move_list.end(), [](pair<vector<int>, int> a, pair<vector<int>, int> b) {return (int)(a.second) > (int)(b.second);});
	
	//to be replaced by move handler

	//cout << "white moves count done " << move_list.size() << endl;
	int max_till_now = INT_MIN;
	for (int i = 0;i < move_list.size();i++) {
		struct board_total_info game_copy = game;
		ai_move_doer((move_list[i].first)[0], (move_list[i].first)[1], game);
		//game.board[(move_list[i].first)[0]] = 0;
		//game.board[(move_list[i].first)[1]] = (move_list[i].first)[2];
		int score = black_play(game, depth - 1, alpha, beta, board_map);
		
		
		if (max_till_now < score) {
			max_till_now = score;
			if (depth == global_depth) {
				mymap[max_till_now] = { (move_list[i].first)[0] ,(move_list[i].first)[1] };
			}
			if (score > alpha) {
				alpha = score;
			}
		}
		//game.board[(move_list[i].first)[0]] = (move_list[i].first)[2];
		//game.board[(move_list[i].first)[1]] = (move_list[i].first)[3];
		game = game_copy;
		alpha = max(alpha, score);
		if (score >= beta) {
			return score;
		}
		
	}
	board_map[game.board] = max_till_now;
	return max_till_now;

	
}


int black_play(struct board_total_info &game, int depth,int alpha,int beta, map<vector<int>, int> &board_map) {
	if (depth == 0) {
		n++;
		return eval_only_peice(1,game.board);
	}
	
	if (board_map.find(game.board) != board_map.end()) {
		return board_map[game.board];
	}
	//move list follows this structure= {{from_loc,final_loc,peice_moved_from_loc,orignal_peice_final_loc},move_imp._score}
	vector<pair<vector<int>, int>> move_list;
	move_list.reserve(200);
	for (int i = 0;i < 64;i++) {
		if (game.board[i] < 0) {
			vector<int> peice_moves;
			getmoves(i, peice_moves, game);
			for (int j = 0;j < peice_moves.size();j++) {
				
				int val = cheack_move_value({ i,peice_moves[j],game.board[i],game.board[peice_moves[j]] }, game.board);
				move_list.push_back({ { i,peice_moves[j],game.board[i],game.board[peice_moves[j]]},val });
			}
		}
	}
	sort(move_list.begin(), move_list.end(), [](pair<vector<int>, int> a, pair<vector<int>, int> b) {return (a.second) > (b.second);});
	int min_till_now = INT_MAX;
	//cout << "black moves count done " << move_list.size() << endl;
	for (int i = 0;i < move_list.size();i++) {
	//	game.board[(move_list[i].first)[0]] = 0;
	//game.board[(move_list[i].first)[1]] = (move_list[i].first)[2];
		struct board_total_info game_copy = game;
		ai_move_doer((move_list[i].first)[0], (move_list[i].first)[1], game);
		int score = white_play(game, depth - 1, alpha, beta, board_map);
		if (score < min_till_now) {
			min_till_now = score;
			if (score < beta) {
				beta = score;
			}
		}
		min_till_now = min(min_till_now, score);
		game = game_copy;
		//game.board[(move_list[i].first)[0]] = (move_list[i].first)[2];
		//game.board[(move_list[i].first)[1]] = (move_list[i].first)[3];
		beta = min(beta, score);
		if (score <= alpha) {
			return score;
		}
	}
	board_map[game.board] = min_till_now;
	return min_till_now;
}


vector<vector<int>> ai_move_handler(struct board_total_info &game) {
	n = 0;

	//string the already visited moves
	map<vector<int>, int> board_map;

	stack<struct board_total_info> mystack;
	

	auto start = high_resolution_clock::now();
	int alpha = INT_MIN;
	int beta = INT_MAX;
	int g=white_play(game, global_depth,alpha,beta,board_map);
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);

	vector<int>  moves_for_this_peice;
	getmoves(mymap[g][0], moves_for_this_peice,game);
	cout << "moves_check:" << n << endl;
	cout << "Time Taken:" << float(duration.count())/ 1000000.0 << endl;
	cout << mymap[g][0] << " " << mymap[g][1] << " " << game.board[mymap[g][0]];
	return {{mymap[g][0],mymap[g][1]},moves_for_this_peice };
	

}