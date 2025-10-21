#pragma once
#include "PlantFactory.h"

/**
 * @class RoseFactory
 * @brief A concrete factory that produces Rose objects.
 */
class RoseFactory : public PlantFactory {
public:
	RoseFactory();
	~RoseFactory();
    
	Plant* createPlant() override;
};

