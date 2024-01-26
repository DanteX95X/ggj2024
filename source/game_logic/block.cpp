#include "block.h"
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/math.hpp>

namespace ggj
{

void Block::_bind_methods()
{
	godot::ClassDB::bind_method(godot::D_METHOD("SetNodeScene"), &Block::SetNodeScene);
	godot::ClassDB::bind_method(godot::D_METHOD("GetNodeScene"), &Block::GetNodeScene);
	ADD_PROPERTY(godot::PropertyInfo(godot::Variant::OBJECT, "NodeScene"), "SetNodeScene", "GetNodeScene");
}

Block::Block()
{

}

void Block::_ready()
{
}

void Block::SetShape(std::vector<godot::Vector2> shape, float step)
{
	nodes.reserve(4);
	this->shape = shape;
	for(const auto& position : shape)
	{
		auto node = static_cast<godot::Node2D*>(nodeScene->instantiate());
		add_child(node);
		node->set_position(position * step);
		nodes.push_back(node);
	}
}

void Block::Rotate(float step)
{
	godot::UtilityFunctions::print("rotating");
	for(std::size_t index = 0; index < shape.size(); ++index)
	{
		auto& position = shape[index];
		auto rotated = position.rotated(Math_PI / 2.0f);
		rotated.x = godot::Math::round(rotated.x);
		rotated.y = godot::Math::round(rotated.y);
		position = rotated;

		auto node = nodes[index];
		node->set_position(get_position() + position * step);
		godot::UtilityFunctions::print(position);
	}
}

godot::Ref<godot::PackedScene> Block::GetNodeScene() const
{
	return nodeScene;
}

void Block::SetNodeScene(godot::Ref<godot::PackedScene> nodeScene)
{
	this->nodeScene = nodeScene;
}

}
