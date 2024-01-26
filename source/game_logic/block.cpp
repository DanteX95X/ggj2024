#include "block.h"
#include <godot_cpp/variant/utility_functions.hpp>

namespace ggj
{

void Block::_bind_methods()
{

}

Block::Block()
{

}

void Block::_ready()
{
	godot::UtilityFunctions::print("Block ready!");
}

}
