all: main.exe

main.exe: main.o Shader.o ShaderProgram.o
	g++ main.o Shader.o ShaderProgram.o -o main.exe -Iinclude -lGL -lGLU -lSDL2

main.o: src/main.cpp
	g++ -c src/main.cpp -Iinclude -lGL -lGLU -lSDL2

Shader.o: src/Shader.cpp
	g++ -c src/Shader.cpp -Iinclude -lGL -lGLU -lSDL2

ShaderProgram.o: src/ShaderProgram.cpp
	g++ -c src/ShaderProgram.cpp -Iinclude -lGL -lGLU -lSDL2

clean:
	rm -rf *.exe *.o
