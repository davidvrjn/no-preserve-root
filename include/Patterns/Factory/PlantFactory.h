#pragma once
#include <memory>

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
 *
 * Seed Cost Design:
 *   - When user plants a new seed, cost should be deducted BEFORE createPlant() is called
 *   - Different plant types should have different seed costs (e.g., R5-R15)
 *   - getSeedCost() method returns the cost to plant this seed
 *
 * Caller workflow:
 *   1. Check if nursery has enough funds (nursery->getMoney() >= factory->getSeedCost())
 *   2. Deduct cost: nursery->adjustMoney(-factory->getSeedCost())
 *   3. Create plant: auto plant = factory->createPlant()
 *   4. Set Seedling state: plant->setState(std::make_unique<Seedling>())
 *   5. Add to Growing group/inventory
 *
 * The idea is that the user can directly plant seeds via the UI, it won't
 * be a command since this would flood the queue.
 */
class PlantFactory {
   public:
    virtual ~PlantFactory() = default;
    
    /**
     * @brief Creates a new plant instance of the specific type.
     * @return A shared_ptr to the newly created Plant.
     */
    virtual std::shared_ptr<Plant> createPlant() = 0;
    
    /**
     * @brief Gets the cost of a seed for this plant type.
     * @return The seed cost in Rands (typically R5-R15).
     *
     * Seed costs should be much lower than the selling price of mature plants.
     * Typical range: R5-R15 for seeds, while mature plants sell for R90-R250.
     */
    virtual double getSeedCost() const = 0;
};