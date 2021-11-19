# Methan
Methan is a C++ library aimed at creating computational graph and optimising such graph throughout parallelisation power.

## Table Of Content
- [Methan](#methan)
  - [Table Of Content](#table-of-content)
  - [Installation](#installation)
  - [Authors](#authors)

## Installation

The installation of the most basic configuration of Methan is really straitforward only using git and cmake. 

First we need to download the repository through the command interface. Methan also depends on submodules that need to be download along.
```sh
git clone https://github.com/BoyeGuillaume/methan.git
cd methan
git submodule update --init --recursive
```

Secondly we can use cmake to build and install to \<path_to_install\> Methan
```sh
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=<path_to_install> -DMETHAN_BUILD_SHARED=ON
cmake --build . --target install
```

## Authors
The library has been made possible thanks to
* BoyeGuillaume
