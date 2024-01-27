#include "base_block.h"

namespace ggj
{

BaseBlock::BaseBlock()
{

}

void BaseBlock::InitializeBlock(std::vector<godot::Vector2i> shape, godot::Vector2i position, int blockIndex, float step)
{
	this->shape = shape;
	this->position = position;
	this->blockIndex = blockIndex;
}

const std::vector<godot::Vector2i>& BaseBlock::GetShape() const
{
	return shape;
}

const int BaseBlock::GetIndex() const
{
	return blockIndex;
}

const godot::Vector2i BaseBlock::GetPosition() const
{
	return position;
}

void BaseBlock::SetPosition(godot::Vector2i position)
{
	this->position = position;
}

}
