#define BOOST_TEST_MODULE Events
#include <boost/test/unit_test.hpp>
#include "core/eventQueue.h"

struct SampleEvent {
    static const EventType type = EventType::SampleEvent;
    SampleEvent(int load) : load(load) { }
    int load;
};

struct AnotherEvent {
    static const EventType type = EventType::AnotherEvent;
    AnotherEvent(bool state) : state(state) { }
    bool state;
};

class SampleReciever {
public:
    void receive(const SampleEvent& event) {
        printf("Instance: %d Event handling... load: %d\n", myInstance, event.load);
        totalEventsReceived++;
        eventsReceived++;
    }

    void receive(const AnotherEvent& event) {
        printf("Instance: %d Another event! State: %d\n", myInstance, event.state);
        totalEventsReceived++;
        eventsReceived++;
    }

    const int myInstance = instanceNum++;
    int eventsReceived = 0;
    static int instanceNum;
    static int totalEventsReceived;
};
int SampleReciever::instanceNum = 0;
int SampleReciever::totalEventsReceived = 0;

BOOST_AUTO_TEST_CASE(single_queue_test) {
    SampleReciever::instanceNum = 0;
    SampleReciever::totalEventsReceived = 0;

    SingleQueue<SampleEvent> singleQueue;
    SampleReciever receiver;
    SampleReciever secondReceiver;

    singleQueue.connect(receiver);
    singleQueue.connect(secondReceiver);

    singleQueue.push({666});
    singleQueue.emplace(444);
    singleQueue.emit();
    
    BOOST_CHECK_EQUAL(receiver.eventsReceived, 2);
    BOOST_CHECK_EQUAL(secondReceiver.eventsReceived, 2);
    BOOST_CHECK_EQUAL(SampleReciever::totalEventsReceived, 4);

    singleQueue.emit(); //empty queue
    BOOST_CHECK_EQUAL(SampleReciever::totalEventsReceived, 4);

    singleQueue.disconnect(receiver);
    singleQueue.push({222});
    singleQueue.emit();
    BOOST_CHECK_EQUAL(receiver.eventsReceived, 2);
    BOOST_CHECK_EQUAL(secondReceiver.eventsReceived, 3);
    BOOST_CHECK_EQUAL(SampleReciever::totalEventsReceived, 5);
}

BOOST_AUTO_TEST_CASE(event_queue_test) {
    SampleReciever::instanceNum = 0;
    SampleReciever::totalEventsReceived = 0;

    EventQueue events;
    SampleReciever receiver;
    SampleReciever secondReceiver;
    
    //connect some receivers
    events.connect<SampleEvent>(receiver);
    events.connect<AnotherEvent>(secondReceiver);

    //insert some events
    events.emplace<SampleEvent>(666);
    events.push(AnotherEvent(true));
    events.push(SampleEvent(222));
    events.emit();
    BOOST_CHECK_EQUAL(receiver.eventsReceived, 2);
    BOOST_CHECK_EQUAL(secondReceiver.eventsReceived, 1);
    
    //empty queue
    events.emit();
    BOOST_CHECK_EQUAL(receiver.eventsReceived, 2);
    BOOST_CHECK_EQUAL(secondReceiver.eventsReceived, 1);

    //lack of receivers
    events.disconnect<AnotherEvent>(secondReceiver);
    events.emplace<AnotherEvent>(false);
    events.emit();
    BOOST_CHECK_EQUAL(SampleReciever::totalEventsReceived, 3);

    //single receiver to many events
    events.connect<AnotherEvent>(receiver);
    events.emplace<AnotherEvent>(true);
    events.emplace<SampleEvent>(42);
    events.emit();
    BOOST_CHECK_EQUAL(SampleReciever::totalEventsReceived, 5);
    BOOST_CHECK_EQUAL(receiver.eventsReceived, 4);
}
