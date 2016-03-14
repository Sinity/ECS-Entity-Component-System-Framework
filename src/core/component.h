#pragma once
#include "entityID.h"
#include "componentRegistrator.h"

namespace EECS {
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
