#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>

#define PI 3.14159265358979323846

typedef struct mat4{
    float _11, _12, _13, _14,
	_21, _22, _23, _24,
	_31, _32, _33, _34,
	_41, _42, _43, _44;
} mat4;


// GL error checking
void checkError(){
    switch (glGetError()){
    case 0: return;
    case GL_INVALID_ENUM: fprintf(stderr, "GL error: Invalid enum\n"); break;
    case GL_INVALID_VALUE: fprintf(stderr, "GL error: Invalid value\n"); break;
    case GL_INVALID_OPERATION: fprintf(stderr, "GL error: Invalid operation\n"); break;
    case GL_OUT_OF_MEMORY: fprintf(stderr, "GL error: Out of memory\n"); break;
    default: fprintf(stderr, "GL error: Unknown\n");
    }
}

// Matrix math
float degToRad(float deg){
    return deg * PI / 180.0;
}

void initMat4(mat4 *m){
    memset(m, 0, sizeof(mat4));
}

void cross(float ax, float ay, float az, float bx, float by, float bz, float *rx, float *ry, float *rz){
    *rx = ay*bz - az*by;
    *ry = az*bx - ax*bz;
    *rz = ax*by - ay*bx;
}

void normalize(float x, float y, float z, float *rx, float *ry, float *rz){
    float len = sqrt(x*x + y*y + z*z);
    *rx = x / len;
    *ry = y / len;
    *rz = z / len;
}

float dot(float ax, float ay, float az, float bx, float by, float bz){
    return ax*bx + ay*by + az*bz;
}

void lookAt(float eyeX, float eyeY, float eyeZ, float x, float y, float z, float upX, float upY, float upZ, mat4 *m){
    float fx, fy, fz, ux, uy, uz, rx, ry, rz;
    initMat4(m);
    normalize(eyeX - x, eyeY - y, eyeZ - z, &fx, &fy, &fz);
    cross(upX, upY, upZ, fx, fx, fz, &rx, &ry, &rz);
    normalize(rx, ry, rz, &rx, &ry, &rz);
    cross(fx, fy, fz, rx, ry, rz, &ux, &uy, &uz);

    m->_11 = rx;
    m->_21 = ry;
    m->_31 = rz;
    m->_12 = ux;
    m->_22 = uy;
    m->_32 = uz;
    m->_13 = fx;
    m->_23 = fy;
    m->_33 = fz;
    m->_14 =-dot(rx, ry, rz, eyeX, eyeY, eyeZ);
    m->_24 =-dot(ux, uy, uz, eyeX, eyeY, eyeZ);
    m->_34 =-dot(fx, fy, fz, eyeX, eyeY, eyeZ);
    m->_44 = 1.0;
}

void frustum(float left, float right, float bottom, float top,
             float near, float far, mat4 *mat){
    initMat4(mat);
    mat->_11 = 2.0 * near / (right - left);
    mat->_13 =  (right + left) / (right - left);
    mat->_22 = 2.0 * near / (top - bottom);
    mat->_23 = (top + bottom) / (top - bottom);
    mat->_33 = -(far + near) / (far - near);
    mat->_34 = - (2.0 * far * near) / (far - near);
    mat->_43 = -1.0;
}

void mat4Print(mat4 *m){
    printf("[%f, %f, %f, %f,\n",   m->_11, m->_12, m->_13, m->_14);
    printf(" %f, %f, %f, %f,\n",   m->_21, m->_22, m->_23, m->_24);
    printf(" %f, %f, %f, %f,\n",   m->_31, m->_32, m->_33, m->_34);
    printf(" %f, %f, %f, %f]\n\n", m->_41, m->_42, m->_43, m->_44);
}

void perspective(int width, int height, float near, float far, float angle,
                 mat4 *mat){
    float scale = tan(degToRad(angle * 0.5)) * near;
    float r = ((float) width / (float) height) * scale;
    float l = -r;
    float t = scale;
    float b = -t;
    frustum(l, r, b, t, near, far, mat);
}

void Mat4Identity(mat4 *m){
    memset(m, 0, sizeof(mat4));
    m->_11 = 1.0;
    m->_22 = 1.0;
    m->_33 = 1.0;
    m->_44 = 1.0;
}

