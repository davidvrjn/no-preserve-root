# Header Guide — no-preserve-root

This file summarizes the implementation contracts and invariants encoded in the project's header files. It is intended for implementers who will write the `.cpp` files. The goal: eliminate ambiguity about ownership, cloning, observers, commands, and serialization.

## High-level principles

- Smart pointers are used everywhere for clear ownership semantics:
  - `std::shared_ptr<T>` for shared ownership.
  - `std::weak_ptr<T>` for non-owning references.
  - `std::unique_ptr<T>` for exclusive ownership (move-only resources, e.g., PlantState in `Plant`).
- Single-owner invariant for inventory components: at most one `Group` may `own` a given `InventoryComponent` (enforced via `owner_` weak_ptr in `InventoryComponent`).
- Two clone flavors are supported:
  - `clone()` — snapshot clone used by Memento: preserves ID and runtime state (used for save/restore).
  - `blueprintClone()` — user-visible blueprint clone: creates a fresh object with new ID and default runtime state.

## Group ownership & auto-move algorithm (implementers)

`Group` may be an owning container (`ownsChildren == true`) or a view/reference container (`ownsChildren == false`). Implement `Group::add()` as follows when `ownsChildren == true`:

1. Lock-check: if `component == nullptr` return (no-op).
2. Check `auto previousOwner = component->getOwner();`
   - If `previousOwner` is non-null and previousOwner.get() != `this`:
     a. Call `previousOwner->remove(component);` (this must remove the component from previousOwner's owned list and clear owner).
3. Insert `component` into `ownedComponents` (if not already present).
4. Call `component->setOwner(shared_from_this());` (Group inherits `enable_shared_from_this`).
5. Ensure invariants: component now reports this group as owner.

When `ownsChildren == false`:
- Do not change component ownership. Insert a `weak_ptr` reference into `referencedComponents`. Do not call `setOwner()`.

`Group::remove()` must:
- Remove from either `ownedComponents` (and clear `component->setOwner(nullptr)`) or `referencedComponents`.
- If removing from ownedComponents, the group must release ownership (component may be destroyed if no other shared_ptr references remain).

Edge cases:
- If `previousOwner->remove()` triggers deletion of the component, `component` may expire; ensure `add()` handles expired pointers gracefully.

## Inventory/Top-level container

- `Inventory` owns top-level components (shared_ptr). Adding/removing to/from `Inventory` follows the same owner transfer rules as Group.
- `Inventory::createIterator()` should return a `CompositeIterator` rooted at the inventory root(s).

## Iterator & TraversalStrategy contract

- `TraversalStrategy::traverse(root, collection) const` must populate `collection` in the traversal order.
- `CompositeIterator` is responsible for owning a `TraversalStrategy` and building a flattened vector of `shared_ptr<InventoryComponent>`.
- `Iterator::next()` returns the next `shared_ptr<InventoryComponent>` or `nullptr` when exhausted. `hasNext()` should be `const` and return whether `next()` would produce a non-null result.
- Implementations **may** build the flattened collection on construction (eager) or compute lazily. If eager, ensure iteration is cheap; if lazy, guard against concurrent modification.

## Plant, State & Observer contracts

- `Plant` owns a `std::unique_ptr<PlantState>`; `setState()` must accept ownership via `std::move`.
- `Plant` implements `Subject` and stores observers as `std::vector<std::weak_ptr<Observer>>`.
- Observer lifecycle:
  - `attach(shared_ptr<Observer>)` stores a `weak_ptr`.
  - `detach(shared_ptr<Observer>)` removes matching weak_ptr entries.
  - `notify()` must lock weak_ptrs, call `observer->update(shared_from_this())` only for still-alive observers, and prune expired entries.
  - `detachAllObservers()` must remove/clear all observers; owners (Groups/Inventory) should call this before destroying a Plant they own.

Edge cases:
- Avoid calling `shared_from_this()` in constructors/destructors.
- `notify()` must handle the case where observers attach/detach while notifications are in progress. A common approach: copy valid `shared_ptr`s into a temporary vector, then call `update()` on each.

## Command & Staff (CoR) contracts

- `Command` has a `Status` enum and serialize/deserialize hooks. Commands hold non-owning `weak_ptr` references to their targets and also persist `targetId` for SaveSystem.
- `Nursery` owns a `std::queue<std::unique_ptr<Command>>` (ownership transferred when queued).
- `Staff` is a Chain of Responsibility; `handleRequest(std::unique_ptr<Command>)` must either handle and consume the command or forward it to successor using `successor->handleRequest(std::move(cmd))`.

## Serialization & Memento (guidance)

Goal: Memento must be able to restore a complete system snapshot (inventory, groups, component IDs, staff queue, command statuses and target IDs).

Recommended JSON shape (high-level):

{
  "day": 123,
  "components": [
    { "id": 1, "type": "Plant", "data": { /* plant fields */ } },
    { "id": 2, "type": "Group", "data": { "name": "Plot A", "owns": true } }
  ],
  "groups": [
    { "id": 2, "members": [1], "owns_mask": [true] }
  ],
  "commands": [
    { "type": "WaterPlantCommand", "status": "Pending", "targetId": 1, "payload": {} }
  ]
}

Implementation notes:
- Save: walk every component, call `component->clone()` for snapshot clones, serialize each clone into a map keyed by `id`.
- Restore: two-pass process:
  1. Create components from serialized entries and map their IDs to instances (but do not set owner/membership yet).
  2. Reconstruct groups and owners by reading membership arrays and setting `component->setOwner()` where applicable.
  3. Reconstruct command queue by creating commands and resolving `targetId` to the correct component `shared_ptr`.

Library choice:
- Probably gonna use `nlohmann/json` (single-header) for convenience. If someone thinks no external dependency, we would have to implement a small JSON serializer for the required fields.

## Error handling

- Decide whether deserialization errors throw exceptions or return error codes. Be consistent across all classes.
- Prefer throwing `std::runtime_error` for invalid serialized input; consumers (SaveSystem) can choose to catch and log.