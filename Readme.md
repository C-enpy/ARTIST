![Build, Test](https://github.com/C-enpy/ARTIST/actions/workflows/cmake-single-platform.yml/badge.svg) [![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=C-enpy_ARTIST&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=C-enpy_ARTIST) [![codecov](https://codecov.io/gh/C-enpy/ARTIST/graph/badge.svg?token=I6X63MBCC7)](https://codecov.io/gh/C-enpy/ARTIST)


# A.R.T.I.S.T. Project - README

## Overview
The A.R.T.I.S.T. (Abstract Rendering Toolkit & Interface for Shader Technology) project is a modern, low-level graphic interface library designed for C++23, offering a cross-platform solution for desktop and mobile platforms. It aims to facilitate the creation of various rendering pipelines, such as single-pass, multi-pass, PBR, and particle systems, through a highly customizable and composition-based approach. Utilizing templates and meta-programming, A.R.T.I.S.T. allows compile-time resolution of pipeline configurations, making it a powerful tool for developers seeking flexibility and performance in graphical applications.

## Purpose and Context
A.R.T.I.S.T. addresses the need for a modular, abstract, and efficient rendering interface in C++ that supports diverse graphical requirements and optimizations across different platforms and APIs. It is particularly suited for developers working on graphical applications requiring customizable rendering pipelines, including games, simulations, and visualization tools.

## Requirements
- **Compiler**: GCC 13.2 or later
- **Build System**: CMake 3.25 or later
- **Package Manager**: Conan 1.62.0
- **Dependencies**: All dependencies are listed in `conanfile.txt`. Ensure to visit each dependency's website or GitHub repository for more information.

## Installation and Build Instructions
1. Update and install essential packages:
   ```sh
   sudo apt-get update
   sudo apt-get install pkg-config ninja-build tree -y
   sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 60
   sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 60
   sudo update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-13 60
   sudo pip install conan==1.62.0
   ```
2. Configure Conan:
   ```sh
   conan profile new default --detect
   # Add the following configurations to ~/.conan/profiles/default
   echo "[conf]\ntools.system.package_manager:sudo=True\ntools.system.package_manager:sudo_askpass=True\ntools.system.package_manager:mode=install" >> ~/.conan/profiles/default
   ```
3. Build the project using CMake:
   ```sh
   cmake --preset {platform}-debug -B ./build
   cmake --build ./build --config Debug
   ```

## Design and Architecture
The A.R.T.I.S.T. project's design and architecture are founded on a highly modular and composition-based approach, enabling extensive customization and flexibility. It employs a sophisticated template and meta-programming techniques to facilitate compile-time resolution of rendering pipeline configurations, optimizing performance and adaptability for various graphical applications.

At the core of A.R.T.I.S.T. are generic pipeline classes such as `Pipeline`, `Pass`, `Shader`, `Uniform`, and `Attribute`, designed to be highly customizable through composition. This design allows developers to define specialized rendering pipelines, tailored to specific needs and hardware capabilities, by composing these basic building blocks in novel ways.

The use of C++20's `requires` keyword for compile-time validation plays a crucial role in ensuring the integrity and compatibility of components within the rendering pipeline. This mechanism allows the library to validate the existence of API-specific implementations and operational methods dynamically, adapting to different APIs and profiles without runtime overhead.

For implementing new APIs and profiles, A.R.T.I.S.T. mandates the definition of API-specific components and validators, ensuring that each pipeline step is optionally included based on the chosen API implementation. This approach mitigates potential compilation errors due to missing component implementations, demonstrating the library's robustness and flexibility.

To extend the library, developers must create API and profile-specific components within designated directories, adhering to the project's structured approach to modular design and architecture. This ensures seamless integration and scalability of the A.R.T.I.S.T. library across different graphical applications and platforms.

## Implementing New APIs

To integrate a new API, such as Vulkan, into the A.R.T.I.S.T. framework, developers must follow a structured approach. Start by creating a new class within the `api` namespace specifically for Vulkan. This class should include forward declarations for the required `vulkan::context::*Context` classes, which in turn, encapsulate the Vulkan-specific context data and components necessary for rendering.

Each `*Context` class must contain forward declarations of the components it utilizes, ensuring that all Vulkan-specific implementations are correctly abstracted and encapsulated. Additionally, developers need to implement validators tailored to Vulkan's unique features and requirements, which will verify the correctness and compatibility of components at compile-time, leveraging C++20's `requires` keyword.

## Adding Profiles for APIs

For adding new profiles, it's advised to start by declaring new profiles, potentially using an enumeration, within `graphic/{api}/profile/*.hpp`. This establishes a clear, extendable method for defining various profiles under a specific API like Vulkan.

Subsequently, specialized implementations for these profiles need to be created in `graphic/{api}/pipeline/component/{pipeline,pass,shader,attribute,uniform}/*.hpp`. This involves crafting Vulkan-specific versions of each pipeline component, adhering to the requirements and optimizations particular to Vulkan. Through this detailed and structured approach, A.R.T.I.S.T. can accommodate a wide range of rendering techniques and optimizations, ensuring high performance and flexibility across different hardware and software environments.