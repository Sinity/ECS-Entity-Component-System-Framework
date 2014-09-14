/** \brief it's header to use when you want to define new Component. It has COMPONENT macro and Component definition. */
#pragma once
#include "componentDef.h"
#include "detail/componentFactory.h"

/** \brief defines new component type. It's normal definition + auto registering component type in ComponentFactory. */
#define COMPONENT(name) struct name ; \
    static ComponentFactoryRegistrator< name > componentFactoryRegistrator_ ## name (#name); \
    struct name : public Component
