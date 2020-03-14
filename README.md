# openGL_learning
learn openGL

## Compile

### WINDOWS MSYS2

### MSYS2-install
pacman -S mingw-w64-i686-freeglut 3.2.1-1
pacman -S mingw-w64-i686-glew 2.1.0-1
pacman -S mingw-w64-i686-libpng

### compile parameter
g++ -O3 -std=c++11 main.cpp -lfreeglut -lopengl32 -lglu32 -lglew32 -lm -o main && ./main.exe