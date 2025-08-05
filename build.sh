#Build on a macbook pro m1
#!/bin/zsh
echo "Start Script!"
echo "Building!"

PROG_NAME=$1
IMGUI_DIR=include/imgui
SFML_PREFIX=/opt/homebrew/opt/sfml@2
GLFW_DIR=include/GLFW
ROOT_DIR=src
CPPFLAGS="-v -Wall -Wunused-result"

echo "Using SFML from: $SFML_PREFIX"

output=$(g++ main.cpp $IMGUI_DIR/imgui*.cpp $ROOT_DIR/*.cpp \
  -std=c++20 \
  -I $SFML_PREFIX/include -I $IMGUI_DIR -I $GLFW_DIR -I $ROOT_DIR \
  -L $SFML_PREFIX/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio \
  -framework OpenGL \
  -o build.out \
  2>&1)

if [[ $? != 0 ]]; then
  echo -e "Error: $output"
else
  echo "Success build!! Running the output!"
  ./build.out
fi
echo "End Script!"
echo "Executable is located ./build.out"

### Command to execute -> ./build.sh main.cpp <-
