#include <string>
#include <iostream>
#include "board.hpp"

constexpr U8 cw_90[64] = {
    48, 40, 32, 24, 16, 8,  0,  7,
    49, 41, 33, 25, 17, 9,  1,  15,
    50, 42, 18, 19, 20, 10, 2,  23,
    51, 43, 26, 27, 28, 11, 3,  31,
    52, 44, 34, 35, 36, 12, 4,  39,
    53, 45, 37, 29, 21, 13, 5,  47,
    54, 46, 38, 30, 22, 14, 6,  55,
    56, 57, 58, 59, 60, 61, 62, 63
};

constexpr U8 acw_90[64] = {
     6, 14, 22, 30, 38, 46, 54, 7,
     5, 13, 21, 29, 37, 45, 53, 15,
     4, 12, 18, 19, 20, 44, 52, 23,
     3, 11, 26, 27, 28, 43, 51, 31,
     2, 10, 34, 35, 36, 42, 50, 39,
     1,  9, 17, 25, 33, 41, 49, 47,
     0,  8, 16, 24, 32, 40, 48, 55,
    56, 57, 58, 59, 60, 61, 62, 63
};

constexpr U8 cw_180[64] = {
    54, 53, 52, 51, 50, 49, 48, 7,
    46, 45, 44, 43, 42, 41, 40, 15,
    38, 37, 18, 19, 20, 33, 32, 23,
    30, 29, 26, 27, 28, 25, 24, 31,
    22, 21, 34, 35, 36, 17, 16, 39,
    14, 13, 12, 11, 10,  9,  8, 47,
     6,  5,  4,  3,  2,  1,  0, 55,
    56, 57, 58, 59, 60, 61, 62, 63
};

constexpr U8 id[64] = {
     0,  1,  2,  3,  4,  5,  6,  7,
     8,  9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23,
    24, 25, 26, 27, 28, 29, 30, 31,
    32, 33, 34, 35, 36, 37, 38, 39,
    40, 41, 42, 43, 44, 45, 46, 47,
    48, 49, 50, 51, 52, 53, 54, 55,
    56, 57, 58, 59, 60, 61, 62, 63
};

#define cw_90_pos(p) cw_90[p]
#define cw_180_pos(p) cw_180[p]
#define acw_90_pos(p) acw_90[p]
#define cw_90_move(m) move_promo(cw_90[getp0(m)], cw_90[getp1(m)], getpromo(m))
#define acw_90_move(m) move_promo(acw_90[getp0(m)], acw_90[getp1(m)], getpromo(m))
#define cw_180_move(p) move_promo(cw_180[getp0(m)], cw_180[getp1(m)], getpromo(m))
#define color(p) ((PlayerColor)(p & (WHITE | BLACK)))

std::unordered_set<U16> transform_moves(const std::unordered_set<U16>& moves, const U8 *transform) {

    std::unordered_set<U16> rot_moves;

    for (U16 move : moves) {
        rot_moves.insert(move_promo(transform[getp0(move)], transform[getp1(move)], getpromo(move)));
    }

    return rot_moves;
}

std::unordered_set<U16> construct_bottom_rook_moves_with_board(const U8 p0, const U8* board) {

    int left_rook_reflect[7] = {0, 8, 16, 24, 32, 40, 48};
    PlayerColor color = color(board[p0]);
    std::unordered_set<U16> rook_moves;
    bool refl_blocked = false;

    if (p0 < 8 || p0 == 13) {
        if (!(board[p0+pos(0,1)] & color)) rook_moves.insert(move(p0, p0+pos(0,1))); // top
        if (p0 == 1) { // top continued on the edge
            for (int y = 1; y<=6; y++) {
                U8 p1 = pos(1, y);
                if (board[p1]) {
                    if (board[p1] & color) break;         // our piece
                    else rook_moves.insert(move(p0, p1)); // their piece - capture
                    break;
                }
                else rook_moves.insert(move(p0, p1));
            }
        }
    }
    else {
        if (!(board[p0-pos(0,1)] & color)) rook_moves.insert(move(p0, p0-pos(0,1))); // bottom
    }

    if (p0 != 6) {
        if (!(board[p0+pos(1,0)] & color)) rook_moves.insert(move(p0, p0+pos(1,0))); // right
    }

    for (int x=getx(p0)-1; x>=0; x--) {
        U8 p1 = pos(x, gety(p0));
        if (board[p1]) {
            refl_blocked = true;
            if (board[p1] & color) break;         // our piece
            else rook_moves.insert(move(p0, p1)); // their piece - capture
            break;
        }
        else {
            rook_moves.insert(move(p0, p1));
        }
    }

    if (refl_blocked) return rook_moves;
    
    if (p0 < 8) {
        for (int p1 : left_rook_reflect) {
            if (board[p1]) {
                if (board[p1] & color) break;         // our piece
                else rook_moves.insert(move(p0, p1)); // their piece - capture
                break;
            }
            else {
                rook_moves.insert(move(p0, p1));
            }
        }
    }

    return rook_moves;
}

