#include "game_manager.h"

#include <godot_cpp/classes/engine.hpp>


namespace ggj
{

void GameManager::_bind_methods()
{
	godot::ClassDB::bind_method(godot::D_METHOD("setSpeed"), &GameManager::SetBlockScene);
	godot::ClassDB::bind_method(godot::D_METHOD("getSpeed"), &GameManager::GetBlockScene);
	ADD_PROPERTY(godot::PropertyInfo(godot::Variant::OBJECT, "speed"), "setSpeed", "getSpeed");
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
	add_child(activeBlock);
}

godot::Ref<godot::PackedScene> GameManager::GetBlockScene() const
{
	return blockScene;
}

void GameManager::SetBlockScene(godot::Ref<godot::PackedScene> blockScene)
{
	this->blockScene = blockScene;
}

Block* GameManager::GetActiveBlock() const
{
	return activeBlock;
}

}
