#pragma once
#include <memory>
#include <algorithm>
#include <type_traits>
#include "singleEventQueue.h"
#include "globalDefs.h"
#include "event.h"

namespace EECS {
/** \brief stores pending messages of arbitrary amount of numbers
*
* It's useful class to communicate between various places of code without coupling. Supports basic operations
* to add new messages to queue, and receivers to these messages. Can emit all messages currently in queue on demand.
*
* receiver method returns true if event is to spread further into
* lower-priority receivers, or false if it should vanish.
*/
class EventQueue {
   public:
    EventQueue() {
        eventQueues.reserve(singleEventQueueArchetypes().size());
        for (const auto& queue : singleEventQueueArchetypes()) {
            eventQueues.emplace_back(queue->getNewClassInstance());
        }
    }

    /** \brief emits all events in system at once, type by type. */
    void emit() {
        for (auto& eventType : eventQueues) {
            if (eventType) {
                eventType->emit();
            }
        }
    }

    /** \brief add existing event object to queue
    *
    * \param event event to add
    *
    * Ownership to the event will be transfered to EventQueue.
    */
    template <typename EventType>
    void push(EventType&& event) {
        getQueue<EventType>()->push(std::move(event));
    }

    /** \brief creates new event in queue
    *
    * \param args arguments to be passed to event's constructor
    *
    * Creates new event in place.
    * Performance comparable to push, maybe a little bit faster.
    */
    template <typename EventType, typename... Args>
    void emplace(Args&&... args) {
        getQueue<EventType>()->emplace(std::forward<Args>(args)...);
    }

    /** \brief connect new receiver to particular event type
    *
    * \param receiver object that will receive events of EventType type
    *
    * Receiver can be any class. Only requirment is possessing receive(const EventType&) method.
    * Receiver will be called every time event of this type will be emited. Single class can receive arbitrary
    * amount of event types.
    */
    template <typename EventType, typename RecieverType>
    void connect(RecieverType& reciever, int priority = 0) {
        getQueue<EventType>()->connect(reciever, priority);
    }

    /** \brief disconnect receiver from particular event type
    *
    * \param receiver object that will be disconnected of EventType events.
    *
    * Receiver still can receive other events.
    */
    template <typename EventType, typename RecieverType>
    void disconnect(RecieverType& reciever) {
        getQueue<EventType>()->disconnect(reciever);
    }

    template <typename EventType, typename ReceiverType>
    void setPriority(ReceiverType& obj, int priority) {
        getQueue<EventType>()->disconnect(obj);
        getQueue<EventType>()->connect(obj, priority);
    }

    void clear() {
        for (auto& queue : eventQueues) {
            queue->clear();
        }
    }

   private:
    std::vector<std::unique_ptr<SingleEventQueueBase>> eventQueues;

    template <typename EventType>
    SingleEventQueue<EventType>* getQueue() {
        static_assert(std::is_base_of<Event<EventType>, EventType>::value, "Template parameter is not an event!");
        size_t eventID = EventID::value<EventType>();
        return (SingleEventQueue<EventType>*)eventQueues[eventID].get();
    }
};
}
