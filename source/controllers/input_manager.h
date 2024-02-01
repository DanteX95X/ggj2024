#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include "../game_logic/game_manager.h"

namespace ggj
{

class InputManager : public godot::Node
{
	GDCLASS(InputManager, godot::Node)
	static void _bind_methods();
public:
	InputManager();

	void _ready() override;
	void _input(const godot::Ref<godot::InputEvent> &event) override;

	void OnGameOver(godot::String message, bool didWin);

private:
	GameManager* gameManager = nullptr;

	inline static const std::string_view LEFT_ACTION{"left"};
	inline static const std::string_view RIGHT_ACTION{"right"};
	inline static const std::string_view ACCELERATE_ACTION{"accelerate"};
	inline static const std::string_view ROTATE_ACTION{"rotate"};
};

}

#endif // INPUTMANAGER_H
