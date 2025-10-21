#pragma once

// Forward declaration
class Plant;

/**
 * @interface PlantFactory
 * @brief The interface for the Factory Method design pattern.
 * 
 * This abstract class defines the "Factory Method" `createPlant()`. Each concrete
 * factory subclass (e.g., CactusFactory) will provide its own implementation of
 * this method to produce a specific type of Plant. This allows the Nursery to
 * create plants without being tightly coupled to their concrete classes.
 */
class PlantFactory {
public:
    virtual ~PlantFactory() {}
    virtual Plant* createPlant() = 0;
};
