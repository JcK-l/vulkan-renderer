////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Event.h
/// \brief This file declares the Event struct, which encapsulates various types of events.
///
/// The Event struct is part of the vkf namespace. It provides a unified interface for handling different types of
/// events, such as keyboard input, mouse movement, mouse button input, mouse scroll, window resize, and window close
/// events.
///
/// \author Joshua Lowe
/// \date 11/5/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace vkf
{

///
/// \struct Event
/// \brief A struct that encapsulates various types of events.
///
/// This struct provides a unified interface for handling different types of events,
/// such as keyboard input, mouse movement, mouse button input, mouse scroll, window resize, and window close events.
///
struct Event
{
    ///
    /// \struct Keyboard
    /// \brief A struct that encapsulates a keyboard event.
    ///
    /// This struct contains the keycode of the key involved in the event and the action performed (e.g., key press, key
    /// release).
    ///
    struct Keyboard
    {
        int keycode; ///< The keycode of the key involved in the event.
        int action;  ///< The action performed (e.g., key press, key release).
    };

    ///
    /// \struct MouseButton
    /// \brief A struct that encapsulates a mouse button event.
    ///
    /// This struct contains the mouse button involved in the event and the action performed (e.g., mouse button press,
    /// mouse button release).
    ///
    struct MouseButton
    {
        int button; ///< The mouse button involved in the event.
        int action; ///< The action performed (e.g., mouse button press, mouse button release).
    };

    ///
    /// \struct MouseMove
    /// \brief A struct that encapsulates a mouse movement event.
    ///
    /// This struct contains the x and y positions of the mouse cursor.
    ///
    struct MouseMove
    {
        double xPos; ///< The x position of the mouse cursor.
        double yPos; ///< The y position of the mouse cursor.
    };

    ///
    /// \struct MouseScroll
    /// \brief A struct that encapsulates a mouse scroll event.
    ///
    /// This struct contains the x and y scroll offsets.
    ///
    struct MouseScroll
    {
        double xScroll; ///< The x scroll offset.
        double yScroll; ///< The y scroll offset.
    };

    ///
    /// \struct Resize
    /// \brief A struct that encapsulates a window resize event.
    ///
    /// This struct contains the new width and height of the window.
    ///
    struct Resize
    {
        int newWidth;  ///< The new width of the window.
        int newHeight; ///< The new height of the window.
    };

    ///
    /// \struct Close
    /// \brief A struct that encapsulates a window close event.
    ///
    struct Close
    {
    };

    ///
    /// \enum Type
    /// \brief An enum that defines the different types of events.
    ///
    enum class Type
    {
        Keyboard,    ///< A keyboard event.
        MouseMove,   ///< A mouse movement event.
        MouseButton, ///< A mouse button event.
        MouseScroll, ///< A mouse scroll event.
        Resize,      ///< A window resize event.
        Close        ///< A window close event.
    };

    ///
    /// \typedef Data
    /// \brief A variant that can hold any type of event data.
    ///
    using Data = std::variant<Keyboard, MouseMove, MouseButton, MouseScroll, Resize, Close>;

    Type type; ///< The type of the event.
    Data data; ///< The data associated with the event.
};
} // namespace vkf