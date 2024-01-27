#include "game_manager.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>


namespace ggj
{

void GameManager::_bind_methods()
{
	godot::ClassDB::bind_method(godot::D_METHOD("SetBlockScene"), &GameManager::SetBlockScene);
	godot::ClassDB::bind_method(godot::D_METHOD("GetBlockScene"), &GameManager::GetBlockScene);
	ADD_PROPERTY(godot::PropertyInfo(godot::Variant::OBJECT, "BlockScene"), "SetBlockScene", "GetBlockScene");
}

GameManager::GameManager()
{

}

void GameManager::_ready()
{
	if(godot::Engine::get_singleton()->is_editor_hint())
	{
		return;
	}

	activeBlock = static_cast<Block*>(blockScene->instantiate());
	activeBlock->set_position(godot::Vector2{LEFT_BOUNDS, 100});
	activeBlock->SetShape(SHAPES.back(), NODE_SIZE);
	add_child(activeBlock);
}

void GameManager::_physics_process(double delta)
{
	auto velocity = previousVelocity + ACCELERATION * delta;
	auto displacement = velocity * delta;
	accumulatedDistance += displacement;
	godot::UtilityFunctions::print("accumulated distance", accumulatedDistance);

	previousVelocity = velocity;

	if(accumulatedDistance > 1)
	{
		accumulatedDistance -= 1;
		activeBlock->translate(godot::Vector2{0, 1} * NODE_SIZE);
	}
}

godot::Ref<godot::PackedScene> GameManager::GetBlockScene() const
{
	return blockScene;
}

void GameManager::SetBlockScene(godot::Ref<godot::PackedScene> blockScene)
{
	this->blockScene = blockScene;
}

void GameManager::MoveBlockLeft()
{
	activeBlock->translate(godot::Vector2{-NODE_SIZE, 0});
}

void GameManager::MoveBlockRight()
{
	activeBlock->translate(godot::Vector2{NODE_SIZE, 0});
}

void GameManager::RotateBlock()
{
	activeBlock->Rotate(NODE_SIZE);
}

Block* GameManager::GetActiveBlock() const
{
	return activeBlock;
}

}
