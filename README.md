# Bens Snake Game

This is a simple snake game made with C++ and SDL2.<br>
The intention of this project is to try out the SDL2 library.

Furthermore, this game provides funny textures, sounds and a highscore display.

## How to play

The snake is controlled by the arrow buttons of the keyboard.

The goal is to get as many apples as possible to reach a new highscore.

## Requirements

### Required tools

- gcc compiler
- cmake

For Debian distributions, cmake can be installed like this.

```
apt install cmake
```

### Required libraries

- SDL2
- SDL2-image
- SDL2-ttf
- SDL2-mixer

For Debian distributions, these libraries can be installed like this.

```
apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev
```

## Build the game

With all required tools and libraries installed, the build can be executed like this.

```
cmake -B build
cmake --build build
```

## Run the game

After the game is built successfully, it can be started like this.

```
cd ./build
./Bens-Snake-Game
```

To run the game in windowed mode, this can be accomplished by passing the desired resolution as argument `<width>x<height>`, for example:<br>
`./Bens-Snake-Game 800x600`