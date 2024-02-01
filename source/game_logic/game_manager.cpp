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

	ADD_SIGNAL(godot::MethodInfo(GAME_OVER_SIGNAL.data(),
	                             godot::PropertyInfo(godot::Variant::STRING, "message"),
	                             godot::PropertyInfo(godot::Variant::BOOL, "did_win")
	                             ));
	ADD_SIGNAL(godot::MethodInfo(TOTAL_ENERGY_UPDATED_SIGNAL.data(),
	                             godot::PropertyInfo(godot::Variant::FLOAT, "total_energy")
	                             ));
	ADD_SIGNAL(godot::MethodInfo(ENERGY_THRESHOLD_UPDATED_SIGNAL.data(),
	                             godot::PropertyInfo(godot::Variant::FLOAT, "energy_threshold")
	                             ));
	ADD_SIGNAL(godot::MethodInfo(MAX_ENERGY_UPDATED_SIGNAL.data(),
	                             godot::PropertyInfo(godot::Variant::FLOAT, "max_energy")
	                             ));
	ADD_SIGNAL(godot::MethodInfo(JAR_BLOCK_ENERGY_THRESHOLD_UPDATED_SIGNAL.data(),
	                             godot::PropertyInfo(godot::Variant::FLOAT, "jar_block_energy_threshold")
	                             ));
	ADD_SIGNAL(godot::MethodInfo(CURRENT_ENERGY_UPDATED_SIGNAL.data(),
	                             godot::PropertyInfo(godot::Variant::FLOAT, "current_energy")
	                             ));
	ADD_SIGNAL(godot::MethodInfo(JAR_BLOCKS_SHATTERED_SIGNAL.data(),
	                             godot::PropertyInfo(godot::Variant::INT, "jar_blocks_shattered")
	                             ));
	ADD_SIGNAL(godot::MethodInfo(JAR_SHATTERING_THRESHOLD_SIGNAL.data(),
	                             godot::PropertyInfo(godot::Variant::INT, "jar_shattering_threshold")
	                             ));
	ADD_SIGNAL(godot::MethodInfo(BLOCK_SHATTERED_SIGNAL.data()));
	ADD_SIGNAL(godot::MethodInfo(JAR_MOVES_UP_SIGNAL.data()));
	ADD_SIGNAL(godot::MethodInfo(JAR_COLLISION_SIGNAL.data(),
	                             godot::PropertyInfo(godot::Variant::BOOL, "is_heavy")
	                             ));
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

	emit_signal(ENERGY_THRESHOLD_UPDATED_SIGNAL.data(), JAR_ENERGY_THRESHOLD);
	emit_signal(JAR_BLOCK_ENERGY_THRESHOLD_UPDATED_SIGNAL.data(), JarBlock::ENERGY_THRESHOLD);
	emit_signal(JAR_SHATTERING_THRESHOLD_SIGNAL.data(), JAR_SHATTER_THRESHOLD);

	jar = static_cast<godot::Node2D*>(jarScene->instantiate());
	call_deferred("add_sibling", jar);
	jar->set_position(godot::Vector2{(LEFT_BOUNDS + GRID_WIDTH * NODE_SIZE)/ 2.0f,
	                                 (GRID_HEIGHT - 1 - INITIAL_JAR_DEPTH) * NODE_SIZE});

	grid = std::vector<std::vector<int>>(GRID_HEIGHT, std::vector<int>(GRID_WIDTH, -1));

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
	if(godot::Engine::get_singleton()->is_editor_hint())
	{
		return;
	}

	if(activeBlock != nullptr)
	{
		CalculateBlockMotion(delta);
		ProcessBlockFall();
	}

	timeCounter += delta;
	if(timeCounter >= JAR_MOVEMENT_TIME)
	{
		timeCounter -= JAR_MOVEMENT_TIME;
		MoveJarUp();
	}
}

void GameManager::SpawnNextBlock()
{
	ResetPhysics();

	std::uniform_int_distribution<unsigned int> rng{0, static_cast<unsigned int>(SHAPES.size()) - 1};
	activeBlock = static_cast<Block*>(SpawnBlockAt(blockScene, SPAWN_POINT, SHAPES[rng(twister)]));
	if(GetAllCollidersInDirection(activeBlock, godot::Vector2i{0,0}, true).size() > 0)
	{
		std::string message = "GAME OVER: The jar went too far";
		activeBlock->queue_free();
		activeBlock = nullptr;
		GameOver(message, false);
	}
}

