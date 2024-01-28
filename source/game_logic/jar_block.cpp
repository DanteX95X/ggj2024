#include "jar_block.h"

#include <godot_cpp/variant/utility_functions.hpp>

namespace ggj
{

void JarBlock::_bind_methods()
{

}

JarBlock::JarBlock()
{

}

float JarBlock::ReceiveEnergy(float energy)
{
	BaseBlock::ReceiveEnergy(energy);

	if(!isShattered)
	{
		totalEnergy += energy;
		return totalEnergy;
	}
	else
	{
		shouldBlink = false;
	}

	return 0;
}

float JarBlock::ProcessEnergy()
{
	godot::UtilityFunctions::print("JarBlock: ", blockIndex, " totalEnergy: ", totalEnergy);

	if(totalEnergy > ENERGY_THRESHOLD)
	{
		godot::UtilityFunctions::print("JarBlock: ", blockIndex, " destroyed");
		isShattered = true;
		get_node<godot::Node2D>(godot::NodePath("Sprite2D"))->set_visible(true);
	}

	float receivedEnergy = totalEnergy;
	totalEnergy = 0;
	return receivedEnergy;
}

bool JarBlock::GetIsShattered() const
{
	return isShattered;
}

}
