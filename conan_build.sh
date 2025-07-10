#!/bin/bash
ENVS="Debug Release RelWithDebInfo"
for build_type in $ENVS; do
  BUILD_DIR="build/$build_type"
  mkdir -p "$BUILD_DIR"
  echo -e "building for $build_type\n"
  conan install . --output-folder="$BUILD_DIR" --build=missing --settings=build_type="$build_type"
  cd "$BUILD_DIR" || (echo "cannot go into cmake dir" && exit 1)
  cmake ../.. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE="$build_type"
  cmake --build . -j
  cd ../..
done