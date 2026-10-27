# Amphibious Robot Simulator

Ground robot, aerial drone and amphibious robot in C++ using virtual and multiple inheritance.

## Features

- Vehicle base class with battery and position
- GroundRobot and AerialDrone using virtual inheritance
- Amphibious robot that inherits both and switches between GROUND and AIR mode (solves the diamond problem)

## How to run

```bash
g++ amphibious_robot.cpp -o amphibious_robot
./amphibious_robot
```

## About

Built in **C++** for the OOP - case study 1 course, Robotics and Artificial Intelligence Engineering.
