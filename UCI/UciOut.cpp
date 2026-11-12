#include "UciOut.h"
#include <iostream>

void UciOut::sendMove(Move bestMove) {
  // convert move into correct notation
  std::string from = "";
  from += (char)(bestMove.from.x + 'a');
  from += (char)('8' - bestMove.from.y);

  std::string to = "";
  to += (char)(bestMove.to.x + 'a');
  to += (char)('8' - bestMove.to.x);

  std::cout << from << to << std::endl;
};

void UciOut::uciHandshake() {
  // respond with uciok
  std::cout << "uciok" << std::endl;
}

void UciOut::uciReady() {
  // respond with readyok
  std::cout << "uciReady" << std::endl;
}
