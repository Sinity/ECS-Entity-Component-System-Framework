#pragma once
#include <memory>
#include "singleEventQueue.h"

/** \brief stores pending messages of arbitrary amount of numbers
*
* It's useful class to communicate between various places of code without coupling. Supports basic operations
* to add new messages to queue, and receivers to these messages. Can emit all messages currently in queue on demand.
*
* Events are emitted to connected receivers in order of connecting. So in this example:
* struct SomeEvent { SomeEvent(int a) : a(a){} int a; };
* eventQueue.emplace<SomeEvent>(666)
* eventQueue.connect<SomeEvent>(r1);
* eventQueue.connect<SomeEvent>(r2);
*
* r1's receive method(SomeEvent& event) : {
*   event.a = 1;
* }
*
* r2 will receive SomeEvent with field a set to 1, not 666.
*
*/
class EventQueue {
public:
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
    template<typename EventType>
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
    template<typename EventType, typename... Args>
    void emplace(Args&& ... args) {
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
    template<typename EventType, typename RecieverType>
    void connect(RecieverType& reciever) {
        getQueue<EventType>()->connect(reciever);
    }

    /** \brief disconnect receiver from particular event type
    *
    * \param receiver object that will be disconnected of EventType events.
    *
    * Receiver still can receive other events.
    */
    template<typename EventType, typename RecieverType>
    void disconnect(RecieverType& reciever) {
        getQueue<EventType>()->disconnect(reciever);
    }

private:
    std::vector<std::unique_ptr<SingleEventQueueBase>> eventQueues;

    template<typename EventType>
    SingleEventQueue<EventType>* getQueue() {
        size_t eventID = EventID::value<EventType>();

        if(eventQueues.size() <= eventID) {
            eventQueues.resize(eventID + 1);
        }
        
        if (!eventQueues[eventID]) {
            eventQueues[eventID] = std::make_unique<SingleEventQueue<EventType>>();
        }

        return (SingleEventQueue<EventType>*)eventQueues[eventID].get();
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
