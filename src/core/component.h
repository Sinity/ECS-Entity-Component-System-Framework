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
* Component definitions should be created with COMPONENT macro, otherwise ECS won't work properly.
*
* Example of component definition:
*
* COMPONENT(ComponentTypeName) {
*   int x = 1;
*   int y = 42;
*   char* buff = nullptr;
*
*   ~ComponentTypeName() {
*       delete buff;
*   }
* };
*
*/
struct Component {
    EntityID entityID;
};
}

// Defines new component type. It's necessary for proper working of the framework. */
#define COMPONENT(name)                                                    \
    struct name;                                                           \
    static EECS::ComponentRegistrator<name> componentRegistrator_##name{}; \
    struct name : public EECS::Component
