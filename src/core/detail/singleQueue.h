#include <vector>
#include "FastDelegate.h"

class SingleQueueBase {
public:
    virtual void emit() = 0;
    virtual ~SingleQueueBase() { }
};

template<typename EventType>
class SingleQueue : public SingleQueueBase {
public:
    void emit() override {
        for(auto& event : events)
            for(auto& delegate : delegates)
                delegate(event);
        events.clear();
    }

    void push(EventType&& event) {
        events.push_back(std::move(event));
    }

    template<typename... Args>
    void emplace(Args&&... args) {
        events.emplace_back(std::forward<Args...>(args...));
    }

	void emplace() {
		events.emplace_back();
	}

    template<typename ObjectType>
    void connect(ObjectType& obj) {
        fastdelegate::FastDelegate1<const EventType&> delegate;
        delegate.bind(&obj, &ObjectType::receive);
        delegates.push_back(std::move(delegate));
    }

    template<typename ObjectType>
    void disconnect(ObjectType& obj) { //TODO: check if creating identical delegate is best way to finding desired delegate, maybe there is better approach.
        fastdelegate::FastDelegate1<const EventType&> delegate;
        delegate.bind(&obj, &ObjectType::receive);
        for(size_t i = 0; i < delegates.size(); i++) {
            if(delegate == delegates[i]) {
                delegates.erase(delegates.begin() + i);
                return;
            }
        }
    }

private:
    std::vector<fastdelegate::FastDelegate1<const EventType&>> delegates;
    std::vector<EventType> events;
};