void Mat4Mult(mat4 *a, mat4 *b, mat4 *r){
    r->_11 = a->_11*b->_11 + a->_12*b->_21 + a->_13*b->_31 + a->_14*b->_41;
    r->_12 = a->_11*b->_12 + a->_12*b->_22 + a->_13*b->_32 + a->_14*b->_42;
    r->_13 = a->_11*b->_13 + a->_12*b->_23 + a->_13*b->_33 + a->_14*b->_43;
    r->_14 = a->_11*b->_14 + a->_12*b->_24 + a->_13*b->_34 + a->_14*b->_44;

    r->_21 = a->_21*b->_11 + a->_22*b->_21 + a->_23*b->_31 + a->_24*b->_41;
    r->_22 = a->_21*b->_12 + a->_22*b->_22 + a->_23*b->_32 + a->_24*b->_42;
    r->_23 = a->_21*b->_13 + a->_22*b->_23 + a->_23*b->_33 + a->_24*b->_43;
    r->_24 = a->_21*b->_14 + a->_22*b->_24 + a->_23*b->_34 + a->_24*b->_44;

    r->_31 = a->_31*b->_11 + a->_32*b->_21 + a->_33*b->_31 + a->_34*b->_41;
    r->_32 = a->_31*b->_12 + a->_32*b->_22 + a->_33*b->_32 + a->_34*b->_42;
    r->_33 = a->_31*b->_13 + a->_32*b->_23 + a->_33*b->_33 + a->_34*b->_43;
    r->_34 = a->_31*b->_14 + a->_32*b->_24 + a->_33*b->_34 + a->_34*b->_44;

    r->_41 = a->_41*b->_11 + a->_42*b->_21 + a->_43*b->_31 + a->_44*b->_41;
    r->_42 = a->_41*b->_12 + a->_42*b->_22 + a->_43*b->_32 + a->_44*b->_42;
    r->_43 = a->_41*b->_13 + a->_42*b->_23 + a->_43*b->_33 + a->_44*b->_43;
    r->_44 = a->_41*b->_14 + a->_42*b->_24 + a->_43*b->_34 + a->_44*b->_44;
}


// Shader compilation
static void showInfoLog(GLuint object){
    GLint logLength;
    char *log;
    glGetShaderiv(object, GL_INFO_LOG_LENGTH, &logLength);
    log = (char*) malloc(logLength);
    glGetShaderInfoLog(object, logLength, NULL, log);
    fprintf(stderr, "%s\n", log);
    free(log);
}

static GLuint makeShaderObject(GLenum type, const char *source){
    GLuint shader;
    GLint shaderOk;
    shader = glCreateShader(type);
    glShaderSource(shader, 1, (const GLchar**)&source, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderOk);
    if (!shaderOk) {
	fprintf(stderr, "Failed to compile %s:\n", source);
	showInfoLog(shader);
	glDeleteShader(shader);
	return 0;
    }
    return shader;
}

unsigned int makeShader
(const char *vertexSource, const char *fragmentSource){
    GLint programOk;
    GLuint vertexShader, fragmentShader = 0;
    vertexShader = makeShaderObject(GL_VERTEX_SHADER, vertexSource);
    fragmentShader = makeShaderObject(GL_FRAGMENT_SHADER, fragmentSource);
    if (!vertexShader || !fragmentShader){
	return 0;
    }
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &programOk);
    if (!programOk) {
	fprintf(stderr, "Failed to link shader program:\n");
	showInfoLog(program);
	glDeleteShader(vertexShader); glDeleteShader(fragmentShader);
	glDeleteProgram(program);
	return 0;
    }
    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(vertexShader); glDeleteShader(fragmentShader);
    return program;
}


// Main functions
GLFWwindow* window;

GLuint vertexArrayID;
GLuint programID;
GLuint MVPlocation;

mat4 MVP;
mat4 view;
mat4 projection;

GLfloat vertexBufferData[] = {
    -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f
};
GLushort indexBufferData[] = { 0, 1, 2 };

static char *vertShader = "\
#version 120\n\n\
attribute vec3 vertex;\n\
attribute vec3 color;\n\
uniform mat4 mvp;\n\
varying vec3 c;\n\
void main(){\n\
    gl_Position = mvp * vec4(vertex, 1.0);\n\
    c = color;\n\
}";

static char *fragShader = "\
#version 120\n\n\
varying vec3 c;\n\
vec4 color;\n\
void main(){\n\
    gl_FragColor = vec4(c, 1.0);\n\
}";

void init(){
    // Init MVP matrix
    MVPlocation = glGetUniformLocation(programID, "mvp");

    perspective(4.0f, 3.0f, 0.1f, 100.0f, 45.0, &projection);
    lookAt(0.0, 0.0, 3.0,
           0.0, 0.0, 0.0,
           0.0, 1.0, 0.0,
           &view);
    Mat4Mult(&projection, &view, &MVP);

    mat4Print(&MVP);

    // Init VAO
    GLuint vertexBuffer, indexBuffer;
    glGenVertexArrays(1, &vertexArrayID);
    glGenBuffers(1, &vertexBuffer);
    glGenBuffers(1, &indexBuffer);

    glBindVertexArray(vertexArrayID);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData),
                 vertexBufferData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 24, (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 24, (void*)12);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexBufferData),
                 indexBufferData, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
}

void render(){
    glUseProgram(programID);
    glUniformMatrix4fv(MVPlocation, 1, GL_TRUE, (float *) &MVP);
    glBindVertexArray(vertexArrayID);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL);
    glBindVertexArray(0);
}

int main(void){
    if(!glfwInit()){
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return -1;
    }
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    /* You'd think that you should do this, but it fails:
       https://github.com/glfw/glfw/issues/19 */
    //glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    /* This is needed, for some reason, or the window won't open: */
    glfwWindowHint(GLFW_ALPHA_BITS, 0);
    window = glfwCreateWindow( 640, 480, "Hello GL ES", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. Probably due to hardware or drivers not being OpenGL 3.3 compatible..\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    programID = makeShader(vertShader, fragShader);
    init();
    do{
        glClear(GL_COLOR_BUFFER_BIT);
        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
        checkError();
    }
    while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0);
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteProgram(programID);
    glfwTerminate();
    return 0;
}