BaseBlock* GameManager::SpawnBlockAt(godot::Ref<godot::PackedScene> scene, godot::Vector2i gridPosition, std::vector<godot::Vector2i> shape)
{
	BaseBlock* block = static_cast<BaseBlock*>(scene->instantiate());
	block->InitializeBlock(shape, gridPosition, blocks.size(), NODE_SIZE);
	blocks.push_back(block);
	block->set_position(godot::Vector2{LEFT_BOUNDS + NODE_SIZE * gridPosition.x,
	                                         TOP_BOUNDS + NODE_SIZE * gridPosition.y});
	call_deferred("add_sibling", block);

	return block;
}

void GameManager::CalculateBlockMotion(double delta)
{
	auto velocity = previousVelocity + acceleration * delta;
	auto displacement = velocity * delta;
	accumulatedDistance += displacement;
	previousVelocity = velocity;

	emit_signal(CURRENT_ENERGY_UPDATED_SIGNAL.data(), velocity * velocity);
}

void GameManager::ProcessBlockFall()
{
	if(accumulatedDistance > 1)
	{
		CheckBlockCollision();
		accumulatedDistance -= 1;

		if(activeBlock != nullptr)
		{
			activeBlock->translate(godot::Vector2{0, 1} * NODE_SIZE);
			activeBlock->SetPosition(activeBlock->GetPosition() + godot::Vector2i{0, 1});
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
	std::vector<int> hitBlocks = GetAllCollidersInDirection(activeBlock, godot::Vector2i{0, 1});
	if(hitBlocks.size() > 0)
	{
		HandleBlockCollision();
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

		for(const auto& neighbourIndex : outgoing)
		{
			frontier.push_back({neighbourIndex, energy});
		}
	}
}

void GameManager::PushJar()
{
	float totalJarEnergy = 0;
	float maxEnergy = 0;
	int shatteredJarBlocks = 0;
	for(const auto& jarBlock : jarBlocks)
	{
		float blockEnergy = jarBlock->ProcessEnergy();
		totalJarEnergy += blockEnergy;
		maxEnergy = godot::Math::max(blockEnergy, maxEnergy);
		if(jarBlock->GetIsShattered())
		{
			++shatteredJarBlocks;
		}
	}

	emit_signal(TOTAL_ENERGY_UPDATED_SIGNAL.data(), totalJarEnergy);
	emit_signal(MAX_ENERGY_UPDATED_SIGNAL.data(), maxEnergy);
	emit_signal(JAR_BLOCKS_SHATTERED_SIGNAL.data(), shatteredJarBlocks);

	if(shatteredJarBlocks >= JAR_SHATTER_THRESHOLD)
	{
		std::string message = "GAME OVER: Jar shattered";
		GameOver(message, false);
	}
	else if(shatteredJarBlocks > previousShatteredBlocks)
	{
		previousShatteredBlocks = shatteredJarBlocks;
		emit_signal(BLOCK_SHATTERED_SIGNAL.data());
	}

	if(totalJarEnergy > JAR_ENERGY_THRESHOLD)
	{
		emit_signal(JAR_COLLISION_SIGNAL.data(), true);
		MoveJarDown();
	}
	else
	{
		emit_signal(JAR_COLLISION_SIGNAL.data(), false);
	}
}

void GameManager::GameOver(std::string message, bool didWin)
{
	emit_signal(GAME_OVER_SIGNAL.data(), godot::String(message.c_str()), didWin);
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
}

void GameManager::MoveJarDown()
{
	for(int y = GRID_HEIGHT - 1; y > 0; --y)
	{
		grid[y] = grid[y - 1];
	}
	grid[0] = std::vector<int>(GRID_WIDTH, -1);

	MoveJar(godot::Vector2i{0, 1});

	if(jarBlocks.front()->GetPosition().y == GRID_HEIGHT -1)
	{
		std::string message = "YOU WON: Jar pushed out";
		GameOver(message, true);
	}
}

void GameManager::MoveJarUp()
{
	emit_signal(JAR_MOVES_UP_SIGNAL.data());

	for(int y = 0; y < GRID_HEIGHT - 1; ++y)
	{
		grid[y] = grid[y + 1];
	}
	grid[GRID_HEIGHT - 1] = std::vector<int>(GRID_WIDTH, -1);

	MoveJar(godot::Vector2i{0, -1});
}

void GameManager::MoveJar(godot::Vector2i displacement)
{
	for(auto& block : blocks)
	{
		block->SetPosition(block->GetPosition() + displacement);
		block->translate(displacement * NODE_SIZE);
	}

	jar->translate(displacement * NODE_SIZE);
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
