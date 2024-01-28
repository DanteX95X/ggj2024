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

	godot::ClassDB::bind_method(godot::D_METHOD("SetJarBlockScene"), &GameManager::SetJarBlockScene);
	godot::ClassDB::bind_method(godot::D_METHOD("GetJarBlockScene"), &GameManager::GetJarBlockScene);
	ADD_PROPERTY(godot::PropertyInfo(godot::Variant::OBJECT, "JarBlockScene"), "SetJarBlockScene", "GetJarBlockScene");

	godot::ClassDB::bind_method(godot::D_METHOD("SetJarScene"), &GameManager::SetJarScene);
	godot::ClassDB::bind_method(godot::D_METHOD("GetJarScene"), &GameManager::GetJarScene);
	ADD_PROPERTY(godot::PropertyInfo(godot::Variant::OBJECT, "JarScene"), "SetJarScene", "GetJarScene");
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

	jar = static_cast<godot::Node2D*>(jarScene->instantiate());
	add_child(jar);
	jar->set_position(godot::Vector2{(LEFT_BOUNDS + GRID_WIDTH * NODE_SIZE)/ 2.0f, (GRID_HEIGHT - 1 - INITIAL_JAR_DEPTH) * NODE_SIZE});
	jar->set_z_index(100);

	grid = std::vector<std::vector<int>>(GRID_HEIGHT, std::vector<int>(GRID_WIDTH, -1));

	//Spawn jar blocks
	for(int x = 0; x < GRID_WIDTH; ++x)
	{
		JarBlock* jarBlock = static_cast<JarBlock*>(SpawnBlockAt(jarBlockScene,
		                                                         godot::Vector2i{x, GRID_HEIGHT - 1 - INITIAL_JAR_DEPTH},
		                                                         {godot::Vector2i{0,0}}));
		BakeBlockOnTheGrid(jarBlock);
		jarBlocks.push_back(jarBlock);
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

	std::uniform_int_distribution<uint> rng{0, static_cast<uint>(SHAPES.size()) - 1};
	activeBlock = static_cast<Block*>(SpawnBlockAt(blockScene, SPAWN_POINT, SHAPES[rng(twister)]));
	if(GetAllCollidersInDirection(activeBlock, godot::Vector2i{0,0}, true).size() > 0)
	{
		//TODO: Handle this game over condition. Cannotspawn.
		godot::UtilityFunctions::print("GAME OVER: BLOCKED");
		activeBlock->queue_free();
		activeBlock = nullptr;
		GameOver();
	}
}

BaseBlock* GameManager::SpawnBlockAt(godot::Ref<godot::PackedScene> scene, godot::Vector2i gridPosition, std::vector<godot::Vector2i> shape)
{
	BaseBlock* block = static_cast<BaseBlock*>(scene->instantiate());
	block->InitializeBlock(shape, gridPosition, blocks.size(), NODE_SIZE);
	blocks.push_back(block);
	block->set_position(godot::Vector2{LEFT_BOUNDS + NODE_SIZE * gridPosition.x,
	                                         TOP_BOUNDS + NODE_SIZE * gridPosition.y});
	add_child(block);

	return block;
}

void GameManager::CalculateBlockMotion(double delta)
{
	auto velocity = previousVelocity + acceleration * delta;
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
			activeBlock->SetPosition(activeBlock->GetPosition() + godot::Vector2i{0, 1});
			CheckBlockCollision();
		}
	}
}

std::vector<int> GameManager::GetAllCollidersInDirection(BaseBlock* block, godot::Vector2i direction, bool collideWithBounds)
{
	std::set<int> hitBlocks{};
	for(const auto& position : block->GetShape())
	{
		godot::Vector2i nodeGridPosition = position + block->GetPosition();
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
		std::vector<int> hitBlocks = GetAllCollidersInDirection(activeBlock, godot::Vector2i{0, 1});
		if(hitBlocks.size() > 0)
		{
			HandleBlockCollision();
		}
	}
}

void GameManager::HandleBlockCollision()
{
	BakeBlockOnTheGrid(activeBlock);
	TransferEnergy();
	PushJar();
	SpawnNextBlock();
}

void GameManager::TransferEnergy()
{
	float initialEnergy = previousVelocity * previousVelocity;
	std::vector<std::pair<int, float>> frontier{{activeBlock->GetIndex(), initialEnergy}};
	std::set<int> visited{};

	while(!frontier.empty())
	{
		std::sort(frontier.begin(), frontier.end(),
		          [&visited, incoming = &incomingEdges, this](const auto& first, const auto& second)
		{
			int firstUnresolvedDependencies = CountBlockDependencies(first.first, visited);
			int secondUnresolvedDependencies = CountBlockDependencies(second.first, visited);
			return firstUnresolvedDependencies > secondUnresolvedDependencies;
		});

		auto [currentBlockIndex, energy] = frontier.back();
		frontier.pop_back();

		visited.insert(currentBlockIndex);

		energy = blocks[currentBlockIndex]->ReceiveEnergy(energy);
		const auto& outgoing = outgoingEdges[currentBlockIndex];
		energy /= outgoing.size();

		godot::UtilityFunctions::print("frontier top: ", currentBlockIndex);

		for(const auto& neighbourIndex : outgoing)
		{
			frontier.push_back({neighbourIndex, energy});
		}
	}
}

