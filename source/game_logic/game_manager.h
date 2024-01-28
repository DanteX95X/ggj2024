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

	void GameOver(std::string message);

	int CountBlockDependencies(int blockIndex, const std::set<int> visited) const;

	void ResetPhysics();
	void BakeBlockOnTheGrid(BaseBlock* block);

	void MoveJarDown();

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

private:
	Block* activeBlock = nullptr;
	float previousVelocity = 0;
	float accumulatedDistance = 0;
	float acceleration = INITIAL_ACCELERATION;


	godot::Ref<godot::PackedScene> blockScene;
	godot::Ref<godot::PackedScene> jarBlockScene;
	godot::Ref<godot::PackedScene> jarScene;

	const float LEFT_BOUNDS = 100.0f;
	const float TOP_BOUNDS = -36.0f;
	const float NODE_SIZE = 32.0f;
	const float INITIAL_ACCELERATION = 0.5f;
	const float ACCELERATION_INCREMENT = 0.25f;
	const float JAR_ENERGY_THRESHOLD = 5.0f;
	const int INITIAL_JAR_DEPTH = 7;
	const int JAR_SHATTER_THRESHOLD =3;

	std::vector<std::vector<int>> grid{};
	std::vector<BaseBlock*> blocks{};
	std::vector<JarBlock*> jarBlocks{};
	godot::Node2D* jar{};

	std::map<int, std::vector<int>> incomingEdges{};
	std::map<int, std::vector<int>> outgoingEdges{};

	const int GRID_WIDTH = 10;
	const int GRID_HEIGHT = 25;
	const godot::Vector2i SPAWN_POINT{5, 5};

	std::mt19937 twister {std::random_device{}()};

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
};

}
#endif // GAMEMANAGER_H
