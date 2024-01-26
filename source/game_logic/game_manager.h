#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <godot_cpp/classes/node.hpp>

#include "block.h"

namespace ggj
{

class GameManager : public godot::Node
{
	GDCLASS(GameManager, godot::Node);
	static void _bind_methods();

public:
	GameManager();

private:
	const Block* activeBlock = nullptr;
};

}
#endif // GAMEMANAGER_H
