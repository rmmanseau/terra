#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <SFML/Graphics.hpp>

class Background : public sf::Drawable
{
    sf::Texture texture;
    sf::Sprite sprite;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

public:
    Background();
    void init(int width, int height, const std::string& fileName, const sf::Color& color);

    void setSize(int w, int h);
};

#endif // BACKGROUND_H
