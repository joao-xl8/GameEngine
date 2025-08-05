COMP4300 - (https://www.youtube.com/@DaveChurchill/videos) 
Credits to Dave Churchill

Game Programming

ECS - Entities, Components, Systems   software design paradigm

 Entity: any object in the game (e.g. player, platform, tile, bullet, enemy)
 Components: properties attached to entities (e.g. position,texture, animation, health, gravity) PURE DATA
 Systems: code / logic that drives behavior (e.g. movement, rendering, sound, physics)

Engine Architecture:

Game
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

Data Management

One of the most important concepts: separate logic & data
Create data structures which manage data so your logic code doesn't worry about it
Complex algorithm shouldn't have to know whether set, vector, map implementation

Entity Manager - Factory design pattern (Will trade some memory for functionality)

Handle the creation, storage, and lifetime of ALL Entity objects
Functions like: add, remove, storage, bookkeeping
Factory should somehow ensure that user cannot create its own Entities
Entity Tags: We may want to group Entities by functionality(e.g. Tile, Player, Bullet, Enemy or an int for optimization). Entity Manager should have functionality for quickly getting Entities of a given tag
Will also store separate data structures(vectors) of Entity objects by their tag for quick retrieval. We
store double the number of pointers to save on computation in our engine

Be careful with Iterator Invalidation

Caused by modifying the contents of a collection as we are iteration through it. And in games this happens
all the time (e.g. spawn explosion entity, remove entities that died). One way to avoid iterator invalidation is to delay the effects of actions that modify collections until it is safe to do so

2D Math

2D Games - Every game's graphics are drawn to a screen, screens are a plane (2D). Screens are made of small,
discrete elements: pixels, which are (usually) squares and have RGB elements.

Color Space - RGB, RGBA, CMYK, Grayscale, Monochrome

2D Positions - Systems display: pixel grid. Grid is in two dimensions (x,y) and we can use Euclidean plane to
visualize individual positions in which each integer position in a display pixel. Most APIs have (0,0)
in top-left
   basic notes:
      a 4,2
      b 6,7
      subtraction
         destination - origin = distance
      addition
         origin + distance = destination
      SOH CAH TOA
      sin Ø = opposite / hypotenuse
      cos Ø = adjacent / hypotenuse
      tan Ø = opposite / adjacent

      Unit Circle
      Opposite = Y
      Adjacent = X
      Hypotenuse = R
      sin(Ø) = Y/R = Y
      cons (Ø) = X/R = X
      tan(Ø) = Y/X = R

      Vector Normalization

      lenght - L = sqrtf(x*x + y*y);

      Normalized - Vec2(V.x/L, V.y/L)
