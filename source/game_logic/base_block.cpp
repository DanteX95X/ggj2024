#include "base_block.h"

#include <godot_cpp/classes/engine.hpp>

namespace ggj
{

void BaseBlock::_bind_methods()
{

}

BaseBlock::BaseBlock()
{

}

void BaseBlock::_process(double delta)
{
	if(shouldBlink && blinkTimeCounter < BLINK_TIME)
	{
		blinkTimeCounter += delta;

		set_visible((godot::Engine::get_singleton()->get_frames_drawn() % 20) > 10);

		if(blinkTimeCounter >= BLINK_TIME)
		{
			shouldBlink = false;
			blinkTimeCounter = 0;
			set_visible(INITIAL_VISIBILITY);
		}
	}
}

void BaseBlock::InitializeBlock(std::vector<godot::Vector2i> shape, godot::Vector2i position, int blockIndex, float step)
{
	this->shape = shape;
	this->position = position;
	this->blockIndex = blockIndex;
}

float BaseBlock::ReceiveEnergy(float energy)
{
	shouldBlink = true;
	return energy;
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
