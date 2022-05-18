#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

struct ShaderProgramSource {
	string VertexSource;
	string FragmentSource;
};

// Lit le fichier shader et stocke dans un buffer le vertex et fragment shader
static ShaderProgramSource ParseShader(const string& filepath) {
	ifstream stream(filepath);

	enum class ShaderType {
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	string line;
	stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line)) {
		if (line.find("#shader") != string::npos) {
			if (line.find("vertex") != string::npos) {
				//on est dans le vertex shader
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != string::npos) {
				//on est dans le fragment shader
				type = ShaderType::FRAGMENT;
			}
		}
		else {
			ss[(int)type] << line << '\n';
		}
	}
	return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const string& source) {
	//crée un shader et retourne on id
	unsigned int id = glCreateShader(type);
	//lecture des fichiers
	const char* src = &source[0];
	glShaderSource(id, 1, &src, nullptr);
	//compilation du code
	glCompileShader(id);

	//vérification des erreurs
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		cout << "Failed to compile shader!" << endl;
		cout << message << endl;

		glDeleteShader(id);
		return 0;
	}


	return id;
}

static int CreateShader(const string& vertexShader, const string& fragmentShader) {
	//crée un Program Shader et retourne son identifiant
	unsigned int program = glCreateProgram();
	//compile les shaders
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
	//linkage des shaders au programme
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	//lie le program shader au programme
	glLinkProgram(program);
	//vérifie si ca a fonctionné
	glValidateProgram(program);
	//détruit les shaders
	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}


int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);


	if (glewInit() != GLEW_OK)
		cout << "Error" << endl;

	cout << glGetString(GL_VERSION) << endl;

	//positions des vertex du triangle
	float positions[6] = {
		-0.5f, -0.5f,
		 0.0f,  0.5f,
		 0.5f, -0.5f
	};

	//crée un buffer
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	//Associe le buffer à GL_ARRAY_BUFFER afin d'y stocker des vertices
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

	//active et définit un tableau de vertices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

	//lit le fichier shader et stocke dans un buffer le vertex et fragment shader
	ShaderProgramSource source = ParseShader("shaders/Basic.shader");

	//crée le shader
	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	//lit le shader à la pipeline
	glUseProgram(shader);


	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		//rend les primitives du buffer
		glDrawArrays(GL_TRIANGLES, 0, 3);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	//libère la mémoire 
	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}