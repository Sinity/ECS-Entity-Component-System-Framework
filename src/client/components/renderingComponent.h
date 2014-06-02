#pragma once
#include "core/component.h"
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <memory>

struct RenderingComponent : public Component {
    static const ComponentType type = ComponentType::RenderingComponent;

    RenderingComponent(Entity owner, ComponentHandle handle, int plane) :
            Component(owner, handle), plane(plane){}

    int plane; //renderer draws first graphics components which have highest plane - so plane INT_MIN will be always drawn last, so nothing will overlap it. 
    std::vector<sf::Drawable*> drawablesList; //Drawables from this list will be drawn in undefined order.
};
