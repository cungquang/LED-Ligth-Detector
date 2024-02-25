**#Simulation of Light Dips Detector**

## Project Description

The purpose of this application is simulation process of LED light Dip Detector.
- A "Dip" is when the light level drops below a threshold light value (0.1V below the current average light level):
  - A "Dip" can be detected when the voltage is 0.1V or more away from the current average light level.
  - Another "Dip" cannot be detected until the light level return above the threshold (use hyteresis 0.03V to prevent the noise to re-trigger incorrectly)

The application includes 

## Sturcture

- `hal/`: Contains all low-level hardware abstraction layer (HAL) modules
- `app/`: Contains all application-specific code. Broken into modules and a main file
- `build/`: Generated by CMake; stores all temporary build files (may be deleted to clean)

```
  .
  ├── app
  │   ├── include
  │   │   └── badmath.h
  │   ├── src
  │   │   ├── badmath.c
  │   │   └── main.c
  │   └── CMakeLists.txt           # Sub CMake file, just for app/
  ├── hal
  │   ├── include
  │   │   └── hal
  │   │       └── button.h
  │   ├── src
  │   │   └── button.c
  │   └── CMakeLists.txt           # Sub CMake file, just for hal/
  ├── CMakeLists.txt               # Main CMake file for the project
  └── README.md
```  

Note: This application is just to help you get started! It also has a bug in its computation (just for fun!)

## Usage

- Install CMake: `sudo apt update` and `sudo apt install cmake`
- When you first open the project, click the "Build" button in the status bar for CMake to generate the `build\` folder and recreate the makefiles.
  - When you edit and save a CMakeLists.txt file, VS Code will automatically update this folder.
- When you add a new file (.h or .c) to the project, you'll need to rerun CMake's build
  (Either click "Build" or resave `/CMakeLists.txt` to trigger VS Code re-running CMake)
- Cross-compile using VS Code's CMake addon:
  - The "kit" defines which compilers and tools will be run.
  - Change the kit via the menu: Help > Show All Commands, type "CMake: Select a kit".
    - Kit "GCC 10.2.1 arm-linux-gnueabi" builds for target.
    - Kit "Unspecified" builds for host (using default `gcc`).
  - Most CMake options for the project can be found in VS Code's CMake view (very left-hand side).
- Build the project using Ctrl+Shift+B, or by the menu: Terminal > Run Build Task...
  - If you try to build but get an error about "build is not a directory", the re-run CMake's build as mentioned above.

## Address Sanitizer

- The address sanitizer built into gcc/clang is very good at catching memory access errors.
- Enable it by uncomment the `fsanitize=address` lines in the root CMakeFile.txt.
- For this to run on the BeagleBone, you must run:
  `sudo apt install libasan6`
  - Without this installed, you'll get an error:   
    "error while loading shared libraries: libasan.so.6: cannot open shared object file: No such file or directory"

## Manually Running CMake

To manually run CMake from the command line use:

```shell
  # Regenerate build/ folder and makefiles:
  rm -rf build/         # Wipes temporary build folder
  cmake -S . -B build   # Generate makefiles in build\

  # Build (compile & link) the project
  cmake --build build
```
