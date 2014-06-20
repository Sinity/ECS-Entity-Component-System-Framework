#include "detail/singleQueue.h"
#include "event.h"

class EventQueue {
public:
    EventQueue() {
        InitializeQueues();
    }

	~EventQueue() {
		for (auto queue : eventQueues) {
			delete queue;
		}
	}

    void emit() {
        for(auto eventType : eventQueues) {
            if(eventType) {
                eventType->emit();
            }
        }
    }


    template<typename EventType>
    void push(EventType&& event) {
        SingleQueue<EventType>* queue = (SingleQueue<EventType>*)eventQueues[(size_t)EventType::type];
        if(!queue) {
            queue = (SingleQueue<EventType>*)(eventQueues[(size_t)EventType::type] = new SingleQueue<EventType>);
        }
        queue->push(std::move(event));
    }


    template<typename EventType, typename... Args>
    void emplace(Args&&... args) {
        SingleQueue<EventType>* queue = (SingleQueue<EventType>*)eventQueues[(size_t)EventType::type];
        if(!queue) {
            queue = (SingleQueue<EventType>*)(eventQueues[(size_t)EventType::type] = new SingleQueue<EventType>);
        }
        queue->emplace(std::forward<Args>(args)...);
    }


    template<typename EventType, typename RecieverType>
    void connect(RecieverType& reciever) {
        SingleQueue<EventType>* queue = (SingleQueue<EventType>*)eventQueues[(size_t)EventType::type];
        if(!queue) {
            queue = (SingleQueue<EventType>*)(eventQueues[(size_t)EventType::type] = new SingleQueue<EventType>);
        }
        queue->connect(reciever);
    }

    
    template<typename EventType, typename RecieverType>
    void disconnect(RecieverType& reciever) {
        SingleQueue<EventType>* queue = (SingleQueue<EventType>*)eventQueues[(size_t)EventType::type];
        queue->disconnect(reciever);
    }

private:
    std::vector<SingleQueueBase*> eventQueues;

    void InitializeQueues() {
        for(size_t i = 0; i < (size_t)EventType::AmountOfEvents; i++) {
            eventQueues.emplace_back(nullptr);
        }
    }
};
