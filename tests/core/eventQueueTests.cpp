#include <catch.hpp>
#include "ecs/ecs.h"
using namespace EECS;

struct AEvent {
    AEvent(int x) : x(x) {}

    int x;
};

struct BEvent {
    BEvent(int y) : y(y) {}

    int y;
};

struct Receiver {
    Receiver(EventQueue& ev) : events(ev) {}

    bool receive(AEvent& aEvent) {
        lastAEvent = aEvent.x;
        return true;
    }

    bool receive(BEvent& bEvent) {
        lastBEvent = bEvent.y;
        return true;
    }

    ~Receiver() {
        events.disconnect<AEvent>(*this);
        events.disconnect<BEvent>(*this);
    }

    EventQueue& events;
    int lastAEvent = -1;
    int lastBEvent = -1;
};

TEST_CASE("Empty queue, connected receiver", "[EventQueue]") {
    EventQueue events;

    Receiver receiver(events);
    events.connect<AEvent>(receiver);

    events.emit();

    REQUIRE(receiver.lastAEvent == -1);
}

TEST_CASE("Single event type, single event, single receiver", "[EventQueue]") {
    EventQueue events;

    Receiver receiver(events);
    events.connect<AEvent>(receiver);

    events.emplace<AEvent>(42);
    events.emit();

    REQUIRE(receiver.lastAEvent == 42);
}

TEST_CASE("Two event types, three receivers(all permutations of connections)") {
    EventQueue events;

    Receiver receiverA(events);
    events.connect<AEvent>(receiverA);

    Receiver receiverB(events);
    events.connect<BEvent>(receiverB);

    Receiver receiverAB(events);
    events.connect<AEvent>(receiverAB);
    events.connect<BEvent>(receiverAB);

    events.emplace<AEvent>(42);
    events.emplace<BEvent>(78);
    events.emit();

    REQUIRE(receiverA.lastAEvent == 42);
    REQUIRE(receiverA.lastBEvent == -1);

    REQUIRE(receiverB.lastAEvent == -1);
    REQUIRE(receiverB.lastBEvent == 78);

    REQUIRE(receiverAB.lastAEvent == 42);
    REQUIRE(receiverAB.lastBEvent == 78);
}

TEST_CASE("Disconnected receiver won't get an event") {
    EventQueue events;

    Receiver receiverA(events);
    Receiver receiverB(events);

    events.connect<AEvent>(receiverA);
    events.connect<AEvent>(receiverB);

    events.emplace<AEvent>(42);
    events.emit();

    REQUIRE(receiverA.lastAEvent == 42);

    events.disconnect<AEvent>(receiverA);
    events.emplace<AEvent>(24);
    events.emit();

    REQUIRE(receiverA.lastAEvent == 42);
    REQUIRE(receiverB.lastAEvent == 24);
}
