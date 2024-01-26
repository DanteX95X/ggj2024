#ifndef BLOCK_H
#define BLOCK_H

#include <godot_cpp/classes/node.hpp>

namespace ggj
{

class Block : public godot::Node
{
	GDCLASS(Block, godot::Node)
	static void _bind_methods();

public:
	Block();

	void _ready() override;
};

}

#endif // BLOCK_H
