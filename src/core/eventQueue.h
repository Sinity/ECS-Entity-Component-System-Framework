#pragma once
#include "detail/singleQueue.h"

class EventQueue {
public:
	~EventQueue() {
		for(auto queue : eventQueues) {
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
		size_t eventID = EventID::value<EventType>();
		if(eventID >= eventQueues.size()) {
			InitializeQueuesTo(eventID);
		}

		SingleQueue<EventType>* queue = (SingleQueue<EventType>*)eventQueues[eventID];
		if(!queue) {
			queue = (SingleQueue<EventType>*)(eventQueues[eventID] = new SingleQueue<EventType>);
		}
		queue->push(std::move(event));
	}


	template<typename EventType, typename... Args>
	void emplace(Args&& ... args) {
		size_t eventID = EventID::value<EventType>();
		if(eventID >= eventQueues.size()) {
			InitializeQueuesTo(eventID);
		}

		SingleQueue<EventType>* queue = (SingleQueue<EventType>*)eventQueues[eventID];
		if(!queue) {
			queue = (SingleQueue<EventType>*)(eventQueues[eventID] = new SingleQueue<EventType>);
		}
		queue->emplace(std::forward<Args>(args)...);
	}


	template<typename EventType, typename RecieverType>
	void connect(RecieverType& reciever) {
		size_t eventID = EventID::value<EventType>();
		if(eventID >= eventQueues.size()) {
			InitializeQueuesTo(eventID);
		}

		SingleQueue<EventType>* queue = (SingleQueue<EventType>*)eventQueues[eventID];
		if(!queue) {
			queue = (SingleQueue<EventType>*)(eventQueues[eventID] = new SingleQueue<EventType>);
		}
		queue->connect(reciever);
	}


	template<typename EventType, typename RecieverType>
	void disconnect(RecieverType& reciever) {
		SingleQueue<EventType>* queue = (SingleQueue<EventType>*)eventQueues[EventID::value<EventType>()];
		queue->disconnect(reciever);
	}

private:
	std::vector<SingleQueueBase*> eventQueues;

	void InitializeQueuesTo(size_t index) {
		if(index == 0) {
			eventQueues.emplace_back(nullptr);
		}
		else {
			for(size_t i = eventQueues.size() - 1; i < index; i++) {
				eventQueues.emplace_back(nullptr);
			}
		}
	}

	class EventID {
	public:
		template<typename T>
		static size_t value() {
			static size_t id = counter++;
			return id;
		}

	private:
		static size_t counter;
	};
};
