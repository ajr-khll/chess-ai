#include "Moves.h"
#include "TextureManager.h"
#include "minimax.h"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <atomic>
#include <cstddef>
#include <string>
#include <thread>
#include <vector>

std::string getTexturePath(int pieceId) {
  switch (pieceId) {
  case W_PAWN:
    return "sprites/white-pawn.png";
  case W_ROOK:
    return "sprites/white-rook.png";
  case W_KNIGHT:
    return "sprites/white-knight.png";
  case W_BISHOP:
    return "sprites/white-bishop.png";
  case W_QUEEN:
    return "sprites/white-queen.png";
  case W_KING:
    return "sprites/white-king.png";

  case B_PAWN:
    return "sprites/black-pawn.png";
  case B_ROOK:
    return "sprites/black-rook.png";
  case B_KNIGHT:
    return "sprites/black-knight.png";
  case B_BISHOP:
    return "sprites/black-bishop.png";
  case B_QUEEN:
    return "sprites/black-queen.png";
  case B_KING:
    return "sprites/black-king.png";
  default:
    return "";
  }
}

std::atomic<bool> running{true};

enum turns { WHITE = 0, BLACK = 1 };
enum modes { DEACTIVED = 0, MOVE = 1 };

std::vector<sf::Texture> TEXTUREMAP = {};
sf::Vector2i SELECTED = {-1, -1};

int mode = DEACTIVED;
std::vector<Move> moves;
sf::Vector2i whiteKingPos;
sf::Vector2i blackKingPos;

static inline bool inBounds(int x, int y) {
  return x >= 0 && x < 8 && y >= 0 && y < 8;
}

void drawTile(sf::RenderWindow *win, sf::Color col, sf::Vector2f pos) {
  sf::RectangleShape tile({150.0f, 150.0f});
  tile.setFillColor(col);
  tile.setPosition(pos);
  win->draw(tile);
}

void drawPlaceholder(sf::RenderWindow *win, sf::Color col, sf::Vector2f pos) {
  sf::CircleShape placeholder(20.0f);
  placeholder.setFillColor(col);
  placeholder.setPosition(pos);
  win->draw(placeholder);
}

sf::Vector2i selectPiece(sf::Vector2i localPosition, GameState &current) {
  sf::Vector2i coordinate = {(localPosition.x / 150), (localPosition.y / 150)};
  int BOARD[8][8];
  std::copy(&current.board[0][0], &current.board[0][0] + 64, &BOARD[0][0]);
  if (mode == DEACTIVED) {

    if (inBounds(coordinate.x, coordinate.y) &&
        BOARD[coordinate.y][coordinate.x] != EMPTY) {

      int clicked = BOARD[coordinate.y][coordinate.x];
      if (clicked != EMPTY && colorOf(clicked) == current.sideToMove) {
        mode = MOVE;
        SELECTED = coordinate;
        return coordinate;
      }

    } else {
      mode = DEACTIVED;
      return sf::Vector2i{-1, -1};
    }
  } else { // mode == MOVE
    Move attempted = {SELECTED, coordinate};
    GameState newState = makeMove(current, attempted, moves);

    bool same = std::equal(&newState.board[0][0], &newState.board[0][0] + 64,
                           &current.board[0][0]);

    if (!same) {
      mode = DEACTIVED;
      current = newState;
      return coordinate;
    }
    if (inBounds(coordinate.x, coordinate.y)) {
      int clicked = BOARD[coordinate.y][coordinate.x];
      if (clicked != EMPTY && colorOf(clicked) == current.sideToMove) {
        SELECTED = coordinate;
        return coordinate;
      }
    }
    mode = DEACTIVED;
    SELECTED = {-1, -1};
    return sf::Vector2i{-1, -1};
  }
  return sf::Vector2i{-1, -1};
};

void drawBoard(sf::RenderWindow *win, int (&PLACEHOLDER)[8][8]) {
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      if (((i + j) % 2) == 0) {
        if (SELECTED.x == j && SELECTED.y == i) {
          drawTile(win, sf::Color(130, 70, 190), {j * 150.0f, i * 150.0f});
        } else {
          drawTile(win, sf::Color(45, 30, 60), {j * 150.0f, i * 150.0f});
        }
      } else {
        if (SELECTED.x == j && SELECTED.y == i) {
          drawTile(win, sf::Color(225, 200, 255), {j * 150.0f, i * 150.0f});

        } else {
          drawTile(win, sf::Color(140, 120, 150), {j * 150.0f, i * 150.0f});
        }
      }
    }
  }
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      if (PLACEHOLDER[j][i] == 1) {
        drawPlaceholder(win, sf::Color::Cyan,
                        {(i * 150.0f) + 55.0f, (j * 150.0f) + 55.0f});
      }
    }
  }
}

