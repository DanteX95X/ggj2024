#ifndef BLOCK_H
#define BLOCK_H

#include <godot_cpp/classes/node2d.hpp>

namespace ggj
{

class Block : public godot::Node2D
{
	GDCLASS(Block, godot::Node2D)
	static void _bind_methods();

public:
	Block();

	void _ready() override;
};

}

#endif // BLOCK_H
