#include "UciParser.h"
#include "../Moves.h"
#include "../TextureManager.h"
#include "../minimax.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <any>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

std::vector<std::string> UciParser::commandParse(std::string line) {
  std::string buffer;
  std::vector<std::string> commands;
  for (char c : line) {
    if (c == ' ') {
      // skip whitespace
      if (buffer.empty()) {
        continue;
      } else {
        commands.push_back(buffer);
        buffer.clear();
      }
    } else {
      buffer += c;
    }
  }

  return commands;
}
// fen parsing
GameState UciParser::positionParse(std::vector<std::string> fenInput) {
  int BOARD[8][8];
  GameState state{};

  // either fen or startpos
  if (fenInput[0] == "startpos") {
    startPos(state);
    return state;
  }

  // grab board file info

  std::string boardInfo = fenInput[1];
  std::vector<int> rankbuffer;
  int rank = 0;
  int file = 0;
  for (char c : boardInfo) {
    if (c == 'R') {
      state.board[file][rank] = W_ROOK;
      file++;
    } else if (c == 'Q') {
      state.board[file][rank] = W_QUEEN;
      file++;
    } else if (c == 'K') {
      state.board[file][rank] = W_KING;
      state.kingPos[0] = {file, rank};
    } else if (c == 'N') {
      state.board[file][rank] = W_KNIGHT;
      file++;
    } else if (c == 'B') {
      state.board[file][rank] = W_BISHOP;
      file++;
    } else if (c == 'P') {
      state.board[file][rank] = W_PAWN;
      file++;
    } else if (c == 'r') {
      state.board[file][rank] = B_ROOK;
      file++;
    } else if (c == 'q') {
      state.board[file][rank] = B_QUEEN;
      file++;
    } else if (c == 'k') {
      state.board[file][rank] = B_KING;
      state.kingPos[1] = {file, rank};
      file++;
    } else if (c == 'n') {
      state.board[file][rank] = B_KNIGHT;
      file++;
    } else if (c == 'b') {
      state.board[file][rank] = B_BISHOP;
      file++;
    } else if (c == 'p') {
      state.board[file][rank] = B_PAWN;
      file++;
    } else if (c == '/') {
      std::copy(rankbuffer.begin(), rankbuffer.end(), BOARD[rank]);
      rankbuffer.clear();
      rank++;
      file = 0;
    } else {
      // should be a number of empty spaces
      int empty = c - '0';
      for (int i = 0; i < empty; i++) {
        state.board[file][rank] = EMPTY;
        file++;
      };
    }
  }
  // next grab the turn
  int side;
  if (fenInput[2] == "w") {
    side = 0;
  } else {
    side = 1;
  }
  state.sideToMove = side;

  // next grab the castling
  std::string castling = fenInput[3];
  // TODO - implement castling logic

  // All of the following parameters are yet to be implemented in game logic.
  // Likely, the GameState struct needs to be expanded.
  std::string enPassantSquare = fenInput[4];

  std::string halfmove = fenInput[5];

  std::string fullmove = fenInput[6];

  return state;
}

// check for input
void UciParser::poll() {
  std::string input;
  std::getline(std::cin, input);
  std::vector<std::string> commands = commandParse(input);
  GameState state;

  if (commands[0] == "uci") {
    output.uciHandshake();
  }
  if (commands[0] == "isready") {
    output.uciReady();
  }
  if (commands[0] == "position") {
    GameState state = positionParse(
        std::vector<std::string>(std::next(commands.begin()), commands.end()));
  }
  if (commands[0] == "go") {
    evaluatedMove bestMove = Minimax(state, state.sideToMove);
    output.sendMove(bestMove);
  }
  if (commands[0] == "stop") {
  }
  if (commands[0] == "quit") {
    // quit progam somehow?
  }
}
