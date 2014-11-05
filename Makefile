all: hello-gl

hello-gl: hello-gl.c
	gcc -Wall -o hello-gl hello-gl.c -lm -lGLESv2 -lglfw -DGLFW_INCLUDE_ES3

clean:
	rm hello-gl
