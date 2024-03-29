#include "input_manager.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/engine.hpp>

namespace ggj
{

const std::string_view ON_GAME_OVER_METHOD{"OnGameOver"};
const std::string_view MESSAGE_PARAMETER{"message"};
const std::string_view DID_WIN_PARAMETER{"didWin"};

void InputManager::_bind_methods()
{
	godot::ClassDB::bind_method(godot::D_METHOD(ON_GAME_OVER_METHOD.data(),
	                                            MESSAGE_PARAMETER.data(),
	                                            DID_WIN_PARAMETER.data()),
	                            &InputManager::OnGameOver);

	godot::ClassDB::bind_method(godot::D_METHOD("SetGameManagerPath"), &InputManager::SetGameManagerPath);
	godot::ClassDB::bind_method(godot::D_METHOD("GetGameManagerPath"), &InputManager::GetGameManagerPath);
	ADD_PROPERTY(godot::PropertyInfo(godot::Variant::NODE_PATH, "GameManagerPath"), "SetGameManagerPath", "GetGameManagerPath");
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

	gameManager = get_node<GameManager>(gameManagerPath);
	gameManager->connect(GameManager::GAME_OVER_SIGNAL.data(), godot::Callable{this, ON_GAME_OVER_METHOD.data()});
}

void InputManager::_input(const godot::Ref<godot::InputEvent>& event)
{
	if(gameManager == nullptr)
	{
		return;
	}

	if(event->is_action_pressed(RIGHT_ACTION.data()))
	{
		gameManager->MoveBlockRight();
	}
	else if(event->is_action_pressed(LEFT_ACTION.data()))
	{
		gameManager->MoveBlockLeft();
	}
	else if(event->is_action_pressed(ROTATE_ACTION.data()))
	{
		gameManager->RotateBlock();
	}
	else if(event->is_action_pressed(ACCELERATE_ACTION.data()))
	{
		gameManager->Accelerate();
	}
}

void InputManager::OnGameOver(godot::String message, bool didWin)
{
	gameManager = nullptr;
}

godot::NodePath InputManager::GetGameManagerPath() const
{
	return gameManagerPath;
}

void InputManager::SetGameManagerPath(godot::NodePath gameManagerPath)
{
	this->gameManagerPath = gameManagerPath;
}

}
