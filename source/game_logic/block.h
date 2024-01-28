#ifndef BLOCK_H
#define BLOCK_H

#include "base_block.h"
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/sprite2d.hpp>

#include <vector>
#include <random>

namespace ggj
{

struct BlockSpan
{
	int left{};
	int right{};
	int top{};
	int bottom{};
};

class Block : public BaseBlock
{
	GDCLASS(Block, BaseBlock)
	static void _bind_methods();

public:
	Block();

	void _ready() override;

	float ReceiveEnergy(float energy) override;

	void InitializeBlock(std::vector<godot::Vector2i> shape, godot::Vector2i position, int blockIndex, float step) override;
	void Rotate(float step);

	void UpdateSpan();

	godot::Ref<godot::PackedScene> GetNodeScene() const;
	void SetNodeScene(godot::Ref<godot::PackedScene> nodeScene);

	BlockSpan GetSpan() const;

private:
	std::mt19937 twister{std::random_device{}()};
	godot::Ref<godot::PackedScene> nodeScene{};
	std::vector<godot::Sprite2D*> nodes{};

	BlockSpan span{};
};

}

#endif // BLOCK_H
