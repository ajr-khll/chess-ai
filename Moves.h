#pragma once
#include <SFML/System/Vector2.hpp>
#include <vector>

std::vector<sf::Vector2i>
calculatePossibleMoves(int piece, sf::Vector2i position, int (&BOARD)[8][8]);
bool isInCheck(sf::Vector2i kingPos, int (&BOARD)[8][8], int myColor);
