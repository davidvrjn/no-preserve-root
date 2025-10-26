#pragma once
#include <string>
#include <vector>

// Forward declare enums from PlantAttributes.h to avoid circular dependency
enum class WaterRequirement;
enum class Season;

enum RequestType { RECOMMENDATION, PURCHASE };

/**
 * @struct PlantSpecification
 * @brief The "Product" in the Builder pattern.
 *
 * This is a simple data structure that holds all the attributes of a customer's
 * request. It is constructed piece by piece by the PlantSpecificationBuilder.
 * It contains all the information a Cashier needs to find a plant and complete a transaction.
 *
 * CUSTOMER REQUEST CREATION LOGIC
 *
 * Customers are spawned by Nursery::spawnCustomer() and make requests using the Builder pattern.
 * The builder is used to construct either a RECOMMENDATION or PURCHASE request.
 *
 * --- RECOMMENDATION REQUESTS ---
 * Context: Customer doesn't know exactly what they want, asks for suggestions
 * Generation Logic:
 *   1. Randomly select WaterRequirement (VERY_LOW, LOW, MEDIUM, HIGH) (from pool)
 *   2. Randomly select Season (SPRING, SUMMER, FALL, WINTER, YEAR_ROUND) (from pool)
 *   3. Use builder: setRequestType(RECOMMENDATION), setWaterRequirement(), setSeasonRequirement()
 *   4. NO decorators (can't decorate something not purchased yet)
 *   5. NO explicit plant name
 *
 * Plant Pool: ANY of the 17 plant types (full catalogue):
 *   Aloe, Bamboo, Basil, Cactus, Daisy, Fern, Ivy, Lavender, Marigold,
 *   Mint, Orchid, Petunia, Rose, SnakePlant, Succulent, Sunflower, Tulip
 *
 * Fulfillment Logic (FulfillCustomerCommand):
 *   - Staff samples 50% of current inventory (plots and storage) randomly
 *   - Checks each sampled plant's waterReq and season
 *   - If a match is found -> SUCCESS (customer accepts it and our reputation increases)
 *   - If no match in sample -> FAILURE (customer leaves and our reputation decreases)
 *
 * Example:
 *   builder.setRequestType(RECOMMENDATION);
 *   builder.setWaterRequirement(WaterRequirement::LOW);
 *   builder.setSeasonRequirement(Season::YEAR_ROUND);
 *   // Could match: Cactus, Succulent, Aloe, SnakePlant, etc.
 *
 * --- PURCHASE REQUESTS ---
 * Context: Customer knows exactly what plant they want (either seasonal or previously grown)
 * Generation Logic:
 *   1. Randomly choose plant source:
 *      a) Current season plants
 *      b) Previously grown types (use Nursery's knownPlantTypes vector)
 *   2. Randomly select explicit plant name from chosen source
 *   3. Randomly decide if decorators are wanted (0-3 decorators)
 *   4. Use builder: setRequestType(PURCHASE), setExplicitName(), addDecorator() (optional)
 *   5. NO water/season requirements needed (explicit name is enough)
 *
 * Plant Pool: EITHER
 *   - Plants matching current season (e.g., if SUMMER: Basil, Lavender, Marigold, Mint, Petunia, Sunflower)
 *   - OR any plant type that has been grown before (tracked in Nursery::knownPlantTypes)
 *
 *
 * Fulfillment Logic (FulfillCustomerCommand):
 *   - Check storage (fully grown plants) for plant with matching explicitName
 *   - If found -> Apply decorators, sell to customer, SUCCESS
 *   - If not found -> FAILURE (customer leaves)
 *
 * Available Decorators: "GiftWrap", "Pot", "Ribbon"
 *
 * Example:
 *   builder.setRequestType(PURCHASE);
 *   builder.setExplicitName("Rose");
 *   builder.addDecorator("GiftWrap");
 *   builder.addDecorator("Pot");
 *   // Wants Rose with gift wrap and pot
 *
 * ========================================
 *
 * For RECOMMENDATION requests:
 *   - Uses waterReq and seasonReq to match plant attributes
 *   - explicitName is empty
 *   - decorators is empty (can't decorate a recommendation)
 *
 * For PURCHASE requests:
 *   - Uses explicitName to specify the exact plant
 *   - waterReq and seasonReq are ignored
 *   - decorators may be populated (gift wrap, pot, ribbon)
 */
struct PlantSpecification {
    // For RECOMMENDATION - describe what customer wants
    WaterRequirement waterReq;
    Season seasonReq;
    
    // For PURCHASE - explicit plant name
    std::string explicitName;
    
    // Decorators (only valid for PURCHASE requests)
    std::vector<std::string> decorators;
    
    RequestType requestType;

    PlantSpecification();
};
