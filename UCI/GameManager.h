#include "../minimax.h"
#include "UciParser.h"

class GameManager {
public:
  void startGameThread(GameState state);

private:
  UciParser input;
  void uciGameLoop();
};
