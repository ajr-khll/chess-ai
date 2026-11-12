#pragma once

#include "../Moves.h"
#include "UciOut.h"
#include <string>
#include <vector>

class UciParser {
private:
  std::vector<std::string> commandParse(std::string line);
  GameState positionParse(std::vector<std::string> fenInput);

  UciOut output;

public:
  void poll();
};
