#include "gui.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>

int messageComponent::draw(sf::RenderWindow &window) {
  if (!this->visible)
    return 0;
  // draw a box with a border
  sf::RectangleShape menu(this->size);
  menu.setFillColor(this->BGColor);
  menu.setPosition({600 - (this->size.x / 2), 600 - (this->size.y / 2)});
  menu.setOutlineThickness(10.0f);
  menu.setOutlineColor(sf::Color(0, 0, 0));

  // write some text!
  sf::Text title(this->font);
  title.setString(text);
  title.setFillColor(this->fontColor);

  sf::FloatRect bounds = title.getLocalBounds();
  title.setOrigin(bounds.position + bounds.size / 2.f);
  title.setPosition({std::round(menu.getPosition().x + this->size.x / 2),
                     std::round(menu.getPosition().y + this->size.y / 2)});

  window.draw(menu);
  window.draw(title);
  return 1;
}

int messageComponent::show() {
  this->visible = true;
  return 1;
}

int messageComponent::erase() {
  this->visible = false;
  return 1;
};
int messageComponent::setSize(sf::Vector2f size) {
  this->size = size;
  return 1;
};
int messageComponent::setText(std::string text) {
  this->text = text;
  return 1;
};
int messageComponent::setFont(sf::Font font) {
  this->font = font;
  return 1;
};
int messageComponent::setBGColor(sf::Color BGColor) {
  this->BGColor = BGColor;
  return 1;
};
int messageComponent::setFontColor(sf::Color fontColor) {
  this->fontColor = fontColor;
  return 1;
};
