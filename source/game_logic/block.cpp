#include "block.h"
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/classes/sprite2d.hpp>

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

float Block::ReceiveEnergy(float energy)
{
	BaseBlock::ReceiveEnergy(energy);
	return energy * (1 - ENERGY_LOSS);
}

void Block::InitializeBlock(std::vector<godot::Vector2i> shape, godot::Vector2i position, int blockIndex, float step)
{
	BaseBlock::InitializeBlock(shape, position, blockIndex, step);

	nodes.reserve(4);
	std::uniform_real_distribution rng{0.0f, 1.0f};
	godot::Color color{rng(twister), rng(twister), rng(twister)};
	for(const auto& position : shape)
	{
		auto node = static_cast<godot::Sprite2D*>(nodeScene->instantiate());
		node->set_modulate(color);
		add_child(node);
		node->set_position(position * step);
		nodes.push_back(node);
	}

	UpdateSpan();
}

void Block::Rotate(float step)
{
	for(std::size_t index = 0; index < shape.size(); ++index)
	{
		auto& position = shape[index];
		godot::Vector2 realPosition = position;
		godot::Vector2 rotated = realPosition.rotated(Math_PI / 2.0f);
		rotated.x = godot::Math::round(rotated.x);
		rotated.y = godot::Math::round(rotated.y);
		position = godot::Vector2i{static_cast<int>(godot::Math::round(rotated.x)),
		           static_cast<int>(godot::Math::round(rotated.y))};

		auto node = nodes[index];
		node->set_position(position * step);
	}

	UpdateSpan();
}

void Block::UpdateSpan()
{
	BlockSpan span;
	for(const auto& position : shape)
	{
		span.left = godot::Math::min(position.x, span.left);
		span.right = godot::Math::max(position.x, span.right);
		span.top = godot::Math::min(position.y, span.top);
		span.bottom = godot::Math::max(position.y, span.bottom);
	}

	this->span = span;
}

godot::Ref<godot::PackedScene> Block::GetNodeScene() const
{
	return nodeScene;
}

void Block::SetNodeScene(godot::Ref<godot::PackedScene> nodeScene)
{
	this->nodeScene = nodeScene;
}

BlockSpan Block::GetSpan() const
{
	return span;
}

}
