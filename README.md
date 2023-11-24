COMP4300

Game Programming

ECS - Entities, Components, Systems   software design paradigm

 Entity: any object in the game (e.g. player, platform, tile, bullet, enemy)
 Components: properties attached to entities (e.g. position,texture, animation, health, gravity) PURE DATA
 Systems: code / logic that drives behavior (e.g. movement, rendering, sound, physics)

Engine Architecture:

GameEngine
 > Scene
  > Systems
  > EntityManager
   > Entity
    > Component

 -> OPP for ECS design <-

RAII
Resource Acquisition is Initialization
Binds the life cycle of  a resource  that must be acquired before use to lifetime of an object
 -> new analogy EAR ( Encapsulate, acquire, release)

Can be cumbersome to create our own  RAII classes for every data type we want to store a pointer to
use std::shared_ptr< T > , which handles RAII for a given data type,
﻿and inc/dec every function it holds a pointer to. Making safe the end of parallel scopes ending in async time.

Shared Pointer
Reference Counted Pointer
Internal counter set to 1 in constructor
Every time the shared_ptr is copied, the counter increases by 1.
Every time a shared_ptr destructs, the counter is decremented by 1.
When count reaches 0, resource deallocated.

What is a Component?

An ECS Component is just DATA!
(Maybe some logic in the constructor (e.g. convert data types in constructor call))
No helper functionality within components, don’t do game logic.
A component class has some intuitive meaning to an Entity which contains it
Ex: Position, Gravity, Health, etc.
