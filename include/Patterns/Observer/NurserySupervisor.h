
#pragma once
#include <memory>

#include "Observer.h"

// Forward declaration
class Nursery;

/**
 * @class NurserySupervisor
 * @brief A concrete Observer that monitors plants for changes.
 *
 * This class's sole responsibility is to translate a notification from a Plant
 * into an actionable Command. It observes a Plant, and when its update() method
 * is called, it inspects the Plant's state and creates the appropriate Command
 * (e.g., WaterPlantCommand), adding it to the Nursery's central request queue.
 */
class NurserySupervisor : public Observer, public std::enable_shared_from_this<NurserySupervisor> {
   private:
    // Use a weak_ptr to avoid ownership cycles; the Nursery owns the supervisor.
    std::weak_ptr<Nursery> nursery;

   public:
   /**
    * @brief Constructs a NurserySupervisor for the given Nursery.
    * @param nursery The Nursery this supervisor monitors plants for.
     */
    NurserySupervisor(const std::shared_ptr<Nursery>& nursery);
    /**
     * @brief Default destructor.
     */
    ~NurserySupervisor() override = default;

     /**
     * @brief Handles notifications from observed Plants.
     * 
     * Inspects the Plant's state and creates appropriate Commands,
     * adding them to the Nursery's request queue.
     * 
     * @param subject The Plant that changed (cast from Subject).
     */
    void update(const std::shared_ptr<Subject>& subject) override;
};
