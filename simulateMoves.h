#pragma once
#include "Moves.h"
#include <SFML/System/Vector2.hpp>

GameState simulateMove(const GameState &current, Move move);

struct evaluatedMove {
  Move move;
  int score = 0;
};
