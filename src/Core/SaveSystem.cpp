#include "../../include/Core/SaveSystem.h"
#include "../../include/Patterns/Memento/Memento.h"
#include "../../include/Core/Nursery.h"

SaveSystem::SaveSystem() = default;

void SaveSystem::save(const std::shared_ptr<Nursery>& nursery, const std::string& filename) {
	(void)nursery; (void)filename; // stub
}

std::unique_ptr<Memento> SaveSystem::load(const std::string& filename) {
	(void)filename; return nullptr;
}

