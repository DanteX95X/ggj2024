#ifndef BASEBLOCK_H
#define BASEBLOCK_H

#include <godot_cpp/classes/node2d.hpp>

#include <vector>

namespace ggj
{

class BaseBlock : public godot::Node2D
{
	GDCLASS(BaseBlock, godot::Node2D);
	static void _bind_methods();

public:
	BaseBlock();

	void _process(double delta) override;

	virtual void InitializeBlock(std::vector<godot::Vector2i> shape, godot::Vector2i position, int blockIndex, float step);
	virtual float ReceiveEnergy(float energy);

	const std::vector<godot::Vector2i>& GetShape() const;
	const int GetIndex() const;
	const godot::Vector2i GetPosition() const;
	void SetPosition(godot::Vector2i position);

protected:
	std::vector<godot::Vector2i> shape{};
	godot::Vector2i position{};
	int blockIndex{};

	const bool INITIAL_VISIBILITY{true};
	bool shouldBlink{false};
	float blinkTimeCounter = 0;
	const float BLINK_TIME = 1.0f;
};

}

#endif // BASEBLOCK_H
