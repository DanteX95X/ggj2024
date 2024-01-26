#ifndef BLOCK_H
#define BLOCK_H

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>

#include <vector>

namespace ggj
{

class Block : public godot::Node2D
{
	GDCLASS(Block, godot::Node2D)
	static void _bind_methods();

public:
	Block();

	void _ready() override;

	void SetShape(std::vector<godot::Vector2> shape, float step);
	void Rotate(float step);

	godot::Ref<godot::PackedScene> GetNodeScene() const;
	void SetNodeScene(godot::Ref<godot::PackedScene> nodeScene);

private:
	godot::Ref<godot::PackedScene> nodeScene{};
	std::vector<godot::Vector2> shape{};
	std::vector<Node2D*> nodes{};
};

}

#endif // BLOCK_H
