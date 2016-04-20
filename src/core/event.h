#pragma once
#include <unordered_map>
#include "globalDefs.h"

namespace EECS {
class EventID {
   public:
    template <typename T>
    static size_t value() {
        static size_t id = counter++;
        return id;
    }

   private:
    static size_t counter;
};

template <typename T>
class EventRegistrator {
   public:
    EventRegistrator() {
        auto id = EventID::value<T>();

        if (singleEventQueueArchetypes().size() <= id) {
            singleEventQueueArchetypes().resize(id + 1);
        }
        singleEventQueueArchetypes()[id] = std::make_unique<SingleEventQueue<T>>();
    }
};

template <typename Derived>
class Event {
    Event() { (void)eventRegistrator; }

    static EventRegistrator<Derived> eventRegistrator;
    friend Derived;
};

template <typename Derived>
EventRegistrator<Derived> Event<Derived>::eventRegistrator;
}
