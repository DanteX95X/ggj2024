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

	float ReceiveEnergy(float energy) override;

	float ProcessEnergy();

	bool GetIsShattered() const;

	inline static const float ENERGY_THRESHOLD{6};

private:
	bool isShattered{false};
	float totalEnergy{};
};

}

#endif // JARBLOCK_H
