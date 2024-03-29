#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/packed_scene.hpp>

#include <vector>
#include <map>
#include <random>
#include "block.h"
#include "jar_block.h"

namespace ggj
{

class GameManager : public godot::Node
{
	GDCLASS(GameManager, godot::Node);

public:
	static void _bind_methods();

	GameManager();

	void _ready() override;
	void _physics_process(double delta) override;

	void SpawnNextBlock();
	BaseBlock* SpawnBlockAt(godot::Ref<godot::PackedScene> scene, godot::Vector2i gridPosition, std::vector<godot::Vector2i> shape);
	void CalculateBlockMotion(double delta);
	void ProcessBlockFall();
	std::vector<int> GetAllCollidersInDirection(BaseBlock* block, godot::Vector2i direction, bool collideWithBounds = false);
	void CheckBlockCollision();
	void HandleBlockCollision();
	void TransferEnergy();
	void PushJar();

	void GameOver(std::string message, bool didWin);

	int CountBlockDependencies(int blockIndex, const std::set<int> visited) const;

	void ResetPhysics();
	void BakeBlockOnTheGrid(BaseBlock* block);

	void MoveJarDown();
	void MoveJarUp();
	void MoveJar(godot::Vector2i displacement);

	void MoveBlockLeft();
	void MoveBlockRight();
	void RotateBlock();
	void Accelerate();

	Block* GetActiveBlock() const;
	BlockSpan GetBlockSpanInGridCoordinates() const;

	godot::Ref<godot::PackedScene> GetBlockScene() const;
	void SetBlockScene(godot::Ref<godot::PackedScene> blockScene);
	godot::Ref<godot::PackedScene> GetJarBlockScene() const;
	void SetJarBlockScene(godot::Ref<godot::PackedScene> jarBlockScene);
	godot::Ref<godot::PackedScene> GetJarScene() const;
	void SetJarScene(godot::Ref<godot::PackedScene> jarScene);

	inline static const std::string_view GAME_OVER_SIGNAL{"game_over"};
	inline static const std::string_view TOTAL_ENERGY_UPDATED_SIGNAL{"total_energy_updated"};
	inline static const std::string_view ENERGY_THRESHOLD_UPDATED_SIGNAL{"energy_threshold_updated"};
	inline static const std::string_view MAX_ENERGY_UPDATED_SIGNAL{"max_energy_updated"};
	inline static const std::string_view JAR_BLOCK_ENERGY_THRESHOLD_UPDATED_SIGNAL{"jar_block_energy_threshold_updated"};
	inline static const std::string_view CURRENT_ENERGY_UPDATED_SIGNAL{"current_energy_updated"};
	inline static const std::string_view JAR_BLOCKS_SHATTERED_SIGNAL{"jar_blocks_shattered"};
	inline static const std::string_view JAR_SHATTERING_THRESHOLD_SIGNAL{"jar_shattering_threshold"};
	inline static const std::string_view BLOCK_SHATTERED_SIGNAL{"block_shattered"};
	inline static const std::string_view JAR_MOVES_UP_SIGNAL{"jar_moves_up"};
	inline static const std::string_view JAR_COLLISION_SIGNAL{"jar_collision"};

private:
	Block* activeBlock = nullptr;
	float previousVelocity = 0;
	float accumulatedDistance = 0;
	float acceleration = INITIAL_ACCELERATION;

	float timeCounter = 0.0f;

	int previousShatteredBlocks = 0;

	godot::Ref<godot::PackedScene> blockScene;
	godot::Ref<godot::PackedScene> jarBlockScene;
	godot::Ref<godot::PackedScene> jarScene;

	std::vector<std::vector<int>> grid{};
	std::vector<BaseBlock*> blocks{};
	std::vector<JarBlock*> jarBlocks{};
	godot::Node2D* jar{};

	std::map<int, std::vector<int>> incomingEdges{};
	std::map<int, std::vector<int>> outgoingEdges{};

	std::mt19937 twister {std::random_device{}()};

	const int GRID_WIDTH = 10;
	const int GRID_HEIGHT = 25;
	const godot::Vector2i SPAWN_POINT{5, 3};

	const std::vector<std::vector<godot::Vector2i>> SHAPES =
	{
	    {{0, 0}, {1, 0}, {-1, 0}, {0, 1} },
	    {{0, 0}, {-1, 0}, {0, 1}, {0, 2} },
	    {{0, 0}, {0, 1}, {0, -1}, {0, 2} },
	    {{0, 0}, {1, 0}, {0, 1}, {0, 2} },
	    {{0, 0}, {1, 0}, {0, 1}, {1, 1} },
	    {{0, 0}, {1, 0}, {0, 1}, {-1, 1} },
	    {{0, 0}, {-1, 0}, {0, 1}, {1, 1} },
	};

	const float LEFT_BOUNDS = 100.0f;
	const float TOP_BOUNDS = -36.0f;
	const float NODE_SIZE = 32.0f;
	const float INITIAL_ACCELERATION = 1.0f;
	const float ACCELERATION_INCREMENT = 0.5f;
	const float JAR_ENERGY_THRESHOLD = 15.0f;
	const int INITIAL_JAR_DEPTH = 9;
	const int JAR_SHATTER_THRESHOLD =3;
	const float JAR_MOVEMENT_TIME = 10.0f;
};

}
#endif // GAMEMANAGER_H
