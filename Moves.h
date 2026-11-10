#pragma once
#include <SFML/System/Vector2.hpp>
#include <optional>
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

std::vector<Move> gatherMoves(GameState game, int side);

bool isInCheck(GameState current, int color);

bool isCheckmated(GameState state, int side);

int colorOf(int piece);

GameState
makeMove(const GameState &current, const Move &move,
         const std::optional<std::vector<Move>> &moves = std::nullopt);
