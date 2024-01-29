#pragma once

// storing inputs as keys
#include <set>

class Input
{
public:
	Input() = default;
	~Input() = default;

public:
    void handle_event(SDL_Event& event) {
        switch (event.type) {
            // keyboard events
            case SDL_EventType::SDL_EVENT_KEY_DOWN:
                if (event.key.repeat) return;
                keysPressed.insert(event.key.keysym.sym);
                keysDown.insert(event.key.keysym.sym);
                break;
            case SDL_EventType::SDL_EVENT_KEY_UP:
                keysReleased.insert(event.key.keysym.sym);
                keysDown.erase(event.key.keysym.sym);
                break;
            // mouse events
            case SDL_EventType::SDL_EVENT_MOUSE_BUTTON_DOWN:
				mouseButtonsPressed.insert(event.button.button);
				mouseButtonsDown.insert(event.button.button);
				break;
			case SDL_EventType::SDL_EVENT_MOUSE_BUTTON_UP:
				mouseButtonsReleased.insert(event.button.button);
				mouseButtonsDown.erase(event.button.button);
				break;
            case SDL_EventType::SDL_EVENT_MOUSE_MOTION: 
                xMouseDelta += event.motion.xrel;
                yMouseDelta += event.motion.yrel;
                xMouse += event.motion.xrel;
                yMouse += event.motion.yrel;
		}
    }
	// per-frame
	void flush() {
		xMouseDelta = yMouseDelta = 0;
		keysPressed.clear();
		keysReleased.clear();
		mouseButtonsPressed.clear();
		mouseButtonsReleased.clear();
	}
	// e.g. on window exit
	void flush_full() {
		flush();
		keysDown.clear();
		mouseButtonsDown.clear();
	}

    // query key and button inputs using find(), which returns the end() iterator if it wasnt found
    bool get_key_down(SDL_Keycode key) { // is key being held down
        return keysDown.find(key) != keysDown.end();
    }
    bool get_key_pressed(SDL_Keycode key) { // was key pressed this frame
        return keysPressed.find(key) != keysPressed.end();
    }
    bool get_key_released(SDL_Keycode key) { // was key released this frame
        return keysReleased.find(key) != keysReleased.end();
    }

    bool get_button_down(uint8_t button) { // is mouse button being held down
        return mouseButtonsDown.find(button) != mouseButtonsDown.end();
    }
    bool get_button_pressed(uint8_t button) { // was mouse button pressed this frame
        return mouseButtonsPressed.find(button) != mouseButtonsPressed.end();
    }
    bool get_button_released(uint8_t button) { // was mouse button released this frame
        return mouseButtonsReleased.find(button) != mouseButtonsReleased.end();
    }

    std::pair<float, float> get_mouse_pos() {
        return { xMouse, yMouse };
    }
    std::pair<float, float> get_mouse_delta() {
        return { xMouseDelta, yMouseDelta };
    }
    

private:
	float xMouseDelta, yMouseDelta;
	float xMouse, yMouse;

	std::set<uint8_t> mouseButtonsPressed;
	std::set<uint8_t> mouseButtonsDown;
	std::set<uint8_t> mouseButtonsReleased;

	std::set<SDL_Keycode> keysPressed;
	std::set<SDL_Keycode> keysDown;
	std::set<SDL_Keycode> keysReleased;
};