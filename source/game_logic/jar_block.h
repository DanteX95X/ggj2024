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
};

}

#endif // JARBLOCK_H