std::unordered_set<U16> construct_bottom_bishop_moves_with_board(const U8 p0, const U8* board) {

    PlayerColor color = color(board[p0]);
    std::unordered_set<U16> bishop_moves;

    // top right - move back
    if (p0 < 6 || p0 == 13) {
        if (!(board[p0+pos(0,1)+pos(1,0)] & color)) bishop_moves.insert(move(p0, p0+pos(0,1)+pos(1,0)));
    }
    // bottom right - move back
    if (p0 > 6) {
        if (!(board[p0-pos(0,1)+pos(1,0)] & color)) bishop_moves.insert(move(p0, p0-pos(0,1)+pos(1,0)));
    }

    std::vector<U8> p1s;

    // top left - forward / reflections
    if (p0 == 1) {
        p1s.push_back(pos(0,1));
        p1s.push_back(pos(1,2));
    }
    else if (p0 == 2) {
        p1s.push_back(pos(1,1));
        p1s.push_back(pos(0,2));
        p1s.push_back(pos(1,3));
    }
    else if (p0 == 3) {
        p1s.push_back(pos(2,1));
        p1s.push_back(pos(1,2));
        p1s.push_back(pos(0,3));
        p1s.push_back(pos(1,4));
        p1s.push_back(pos(2,5));
        p1s.push_back(pos(3,6));
    }
    else if (p0 == 4 || p0 == 5) {
        p1s.push_back(p0-pos(2,0));
        p1s.push_back(p0+pos(0,1)-pos(1,0));
    }
    else if (p0 == 6) {
        p1s.push_back(pos(5,1));
    }
    else if (p0 == 10) {
        p1s.push_back(pos(1,0));
        p1s.push_back(pos(0,1));
    }
    else if (p0 == 11) {
        p1s.push_back(pos(2,0));
        p1s.push_back(pos(1,1));
        p1s.push_back(pos(0,2));
    }
    else if (p0 == 12) {
        p1s.push_back(pos(3,0));
        p1s.push_back(pos(2,1));
        p1s.push_back(pos(1,2));
        p1s.push_back(pos(0,3));
    }
    else if (p0 == 13) {
        p1s.push_back(pos(4,0));
        p1s.push_back(pos(3,1));
    }

    for (auto p1 : p1s) {
        if (board[p1]) {
            if (board[p1] & color) break;           // our piece
            else bishop_moves.insert(move(p0, p1)); // their piece - capture
            break;
        }
        else {
            bishop_moves.insert(move(p0, p1));
        }
    }

    return bishop_moves;
}

std::unordered_set<U16> construct_bottom_pawn_moves_with_board(const U8 p0, const U8 *board, bool promote = false) {
    
    PlayerColor color = color(board[p0]);
    std::unordered_set<U16> pawn_moves;

    if (!(board[pos(getx(p0)-1,0)] & color)) {
        if (promote) {
            pawn_moves.insert(move_promo(p0, pos(getx(p0)-1,0), PAWN_ROOK));
            pawn_moves.insert(move_promo(p0, pos(getx(p0)-1,0), PAWN_BISHOP));
        }
        else {
            pawn_moves.insert(move(p0, pos(getx(p0)-1,0)));
        }
    }
    if (!(board[pos(getx(p0)-1,1)] & color)) {
        if (promote) {
            pawn_moves.insert(move_promo(p0, pos(getx(p0)-1,1), PAWN_ROOK));
            pawn_moves.insert(move_promo(p0, pos(getx(p0)-1,1), PAWN_BISHOP));
        }
        else {
            pawn_moves.insert(move(p0, pos(getx(p0)-1,1)));
        }
    }
    if (p0 == 10 && !(board[17] & color)) pawn_moves.insert(move(p0, 17));

    return pawn_moves;
}

