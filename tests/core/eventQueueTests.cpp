#include <catch.hpp>
#include "ecs/ecs.h"
using namespace EECS;

struct AEvent : Event<AEvent> {
    AEvent(int x) : x(x) {}

    int x;
};

struct BEvent : Event<BEvent> {
    BEvent(int y) : y(y) {}

    int y;
};

struct Receiver : Receives<Receiver, AEvent, BEvent> {
    Receiver(EventQueue& ev) : Receives(ev) {}

    bool receive(AEvent& aEvent) {
        lastAEvent = aEvent.x;
        return true;
    }

    bool receive(BEvent& bEvent) {
        lastBEvent = bEvent.y;
        return true;
    }

    int lastAEvent = -1;
    int lastBEvent = -1;
};

struct ExclusiveReceiver : Receives<ExclusiveReceiver, AEvent> {
    ExclusiveReceiver(EventQueue& ev) : Receives(ev) {}

    bool receive(AEvent& aEvent) {
        lastEvent = aEvent.x;
        return false;
    }

    int lastEvent = -1;
};

TEST_CASE("Empty queue, connected receiver", "[EventQueue]") {
    EventQueue events;
    Receiver receiver(events);

    events.emit();

    REQUIRE(receiver.lastAEvent == -1);
}

TEST_CASE("Single event type, single event, single receiver", "[EventQueue]") {
    EventQueue events;
    Receiver receiver(events);

    events.emplace<AEvent>(42);
    events.emit();

    REQUIRE(receiver.lastAEvent == 42);
}

TEST_CASE("Two event types, three receivers(all permutations of connections)") {
    EventQueue events;

    Receiver receiverA(events);
    events.disconnect<BEvent>(receiverA);

    Receiver receiverB(events);
    events.disconnect<AEvent>(receiverB);

    Receiver receiverAB(events);

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
    Receiver receiverX(events);
    Receiver receiverY(events);

    events.emplace<AEvent>(42);
    events.emit();

    REQUIRE(receiverX.lastAEvent == 42);

    events.disconnect<AEvent>(receiverX);
    events.emplace<AEvent>(24);
    events.emit();

    REQUIRE(receiverX.lastAEvent == 42);
    REQUIRE(receiverY.lastAEvent == 24);
}

TEST_CASE("Priority system works") {
    EventQueue events;
    ExclusiveReceiver aReceiver(events);
    ExclusiveReceiver bReceiver(events);

    events.setPriority<AEvent>(aReceiver, 0);
    events.setPriority<AEvent>(bReceiver, 1);

    events.emplace<AEvent>(6);
    events.emit();

    // only most prioritized receiver got the message
    REQUIRE(aReceiver.lastEvent == 6);
    REQUIRE(bReceiver.lastEvent != 6);

    // change priority of receivers
    events.setPriority<AEvent>(bReceiver, -1);
    events.emplace<AEvent>(3);
    events.emit();

    // this time other receiver got the message
    REQUIRE(aReceiver.lastEvent == 6);
    REQUIRE(bReceiver.lastEvent == 3);
}
