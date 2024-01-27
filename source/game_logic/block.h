#ifndef BLOCK_H
#define BLOCK_H

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>

#include <vector>

namespace ggj
{

struct BlockSpan
{
	int left{};
	int right{};
	int top{};
	int bottom{};
};

class Block : public godot::Node2D
{
	GDCLASS(Block, godot::Node2D)
	static void _bind_methods();

public:
	Block();

	void _ready() override;

	void SetShape(std::vector<godot::Vector2i> shape, float step);
	void Rotate(float step);

	void UpdateSpan();

	godot::Ref<godot::PackedScene> GetNodeScene() const;
	void SetNodeScene(godot::Ref<godot::PackedScene> nodeScene);

	BlockSpan GetSpan() const;

private:
	godot::Ref<godot::PackedScene> nodeScene{};
	std::vector<godot::Vector2i> shape{};
	std::vector<Node2D*> nodes{};

	BlockSpan span{};
};

}

#endif // BLOCK_H
