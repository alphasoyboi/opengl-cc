#include <cstring>
#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

char*  string_from_file(const char* file_path);
GLuint shader_create   (const char* file_path);
GLuint program_create  (GLuint* shaders, size_t shader_num);

int main()
{
	if (!glfwInit()) {
		std::cout << "could not initialize GLFW\n";
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	GLFWmonitor* mon = glfwGetPrimaryMonitor();
	const GLFWvidmode* vmode = glfwGetVideoMode(mon);
	const char* win_title = "opengl";
	GLFWwindow* win = glfwCreateWindow(640, 480, win_title, NULL, NULL);
	//GLFWwindow* win = glfwCreateWindow(vmode->width, vmode->height, win_title, mon, NULL);
	if (!win) {
		std::cout << "could not create GLFW window\n";
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	std::cout << "GLFW version:\t\t" << glfwGetVersionString() << "\n";
	glfwMakeContextCurrent(win);

	if (!gladLoadGL()) {
		printf("could not load opengl\n");
		exit(EXIT_FAILURE);
	}
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version  = glGetString(GL_VERSION);
	std::cout << "OpenGL renderer:\t" << renderer << "\n";
	std::cout << "OpenGL version:\t\t" << version << "\n";

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	GLfloat verts_first[] = {
		 0.5f,  0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
	};
	GLfloat verts_second[] = {
		-0.5f,  0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
	};

	GLuint vbo_first;
	glGenBuffers(1, &vbo_first);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_first);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts_first), verts_first, GL_STATIC_DRAW);
	GLuint vbo_second;
	glGenBuffers(1, &vbo_second);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_second);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts_second), verts_second, GL_STATIC_DRAW);

	GLuint vao;
	glGenVertexArrays        (1, &vao);
	glBindVertexArray        (vao);
	glEnableVertexAttribArray(0);
	glBindBuffer             (GL_ARRAY_BUFFER, vbo_first);
	glVertexAttribPointer    (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer             (GL_ARRAY_BUFFER, vbo_second);
	glVertexAttribPointer    (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	GLuint shaders[2];
	shaders[0] = shader_create("res/shaders/simple.vert");
	shaders[1] = shader_create("res/shaders/simple.frag");
	GLuint prog = program_create(shaders, 2);

	while (!glfwWindowShouldClose(win)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(prog);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);

		glfwPollEvents();
		glfwSwapBuffers(win);
		
		if (GLFW_PRESS == glfwGetKey(win, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(win, true);
		}
	}

	glfwDestroyWindow(win);
	glfwTerminate();

	return 0;
}

//	Returns heap allocated, null terminated, char array.
//	Example: 
//		char str = string_from_file("path/to/file");
//		...
//		delete[] str;
char* string_from_file(const char* file_path)
{
	std::ifstream file;
	file.open(file_path, std::ifstream::binary);

	if (!file.is_open()) {
		std::cout << "could not open file: " << file_path << "\n";
		return nullptr;
	}

	// Get file length.
	file.seekg(0, file.end);
	size_t len = file.tellg();
	file.seekg(0, file.beg);

	char* buf = new char[len + 1]; // Allocate memory, plus 1 for null terminator.
	file.read(buf, len);           // Read data as a block.
	buf[len] = '\0';               // Add null terminator.

	file.close();
	return buf;
}

//	Assumes shader type and returns a new compiled shader ID given a, null terminated c-string, file path.
//	The file should have one of the following extension:
//		* .vert
//		* .frag
//		* .geom
//		* .tesc
//		* .tese
GLuint shader_create(const char* file_path)
{
	// Determine the shader type by checking the last character in the file path.
	// This is not foolproof, so don't be a fool and give the file the right extension.
	GLenum type;
	char last_char = file_path[strlen(file_path) - 1]; // Get last character in the file path before the null terminator.
	switch (last_char) {
		case 't':
			type = GL_VERTEX_SHADER;
			break;
		case 'g':
			type = GL_FRAGMENT_SHADER;
			break;
		case 'm':
			type = GL_GEOMETRY_SHADER;
			break;
		case 'c':
			type = GL_TESS_CONTROL_SHADER;
			break;
		case 'e':
			type = GL_TESS_EVALUATION_SHADER;
			break;
		case 'p':
			std::cout << "compute shaders not supported: " << file_path << " not loaded\n";
			return 0;
		default:
			std::cout << "unknown shader type: " << file_path << " not loaded\n";
			return 0;
	}
	GLuint shader = glCreateShader(type);

	// Get, and load, shader source.
	char* shader_source = string_from_file(file_path);
	if (!shader_source) {
		return 0;
	}
	glShaderSource(shader, 1, &shader_source, NULL);
	delete[] shader_source;

	// Compile and check for errors.
	glCompileShader(shader);
	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		// Create log message buffer.
		GLsizei log_len;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
		GLchar* log_msg = new char[log_len];

		// Get message and print.
		glGetShaderInfoLog(shader, log_len, NULL, log_msg);
		std::cout << "could not compile shader, " << file_path << ":\n" << log_msg << "\n";
		delete[] log_msg;
		return 0;
	}

	return shader;
}

GLuint program_create(GLuint* shaders, size_t shader_num)
{
	GLuint prog = glCreateProgram();
	for (int i = 0; i < shader_num; i++) {
		glAttachShader(prog, shaders[i]);
	}

	glLinkProgram(prog);
	GLint linked;
	glGetProgramiv(prog, GL_LINK_STATUS, &linked);
	if (!linked) {
		// Create log message buffer.
		GLsizei log_len;
		glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &log_len);
		GLchar* log_msg = new char[log_len];

		// Get message and print.
		glGetProgramInfoLog(prog, log_len, NULL, log_msg);
		std::cout << "could not link program, #" << prog << ":\n" << log_msg << "\n";
		delete[] log_msg;
		return 0;
	}

	return prog;
}