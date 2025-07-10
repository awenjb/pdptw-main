# This is a load-dependent PDPTW solved with SLNS

Gitlab : https://gitlab.imt-atlantique.fr/a24jacqb/pdptw-main.git

This code is based on Nicolas Pierre's code for the 2E-VRP-TW

# Installation
 
## Requirements

You need a c++ compiler compatible with c++20. gcc 12 and msvc of end of 2023 worked.
You need to install cmake (>=3.25) and conan (>=2.0) and add them to your path. Both can be installed easily with pip !
Nothing to setup for cmake. For conan you must run once :

'''bash
conan profile detect
'''

and check the coherence of the values against what your machine/compiler is. If you want to modify it, check the conan documentation about profiles.

## Script setup

Run the script to install dependencies, and run a compilation once in each build type (release, debug, rel with deb info):
```bash
# on linux (or wsl)
./conan_build.sh
# on windows (in cmd not powershell)
conan_build.cmd
```

All executables will be located in `build/BUILD_TYPE` (`build/BUILD_TYPE/BUILD_TYPE` on windows...) with all the
compilation stuff (it should always compile directly inside with your compiler).
