#pragma once
#include "core/task.h"
#include "core/engine.h"

struct UnknownSFMLEvent;
struct ApplicationClosedEvent;
struct KeyPressed;
struct KeyReleased;
struct TextEntered;
struct MouseButtonPressed;
struct MouseButtonReleased;
struct MouseMoved;
class DebugTask : public Task {
public:
    DebugTask(Engine& engine);
    void update() override;
    void receive(const UnknownSFMLEvent& unknownSFMLEvent);
    void receive(const ApplicationClosedEvent& appClosed);
    void receive(const KeyPressed& keyPressed);
    void receive(const KeyReleased& keyReleased);
    void receive(const TextEntered& textEntered);
    void receive(const MouseButtonPressed& mouseButtonPressed);
    void receive(const MouseButtonReleased& mouseButtonReleased);
    void receive(const MouseMoved& mouseMoved);                                                       

private:
    Logger debugLogger;
};
