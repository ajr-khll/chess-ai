#pragma once
#include "Moves.h"

struct evaluatedMove : Move {
  int score = 0;
};

evaluatedMove Minimax(GameState state, int side, int alpha = -1000000,
                      int beta = 10000000, Move lastMove = {{-1, -1}, {-1, -1}},
                      int depth = 0);