std::unordered_set<U16> construct_bottom_king_moves_with_board(const U8 p0, const U8 *board) {

    // king can't move into check. See if these squares are under threat from 
    // enemy pieces as well.
    
    PlayerColor color = color(board[p0]);
    std::unordered_set<U16> king_moves;
    if (!(board[pos(getx(p0)-1,0)] & color)) king_moves.insert(move(p0, pos(getx(p0)-1,0)));
    if (!(board[pos(getx(p0)-1,1)] & color)) king_moves.insert(move(p0, pos(getx(p0)-1,1)));
    if (p0 != 6 && !(board[pos(getx(p0)+1,0)] & color)) king_moves.insert(move(p0, pos(getx(p0)+1,0)));
    if (p0 != 6 && !(board[pos(getx(p0)+1,1)] & color)) king_moves.insert(move(p0, pos(getx(p0)+1,1)));
    if (!(board[pos(getx(p0),gety(p0)^1)] & color)) king_moves.insert(move(p0, pos(getx(p0),gety(p0)^1)));

    return king_moves;
}

char piece_to_char(U8 piece) {
    char ch = '.';
    if      (piece & PAWN)   ch = 'p';
    else if (piece & ROOK)   ch = 'r';
    else if (piece & BISHOP) ch = 'b';
    else if (piece & KING)   ch = 'k';

    if (piece & WHITE) ch = ch - ('a'-'A');

    return ch;
}

std::string board_to_str(const U8 *board) {

    std::string board_str = ".......\n.......\n..   ..\n..   ..\n..   ..\n.......\n.......\n";

    for (int i=0; i<56; i++) {
        U8 piece = board[i];
        if (board_str[i] == '\n' || board_str[i] == ' ') continue;
        board_str[(48-(i/8)*8) + i%8] = piece_to_char(piece);
    }

    return board_str;
}

std::string all_boards_to_str(const Board& b) {

    std::string board_str(256, ' ');
    std::string board_mask = ".......\n.......\n..   ..\n..   ..\n..   ..\n.......\n.......\n";

    const U8 (*boards)[64] = &(b.board_0);

    for (int b=0; b<4; b++) {
        for (int i=0; i<56; i++) {
            if (board_mask[i] == '\n' || board_mask[i] == ' ') continue;
            board_str[(224-(i/8)*32) + b*8 + i%8] = piece_to_char(boards[b][i]);
        }
    }

    for (int i=31; i<256; i+=32) {
        board_str[i] = '\n';
    }

    return board_str;
}

std::string move_to_str(U16 move) {

    std::string s = "a1a1";
    s[0] += getx(getp0(move));
    s[1] += gety(getp0(move));
    s[2] += getx(getp1(move));
    s[3] += gety(getp1(move));
    if (getpromo(move) & PAWN_BISHOP) {
        s += "b";
    }
    else if (getpromo(move) & PAWN_ROOK) {
        s += "r";
    }

    return s;
}

U16 str_to_move(std::string move) {
    
    U8 x0 = move[0] - 'a';
    U8 y0 = move[1] - '1';
    U8 x1 = move[2] - 'a';
    U8 y1 = move[3] - '1';
    U8 promo = 0;
    if (move.size() > 4) {
        if (move[4] == 'r') promo = PAWN_ROOK;
        else promo = PAWN_BISHOP;
    }

    return move_promo(pos(x0,y0), pos(x1,y1), promo);
}

