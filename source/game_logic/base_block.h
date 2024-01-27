#ifndef BASEBLOCK_H
#define BASEBLOCK_H

#include <godot_cpp/classes/node2d.hpp>

#include <vector>

namespace ggj
{

class BaseBlock : public godot::Node2D
{
public:
	BaseBlock();

	virtual void InitializeBlock(std::vector<godot::Vector2i> shape, godot::Vector2i position, int blockIndex, float step);
	virtual void ReceiveEnergy(float energy) = 0;

	const std::vector<godot::Vector2i>& GetShape() const;
	const int GetIndex() const;
	const godot::Vector2i GetPosition() const;
	void SetPosition(godot::Vector2i position);

protected:
	std::vector<godot::Vector2i> shape{};
	godot::Vector2i position{};
	int blockIndex{};
};

}

#endif // BASEBLOCK_H
