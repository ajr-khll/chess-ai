#include "TextureManager.h"
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
#include <cstddef>
#include <iostream>
#include <iterator>
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

enum turns { WHITE = 0, BLACK = 1 };
enum modes { DEACTIVED = 0, MOVE = 1 };

std::vector<sf::Texture> TEXTUREMAP = {};
sf::Vector2i SELECTED = {-1, -1};
size_t turn = WHITE;
size_t mode = DEACTIVED;
std::vector<sf::Vector2i> moves;

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

bool makeMove(int (&BOARD)[8][8], std::vector<sf::Vector2i> moves,
              sf::Vector2i move) {
  auto it = std::find(moves.begin(), moves.end(), move);
  if (it != moves.end()) {
    int piece = BOARD[SELECTED.y][SELECTED.x];
    BOARD[SELECTED.y][SELECTED.x] = EMPTY;
    BOARD[move.y][move.x] = piece;
    turn &= 0; // flips the value. Thank you EE109 for teaching me this.
    return true;
  } else {
    return false;
  }
}

sf::Vector2i selectPiece(sf::Vector2i localPosition, int (&BOARD)[8][8]) {
  sf::Vector2i coordinate = {(localPosition.x / 150), (localPosition.y / 150)};
  if (mode == DEACTIVED) {
    if (BOARD[(int)coordinate.y][(int)coordinate.x]) {
      mode = MOVE;
      SELECTED = coordinate;
      return coordinate;
    } else {
      mode = DEACTIVED;
      return sf::Vector2i{-1, -1};
    }
  } else {
    mode = DEACTIVED;
    if (makeMove(BOARD, moves, coordinate)) {
      return coordinate;
    } else {
      return sf::Vector2i{-1, -1};
    }
  };
}

std::vector<sf::Vector2i> calculatePossibleMoves(int piece,
                                                 sf::Vector2i position,
                                                 int (&BOARD)[8][8],
                                                 sf::RenderWindow *window) {
  std::vector<sf::Vector2i> moves;
  if (piece == W_PAWN) {
    if (BOARD[position.y - 1][position.x] == EMPTY) {
      moves.push_back(sf::Vector2i{position.x, position.y - 1});
    }
    if (BOARD[position.y - 1][position.x - 1] != EMPTY) {
      moves.push_back(sf::Vector2i{position.x - 1, position.y - 1});
    }
    if (position.y == 6 && BOARD[position.y - 2][position.x] == EMPTY) {
      moves.push_back(sf::Vector2i{position.x, position.y - 2});
    }
  }
  if (piece == B_PAWN) {
    if (BOARD[position.y + 1][position.x] == EMPTY) {
      moves.push_back(sf::Vector2i{position.x, position.y + 1});
    }
  }

  return moves;
}

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

void renderingThread(sf::RenderWindow *win, int (&BOARD)[8][8],
                     int (&PLACEHOLDER)[8][8]) {
  bool set = win->setActive(true);

  TextureManager texManager;
  while (win->isOpen()) {
    // draw n stuff
    while (const std::optional event = win->pollEvent()) {
      if (event->is<sf::Event::Closed>())
        win->close();
    }
    win->clear();
    drawBoard(win, PLACEHOLDER);
    drawPieces(BOARD, win, texManager);
    win->display();
  };
};

int main() {
  sf::RenderWindow window(sf::VideoMode({1200, 1200}), "Chess");
  bool set = window.setActive(false);
  window.setPosition({0, 0});
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

  int placeholder[8][8] = {0};

  std::thread thread(&renderingThread, &window, std::ref(board),
                     std::ref(placeholder));

  while (window.isOpen()) {
    // game loop
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
      if (const auto *mouseEvent =
              event->getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseEvent->button == sf::Mouse::Button::Left) {
          std::fill(&placeholder[0][0],
                    &placeholder[0][0] + sizeof(placeholder) / sizeof(int), 0);

          sf::Vector2i mousePos(mouseEvent->position.x, mouseEvent->position.y);
          sf::Vector2i coordinate = selectPiece(mousePos, board);
          if (board[coordinate.y][coordinate.x] / 6 == turn) {
            moves = calculatePossibleMoves(board[coordinate.y][coordinate.x],
                                           coordinate, board, &window);

            for (sf::Vector2i move : moves) {
              placeholder[move.y][move.x] = 1;
            }
          }
        }
      }
    }
  };

  thread.join();
}