std::unordered_set<U16> Board::get_pseudolegal_moves_for_piece(U8 piece_pos) const {

    std::unordered_set<U16> moves;
    U8 piece_id = this->board_0[piece_pos];

    std::unordered_set<U8> bottom({ 1, 2, 3, 4, 5, 6, 10, 11, 12, 13 });
    std::unordered_set<U8> left({ 0, 8, 16, 24, 32, 40, 9, 17, 25, 33 });
    std::unordered_set<U8> top({ 48, 49, 50, 51, 52, 53, 41, 42, 43, 44 });
    std::unordered_set<U8> right({ 54, 46, 38, 30, 22, 14, 45, 37, 29, 21 });

    const U8 *board = this->board_0;
    const U8 *coord_map = id;
    const U8 *inv_coord_map = id;
    if      (left.count(piece_pos))  { board = this->board_270;  coord_map = acw_90; inv_coord_map = cw_90;  }
    else if (top.count(piece_pos))   { board = this->board_180; coord_map = cw_180; inv_coord_map = cw_180; }
    else if (right.count(piece_pos)) { board = this->board_90; coord_map = cw_90;  inv_coord_map = acw_90; }

    if (piece_id & PAWN) {
        if (((piece_pos == 51 || piece_pos == 43) && (piece_id & WHITE)) || 
            ((piece_pos == 11 || piece_pos == 3)  && (piece_id & BLACK)) ) {
            moves = construct_bottom_pawn_moves_with_board(coord_map[piece_pos], board, true);
        }
        else {
            moves = construct_bottom_pawn_moves_with_board(coord_map[piece_pos], board);
        }
    }
    else if (piece_id & ROOK) {
        moves = construct_bottom_rook_moves_with_board(coord_map[piece_pos], board);
    }
    else if (piece_id & BISHOP) {
        moves = construct_bottom_bishop_moves_with_board(coord_map[piece_pos], board);
    }
    else if (piece_id & KING) {
        moves = construct_bottom_king_moves_with_board(coord_map[piece_pos], board);
    }

    moves = transform_moves(moves, inv_coord_map);

    return moves;
}

void rotate_board(U8 *src, U8 *tgt, const U8 *transform) {

    for (int i=0; i<64; i++) {
        tgt[transform[i]] = src[i];
    }
}

Board::Board(): board_0{} {

    this->board_0[this->b_rook_ws]  = BLACK | ROOK;
    this->board_0[this->b_rook_bs]  = BLACK | ROOK;
    this->board_0[this->b_king   ]  = BLACK | KING;
    this->board_0[this->b_bishop ]  = BLACK | BISHOP;
    this->board_0[this->b_pawn_ws]  = BLACK | PAWN;
    this->board_0[this->b_pawn_bs]  = BLACK | PAWN;

    this->board_0[this->w_rook_ws]  = WHITE | ROOK;
    this->board_0[this->w_rook_bs]  = WHITE | ROOK;
    this->board_0[this->w_king   ]  = WHITE | KING;
    this->board_0[this->w_bishop ]  = WHITE | BISHOP;
    this->board_0[this->w_pawn_ws]  = WHITE | PAWN;
    this->board_0[this->w_pawn_bs]  = WHITE | PAWN;

    rotate_board(this->board_0, this->board_90, cw_90);
    rotate_board(this->board_0, this->board_180, cw_180);
    rotate_board(this->board_0, this->board_270, acw_90);
}

// Optimization: generate inverse king moves
// For now, just generate moves of the opposite color and check if any of them
// attack the king square
bool Board::in_check() const {

    auto pseudolegal_moves = _get_pseudolegal_moves_for_side(this->player_to_play ^ (WHITE | BLACK));
    auto king_pos = this->w_king;
    // can make this branchless for kicks but won't add much performance
    if (this->player_to_play == BLACK) {
        king_pos = this->b_king;
    }

    for (auto move : pseudolegal_moves) {
        if (getp1(move) == king_pos) {
            std::cout << "move " << move_to_str(move) << " causes check.\n";
            return true;
        }
    }

    return false;
}

std::unordered_set<U16> Board::get_pseudolegal_moves() const {
    return _get_pseudolegal_moves_for_side(this->player_to_play);
}

std::unordered_set<U16> Board::_get_pseudolegal_moves_for_side(U8 color) const {

    std::unordered_set<U16> pseudolegal_moves;

    U8 *pieces = nullptr;

    if (color == WHITE) {
        pieces = ((U8*)this) + 6;
    }
    else {
        pieces = (U8*)this;
    }

    for (int i=0; i<6; i++) {
        if (pieces[i] == DEAD) continue;
        auto piece_moves = this->get_pseudolegal_moves_for_piece(pieces[i]);
        pseudolegal_moves.insert(piece_moves.begin(), piece_moves.end());
    }

    return pseudolegal_moves;

}

