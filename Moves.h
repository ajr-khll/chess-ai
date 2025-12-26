#pragma once
#include <SFML/System/Vector2.hpp>
#include <vector>

struct Move {
  sf::Vector2i from;
  sf::Vector2i to;
};

struct Undo {
  Move move;
  int movedPiece;
  int capturedPiece;
  sf::Vector2i kingPosBefore[2];
};

struct GameState {
  int board[8][8];
  int sideToMove;
  sf::Vector2i kingPos[2];
};

std::vector<Move> calculatePossibleMoves(int piece, sf::Vector2i position,
                                         GameState game);
bool isInCheck(GameState current, int color);

int colorOf(int piece);

GameState
makeMove(const GameState &current, const Move &move,
         const std::optional<std::vector<Move>> &moves = std::nullopt);
