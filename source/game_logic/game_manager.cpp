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

	SpawnBlockAt(godot::Vector2i{1, 0}, SHAPES.back());
	SnapBlockToGrid();
}

void GameManager::_physics_process(double delta)
{
	auto velocity = previousVelocity + ACCELERATION * delta;
	auto displacement = velocity * delta;
	accumulatedDistance += displacement;
	previousVelocity = velocity;

	if(accumulatedDistance > 1)
	{
		accumulatedDistance -= 1;
//		activeBlock->translate(godot::Vector2{0, 1} * NODE_SIZE);
//		blockGridPosition += godot::Vector2i{0, 1};
	}
}

void GameManager::SpawnBlockAt(godot::Vector2i gridPosition, std::vector<godot::Vector2i> shape)
{
	activeBlock = static_cast<Block*>(blockScene->instantiate());
	activeBlock->set_position(godot::Vector2{LEFT_BOUNDS + NODE_SIZE * gridPosition.x,
	                                         TOP_BOUNDS + NODE_SIZE * gridPosition.y});
	activeBlock->SetShape(shape, NODE_SIZE);
	add_child(activeBlock);
	blockGridPosition = gridPosition;
}

void GameManager::MoveBlockLeft()
{
	activeBlock->translate(godot::Vector2{-NODE_SIZE, 0});
	blockGridPosition.x -= 1;
	SnapBlockToGrid();
}

void GameManager::MoveBlockRight()
{
	activeBlock->translate(godot::Vector2{NODE_SIZE, 0});
	blockGridPosition.x += 1;
	SnapBlockToGrid();
}

void GameManager::RotateBlock()
{
	activeBlock->Rotate(NODE_SIZE);
	SnapBlockToGrid();
}

void GameManager::SnapBlockToGrid()
{
	BlockSpan span = activeBlock->GetSpan();
	BlockSpan spanOnGrid{};
	spanOnGrid.left = blockGridPosition.x + span.left;
	spanOnGrid.right = blockGridPosition.x + span.right;
	spanOnGrid.top = blockGridPosition.y + span.top;
	spanOnGrid.bottom = blockGridPosition.y + span.bottom;

	if(spanOnGrid.left < 0)
	{
		blockGridPosition.x -= spanOnGrid.left;
		const auto& position = activeBlock->get_position();
		activeBlock->translate(godot::Vector2{-spanOnGrid.left * NODE_SIZE, 0});
	}
	else if(spanOnGrid.right >= GRID_WIDTH)
	{
		int displacement = spanOnGrid.right - GRID_WIDTH + 1;
		blockGridPosition.x -= displacement;
		activeBlock->translate(godot::Vector2{-displacement * NODE_SIZE, 0});
	}
}

Block* GameManager::GetActiveBlock() const
{
	return activeBlock;
}

godot::Ref<godot::PackedScene> GameManager::GetBlockScene() const
{
	return blockScene;
}

void GameManager::SetBlockScene(godot::Ref<godot::PackedScene> blockScene)
{
	this->blockScene = blockScene;
}

}