Board* Board::copy() const {

    Board *b = new Board();
    memcpy(b, this, sizeof(Board));

    return b;
}

// legal move generation:
// if the king is under check: (Possible optimization, as this should be faster)
//     Look for moves that protect the king from check
// else:
//     Get all pseudolegal moves
//     for each pseudolegal move for our color:
//         if doing this move will leave the king in threat from opponent's pieces
//             don't add the move to legal moves
//         else
//             add to legal moves
//
// Only implement the else case for now
std::unordered_set<U16> Board::get_legal_moves() const {

    Board* c = this->copy();
    auto pseudolegal_moves = get_pseudolegal_moves();
    std::unordered_set<U16> legal_moves;

    for (auto move : pseudolegal_moves) {
        c->_do_move(move);

        std::cout << "Checking move " << move_to_str(move) << "\n";
        if (!c->in_check()) {
            legal_moves.insert(move);
        }
        else {
            std::cout << move_to_str(move) << " is illegal. Not Adding.\n";
        }

        c->_undo_move(move);
    }

    delete c;

    return legal_moves;
}

void Board::do_move(U16 move) {
    _do_move(move);
    _flip_player();
}

void Board::_flip_player() {
    this->player_to_play = (PlayerColor)(this->player_to_play ^ (WHITE | BLACK));
}

void Board::_do_move(U16 move) {

    U8 p0 = getp0(move);
    U8 p1 = getp1(move);
    U8 promo = getpromo(move);

    U8 piecetype = this->board_0[p0];
    this->last_killed_piece = this->board_0[p1];
    this->last_killed_piece_idx = -1;

    // scan and get piece from coord
    U8 *pieces = (U8*)this;
    for (int i=0; i<12; i++) {
        if (pieces[i] == p1) {
            pieces[i] = DEAD;
            this->last_killed_piece_idx = i;
        }
        if (pieces[i] == p0) {
            pieces[i] = p1;
        }
    }

    if (promo == PAWN_ROOK) {
        piecetype = (piecetype & (WHITE | BLACK)) | ROOK;
    }
    else if (promo == PAWN_BISHOP) {
        piecetype = (piecetype & (WHITE | BLACK)) | BISHOP;
    }

    this->board_0[p1]           = piecetype;
    this->board_90[cw_90[p1]]   = piecetype;
    this->board_180[cw_180[p1]] = piecetype;
    this->board_270[acw_90[p1]] = piecetype;

    this->board_0[p0]           = 0;
    this->board_90[cw_90[p0]]   = 0;
    this->board_180[cw_180[p0]] = 0;
    this->board_270[acw_90[p0]] = 0;

}

void Board::undo_move(U16 move) {
    _undo_move(move);
    _flip_player();
}

void Board::_undo_move(U16 move) {

    U8 p0 = getp0(move);
    U8 p1 = getp1(move);
    U8 promo = getpromo(move);

    U8 piecetype = this->board_0[p1];
    U8 deadpiece = this->last_killed_piece;
    this->last_killed_piece = 0;

    // scan and get piece from coord
    U8 *pieces = (U8*)this;
    if (this->last_killed_piece_idx >= 0) {
        pieces[this->last_killed_piece_idx] = p1;
        std::cout << "Set piece " << this->last_killed_piece_idx << "'s position to p1\n";
        this->last_killed_piece_idx = -1;
        // TODO check moves where killing the piece doesn't work
    }
    for (int i=0; i<12; i++) {
        if (pieces[i] == p1) {
            pieces[i] = p0;
            break;
        }
    }

    if (promo == PAWN_ROOK) {
        piecetype = (piecetype & (WHITE | BLACK)) | ROOK;
    }
    else if (promo == PAWN_BISHOP) {
        piecetype = (piecetype & (WHITE | BLACK)) | BISHOP;
    }

    this->board_0[p0]           = piecetype;
    this->board_90[cw_90[p0]]   = piecetype;
    this->board_180[cw_180[p0]] = piecetype;
    this->board_270[acw_90[p0]] = piecetype;

    this->board_0[p1]           = deadpiece;
    this->board_90[cw_90[p1]]   = deadpiece;
    this->board_180[cw_180[p1]] = deadpiece;
    this->board_270[acw_90[p1]] = deadpiece;
}
