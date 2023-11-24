#Build on a macbook pro m1 
!/bin/zsh
echo "Start Script!"
echo "Building!"

PROG_NAME=$1
IMGUI_DIR=include/imgui
GLFW_DIR=include/GLFW
output=$(g++ $PROG_NAME $IMGUI_DIR/imgui*.cpp \
  -std=c++20 -Wno-unused-result \
  -I/opt/homebrew/Cellar/sfml/2.6.1/include -I $IMGUI_DIR -I $GLFW_DIR \
  -L/opt/homebrew/Cellar/sfml/2.6.1/lib -lsfml-graphics -lsfml-window -lsfml-system  \
  -framework OpenGL \
  -o build.out \
  )

if [[ $? != 0 ]]; then
  echo -e "Error:$output"
else
  echo "Success build!! Running the output!"
  ./build.out
fi
echo "End Script!"
echo "executable is located ./build.out"

### Command to execute ->         ./build.sh main.cpp