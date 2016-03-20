#pragma once
#include <type_traits>
#include "task.h"
#include "ecs.h"

namespace EECS {
template <class Derived, class HEvent, class... TEvents>
class Receives {
    EventQueue* events;

    Receives(EventQueue& events) : events(&events) { connect<HEvent, TEvents...>(); }

    // when defining Derived class, Receives must be *after* Task<T> on inheritance list if this constructor is used
    Receives() {
        static_assert(std::is_base_of<TaskBase, Derived>::value,
                      "For classes which are not Tasks you need to provide EventQueue in constructor");

        events = &((TaskBase*)this)->ecs.events;
        connect<HEvent, TEvents...>();
    }

    ~Receives() { disconnect<HEvent, TEvents...>(); }

    template <class H, class S, class... Tail>
    void connect() {
        events->connect<H>((Derived&)*this);
        connect<S, Tail...>();
    };

    template <class H, class S, class... T>
    void disconnect() {
        events->disconnect<H>((Derived&)*this);
        disconnect<S, T...>();
    };

    template <class Last>
    void connect() {
        events->connect<Last>((Derived&)*this);
    }

    template <class Last>
    void disconnect() {
        events->disconnect<Last>((Derived&)*this);
    }

    friend Derived;
};
}
