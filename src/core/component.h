#pragma once
#include <unordered_map>
#include "componentContainerID.h"
#include "globalDefs.h"
#include "entityID.h"

namespace EECS {
// Used for registering component type in the system.
// Allows for reflection stuff, like defining Entity archetypes from data.
template <typename T>
class ComponentRegistrator {
   public:
    ComponentRegistrator() {
        auto id = ComponentContainerID::get<T>();

        if (singleComponentContainerArchetypes().size() <= id) {
            singleComponentContainerArchetypes().resize(id + 1);
        }
        singleComponentContainerArchetypes()[id] = std::make_unique<ComponentContainer<T>>();
    }
};

/** \brief base Component type
*
* Each component have entityID member, which defines to what Entity given component belongs to.
*
* You can define any method, but it's meant as a structure of data, not a class.
*
* When you define component, you need to supply it's type in template argument of the Component(base class).
*
* Example of component definition:
*
* struct ComponentTypename : Component<ComponentTypename> {
*   int x = 1;
*   int y = 42;
*   char* buff = nullptr;
*
*   ~ComponentTypename() {
*       delete buff;
*   }
* };
*
*/
template <typename Derived>
struct Component {
    EntityID entityID;

   private:
    Component() { (void)componentRegistrator; }

    static ComponentRegistrator<Derived> componentRegistrator;

    friend Derived;
};

template <typename Derived>
ComponentRegistrator<Derived> Component<Derived>::componentRegistrator;
}
