#include "Moves.h"
#include "TextureManager.h"

#include <array>

enum colors { WHITE = 0, BLACK = 1 };

static inline bool inBounds(int x, int y) {
  return x >= 0 && x < 8 && y >= 0 && y < 8;
}

static inline int colorOf(int piece) {
  if (piece == EMPTY)
    return -1;
  return (piece - 1) / 6;
}

static inline bool isEnemy(int piece, int other) {
  if (other == EMPTY)
    return false;
  return colorOf(piece) != colorOf(other);
}

static inline bool isFriend(int piece, int other) {
  if (other == EMPTY)
    return false;
  return colorOf(piece) == colorOf(other);
}

static void addSlidingMoves(std::vector<sf::Vector2i> &out, int piece,
                            sf::Vector2i pos, int (&BOARD)[8][8],
                            const std::vector<sf::Vector2i> &directions) {
  for (auto d : directions) {
    int x = pos.x + d.x;
    int y = pos.y + d.y;

    while (inBounds(x, y)) {
      int cell = BOARD[y][x];

      if (cell == EMPTY) {
        out.push_back({x, y});
      } else {
        if (isEnemy(piece, cell))
          out.push_back({x, y});
        break; // blocked
      }

      x += d.x;
      y += d.y;
    }
  }
}

std::vector<sf::Vector2i>
calculatePossibleMoves(int piece, sf::Vector2i position, int (&BOARD)[8][8]) {
  std::vector<sf::Vector2i> moves;

  if (piece == EMPTY)
    return moves;
  if (!inBounds(position.x, position.y))
    return moves;

  const int myColor = colorOf(piece);
  const bool isWhite = (myColor == 0);
  const bool isBlack = (myColor == 1);

  // ---------------- Pawns ----------------
  if (piece == W_PAWN || piece == B_PAWN) {
    const int dir = isWhite ? -1 : +1;
    const int startRank = isWhite ? 6 : 1;

    const int x = position.x;
    const int y = position.y;

    // 1 step forward
    int y1 = y + dir;
    if (inBounds(x, y1) && BOARD[y1][x] == EMPTY) {
      moves.push_back({x, y1});

      // 2 steps forward
      int y2 = y + 2 * dir;
      if (y == startRank && inBounds(x, y2) && BOARD[y2][x] == EMPTY) {
        moves.push_back({x, y2});
      }
    }

    for (int dx : {-1, +1}) {
      int cx = x + dx;
      int cy = y + dir;
      if (inBounds(cx, cy) && isEnemy(piece, BOARD[cy][cx])) {
        moves.push_back({cx, cy});
      }
    }

    return moves;
  }

  if (piece == W_KNIGHT || piece == B_KNIGHT) {
    static const std::array<sf::Vector2i, 8> jumps = {{{+1, +2},
                                                       {+2, +1},
                                                       {+2, -1},
                                                       {+1, -2},
                                                       {-1, -2},
                                                       {-2, -1},
                                                       {-2, +1},
                                                       {-1, +2}}};

    for (auto j : jumps) {
      int nx = position.x + j.x;
      int ny = position.y + j.y;
      if (!inBounds(nx, ny))
        continue;

      int cell = BOARD[ny][nx];
      if (cell == EMPTY || isEnemy(piece, cell)) {
        moves.push_back({nx, ny});
      }
    }
    return moves;
  }

  if (piece == W_KING || piece == B_KING) {
    for (int dy = -1; dy <= 1; ++dy) {
      for (int dx = -1; dx <= 1; ++dx) {
        if (dx == 0 && dy == 0)
          continue;
        int nx = position.x + dx;
        int ny = position.y + dy;
        if (!inBounds(nx, ny))
          continue;

        int cell = BOARD[ny][nx];
        if (cell == EMPTY || isEnemy(piece, cell)) {
          moves.push_back({nx, ny});
        }
      }
    }
    return moves;
  }

  static const std::vector<sf::Vector2i> rookDirs = {
      {+1, 0}, {-1, 0}, {0, +1}, {0, -1}};
  static const std::vector<sf::Vector2i> bishopDirs = {
      {+1, +1}, {+1, -1}, {-1, +1}, {-1, -1}};

  if (piece == W_ROOK || piece == B_ROOK) {
    addSlidingMoves(moves, piece, position, BOARD, rookDirs);
    return moves;
  }

  if (piece == W_BISHOP || piece == B_BISHOP) {
    addSlidingMoves(moves, piece, position, BOARD, bishopDirs);
    return moves;
  }

  if (piece == W_QUEEN || piece == B_QUEEN) {
    std::vector<sf::Vector2i> queenDirs = rookDirs;
    queenDirs.insert(queenDirs.end(), bishopDirs.begin(), bishopDirs.end());
    addSlidingMoves(moves, piece, position, BOARD, queenDirs);
    return moves;
  }

  return moves;
}

