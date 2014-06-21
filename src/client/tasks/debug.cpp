#include "debug.h"
#include "events/system/applicationClosed.h"
#include "events/system/keyPressed.h"
#include "events/system/keyReleased.h"
#include "events/system/mouseButtonPressed.h"
#include "events/system/mouseButtonReleased.h"
#include "events/system/mouseMoved.h"
#include "events/system/textEntered.h"
#include "events/system/unknownSFMLEvent.h"

DebugTask::DebugTask(Engine& engine) : 
    Task(engine), debugLogger("DEBUG") {

    engine.events.connect<UnknownSFMLEvent>(*this);
    engine.events.connect<ApplicationClosedEvent>(*this);
    engine.events.connect<KeyPressed>(*this);
    engine.events.connect<KeyReleased>(*this);
    engine.events.connect<TextEntered>(*this);
    engine.events.connect<MouseButtonPressed>(*this);
    engine.events.connect<MouseButtonReleased>(*this);
    engine.events.connect<MouseMoved>(*this);

    if(engine.config.get("tasks.debugTask.log") == "true") { //TODO: set output here.
    }
}

void DebugTask::update() {
}

void DebugTask::receive(const UnknownSFMLEvent& unknownSFMLEvent) {
    debugLogger.info("Unknown sfml event{type=%u}", unknownSFMLEvent.event.type);
}

void DebugTask::receive(const ApplicationClosedEvent& appClosed) {
    debugLogger.info("Application closed event received");
}

void DebugTask::receive(const KeyPressed& keyPressed) {
    debugLogger.info("Pressed key '%u'{alt=%d,control=%d,shift=%d,system=%d}", keyPressed.key.code, keyPressed.key.alt, keyPressed.key.control, keyPressed.key.shift, keyPressed.key.system);
}

void DebugTask::receive(const KeyReleased& keyReleased) {
    debugLogger.info("Released key '%u'{alt=%d,control=%d,shift=%d,system=%d}", keyReleased.key.code, keyReleased.key.alt, keyReleased.key.control, keyReleased.key.shift, keyReleased.key.system);
}

void DebugTask::receive(const TextEntered& textEntered) {
    debugLogger.info("Entered text{unicode='%u',asAscii='%c'}", textEntered.text.unicode, textEntered.text.unicode);
}

void DebugTask::receive(const MouseButtonPressed& mouseButtonPressed) {
    debugLogger.info("Pressed mouse button{code='%u',position=<%u, %u>}", mouseButtonPressed.button.button, mouseButtonPressed.button.x, mouseButtonPressed.button.y);
}

void DebugTask::receive(const MouseButtonReleased& mouseButtonReleased) {
    debugLogger.info("Released mouse button{code='%u',position=<%u, %u>}", mouseButtonReleased.button.button, mouseButtonReleased.button.x, mouseButtonReleased.button.y);
}

void DebugTask::receive(const MouseMoved& mouseMoved) {                                                       
    debugLogger.info("Mouse location{%u, %u}", mouseMoved.where.x, mouseMoved.where.y);
}

