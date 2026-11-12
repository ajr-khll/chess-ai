#include "GameManager.h"
#include <thread>

void GameManager::startGameThread(GameState state) {
  std::thread calculateMove(Minimax(state, state.sideToMove), 1);
};
