#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <godot_cpp/classes/node.hpp>
#include "../game_logic/game_manager.h"

namespace ggj
{

class InputManager : public godot::Node
{
	GDCLASS(InputManager, godot::Node)
	static void _bind_methods();
public:
	InputManager();

private:
	const GameManager* gameManager = nullptr;
};

}

#endif // INPUTMANAGER_H
