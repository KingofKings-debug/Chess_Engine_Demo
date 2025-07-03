#include <stdio.h>
#include <stdlib.h>
#include <set>
#include <vector>
#include <map>
#include <iostream>
using namespace std;

int global_depth = 6;
map<int, vector<int>> mymap;


void get_Knight_moves(int t, vector<int>& ans, int peice, int peice_board[]);
void get_bishop_moves(int t, vector<int>& ans, int peice, bool is_for_king, int peice_board[]);
void get_rook_moves(int t, vector<int>& ans, int peice, bool is_for_king, int peice_board[]);
void getmoves(int t, vector<int>& p_moves, int active_players, int peice_board[]);
int white_play(int board[], int depth,int alpha,int beta);
int black_play(int board[], int depth, int alpha, int beta);


int eval_pawn(int side, int board[],int loc[]) {
	int evalutaion = 0;
	set<int> myset;
	for (int i = 0;i < 8;i++) {
		if (loc[i] == -1) { break; }
		if (myset.empty() || myset.count(loc[i] % 8) == 0) {
			evalutaion += 100;
			myset.insert(loc[i] % 8);
		}
		else {
			evalutaion += 50;
		}
	}
	return evalutaion;
}

int eval_bishop(int side, int board[], int loc[]) {
	int evaluation = 0;
	for (int i = 0;i < 2;i++) {
		if (loc[i] == -1) {
			break;
		}
		evaluation += 300;
		vector<int> moves;
		//get_bishop_moves(loc[i], moves,false, board[loc[i]], board);
		evaluation += moves.size();
	}
	return evaluation;
}

int eval_knight(int side, int board[], int loc[]) {
	int evaluation = 0;
	for (int i = 0;i < 2;i++) {
		if (loc[i] == -1) {
			break;
		}
		evaluation += 300;
		vector<int> moves;
		//get_Knight_moves(loc[i], moves, board[loc[i]], board);
		evaluation += moves.size();
	}
	return evaluation;
}

int eval_rook(int side, int board[], int loc[]) {
	int evaluation = 0;
	for (int i = 0;i < 2;i++) {
		if (loc[i] == -1) {
			break;
		}
		evaluation += 500;
		vector<int> moves;
		//get_rook_moves(loc[i], moves, false, board[loc[i]], board);
		evaluation += moves.size();
	}
	return evaluation;
}
int evaluate_board_with_maps(int side,int board[],int pawn[],int bishop[], int knight[], int rook[]) {
	int eval = 0;

	eval += eval_pawn(side, board, pawn);

	eval += eval_bishop(side, board, bishop);
	
	eval += eval_knight(side, board, knight);
	
	eval += eval_rook(side, board, rook);
	
	return eval;
}


int eveluate_baord_with_peice(int side,int board[]) {
	int evaluation = 0;
	int pawn[8],p= 0;
	int bishop[2],b=0;
	int knight[2],k=0;
	int rook[2],r=0;
	for (int i = 0;i <=63;i++) {
		if (board[i] * side > 0) {
			int peice = abs(board[i]);
			if (peice<=8 && peice>=1){
				pawn[p] = i;
				p++;
			}
			else if (peice == 9 || peice == 10) {
				rook[r] = i;
				r++;
			}
			else if (peice == 11 || peice == 12) {
				knight[k] = i;
				k++;
			}
			else if (peice == 13 || peice == 14) {
				bishop[b] = i;
				b++;
			}
			else if (peice == 15) {
				evaluation += 800;
			}
			else if (peice == 16) {
				evaluation += 10000;
			}
		}
	}
	while (r < 2) {
		rook[r] = -1;
		r++;
	}
	while (b < 2) {
		bishop[b] = -1;
		b++;
	}
	while (k < 2) {
		knight[k] = -1;
		k++;
	}
	while (p < 8) {
		pawn[p] = -1;
		p++;
	}
	
	return evaluation+evaluate_board_with_maps(side,board,pawn,bishop,knight,rook);
}




int white_play(int board[],int depth,int alpha,int beta) {
	if (depth == 0) {
		return eveluate_baord_with_peice(1, board);
	}
	int max_till_now = INT_MIN;
	for (int i = 0;i < 64;i++) {
		if (board[i] > 0) {
			vector<int> p_moves;
			getmoves(i, p_moves, 1, board);
			for (auto u : p_moves) {
				int peice_to_be_moved = board[i];
				int peice_located = board[u];
				board[u] = board[i];
				board[i] = 0;

				int score=black_play(board, depth - 1,alpha,beta);
				if (score > max_till_now) {
					max_till_now = score;
					if (depth == global_depth) {
						mymap[max_till_now] = { i,u };
					}

				}

				board[i] = peice_to_be_moved;
				board[u] = peice_located;
				if (max_till_now >= beta) {
					break;
				}
			}
		}
		
		alpha = max(alpha, max_till_now);
	}
	return max_till_now;
}
int black_play(int board[], int depth,int alpha,int beta) {
	if (depth == 0) {
		return (-1)*eveluate_baord_with_peice(-1, board);
	}
	int max_got_from_all_moves = INT_MAX;
	for (int i = 0;i < 64;i++) {
		if (board[i] < 0) {
			vector<int> p_moves;
			getmoves(i, p_moves, -1, board);
			for (auto u : p_moves) {
				int peice_to_be_moved = board[i];
				int peice_located = board[u];
				board[u] = board[i];
				board[i] = 0;
				max_got_from_all_moves = min(max_got_from_all_moves, white_play(board, depth-1,alpha,beta));

				board[i] = peice_to_be_moved;
				board[u] = peice_located;
				if (max_got_from_all_moves <= alpha) {
					break;
				}
			}
		}
		
		beta = min(beta, max_got_from_all_moves);
	}
	return max_got_from_all_moves;
}

vector<vector<int>> ai_move_handler(int board[]) {
	int g=white_play(board, global_depth,INT_MIN,INT_MAX);
	vector<int>  moves_for_this_peice;
	getmoves(mymap[g][0], moves_for_this_peice, 1,board);
	return {{mymap[g][0],mymap[g][1]},moves_for_this_peice };
}