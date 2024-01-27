#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/packed_scene.hpp>

#include <vector>
#include "block.h"

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

	void SpawnBlockAt(godot::Vector2i gridPosition, std::vector<godot::Vector2i> shape);
	void CalculateBlockMotion(double delta);
	void ProcessBlockFall();
	void HandleBlockCollision();

	void MoveBlockLeft();
	void MoveBlockRight();
	void RotateBlock();
	void SnapBlockToGrid();

	Block* GetActiveBlock() const;
	BlockSpan GetBlockSpanInGridCoordinates() const;

	godot::Ref<godot::PackedScene> GetBlockScene() const;
	void SetBlockScene(godot::Ref<godot::PackedScene> blockScene);

private:
	Block* activeBlock = nullptr;
	godot::Vector2i blockGridPosition{0, 0};
	float previousVelocity = 0;
	float accumulatedDistance = 0;


	godot::Ref<godot::PackedScene> blockScene;

	const float LEFT_BOUNDS = 100.0f;
	const float TOP_BOUNDS = 100.0f;
	const float NODE_SIZE = 64.0f;
	const float ACCELERATION = 0.5f;

	const int GRID_WIDTH = 10;
	const int GRID_HEIGHT = 7;

	const std::vector<std::vector<godot::Vector2i>> SHAPES =
	{
	    {{0, 0}, {1, 0}, {-1, 0}, {0, 1} },
	    {{0, 0}, {-1, 0}, {0, 1}, {0, 2} }
	};
};

}
#endif // GAMEMANAGER_H
