#pragma once
#include <vector>
#include "FastDelegate.h"

namespace EECS {
class SingleEventQueueBase {
   public:
    virtual void emit() = 0;

    virtual ~SingleEventQueueBase() {}

    virtual std::unique_ptr<SingleEventQueueBase> getNewClassInstance() const = 0;

    virtual void clear() = 0;
};

template <typename EventType>
class SingleEventQueue : public SingleEventQueueBase {
    struct DelegateEntry {
        fastdelegate::FastDelegate1<EventType&, bool> delegate;
        int priority;
    };

   public:
    void emit() override {
        for (auto& event : events) {
            for (auto& delegate : delegates) {
                if (!delegate.delegate(event)) {
                    break;
                }
            }
        }
        events.clear();
    }

    void push(EventType&& event) { events.push_back(std::move(event)); }

    template <typename... Args>
    void emplace(Args&&... args) {
        events.emplace_back(std::forward<Args>(args)...);
    }

    template <typename ObjectType>
    void connect(ObjectType& obj, int priority) {
        auto place = std::lower_bound(delegates.begin(), delegates.end(), priority,
                                      [](const auto& delegate, int priority) { return delegate.priority < priority; });

        fastdelegate::FastDelegate1<EventType&, bool> delegate;
        delegate.bind(&obj, &ObjectType::receive);
        delegates.insert(place, {std::move(delegate), priority});
    }

    template <typename ObjectType>
    void disconnect(ObjectType& obj) {
        fastdelegate::FastDelegate1<EventType&, bool> delegate;
        delegate.bind(&obj, &ObjectType::receive);
        for (size_t i = 0; i < delegates.size(); i++) {
            if (delegates[i].delegate == delegate) {
                delegates.erase(delegates.begin() + i);
                return;
            }
        }
    }

    void clear() override {
        events.clear();
        delegates.clear();
    }

    // returns new object of the same class as *this*.
    std::unique_ptr<SingleEventQueueBase> getNewClassInstance() const override {
        return std::make_unique<SingleEventQueue<EventType>>();
    }

   private:
    std::vector<DelegateEntry> delegates;
    std::vector<EventType> events;
};
}
