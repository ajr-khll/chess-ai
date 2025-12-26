#include "Moves.h"
#include "TextureManager.h"
#include "simulateMoves.h"
#include <cstddef>

// Array for values of pieces
// 0: Empty
// 1-6:  W_PAWN, W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING
// 7-12: B_PAWN, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING

enum colors { WHITE = 0, BLACK = 1 };

static const int PIECE_VALUES[13] = {
    0,     // EMPTY
    100,   // W_PAWN
    320,   // W_KNIGHT
    330,   // W_BISHOP
    500,   // W_ROOK
    900,   // W_QUEEN
    20000, // W_KING (Arbitrarily high so the engine never sacrifices it)
    100,   // B_PAWN
    320,   // B_KNIGHT
    330,   // B_BISHOP
    500,   // B_ROOK
    900,   // B_QUEEN
    20000  // B_KING
};

// --------------------------------------------------------------------------
// PIECE-SQUARE TABLES (White's Perspective)
// --------------------------------------------------------------------------
// These are flat arrays of 64 integers.
// Access them using: table[y * 8 + x]

// PAWNS: Encourage moving forward and controlling the center (d4/e4).
static const int mvv_luv[64] = {
    0,   0,   0,   0,
    0,   0,   0,   0, // Rank 8 (Promoted - usually irrelevant here)
    50,  50,  50,  50,
    50,  50,  50,  50, // Rank 7 (Almost promoted!)
    10,  10,  20,  30,
    30,  20,  10,  10, // Rank 6
    5,   5,   10,  25,
    25,  10,  5,   5, // Rank 5
    0,   0,   0,   20,
    20,  0,   0,   0, // Rank 4
    5,   -5,  -10, 0,
    0,   -10, -5,  5, // Rank 3
    5,   10,  10,  -20,
    -20, 10,  10,  5, // Rank 2 (Don't move f/g/h pawns too early)
    0,   0,   0,   0,
    0,   0,   0,   0 // Rank 1 (Base)
};

// KNIGHTS: Strong in the center, terrible at the edges/corners.
static const int knight_pst[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50, -40, -20, 0,   0,   0,
    0,   -20, -40, -30, 0,   10,  15,  15,  10,  0,   -30, -30, 5,
    15,  20,  20,  15,  5,   -30, -30, 0,   15,  20,  20,  15,  0,
    -30, -30, 5,   10,  15,  15,  10,  5,   -30, -40, -20, 0,   5,
    5,   0,   -20, -40, -50, -40, -30, -30, -30, -30, -40, -50};

// BISHOPS: Good on long diagonals, better in center than corners.
static const int bishop_pst[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20, -10, 0,   0,   0,   0,
    0,   0,   -10, -10, 0,   5,   10,  10,  5,   0,   -10, -10, 5,
    5,   10,  10,  5,   5,   -10, -10, 0,   10,  10,  10,  10,  0,
    -10, -10, 10,  10,  10,  10,  10,  10,  -10, -10, 5,   0,   0,
    0,   0,   5,   -10, -20, -10, -10, -10, -10, -10, -10, -20};

// ROOKS: Bonus for 7th rank (attacking enemy pawns) and centering.
static const int rook_pst[64] = {
    0, 0, 0, 0, 0, 0, 0, 0, 5, 10, 10, 10, 10, 10, 10, 5, // 7th Rank (Pig
                                                          // on the 7th)
    -5, 0, 0, 0, 0, 0, 0, -5, -5, 0, 0, 0, 0, 0, 0, -5, -5, 0, 0, 0, 0, 0, 0,
    -5, -5, 0, 0, 0, 0, 0, 0, -5, -5, 0, 0, 0, 0, 0, 0, -5, 0, 0, 0, 5, 5, 0, 0,
    0 // Castling squares
      // slightly better
};

// QUEENS: Generally kept simple. Avoid corners, stay somewhat central.
static const int queen_pst[64] = {
    -20, -10, -10, -5, -5, -10, -10, -20, -10, 0,   0,   0,  0,  0,   0,   -10,
    -10, 0,   5,   5,  5,  5,   0,   -10, -5,  0,   5,   5,  5,  5,   0,   -5,
    0,   0,   5,   5,  5,  5,   0,   -5,  -10, 5,   5,   5,  5,  5,   0,   -10,
    -10, 0,   5,   0,  0,  0,   0,   -10, -20, -10, -10, -5, -5, -10, -10, -20};

