#pragma once
#include <vector>
#include "FastDelegate.h"

class SingleEventQueueBase {
public:
    virtual void emit() = 0;

    virtual ~SingleEventQueueBase() {
    }

    virtual void clear() = 0;
};

template<typename EventType>
class SingleEventQueue : public SingleEventQueueBase {
public:
    void emit() override {
        for (auto& event : events) {
            for (auto& delegate : delegates) {
                delegate(event);
            }
        }
        events.clear();
    }

    void push(EventType&& event) {
        events.push_back(std::move(event));
    }

    template<typename... Args>
    void emplace(Args&& ... args) {
        events.emplace_back(std::forward<Args>(args)...);
    }

    template<typename ObjectType>
    void connect(ObjectType& obj) {
        fastdelegate::FastDelegate1<EventType&> delegate;
        delegate.bind(&obj, &ObjectType::receive);
        delegates.push_back(std::move(delegate));
    }

    template<typename ObjectType>
    void disconnect(ObjectType& obj) {
        fastdelegate::FastDelegate1<EventType&> delegate;  // Probable culprit for receiver destructor disconnecting crash
        delegate.bind(&obj, &ObjectType::receive);
        for (size_t i = 0; i < delegates.size(); i++) {
            if (delegates[i] == delegate) {
                delegates.erase(delegates.begin() + i);
                return;
            }
        }
    }

    void clear() override {
       events.clear();
       delegates.clear();
    }

private:
    std::vector<fastdelegate::FastDelegate1<EventType&>> delegates;
    std::vector<EventType> events;
};
