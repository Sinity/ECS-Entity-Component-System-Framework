#pragma once
#include <unordered_map>
#include "eventQueue.h"
#include "globalDefs.h"

namespace EECS {
template <typename T>
class EventRegistrator {
   public:
    EventRegistrator() {
        auto id = EventQueue::EventID::value<T>();

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
