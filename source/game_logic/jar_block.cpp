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
	if(!isShattered)
	{
		totalEnergy += energy;
	}

	return totalEnergy;
}

float JarBlock::ProcessEnergy()
{
	godot::UtilityFunctions::print("JarBlock: ", blockIndex, " totalEnergy: ", totalEnergy);

	if(totalEnergy > ENERGY_THRESHOLD)
	{
		godot::UtilityFunctions::print("JarBlock: ", blockIndex, " destroyed");
		isShattered = true;
	}

	float receivedEnergy = totalEnergy;
	totalEnergy = 0;
	return receivedEnergy;
}

}
