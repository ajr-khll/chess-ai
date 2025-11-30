#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <map>
#include <string>

enum PieceIDs {
  EMPTY = 0,
  W_PAWN = 1,
  W_ROOK = 2,
  W_KNIGHT = 3,
  W_BISHOP = 4,
  W_QUEEN = 5,
  W_KING = 6,
  B_PAWN = 7,
  B_ROOK = 8,
  B_KNIGHT = 9,
  B_BISHOP = 10,
  B_QUEEN = 11,
  B_KING = 12
};

class TextureManager {
private:
  std::map<int, sf::Texture> textures;

public:
  TextureManager() {
    loadTexture(W_PAWN, "sprites/white-pawn.png");
    loadTexture(W_ROOK, "sprites/white-rook.png");
    loadTexture(W_KNIGHT, "sprites/white-knight.png");
    loadTexture(W_BISHOP, "sprites/white-bishop.png");
    loadTexture(W_QUEEN, "sprites/white-queen.png");
    loadTexture(W_KING, "sprites/white-king.png");

    loadTexture(B_PAWN, "sprites/black-pawn.png");
    loadTexture(B_ROOK, "sprites/black-rook.png");
    loadTexture(B_KNIGHT, "sprites/black-knight.png");
    loadTexture(B_BISHOP, "sprites/black-bishop.png");
    loadTexture(B_QUEEN, "sprites/black-queen.png");
    loadTexture(B_KING, "sprites/black-king.png");
  }

  void loadTexture(int id, const std::string &filename) {
    sf::Texture tex;
    if (!tex.loadFromFile(filename)) {
      std::cerr << "ERROR: Failed to load " << filename << std::endl;
    } else {
      tex.setSmooth(true);
      textures[id] = tex;
    }
  }

  sf::Texture *getTexture(int id) {
    if (textures.find(id) != textures.end()) {
      return &textures[id];
    }
    return nullptr;
  }
};
