/** \brief it's header to use when you want to define new Component.*/
#pragma once
#include "core/componentDef.h"
#include "core/detail/componentFactory.h"

/** \brief defines new component type. It's normal definition + auto registering component type in ComponentFactory. */
#define COMPONENT(name) struct name ; \
    static ComponentFactoryRegistrator< name > componentFactoryRegistrator_ ## name (#name); \
    struct name : public Component
