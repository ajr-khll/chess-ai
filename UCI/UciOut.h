#pragma once

#include "../Moves.h"

class UciOut {
public:
  void uciHandshake();
  void uciReady();
  void sendMove(Move bestmove);
};