void GameManager::PushJar()
{
	float totalJarEnergy = 0;
	int shatteredJarBlocks = 0;
	for(const auto& jarBlock : jarBlocks)
	{
		totalJarEnergy += jarBlock->ProcessEnergy();
		if(jarBlock->GetIsShattered())
		{
			++shatteredJarBlocks;
		}
	}

	if(shatteredJarBlocks >= JAR_SHATTER_THRESHOLD)
	{
		godot::UtilityFunctions::print("GAME OVER: Jar shattered");
		GameOver();
	}

	if(totalJarEnergy > JAR_ENERGY_THRESHOLD)
	{
		godot::UtilityFunctions::print("jar pushed, energy: ", totalJarEnergy);
		MoveJarDown();
	}
}

void GameManager::GameOver()
{
	queue_free();
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
	acceleration = INITIAL_ACCELERATION;
}

void GameManager::BakeBlockOnTheGrid(BaseBlock* block)
{
	incomingEdges[block->GetIndex()] = {};
	outgoingEdges[block->GetIndex()] = {};
	const auto bottomColliders = GetAllCollidersInDirection(block, godot::Vector2i{0, 1});
	const auto topColliders = GetAllCollidersInDirection(block, godot::Vector2i{0, -1});

	for(const auto& collider : bottomColliders)
	{
		outgoingEdges[block->GetIndex()].push_back(collider);
		incomingEdges[collider].push_back(block->GetIndex());
	}
	for(const auto& collider : topColliders)
	{
		incomingEdges[block->GetIndex()].push_back(collider);
		outgoingEdges[collider].push_back(block->GetIndex());
	}

	auto& shape = block->GetShape();
	for(const auto& position : shape)
	{
		godot::Vector2i gridPosition = position + block->GetPosition();
		grid[gridPosition.y][gridPosition.x] = block->GetIndex();
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

void GameManager::MoveJarDown()
{
	for(int y = GRID_HEIGHT - 1; y > 0; --y)
	{
		grid[y] = grid[y -1];
	}
	grid[0] = std::vector<int>(GRID_WIDTH, -1);

	godot::Vector2i displacement{0, 1};
	for(auto& block : blocks)
	{
		block->SetPosition(block->GetPosition() + displacement);
		block->translate(displacement * NODE_SIZE);
	}

	jar->translate(displacement * NODE_SIZE);

	if(jarBlocks.front()->GetPosition().y == GRID_HEIGHT -1)
	{
		godot::UtilityFunctions::print("GAME WON: JAR PUSHED OUT");
		GameOver();
	}
}

void GameManager::MoveBlockLeft()
{
	if(activeBlock != nullptr && GetAllCollidersInDirection(activeBlock, godot::Vector2i{-1, 0}, true).size() == 0)
	{
		activeBlock->translate(godot::Vector2{-NODE_SIZE, 0});
		const auto& activePosition = activeBlock->GetPosition();
		activeBlock->SetPosition({activePosition.x - 1, activePosition.y});
	}
}

void GameManager::MoveBlockRight()
{
	if(activeBlock != nullptr && GetAllCollidersInDirection(activeBlock, godot::Vector2i{1, 0}, true).size() == 0)
	{
		activeBlock->translate(godot::Vector2{NODE_SIZE, 0});
		const auto& activePosition = activeBlock->GetPosition();
		activeBlock->SetPosition({activePosition.x + 1, activePosition.y});
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
		while(GetAllCollidersInDirection(activeBlock, godot::Vector2i{0,0}, true).size() > 0 && counter <= 4);
	}
}

void GameManager::Accelerate()
{
	acceleration += ACCELERATION_INCREMENT;
}

Block* GameManager::GetActiveBlock() const
{
	return activeBlock;
}

BlockSpan GameManager::GetBlockSpanInGridCoordinates() const
{
	BlockSpan span = activeBlock->GetSpan();
	BlockSpan spanOnGrid{};
	const auto& blockGridPosition = activeBlock->GetPosition();
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

godot::Ref<godot::PackedScene> GameManager::GetJarBlockScene() const
{
	return jarBlockScene;
}

void GameManager::SetJarBlockScene(godot::Ref<godot::PackedScene> jarBlockScene)
{
	this->jarBlockScene = jarBlockScene;
}

godot::Ref<godot::PackedScene> GameManager::GetJarScene() const
{
	return jarScene;
}

void GameManager::SetJarScene(godot::Ref<godot::PackedScene> jarScene)
{
	this->jarScene = jarScene;
}

}
