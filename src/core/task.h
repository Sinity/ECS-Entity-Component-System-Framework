#pragma once
#include <chrono>

namespace EECS {

    class ECS;

    /** \brief implements independient portion of code, that is executed with some frequency
    *
    *   Tasks are usually called Systems in Entity-Component-System frameworks. I think Task is better name.
    *
    *   It is intended to operate on some component type. Example of Task may be PhysicsIntegrator, which gets
    *   PhysicalBodyComponent and PositionComponent(by intersection, for example),
    *   then calculates new position(PositionComponent) and velocity(PhysicalBodyComponent).
    *
    *   Another example might be Renderer, which gets PositionComponent and SpriteComponent,
    *   and then displays sprite on screen at desired Position.
    *
    *   But Tasks are flexible, so you can use it to do any thing that should be done periodically.
    *
    *   By default, frequency will be once per game loop iteration(in config, task.defaultTaskFrequency).
    */
    class Task {
    public:
        Task(ECS& engine);

        /** \brief called at given frequency, derived class must implement it */
        virtual void update() = 0;

        virtual ~Task() {
        }

        std::chrono::milliseconds frequency;
        std::chrono::milliseconds accumulatedTime{0};

    protected:
        ECS& engine;
    };
}
