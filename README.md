# OpenGL Learning

Learn OpenGL

## Compile

### MSYS2

* Dependencies
```
pacman -S mingw-w64-i686-freeglut 3.2.1-1
pacman -S mingw-w64-i686-glew 2.1.0-1
pacman -S mingw-w64-i686-libpng
```

```
g++ -O3 -std=c++11 main.cpp -lfreeglut -lopengl32 -lglu32 -lglew32 -lm -o main && ./main.exe
```

### macOS

```
g++ -O3 -std=c++11 main.cpp -framework OpenGL -framework GLUT
```
