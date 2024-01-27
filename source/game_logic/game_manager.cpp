#include "game_manager.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <set>
#include <algorithm>

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

	grid = std::vector<std::vector<int>>(GRID_HEIGHT, std::vector<int>(GRID_WIDTH, -1));

	//Spawn jar blocks
	for(int x = 0; x < GRID_WIDTH; ++x)
	{
		SpawnBlockAt(godot::Vector2i{x, GRID_HEIGHT - 1}, {godot::Vector2i{0,0}});
		BakeBlockOnTheGrid();
	}

	SpawnNextBlock();
}

void GameManager::_physics_process(double delta)
{
	if(activeBlock != nullptr)
	{
		CalculateBlockMotion(delta);
		ProcessBlockFall();
	}
}

void GameManager::SpawnNextBlock()
{
	ResetPhysics();
	//TODO: handle game over condition
	//TODO: randomize shapes
	SpawnBlockAt(godot::Vector2i{5, 0}, SHAPES.back());
	if(GetAllCollidersInDirection(godot::Vector2i{0,0}, true).size() > 0)
	{
		//TODO: Handle this game over condition. Cannotspawn.
		activeBlock->queue_free();
		activeBlock = nullptr;
	}
}

void GameManager::SpawnBlockAt(godot::Vector2i gridPosition, std::vector<godot::Vector2i> shape)
{
	activeBlockIndex = blocks.size();
	activeBlock = static_cast<Block*>(blockScene->instantiate());
	blocks.push_back(activeBlock);
	activeBlock->set_position(godot::Vector2{LEFT_BOUNDS + NODE_SIZE * gridPosition.x,
	                                         TOP_BOUNDS + NODE_SIZE * gridPosition.y});
	activeBlock->SetShape(shape, NODE_SIZE);
	add_child(activeBlock);
	blockGridPosition = gridPosition;
}

void GameManager::CalculateBlockMotion(double delta)
{
	auto velocity = previousVelocity + ACCELERATION * delta;
	auto displacement = velocity * delta;
	accumulatedDistance += displacement;
	previousVelocity = velocity;
}

void GameManager::ProcessBlockFall()
{
	if(accumulatedDistance > 1)
	{
		CheckBlockCollision(); //TODO: Consider not duplicating collisions.
		accumulatedDistance -= 1;

		if(activeBlock != nullptr)
		{
			activeBlock->translate(godot::Vector2{0, 1} * NODE_SIZE);
			blockGridPosition += godot::Vector2i{0, 1};
			CheckBlockCollision();
		}
	}
}

std::vector<int> GameManager::GetAllCollidersInDirection(godot::Vector2i direction, bool collideWithBounds)
{
	std::set<int> hitBlocks{};
	for(const auto& position : activeBlock->GetShape())
	{
		godot::Vector2i nodeGridPosition = position + blockGridPosition;
		godot::Vector2i collidingNode = nodeGridPosition + direction;

		if(collidingNode.y < 0 || collidingNode.x < 0 ||
		   collidingNode.x >= GRID_WIDTH || collidingNode.y >= GRID_HEIGHT)
		{
			if(collideWithBounds)
			{
				hitBlocks.insert(-1);
			}

			continue;
		}

		int hitBlockIndex = grid[collidingNode.y][collidingNode.x];
		if(hitBlockIndex > -1)
		{
			hitBlocks.insert(hitBlockIndex);
		}
	}

	return std::vector<int>(hitBlocks.begin(), hitBlocks.end());
}

void GameManager::CheckBlockCollision()
{
	BlockSpan spanOnGrid = GetBlockSpanInGridCoordinates();
	if(spanOnGrid.bottom >= GRID_HEIGHT - 1)
	{
		HandleBlockCollision();
	}
	else
	{
		std::vector<int> hitBlocks = GetAllCollidersInDirection(godot::Vector2i{0, 1});
		if(hitBlocks.size() > 0)
		{
			HandleBlockCollision();
		}
	}
}

void GameManager::HandleBlockCollision()
{
	BakeBlockOnTheGrid();
	//TODO: energy transfer calculations
	TransferEnergy();
	SpawnNextBlock();
}

