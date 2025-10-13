# ECS Implementation Shortcomings

## Fragile lifetime management in `World`
* `World` stores raw pointers for both the global allocator and every component pool, and the destructor manually deletes them via `destructorMap` and a raw `delete` for the allocator.【F:ShootFast/Header/Independent/ECS/World.hpp†L18-L94】【F:ShootFast/Source/Independent/ECS/World.cpp†L1-L37】
* Because the class relies on the compiler-generated copy and move operations, copying a `World` would duplicate the raw pointers and trigger double-frees or dangling references when either copy is destroyed.【F:ShootFast/Header/Independent/ECS/World.hpp†L18-L94】

## Type-unsafe component storage
* `World` erases the component pool type into a `void*` stored in `poolMap`, with bookkeeping lambdas in separate maps to recover the type-specific operations.【F:ShootFast/Header/Independent/ECS/World.hpp†L58-L94】
* Any mismatch between the stored pointer and the helper lambdas would only surface as undefined behaviour at runtime, and there is no compile-time guarantee that the correct deleter/remover is used.

## Limited component construction
* `ComponentPool::Emplace` only accepts a `const T&` and copies the component into the dense storage, so move-only types or expensive-to-copy components cannot be added to the world.【F:ShootFast/Header/Independent/ECS/ComponentPool.hpp†L17-L61】
* Re-adding an existing component also copies into the dense array instead of constructing in-place, preventing perfect forwarding of constructor arguments.【F:ShootFast/Header/Independent/ECS/ComponentPool.hpp†L17-L41】

## Missing safety checks in the allocator
* `GameObjectAllocator::Destroy` blindly pushes the id into `freeList`, allowing the same id to be recycled multiple times if `Destroy` is called twice for the same entity, which would later hand out duplicate ids.【F:ShootFast/Source/Independent/ECS/GameObjectAllocator.cpp†L5-L21】

## Dead code / incomplete functionality
* `World` declares helper templates to pick the smallest component pool for iteration, but the implementation in `World.cpp` references a non-existent `GetOwnerList()` accessor, suggesting the code is incomplete and would not compile if instantiated.【F:ShootFast/Header/Independent/ECS/World.hpp†L70-L88】【F:ShootFast/Source/Independent/ECS/World.cpp†L39-L48】【F:ShootFast/Header/Independent/ECS/ComponentPool.hpp†L55-L61】