// KINGS (MIDDLE GAME): Highly penalized for being in the center or open files.
// Encourages castling into the corners (g1/b1).
static const int king_pst[64] = {
    -30, -40, -40, -50, -50, -40, -40, -30, -30, -40, -40, -50, -50, -40, -40,
    -30, -30, -40, -40, -50, -50, -40, -40, -30, -30, -40, -40, -50, -50, -40,
    -40, -30, -20, -30, -30, -40, -40, -30, -30, -20, -10, -20, -20, -20, -20,
    -20, -20, -10, 20,  20,  0,   0,   0,   0,   20,  20, // Pawn shield rank
    20,  30,  10,  0,   0,   10,  30,  20 // Back rank (Corners are safest)
};

int positionalScore(int piece, int x, int y) {
  if (piece == EMPTY)
    return 0;

  const int *table = nullptr;
  if (piece == W_PAWN || piece == B_PAWN)
    table = mvv_luv;
  else if (piece == W_KNIGHT || piece == B_KNIGHT)
    table = knight_pst;
  else if (piece == W_BISHOP || piece == B_BISHOP)
    table = bishop_pst;
  else if (piece == W_ROOK || piece == B_ROOK)
    table = rook_pst;
  else if (piece == W_QUEEN || piece == B_QUEEN)
    table = queen_pst;
  else if (piece == W_KING || piece == B_KING)
    table = king_pst;

  if (!table)
    return 0;

  if (colorOf(piece) == WHITE) {
    return table[y * 8 + x];
  } else {
    return table[(7 - y) * 8 + x];
  }
}

int evaluateScore(GameState state, int color) {
  // Given a game state, return an integer value for the score of the game
  int score = 0;
  for (int y = 0; y < 8; ++y) {
    for (int x = 0; x < 8; ++x) {
      int piece = state.board[y][x];
      if (colorOf(piece) == color) {
        score += PIECE_VALUES[piece];
        score += positionalScore(piece, x, y);

      } else {
        score -= PIECE_VALUES[state.board[y][x]];
        score -= positionalScore(piece, x, y);
      }
    }
  }
  return score;
}

// Minimax algorithm, recursive
const int CAP = 5; // think no more than 5 moves ahead
int counter = 0;

evaluatedMove Minimax(GameState state, int side, Move lastMove, int depth) {
  // base case
  if (depth == CAP) {
    return {lastMove, evaluateScore(state, side)};
  }
  evaluatedMove bestMove;
  bestMove.move = {{-1, -1}, {-1, -1}};
  if (side == BLACK)
    bestMove.score = -999999;
  else
    bestMove.score = 999999;
  // recursive case. keep growing tree until depth == 4
  // first, get every possible move
  std::vector<Move> moves;
  for (int y = 0; y < 8; ++y) {
    for (int x = 0; x < 8; ++x) {
      int piece = state.board[y][x];
      if (colorOf(piece) == side) {
        std::vector<Move> pieceMoves =
            calculatePossibleMoves(piece, {x, y}, state);
        moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
      }
    }
  }
  if (moves.empty()) {
    if (isInCheck(state, side)) {
      // If White is mated, return a very low number (but slightly higher if it
      // takes longer) If Black is mated, return a very high number (but
      // slightly lower if it takes longer)
      return {{{-1, -1}, {-1, -1}},
              (side == WHITE) ? -1000000 - depth : 1000000 + depth};
    } else {
      return {{{-1, -1}, {-1, -1}}, 0};
    }
  }
  // Now moves is a list of all possible moves we can make in our current
  // state.
  int enemy = !side;
  for (Move move : moves) {
    evaluatedMove result =
        Minimax(simulateMove(state, move), enemy, move, depth + 1);
    if (side == BLACK) {
      if (result.score > bestMove.score) {
        bestMove.score = result.score;
        bestMove.move = move;
      }
    } else {
      if (result.score < bestMove.score) {
        bestMove.score = result.score;
        bestMove.move = move;
      }
    }
  }

  return bestMove;
}
