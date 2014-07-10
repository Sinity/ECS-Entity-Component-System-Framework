#include "debug.h"
#include "tool/loggerConsoleOutput.h"
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

    if(engine.config.get("tasks.debugTask.log") == "true") {
		std::shared_ptr<ConsoleOutput> cOut = std::make_shared<ConsoleOutput>();
		cOut->setMinPriority(LogType::Information);
		debugLogger.addOutput(std::move(cOut));
    }
}

void DebugTask::update() {
}

void DebugTask::receive(const UnknownSFMLEvent& unknownSFMLEvent) {
    debugLogger.info("Unknown sfml event{type=", (unsigned int)unknownSFMLEvent.event.type, "}");
}

void DebugTask::receive(const ApplicationClosedEvent& appClosed) {
	(void)appClosed;
    debugLogger.info("Application closed event received");
}

void DebugTask::receive(const KeyPressed& keyPressed) {
    debugLogger.info("Pressed key '", (unsigned int)keyPressed.key.code, "'{alt=", keyPressed.key.alt,
		",control=", keyPressed.key.control, ",shift=", keyPressed.key.shift, ",system=", keyPressed.key.system, "}");
}

void DebugTask::receive(const KeyReleased& keyReleased) {
	debugLogger.info("Released key '", (unsigned int)keyReleased.key.code, "'{alt=", keyReleased.key.alt,
		",control=", keyReleased.key.control, ",shift=", keyReleased.key.shift, ",system=", keyReleased.key.system, "}");
}

void DebugTask::receive(const TextEntered& textEntered) {
    debugLogger.info("Entered text{unicode='", (unsigned int)textEntered.text.unicode,
		"',asAscii='", (char)textEntered.text.unicode, "'}");
}

void DebugTask::receive(const MouseButtonPressed& mouseButtonPressed) {
	debugLogger.info("Pressed mouse button{code='", (unsigned int)mouseButtonPressed.button.button,
		"',position=<", mouseButtonPressed.button.x, ", ", mouseButtonPressed.button.x, ">}");
}

void DebugTask::receive(const MouseButtonReleased& mouseButtonReleased) {
	debugLogger.info("Released mouse button{code='", (unsigned int)mouseButtonReleased.button.button,
		"',position=<", mouseButtonReleased.button.x, ", ", mouseButtonReleased.button.x, ">}");
}

void DebugTask::receive(const MouseMoved& mouseMoved) {                                                       
	debugLogger.info("Mouse location{", mouseMoved.where.x, ", ", mouseMoved.where.y, "}");
}

