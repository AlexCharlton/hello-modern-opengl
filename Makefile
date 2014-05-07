all: hello-gl

hello-gl: hello-gl.c
	gcc -Wall -o hello-gl hello-gl.c -lm -lGLEW -lGL -lglfw

clean:
	rm hello-gl
