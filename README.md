# nNewton (WIP)

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)]() [![WIP](https://img.shields.io/badge/status-work--in--progress-orange)]()

> ⚠ **Work in Progress**  
>  **This project is a work in progress.The API and functionality may change significantly. Use at your own risk.**
## About  
  
nNewton is a lightweight real-time physics library written in modern C++.  
It is designed to provide a lightweight, Modern and efficient physics simulation backend for games, simulations, and interactive applications.
           
> nNewton is implemented as a **static C++ library**.  
> The engine itself does not depend on any rendering system. Visualization and debugging tools are implemented in a separate **SandBox** application.

## Features

### Implemented

**Core Library**  
- Math utilities ([nMath](https://github.com/mayankbairagi0312/nMath))
- Dynamic World  

**Sandbox / Testbed**  
- Window system  
- Keyboard and mouse input  
- Camera controls  
- Debug rendering interface  
- Debug visualization

### In Progress

- Collision Shapes 
- Broadphase collision detection
- Rigid body system

## File Structure
```
├───external
├───include
│   └───nNewton
├───SandBox
│   ├───assets
│   ├───Core
│   ├───DebugUI
│   ├───PhysicsSystem
│   ├───Renderer
│   └───tests
│───src
│   ├───collision
│   │   ├───Collision_Broadphase
│   │   └───Collision_NarrowPhase
│   ├───common
│   ├───dynamics
│   ├───math
│   └───solver
└───README
```
## Requirements

- **C++20** or later
- Compiler: GCC 10+, Clang 12+, MSVC 2022+


----

