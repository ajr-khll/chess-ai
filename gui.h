#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>

class messageComponent {
public:
  int draw(sf::RenderWindow &window);
  int show();
  int erase();
  int setSize(sf::Vector2f size);
  int setText(std::string text);
  int setFont(sf::Font font);
  int setBGColor(sf::Color BGColor);
  int setFontColor(sf::Color fontColor);

private:
  sf::Vector2f size;
  std::string text;
  sf::Font font;
  sf::Color BGColor;
  sf::Color fontColor;
  bool visible;
};
