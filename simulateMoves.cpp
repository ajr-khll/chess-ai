#include "Moves.h"
#include <SFML/System/Vector2.hpp>
#include <vector>

// function to simulate a game state. use this to remove illegal "check" moves
// and also for minimax

GameState simulateMove(const GameState &current, Move move) {
  return makeMove(current, move);
}