void drawPiece(sf::RenderWindow *win, int piece, sf::Vector2f coordinate,
               TextureManager &texManager) {
  if (piece != 0) {
    sf::Texture *texture = texManager.getTexture(piece);
    texture->setSmooth(true);
    sf::Sprite sprite(*texture);
    sprite.setPosition(coordinate * 150.0f);
    sprite.setScale({1.2f, 1.2f});

    win->draw(sprite);
  }
}

void drawPieces(int board[][8], sf::RenderWindow *win,
                TextureManager &texManager) {
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      drawPiece(win, board[j][i], {(float)i, (float)j}, texManager);
    }
  }
}

void renderingThread(sf::RenderWindow *win, GameState *state,
                     int (&PLACEHOLDER)[8][8]) {
  bool set = win->setActive(true);

  TextureManager texManager;
  while (win->isOpen()) {
    // draw n stuff
    while (running) {
      win->clear();
      drawBoard(win, PLACEHOLDER);
      drawPieces(state->board, win, texManager);
      win->display();
    }
  };
};

int main() {
  sf::RenderWindow window(sf::VideoMode({1200, 1200}), "Chess");
  bool set = window.setActive(false);
  window.setPosition({0, 0});
  window.setFramerateLimit(60);
  GameState game;

  int board[8][8] = {
      {B_ROOK, B_KNIGHT, B_BISHOP, B_QUEEN, B_KING, B_BISHOP, B_KNIGHT,
       B_ROOK}, // Row 0 (Black Back Rank)
      {B_PAWN, B_PAWN, B_PAWN, B_PAWN, B_PAWN, B_PAWN, B_PAWN,
       B_PAWN}, // Row 1 (Black Pawns)
      {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}, // Row 2
      {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}, // Row 3
      {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}, // Row 4
      {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}, // Row 5
      {W_PAWN, W_PAWN, W_PAWN, W_PAWN, W_PAWN, W_PAWN, W_PAWN,
       W_PAWN}, // Row 6 (White Pawns)
      {W_ROOK, W_KNIGHT, W_BISHOP, W_QUEEN, W_KING, W_BISHOP, W_KNIGHT,
       W_ROOK} // Row 7 (White Back Rank)
  };

  std::copy(&board[0][0], &board[0][0] + 64,
            &game.board[0][0]); // assign start gamestate board using memory
                                // copy (what a wonderful programming language
                                // this is)
  game.sideToMove = WHITE;
  game.kingPos[WHITE] = {4, 7};
  game.kingPos[BLACK] = {4, 0};

  int placeholder[8][8] = {{0}};

  std::thread thread(&renderingThread, &window, &game, std::ref(placeholder));

  while (window.isOpen()) {
    // game loop
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
        running = false;
      }
      if (const auto *mouseEvent =
              event->getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseEvent->button == sf::Mouse::Button::Left) {
          std::fill(&placeholder[0][0],
                    &placeholder[0][0] + sizeof(placeholder) / sizeof(int), 0);

          sf::Vector2i mousePos(mouseEvent->position.x, mouseEvent->position.y);
          sf::Vector2i coordinate = selectPiece(mousePos, game);
          if (colorOf(game.board[coordinate.y][coordinate.x]) ==
              game.sideToMove) {
            moves = calculatePossibleMoves(
                game.board[coordinate.y][coordinate.x], coordinate, game);

            for (Move move : moves) {
              placeholder[move.to.y][move.to.x] = 1;
            }
          }
        }
      }
      if (const auto *keyboardEvent = event->getIf<sf::Event::KeyPressed>()) {
        if (keyboardEvent->code == sf::Keyboard::Key::Backslash) {
          if (game.sideToMove == BLACK) {
            // we run minimax for black
            evaluatedMove bestMove = Minimax(game, BLACK);
            std::cout << "Best Move for Black: " << bestMove.move.from.y << ", "
                      << bestMove.move.from.x << " -> " << bestMove.move.to.y
                      << ", " << bestMove.move.to.x << std::endl;
            game = makeMove(game, bestMove.move);
          }
        }
      }
    }
  };

  thread.join();
}
