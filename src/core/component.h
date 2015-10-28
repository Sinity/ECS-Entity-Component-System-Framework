#pragma once
#include "entityID.h"
#include "componentRegistrator.h"

/** \brief base Component type
*
* Each component have entityID member, which defines to what Entity component belongs to. Entity is virtual construct,
* it is just number, really Entities don't exist. Entity is just sum of it's components.
*
* You can define any method, constructor, but it's meant as a structure of data, not a class.
*
* Component definitions should be created with COMPONENT macro, for benefits of introspection
*     (and otherwise component manager won't work properly).
* It looks like that, for example:
*
* COMPONENT(ComponentTypeName) {
*   int x;
*   int y = 42;
*   char* buff;
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


// defines new component type. It's normal definition + auto registering component type. */
#define COMPONENT(name)                                                   \
    struct name;                                                          \
    static ComponentRegistrator<name> componentRegistrator_##name(#name); \
    struct name : public Component
