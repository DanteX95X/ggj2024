#include "input_manager.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/engine.hpp>

namespace ggj
{

void InputManager::_bind_methods()
{

}

InputManager::InputManager()
{

}

void InputManager::_ready()
{
	if(godot::Engine::get_singleton()->is_editor_hint())
	{
		return;
	}

	gameManager =  godot::Node::get_node<GameManager>(godot::NodePath("GameManager"));
	godot::UtilityFunctions::print("manager acquired");
}

void InputManager::_input(const godot::Ref<godot::InputEvent>& event)
{
	if(event->is_action_pressed("left"))
	{
		godot::UtilityFunctions::print("moved left on event");
		gameManager->GetActiveBlock()->translate(godot::Vector2(1, 0));
	}
}

}
