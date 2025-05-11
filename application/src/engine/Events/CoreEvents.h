#pragma once
#include "pch.h"

#include "Event.h"

namespace CW {

    class WindowClosed
        : public Event
    {
    public:
        WindowClosed() = default;
        EventType GetEventType() const override { return EventType::Closed; }
        static EventType GetStaticEventType() { return EventType::Closed; }
    };

    class WindowResized
        : public Event
    {
    public:
        sf::Vector2u Size;

        WindowResized(const sf::Event::Resized& e)
            : Size(e.size)
        {
        }
        EventType GetEventType() const override { return EventType::Resized; }
        static EventType GetStaticEventType() { return EventType::Resized; }
    };

    class KeyPressed
        : public Event
    {
    public:
        sf::Event::KeyPressed Data;

        KeyPressed(const sf::Event::KeyPressed& e)
            : Data(e)
        {
        }
        EventType GetEventType() const override { return EventType::KeyPressed; }
        static EventType GetStaticEventType() { return EventType::KeyPressed; }
    };

    class KeyReleased
        : public Event
    {
    public:
        sf::Event::KeyReleased Data;

        KeyReleased(const sf::Event::KeyReleased& e)
            : Data(e)
        {
        }
        EventType GetEventType() const override { return EventType::KeyReleased; }
        static EventType GetStaticEventType() { return EventType::KeyReleased; }
    };

    class MouseWheelScrolled
        : public Event
    {
    public:
        sf::Event::MouseWheelScrolled Data;

        MouseWheelScrolled(const sf::Event::MouseWheelScrolled& e)
            : Data(e)
        {
        }
        EventType GetEventType() const override { return EventType::MouseWheelScrolled; }
        static EventType GetStaticEventType() { return EventType::MouseWheelScrolled; }
    };

    class MouseButtonPressed
        : public Event
    {
    public:
        sf::Event::MouseButtonPressed Data;

        MouseButtonPressed(const sf::Event::MouseButtonPressed& e)
            : Data(e)
        {
        }
        EventType GetEventType() const override { return EventType::MouseButtonPressed; }
        static EventType GetStaticEventType() { return EventType::MouseButtonPressed; }
    };

    class MouseButtonReleased
        : public Event
    {
    public:
        sf::Event::MouseButtonReleased Data;

        MouseButtonReleased(const sf::Event::MouseButtonReleased& e)
            : Data(e)
        {
        }
        EventType GetEventType() const override { return EventType::MouseButtonReleased; }
        static EventType GetStaticEventType() { return EventType::MouseButtonReleased; }
    };

    class MouseMoved
        : public Event
    {
    public:
        sf::Event::MouseMoved Data;

        MouseMoved(const sf::Event::MouseMoved& e)
            : Data(e)
        {
        }
        EventType GetEventType() const override { return EventType::MouseMoved; }
        static EventType GetStaticEventType() { return EventType::MouseMoved; }
    };

} // CW