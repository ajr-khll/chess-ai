#pragma once
#include "Moves.h"

struct evaluatedMove {
  Move move;
  int score = 0;
};

evaluatedMove Minimax(GameState state, int side,
                      Move lastMove = {{-1, -1}, {-1, -1}}, int depth = 0);