bool isSquareAttacked(int (&BOARD)[8][8], sf::Vector2i target,
                      int attackerColor) {
  const int tx = target.x;
  const int ty = target.y;
  if (!inBounds(tx, ty))
    return false;
  // pawns!!
  if (attackerColor == WHITE) {
    int py = ty + 1;
    if (inBounds(tx - 1, py) && BOARD[py][tx - 1] == W_PAWN)
      return true;
    if (inBounds(tx + 1, py) && BOARD[py][tx + 1] == W_PAWN)
      return true;
  } else if (attackerColor == BLACK) {
    int py = ty - 1;
    if (inBounds(tx - 1, py) && BOARD[py][tx - 1] == B_PAWN)
      return true;
    if (inBounds(tx + 1, py) && BOARD[py][tx + 1] == B_PAWN)
      return true;
  }

  // knights!!
  static const std::array<sf::Vector2i, 8> knightD = {{{+1, +2},
                                                       {+2, +1},
                                                       {+2, -1},
                                                       {+1, -2},
                                                       {-1, -2},
                                                       {-2, -1},
                                                       {-2, +1},
                                                       {-1, +2}}};
  const int attackerKnight =
      (attackerColor == WHITE)
          ? W_KNIGHT
          : B_KNIGHT; // pick out which knight we care about
  for (auto d : knightD) {
    if (inBounds(tx + d.x, ty + d.y) &&
        BOARD[tx + d.x][ty + d.y] == attackerKnight)
      return true;
  }

  const int attackerKing = (attackerColor == WHITE) ? W_KING : B_KING;
  for (int dy = -1; dy <= 1; ++dy) {
    for (int dx = -1; dx <= 1; ++dx) {
      if (dx == 0 && dy == 0)
        continue;
      int x = tx + dx, y = ty + dy;
      if (inBounds(x, y) && BOARD[y][x] == attackerKing)
        return true;
    }
  }

  static const std::array<sf::Vector2i, 4> rookDirs = {
      {{+1, 0}, {-1, 0}, {0, +1}, {0, -1}}};
  static const std::array<sf::Vector2i, 4> bishopDirs = {
      {{+1, +1}, {+1, -1}, {-1, +1}, {-1, -1}}};

  const int attackerRook = (attackerColor == WHITE) ? W_ROOK : B_ROOK;
  const int attackerBish = (attackerColor == WHITE) ? W_BISHOP : B_BISHOP;
  const int attackerQueen = (attackerColor == WHITE) ? W_QUEEN : B_QUEEN;

  for (auto d : rookDirs) {
    int x = tx + d.x, y = ty + d.y;
    while (inBounds(x, y)) {
      int p = BOARD[y][x];
      if (p != EMPTY) {
        if (p == attackerRook || p == attackerQueen)
          return true;
        break; // blocked by something else
      }
      x += d.x;
      y += d.y;
    }
  }

  for (auto d : bishopDirs) {
    int x = tx + d.x, y = ty + d.y;
    while (inBounds(x, y)) {
      int p = BOARD[y][x];
      if (p != EMPTY) {
        if (p == attackerBish || p == attackerQueen)
          return true;
        break;
      }
      x += d.x;
      y += d.y;
    }
  }

  return false;
}

bool isInCheck(sf::Vector2i kingPos, int (&BOARD)[8][8], int myColor) {
  int enemyColor = myColor ^ 1;
  return isSquareAttacked(BOARD, kingPos, enemyColor);
}
