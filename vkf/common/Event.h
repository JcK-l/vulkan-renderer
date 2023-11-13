/// \file
/// \brief

//
// Created by Joshua Lowe on 11/5/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#ifndef VULKANRENDERER_EVENT_H
#define VULKANRENDERER_EVENT_H

#include <variant>

namespace vkf::common {
    /// \brief Object containing EventType and EventData
    struct Event {
        /// \brief Event with keycode and action values
        struct Keyboard {
            int keycode;
            int action;
        };

        /// \brief Event with button and action values
        struct MouseButton {
            int button;
            int action;
        };

        /// \brief Event with x and y position values
        struct MouseMove {
            double xPos;
            double yPos;
        };

        /// \brief Event with x and y scroll values
        struct MouseScroll {
            double xScroll;
            double yScroll;
        };

        /// \brief Event with height and width values
        struct Resize {
            int newWidth;
            int newHeight;
        };

        /// \brief Event with no data
        struct Close {
        };

        enum class Type {
            Keyboard, MouseMove, MouseButton, MouseScroll, Resize, Close
        };
        using Data = std::variant<Keyboard, MouseMove, MouseButton, MouseScroll, Resize, Close>;

        Type type;
        Data data;
    };
} // namespace vkf::common

#endif //VULKANRENDERER_EVENT_H