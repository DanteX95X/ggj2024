#ifndef JARBLOCK_H
#define JARBLOCK_H

#include "base_block.h"

namespace ggj
{

class JarBlock : public BaseBlock
{
	GDCLASS(JarBlock, BaseBlock);

	static void _bind_methods();

public:
	JarBlock();

	void ReceiveEnergy(float energy) override;

	float ProcessEnergy();

private:
	const float ENERGY_THRESHOLD{6};
	bool isShattered{false};
	float totalEnergy{};
};

}

#endif // JARBLOCK_H
