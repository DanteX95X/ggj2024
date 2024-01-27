#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/packed_scene.hpp>

#include <vector>
#include <map>
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

	int CountBlockDependencies(int blockIndex, const std::set<int> visited) const;

	void ResetPhysics();
	void BakeBlockOnTheGrid(BaseBlock* block);

	void MoveJarDown();

	void MoveBlockLeft();
	void MoveBlockRight();
	void RotateBlock();

	Block* GetActiveBlock() const;
	BlockSpan GetBlockSpanInGridCoordinates() const;

	godot::Ref<godot::PackedScene> GetBlockScene() const;
	void SetBlockScene(godot::Ref<godot::PackedScene> blockScene);
	godot::Ref<godot::PackedScene> GetJarBlockScene() const;
	void SetJarBlockScene(godot::Ref<godot::PackedScene> jarBlockScene);

private:
	Block* activeBlock = nullptr;
	float previousVelocity = 0;
	float accumulatedDistance = 0;


	godot::Ref<godot::PackedScene> blockScene;
	godot::Ref<godot::PackedScene> jarBlockScene;

	const float LEFT_BOUNDS = 100.0f;
	const float TOP_BOUNDS = 60.0f;
	const float NODE_SIZE = 32.0f;
	const float ACCELERATION = 0.5f;
	const float JAR_ENERGY_THRESHOLD = 5.0f;
	const int INITIAL_JAR_DEPTH = 7;

	std::vector<std::vector<int>> grid{};
	std::vector<BaseBlock*> blocks{};
	std::vector<JarBlock*> jarBlocks{};

	std::map<int, std::vector<int>> incomingEdges{};
	std::map<int, std::vector<int>> outgoingEdges{};

	const int GRID_WIDTH = 10;
	const int GRID_HEIGHT = 20;

	const std::vector<std::vector<godot::Vector2i>> SHAPES =
	{
	    {{0, 0}, {1, 0}, {-1, 0}, {0, 1} },
	    {{0, 0}, {-1, 0}, {0, 1}, {0, 2} },
	    {{0, 0}, {0, 1}, {0, 3}, {0, 2} }
	};
};

}
#endif // GAMEMANAGER_H