void GameManager::TransferEnergy()
{
	std::vector<int> frontier{activeBlockIndex};
	std::set<int> visited{};

	while(!frontier.empty())
	{
		std::sort(frontier.begin(), frontier.end(),
		          [&visited, incoming = &incomingEdges, this](const int& first, const int& second)
		{
			int firstUnresolvedDependencies = CountBlockDependencies(first, visited);
			int secondUnresolvedDependencies = CountBlockDependencies(second, visited);
			return firstUnresolvedDependencies > secondUnresolvedDependencies;
		});

		int currentNodeIndex = frontier.back();
		frontier.pop_back();

		if(visited.find(currentNodeIndex) != visited.end())
		{
			continue;
		}
		visited.insert(currentNodeIndex);

		//TODO: pass energy
		godot::UtilityFunctions::print("frontier top: ", currentNodeIndex);

		const auto& outgoing = outgoingEdges[currentNodeIndex];
		frontier.insert(frontier.end(), outgoing.begin(), outgoing.end());
	}
}

int GameManager::CountBlockDependencies(int blockIndex, const std::set<int> visited) const
{
	int unresolvedDependencies = 0;
	const auto& firstIncoming = incomingEdges.at(blockIndex);
	std::for_each(firstIncoming.begin(), firstIncoming.end(),
	              [&visited, &unresolvedDependencies](int blockIndex)
	{
		if(visited.find(blockIndex) == visited.end())
		{
			++unresolvedDependencies;
		}
	});

	return unresolvedDependencies;
}

void GameManager::ResetPhysics()
{
	previousVelocity = 0;
	accumulatedDistance = 0;
}

void GameManager::BakeBlockOnTheGrid()
{
	incomingEdges[activeBlockIndex] = {};
	outgoingEdges[activeBlockIndex] = {};
	const auto bottomColliders = GetAllCollidersInDirection(godot::Vector2i{0, 1});
	const auto topColliders = GetAllCollidersInDirection(godot::Vector2i{0, -1});

	for(const auto& collider : bottomColliders)
	{
		outgoingEdges[activeBlockIndex].push_back(collider);
		incomingEdges[collider].push_back(activeBlockIndex);
	}
	for(const auto& collider : topColliders)
	{
		incomingEdges[activeBlockIndex].push_back(collider);
		outgoingEdges[collider].push_back(activeBlockIndex);
	}

	auto& shape = activeBlock->GetShape();
	for(const auto& position : shape)
	{
		godot::Vector2i gridPosition = position + blockGridPosition;
		grid[gridPosition.y][gridPosition.x] = activeBlockIndex;
	}

	godot::UtilityFunctions::print("Grid: ");
	for(const auto& row : grid)
	{
		godot::String serializedRow = "";
		for(const auto& cell : row)
		{
			serializedRow += " ";
			if(cell != -1)
			{
				serializedRow += std::to_string(cell).c_str();
			}
			else
			{
				serializedRow += "X";
			}
		}
		godot::UtilityFunctions::print(serializedRow);
	}
}

void GameManager::MoveBlockLeft()
{
	if(activeBlock != nullptr && GetAllCollidersInDirection(godot::Vector2i{-1, 0}, true).size() == 0)
	{
		activeBlock->translate(godot::Vector2{-NODE_SIZE, 0});
		blockGridPosition.x -= 1;
	}
}

void GameManager::MoveBlockRight()
{
	if(activeBlock != nullptr && GetAllCollidersInDirection(godot::Vector2i{1, 0}, true).size() == 0)
	{
		activeBlock->translate(godot::Vector2{NODE_SIZE, 0});
		blockGridPosition.x += 1;
	}
}

void GameManager::RotateBlock()
{
	if(activeBlock != nullptr)
	{
		int counter = 0;
		do
		{
			++counter;
			activeBlock->Rotate(NODE_SIZE);
		}
		while(GetAllCollidersInDirection(godot::Vector2i{0,0}, true).size() > 0 && counter <= 4);
	}
}

Block* GameManager::GetActiveBlock() const
{
	return activeBlock;
}

BlockSpan GameManager::GetBlockSpanInGridCoordinates() const
{
	BlockSpan span = activeBlock->GetSpan();
	BlockSpan spanOnGrid{};
	spanOnGrid.left = blockGridPosition.x + span.left;
	spanOnGrid.right = blockGridPosition.x + span.right;
	spanOnGrid.top = blockGridPosition.y + span.top;
	spanOnGrid.bottom = blockGridPosition.y + span.bottom;
	return spanOnGrid;
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
