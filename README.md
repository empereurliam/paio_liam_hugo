# HackaGames v0.1.0 - An Hackable Games' Engine

![](resources/logo-128.png)

**HackaGames** aims to be a *C* game engine dedicated for the development of Artificial Intelligence (AIs) in any languages.
The project is based on a multimedia library *RayLib* for rendering and is developed accordingly to [KISS](https://fr.wikipedia.org/wiki/Principe_KISS)  (Keep It Stupid Simple) principle.
The main feature of this project is to model the world as a tabletop seen as a collection of cells connected together on with 
interacting entities (Pieces) can be moved.
Players and AIs would be responsible for Pieces’ behaviors and can be implemented in independent programs in any language of your choice.
In other terms, **HackaGames** implement a simple client/server architecture to permit AI to take a seat on a game through a simple network protocol.

**HackaGames** is seen as an API for game development.
Several games are proposed with the API for example

- **421**: A very simple one player dice game to get the concept of AI implementation (not a core HackaGames client/server game).
- **RISKY**: A turn-based strategic game where players control armies fighting for a territory.

## Concurency:

**HackaGame** is not what you looking for ? Try those solutions:

- https://ludii.games/ "a general game system designed to play, evaluate and design a wide range of games" (JAVA)
- https://www.pommerman.com/ an hackable Bomberman game (Python)
- https://www.codingame.com web-based environment for *NPC* development (complete solution but not open).

## License

**HackaGame** is distributed under the [MIT license](./LICENCE.md).

## Installation (Linux)

**HackaGames** is natively developed on and for Linux systems.
Commands are given regarding Ubuntu-like distribution.

The classical way to get **HackaGames** is to clone then buid the project.
So first, you can clone this repository (game engine plus games):

```bash
git clone https://bitbucket.org/imt-mobisyst/hackagames.git
cd hackagames
```

### Dependencies:

**HackaGame** requires C/C++ development tools, CMake and [RayLib dependencies](https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux)

```bash
sudo apt update
sudo apt install -y \
  build-essential git cmake \
  libasound2-dev mesa-common-dev \
  libx11-dev libxi-dev xorg-dev \
  libgl1-mesa-dev libglu1-mesa-dev
```

Then get and build RayLib (version `3.0.0`):

```bash
./bin/get-raylib.sh
```

<!--
And finally mosquitto:

```bash
sudo apt-add-repository ppa:mosquitto-dev/mosquitto-ppa
sudo apt update
sudo apt install mosquitto mosquitto-dev
```
-->


### **HackaGame**

A simple build scripts would generate the overall project:

```bash
./bin/build
```

It will enter each HackaGames submodules including games and build them with cmake, starting by hackagames libraries and tools.

To build only the librairies and tools, enter `hackagames` directopy and build it with cmake.

```sh
cd hackagames
cmake ..
make
```

## Getting started

The easiest way is to enter in one of the example games as [risky](./game-risky) .

Start a game `./play-risky.py` and read the `game-risky/README.md`.

## In this repository

Directories:

- *bin* : scripts for project management.
- *.git* : git version management directory.
- *dpd* : (generated files) included dependencies (RayLib and potentionnaly in the future: Wanda / Mosquitto / Igraph / ... )
- *doc* : some documentation of the project (to be generated).
- *hackagames* : Librairy and tools like interfaces in different programing language to help connect a game.
- *resources* : some resources to illustrate **HackaGames** project.
- *game-** : game examples on the top of **HackaGames** API.

Root Files:

- *README.md* : Your servitor.
- *LICENCE.md* : The Applied MIT license.
- *CMakefile* : Instructions for `CMake` construction

### Contributors

- Permanent contributor:
  * **Guillaume LOZENGUEZ** - [guillaume.lozenguez@imt-norrd-europe.fr](mailto:guillaume.lozenguez@imt-norrd-europe.fr)
- 1st version of Risky game: **Ewen MADEC** and **Timothy LAIRD** (April 2021)
