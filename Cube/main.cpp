#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <windows.h>
#include <cstdlib>
#include <sstream>
#include <queue>
#include <string.h>
#include "shaders.h"
#include "cubos.h"
#include "camera.h"

#define _USE_MATH_DEFINES
#define SHELL_TOK_DELIM " " 

#ifdef __cplusplus
extern "C" {
#endif
#include "include/search.h"
#ifdef __cplusplus
}
#endif

////////////////////////////////GLOBAL////////////////////////////////////////////////
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
//animation variables
float limitss = 0.0;
float limit_side = 0.0;
float limit_up = 0.0;
float limit_side_left = 0.0;
float limit_side_left_u = 0.0;
float limit_side_right_d = 0.0;
float limit_side_left_d = 0.0;
float limit_mid_d = 0.0;
float limit_side_right_d_2 = 0.0;
float limit_side_left_d_2 = 0.0;
float limit_mid_up = 0.0;
float limit_mid_down = 0.0;

//animation flags
bool flag_1 = true;
bool flag_2 = false;
bool flag_3 = false;
bool flag_4 = false;
bool flag_5 = false;
bool flag_6 = false;
bool flag_7 = false;
bool flag_8 = false;
bool flag_9 = false;
bool flag_10 = false;
bool flag_11 = false;
bool flag_12 = false;
bool flag_solve = true;
bool flag_scramble = false;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//auxiliar
bool MOVES[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
int SCRAMBLE_CONT = 999;
int LAST = 999;
bool SOLVING = 0;
bool SOLVE = 0;
bool TWO_TIMES = 0;
bool CHECK_SOLUTION = 0;
bool is_solve = false;
std::queue<char*> resolution;

std::vector<char*> split_line(std::string command) {//parser según SHELL_TOK_DELIM
	char* token;
	char* next_token = NULL;
	std::vector<char*> palabras;

	char* c_command = new char[command.length() + 1];
	strcpy_s(c_command, command.length() + 1, command.c_str());

	token = strtok_s(c_command, SHELL_TOK_DELIM, &next_token);
	while (token != NULL) {
		palabras.push_back(token);

		token = strtok_s(NULL, SHELL_TOK_DELIM, &next_token);
	}

	return palabras;
}

void switch_cube(cubo*& a, cubo*& b) {
	if (a == b) {
		return;
	}
	cubo* ptr = NULL;
	ptr = a;
	a = b;
	b = ptr;
}

int find(int& num, int* arr) {
	int* p = arr; int i = 0;

	while (*p != num && i<9) {
		p++; i++;
	}

	return i;
}

void updates_cubes(std::vector<cubo*>& cubes, int* nums, int* aux) {
	bool verified[9] = { 0,0,0,0,0,0,0,0,0 };
	int cont = 0;
	int busqueda = nums[0];
	int inspector = nums[0];

	while (cont < 9) {
		//verifica si la posicion no fue ordenada todavia
		int i = find(busqueda, aux);//que cubo se va a mover a la posicion busqueda en cubes
		if (!verified[i]) {
			switch_cube(cubes[busqueda], cubes[nums[i]]);//swap
			verified[find(busqueda, nums)] = 1;
			busqueda = nums[i];//actualiza la busqueda en la posicion donde se posiciono el antiguo cubo
		}
		else {
			//condicion para verificar que todo esta ordenado
			verified[find(busqueda, nums)] = 1;
			cont++;
			inspector = nums[cont];
			busqueda = inspector;
		}
	}

}

char* obtain_colors(std::vector<cubo*>& cubes) {
	int order_cubes[54] = {2,0,1,11,9,10,20,18,19,19,10,1,22,13,4,25,16,7,20,18,19,23,21,22,26,24,25,26,24,25,17,15,16,8,6,7,2,11,20,5,14,23,8,17,26,1,0,2,4,3,5,7,6,8};
	int order_faces[6] = {5,3,1,4,2,0};
	int aux = 0;
	int c = 0;
	std::vector<char> obtained;

	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 9; j++) {
			c = cubes[order_cubes[aux]]->color[order_faces[i]];
			switch (c) {
				case 0://AMARILLO
					obtained.push_back('B');
					break;
				case 1://ROJO
					obtained.push_back('U');
					break;
				case 2://VERDE
					obtained.push_back('L');
					break;
				case 3://BLANCO
					obtained.push_back('R');
					break;
				case 4://AZUL
					obtained.push_back('D');
					break;
				case 5://NARANJA
					obtained.push_back('F');
					break;
				case 6:
					//std::cout << "ERROR_OBTAIN_COLORS";
					break;
				default:
					//std::cout << "ERROR_OBTAIN_COLORS";
					break;
			}
			aux++;
		}
	}

	char* o = new char[54];
	for (int i = 0; i < 54; ++i) {
		o[i] = obtained[i];
	}

	/*std::cout << "Obtained Faces: ";
	for (int i = 0; i < 54; ++i) {
		std::cout << o[i];
	}std::cout << std::endl;*/

	return o;
}

std::string solution(int aux, char** aux2) {//Solver
	char patternized[64];
	std::string s;
	std::stringstream ss;

	char* facelets = aux2[1];
	if (aux > 2) {
		patternize(facelets, aux2[2], patternized);
		facelets = patternized;
	}
	char* sol = solution(
		facelets,
		24,
		1000,
		0,
		"cache"
	);
	if (sol == NULL) {
		std::string nn = "Unsolvable cube!";
		return nn;
	}

	ss << sol;
	s = ss.str();
	std::cout << "Solucion: ";
	std::cout << s << std::endl;
	free(sol);

	return s;
}

void scramble() {
	while (true) {
		srand(time(0));
		int movement = rand() % 12;
		if (movement != LAST) {
			MOVES[movement] = 1;
			LAST = movement;

			//////////////////MOVES/////////////
			/*switch (movement) {
			case 0:
				std::cout << "F, ";
				break;
			case 1:
				std::cout << "F', ";
				break;
			case 2:
				std::cout << "R, ";
				break;
			case 3:
				std::cout << "R', ";
				break;
			case 4:
				std::cout << "U, ";
				break;
			case 5:
				std::cout << "U', ";
				break;
			case 6:
				std::cout << "B, ";
				break;
			case 7:
				std::cout << "B', ";
				break;
			case 8:
				std::cout << "L, ";
				break;
			case 9:
				std::cout << "L', ";
				break;
			case 10:
				std::cout << "D, ";
				break;
			case 11:
				std::cout << "D', ";
				break;
			default:
				break;
			}*/
			break;
		}
	}
}

void solution_application() {
	char aux0 = (resolution.front())[0];
	char aux1 = (resolution.front())[1];
	std::string reverse = "'";

	switch (aux0) {
		case 'F': {
			if (aux1=='2'){
				if (TWO_TIMES) {
					MOVES[0] = 1;
					TWO_TIMES = 0;
					resolution.pop();
				}
				else {
					MOVES[0] = 1;
					TWO_TIMES = 1;
				}
			}
			else if (aux1 == reverse[0]){
				MOVES[1] = 1;
				resolution.pop();
			}
			else {
				MOVES[0] = 1;
				resolution.pop();
			}
		}
			  break;
		case 'B': {
			if (aux1 == '2') {
				if (TWO_TIMES) {
					MOVES[6] = 1;
					TWO_TIMES = 0;
					resolution.pop();
				}
				else {
					MOVES[6] = 1;
					TWO_TIMES = 1;
				}
			}
			else if (aux1 == reverse[0]) {
				MOVES[7] = 1;
				resolution.pop();
			}
			else {
				MOVES[6] = 1;
				resolution.pop();
			}
		}
			  break;
		case 'L': {
			if (aux1 == '2') {
				if (TWO_TIMES) {
					MOVES[8] = 1;
					TWO_TIMES = 0;
					resolution.pop();
				}
				else {
					MOVES[8] = 1;
					TWO_TIMES = 1;
				}
			}
			else if (aux1 == reverse[0]) {
				MOVES[9] = 1;
				resolution.pop();
			}
			else {
				MOVES[8] = 1;
				resolution.pop();
			}
		}
			  break;
		case 'R': {
			if (aux1 == '2') {
				if (TWO_TIMES) {
					MOVES[2] = 1;
					TWO_TIMES = 0;
					resolution.pop();
				}
				else {
					MOVES[2] = 1;
					TWO_TIMES = 1;
				}
			}
			else if (aux1 == reverse[0]) {
				MOVES[3] = 1;
				resolution.pop();
			}
			else {
				MOVES[2] = 1;
				resolution.pop();
			}
		}
			  break;
		case 'U': {
			if (aux1 == '2') {
				if (TWO_TIMES) {
					MOVES[4] = 1;
					TWO_TIMES = 0;
					resolution.pop();
				}
				else {
					MOVES[4] = 1;
					TWO_TIMES = 1;
				}
			}
			else if (aux1 == reverse[0]) {
				MOVES[5] = 1;
				resolution.pop();
			}
			else {
				MOVES[4] = 1;
				resolution.pop();
			}
		}
			  break;
		case 'D': {
			if (aux1 == '2') {
				if (TWO_TIMES) {
					MOVES[10] = 1;
					TWO_TIMES = 0;
					resolution.pop();
				}
				else {
					MOVES[10] = 1;
					TWO_TIMES = 1;
				}
			}
			else if (aux1 == reverse[0]) {
				MOVES[11] = 1;
				resolution.pop();
			}
			else {
				MOVES[10] = 1;
				resolution.pop();
			}
		}
			  break;
		default:
			break;
	}

	if (resolution.empty()) {
		SOLVING = 0;
		CHECK_SOLUTION = 1;
	}

}

int main(int argc, char** argv){
	std::cout << "Please enter the base directory: ";
	std::string base_dir = "";
	std::cin >> base_dir;
	std::cout << "\n" << base_dir << "\n" << "\n";

	srand(time(NULL));
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glEnable(GL_DEPTH_TEST);

	std::string vertex_shader_path, fragment_shader_path, fragment_shader_path_2;
	vertex_shader_path = base_dir + "/vshader.fs";
	fragment_shader_path = base_dir + "/fshader.fs";
	fragment_shader_path_2 = base_dir + "/fshader_2.fs";

	Shader shader_t(vertex_shader_path.c_str(), fragment_shader_path.c_str());
	Shader shader_t_2(vertex_shader_path.c_str(), fragment_shader_path_2.c_str());

	//=================Caras del Cubo=================//
	//==================================================================================================================//

	std::string texture_path;
	texture_path = base_dir + "/amarillo.jpg";
	Textures texture_1(texture_path.c_str(), GL_TEXTURE_2D);
	texture_path = base_dir + "/rojo.jpg";
	Textures texture_2(texture_path.c_str(), GL_TEXTURE_2D);
	texture_path = base_dir + "/verde.jpg";
	Textures texture_3(texture_path.c_str(), GL_TEXTURE_2D);
	texture_path = base_dir + "/blanco.jpg";
	Textures texture_4(texture_path.c_str(), GL_TEXTURE_2D);
	texture_path = base_dir + "/azul.jpg";
	Textures texture_5(texture_path.c_str(), GL_TEXTURE_2D);
	texture_path = base_dir + "/anaranjado.jpg";
	Textures texture_6(texture_path.c_str(), GL_TEXTURE_2D);
	texture_path = base_dir + "/negro.jpg";
	Textures texture_7(texture_path.c_str(), GL_TEXTURE_2D);

	std::vector<Textures> textures;
	textures.push_back(texture_1);
	textures.push_back(texture_2);
	textures.push_back(texture_3);
	textures.push_back(texture_4);
	textures.push_back(texture_5);
	textures.push_back(texture_6);
	textures.push_back(texture_7);

	std::vector<std::vector<glm::vec3>> worldPostions = {
		////////////////////////////////////////////////////// C ////////////////////////////////////////////
		{glm::vec3(3.0f, 4.0f, 0.0f),
		glm::vec3(4.0f, 4.0f, 0.0f),
		glm::vec3(2.0f, 4.0f, 0.0f),
		glm::vec3(3.0f, 3.0f, 0.0f),
		glm::vec3(4.0f, 3.0f, 0.0f),
		glm::vec3(2.0f, 3.0f, 0.0f),
		glm::vec3(3.0f, 2.0f, 0.0f),
		glm::vec3(4.0f, 2.0f, 0.0f),
		glm::vec3(2.0f, 2.0f, 0.0f),

		glm::vec3(3.0f, 4.0f, 1.0f),
		glm::vec3(4.0f, 4.0f, 1.0f),
		glm::vec3(2.0f, 4.0f, 1.0f),
		glm::vec3(3.0f, 3.0f, 1.0f),
		glm::vec3(4.0f, 3.0f, 1.0f),
		glm::vec3(2.0f, 3.0f, 1.0f),
		glm::vec3(3.0f, 2.0f, 1.0f),
		glm::vec3(4.0f, 2.0f, 1.0f),
		glm::vec3(2.0f, 2.0f, 1.0f),

		glm::vec3(3.0f, 4.0f, 2.0f),
		glm::vec3(4.0f, 4.0f, 2.0f),
		glm::vec3(2.0f, 4.0f, 2.0f),
		glm::vec3(3.0f, 3.0f, 2.0f),
		glm::vec3(4.0f, 3.0f, 2.0f),
		glm::vec3(2.0f, 3.0f, 2.0f),
		glm::vec3(3.0f, 2.0f, 2.0f),
		glm::vec3(4.0f, 2.0f, 2.0f),
		glm::vec3(2.0f, 2.0f, 2.0f)},

		{glm::vec3(0.0f, 7.0f, 0.0f),
		glm::vec3(1.0f, 7.0f, 0.0f),
		glm::vec3(-1.0f, 7.0f, 0.0f),
		glm::vec3(0.0f, 6.0f, 0.0f),
		glm::vec3(1.0f, 6.0f, 0.0f),
		glm::vec3(-1.0f, 6.0f, 0.0f),
		glm::vec3(0.0f, 5.0f, 0.0f),
		glm::vec3(1.0f, 5.0f, 0.0f),
		glm::vec3(-1.0f, 5.0f, 0.0f),

		glm::vec3(0.0f, 7.0f, 1.0f),
		glm::vec3(1.0f, 7.0f, 1.0f),
		glm::vec3(-1.0f, 7.0f, 1.0f),
		glm::vec3(0.0f, 6.0f, 1.0f),
		glm::vec3(1.0f, 6.0f, 1.0f),
		glm::vec3(-1.0f, 6.0f, 1.0f),
		glm::vec3(0.0f, 5.0f, 1.0f),
		glm::vec3(1.0f, 5.0f, 1.0f),
		glm::vec3(-1.0f, 5.0f, 1.0f),

		glm::vec3(0.0f, 7.0f, 2.0f),
		glm::vec3(1.0f, 7.0f, 2.0f),
		glm::vec3(-1.0f, 7.0f, 2.0f),
		glm::vec3(0.0f, 6.0f, 2.0f),
		glm::vec3(1.0f, 6.0f, 2.0f),
		glm::vec3(-1.0f, 6.0f, 2.0f),
		glm::vec3(0.0f, 5.0f, 2.0f),
		glm::vec3(1.0f, 5.0f, 2.0f),
		glm::vec3(-1.0f, 5.0f, 2.0f)},

		{glm::vec3(-3.0f, 7.0f, 0.0f),
		glm::vec3(-2.0f, 7.0f, 0.0f),
		glm::vec3(-4.0f, 7.0f, 0.0f),
		glm::vec3(-3.0f, 6.0f, 0.0f),
		glm::vec3(-2.0f, 6.0f, 0.0f),
		glm::vec3(-4.0f, 6.0f, 0.0f),
		glm::vec3(-3.0f, 5.0f, 0.0f),
		glm::vec3(-2.0f, 5.0f, 0.0f),
		glm::vec3(-4.0f, 5.0f, 0.0f),

		glm::vec3(-3.0f, 7.0f, 1.0f),
		glm::vec3(-2.0f, 7.0f, 1.0f),
		glm::vec3(-4.0f, 7.0f, 1.0f),
		glm::vec3(-3.0f, 6.0f, 1.0f),
		glm::vec3(-2.0f, 6.0f, 1.0f),
		glm::vec3(-4.0f, 6.0f, 1.0f),
		glm::vec3(-3.0f, 5.0f, 1.0f),
		glm::vec3(-2.0f, 5.0f, 1.0f),
		glm::vec3(-4.0f, 5.0f, 1.0f),

		glm::vec3(-3.0f, 7.0f, 2.0f),
		glm::vec3(-2.0f, 7.0f, 2.0f),
		glm::vec3(-4.0f, 7.0f, 2.0f),
		glm::vec3(-3.0f, 6.0f, 2.0f),
		glm::vec3(-2.0f, 6.0f, 2.0f),
		glm::vec3(-4.0f, 6.0f, 2.0f),
		glm::vec3(-3.0f, 5.0f, 2.0f),
		glm::vec3(-2.0f, 5.0f, 2.0f),
		glm::vec3(-4.0f, 5.0f, 2.0f)},

		{glm::vec3(-6.0f, 4.0f, 0.0f),
		glm::vec3(-5.0f, 4.0f, 0.0f),
		glm::vec3(-7.0f, 4.0f, 0.0f),
		glm::vec3(-6.0f, 3.0f, 0.0f),
		glm::vec3(-5.0f, 3.0f, 0.0f),
		glm::vec3(-7.0f, 3.0f, 0.0f),
		glm::vec3(-6.0f, 2.0f, 0.0f),
		glm::vec3(-5.0f, 2.0f, 0.0f),
		glm::vec3(-7.0f, 2.0f, 0.0f),

		glm::vec3(-6.0f, 4.0f, 1.0f),
		glm::vec3(-5.0f, 4.0f, 1.0f),
		glm::vec3(-7.0f, 4.0f, 1.0f),
		glm::vec3(-6.0f, 3.0f, 1.0f),
		glm::vec3(-5.0f, 3.0f, 1.0f),
		glm::vec3(-7.0f, 3.0f, 1.0f),
		glm::vec3(-6.0f, 2.0f, 1.0f),
		glm::vec3(-5.0f, 2.0f, 1.0f),
		glm::vec3(-7.0f, 2.0f, 1.0f),

		glm::vec3(-6.0f, 4.0f, 2.0f),
		glm::vec3(-5.0f, 4.0f, 2.0f),
		glm::vec3(-7.0f, 4.0f, 2.0f),
		glm::vec3(-6.0f, 3.0f, 2.0f),
		glm::vec3(-5.0f, 3.0f, 2.0f),
		glm::vec3(-7.0f, 3.0f, 2.0f),
		glm::vec3(-6.0f, 2.0f, 2.0f),
		glm::vec3(-5.0f, 2.0f, 2.0f),
		glm::vec3(-7.0f, 2.0f, 2.0f)},

		{ glm::vec3(-6.0f, 1.0f, 0.0f),
		glm::vec3(-5.0f, 1.0f, 0.0f),
		glm::vec3(-7.0f, 1.0f, 0.0f),
		glm::vec3(-6.0f, 0.0f, 0.0f),
		glm::vec3(-5.0f, 0.0f, 0.0f),
		glm::vec3(-7.0f, 0.0f, 0.0f),
		glm::vec3(-6.0f, -1.0f, 0.0f),
		glm::vec3(-5.0f, -1.0f, 0.0f),
		glm::vec3(-7.0f, -1.0f, 0.0f),

		glm::vec3(-6.0f, 1.0f, 1.0f),
		glm::vec3(-5.0f, 1.0f, 1.0f),
		glm::vec3(-7.0f, 1.0f, 1.0f),
		glm::vec3(-6.0f, 0.0f, 1.0f),
		glm::vec3(-5.0f, 0.0f, 1.0f),
		glm::vec3(-7.0f, 0.0f, 1.0f),
		glm::vec3(-6.0f, -1.0f, 1.0f),
		glm::vec3(-5.0f, -1.0f, 1.0f),
		glm::vec3(-7.0f, -1.0f, 1.0f),

		glm::vec3(-6.0f, 1.0f, 2.0f),
		glm::vec3(-5.0f, 1.0f, 2.0f),
		glm::vec3(-7.0f, 1.0f, 2.0f),
		glm::vec3(-6.0f, 0.0f, 2.0f),
		glm::vec3(-5.0f, 0.0f, 2.0f),
		glm::vec3(-7.0f, 0.0f, 2.0f),
		glm::vec3(-6.0f, -1.0f, 2.0f),
		glm::vec3(-5.0f, -1.0f, 2.0f),
		glm::vec3(-7.0f, -1.0f, 2.0f) },

		{ glm::vec3(-6.0f, -2.0f, 0.0f),
		glm::vec3(-5.0f, -2.0f, 0.0f),
		glm::vec3(-7.0f, -2.0f, 0.0f),
		glm::vec3(-6.0f, -3.0f, 0.0f),
		glm::vec3(-5.0f, -3.0f, 0.0f),
		glm::vec3(-7.0f, -3.0f, 0.0f),
		glm::vec3(-6.0f, -4.0f, 0.0f),
		glm::vec3(-5.0f, -4.0f, 0.0f),
		glm::vec3(-7.0f, -4.0f, 0.0f),

		glm::vec3(-6.0f, -2.0f, 1.0f),
		glm::vec3(-5.0f, -2.0f, 1.0f),
		glm::vec3(-7.0f, -2.0f, 1.0f),
		glm::vec3(-6.0f, -3.0f, 1.0f),
		glm::vec3(-5.0f, -3.0f, 1.0f),
		glm::vec3(-7.0f, -3.0f, 1.0f),
		glm::vec3(-6.0f, -4.0f, 1.0f),
		glm::vec3(-5.0f, -4.0f, 1.0f),
		glm::vec3(-7.0f, -4.0f, 1.0f),

		glm::vec3(-6.0f, -2.0f, 2.0f),
		glm::vec3(-5.0f, -2.0f, 2.0f),
		glm::vec3(-7.0f, -2.0f, 2.0f),
		glm::vec3(-6.0f, -3.0f, 2.0f),
		glm::vec3(-5.0f, -3.0f, 2.0f),
		glm::vec3(-7.0f, -3.0f, 2.0f),
		glm::vec3(-6.0f, -4.0f, 2.0f),
		glm::vec3(-5.0f, -4.0f, 2.0f),
		glm::vec3(-7.0f, -4.0f, 2.0f) },

		{ glm::vec3(-6.0f, -5.0f, 0.0f),
		glm::vec3(-5.0f, -5.0f, 0.0f),
		glm::vec3(-7.0f, -5.0f, 0.0f),
		glm::vec3(-6.0f, -6.0f, 0.0f),
		glm::vec3(-5.0f, -6.0f, 0.0f),
		glm::vec3(-7.0f, -6.0f, 0.0f),
		glm::vec3(-6.0f, -7.0f, 0.0f),
		glm::vec3(-5.0f, -7.0f, 0.0f),
		glm::vec3(-7.0f, -7.0f, 0.0f),

		glm::vec3(-6.0f, -5.0f, 1.0f),
		glm::vec3(-5.0f, -5.0f, 1.0f),
		glm::vec3(-7.0f, -5.0f, 1.0f),
		glm::vec3(-6.0f, -6.0f, 1.0f),
		glm::vec3(-5.0f, -6.0f, 1.0f),
		glm::vec3(-7.0f, -6.0f, 1.0f),
		glm::vec3(-6.0f, -7.0f, 1.0f),
		glm::vec3(-5.0f, -7.0f, 1.0f),
		glm::vec3(-7.0f, -7.0f, 1.0f),

		glm::vec3(-6.0f, -5.0f, 2.0f),
		glm::vec3(-5.0f, -5.0f, 2.0f),
		glm::vec3(-7.0f, -5.0f, 2.0f),
		glm::vec3(-6.0f, -6.0f, 2.0f),
		glm::vec3(-5.0f, -6.0f, 2.0f),
		glm::vec3(-7.0f, -6.0f, 2.0f),
		glm::vec3(-6.0f, -7.0f, 2.0f),
		glm::vec3(-5.0f, -7.0f, 2.0f),
		glm::vec3(-7.0f, -7.0f, 2.0f) },

		{ glm::vec3(-3.0f, -8.0f, 0.0f),
		glm::vec3(-2.0f, -8.0f, 0.0f),
		glm::vec3(-4.0f, -8.0f, 0.0f),
		glm::vec3(-3.0f, -9.0f, 0.0f),
		glm::vec3(-2.0f, -9.0f, 0.0f),
		glm::vec3(-4.0f, -9.0f, 0.0f),
		glm::vec3(-3.0f, -10.0f, 0.0f),
		glm::vec3(-2.0f, -10.0f, 0.0f),
		glm::vec3(-4.0f, -10.0f, 0.0f),

		glm::vec3(-3.0f, -8.0f, 1.0f),
		glm::vec3(-2.0f, -8.0f, 1.0f),
		glm::vec3(-4.0f, -8.0f, 1.0f),
		glm::vec3(-3.0f, -9.0f, 1.0f),
		glm::vec3(-2.0f, -9.0f, 1.0f),
		glm::vec3(-4.0f, -9.0f, 1.0f),
		glm::vec3(-3.0f, -10.0f, 1.0f),
		glm::vec3(-2.0f, -10.0f, 1.0f),
		glm::vec3(-4.0f, -10.0f, 1.0f),

		glm::vec3(-3.0f, -8.0f, 2.0f),
		glm::vec3(-2.0f, -8.0f, 2.0f),
		glm::vec3(-4.0f, -8.0f, 2.0f),
		glm::vec3(-3.0f, -9.0f, 2.0f),
		glm::vec3(-2.0f, -9.0f, 2.0f),
		glm::vec3(-4.0f, -9.0f, 2.0f),
		glm::vec3(-3.0f, -10.0f, 2.0f),
		glm::vec3(-2.0f, -10.0f, 2.0f),
		glm::vec3(-4.0f, -10.0f, 2.0f) },

		{ glm::vec3(0.0f, -8.0f, 0.0f),
		glm::vec3(1.0f, -8.0f, 0.0f),
		glm::vec3(-1.0f, -8.0f, 0.0f),
		glm::vec3(0.0f, -9.0f, 0.0f),
		glm::vec3(1.0f, -9.0f, 0.0f),
		glm::vec3(-1.0f, -9.0f, 0.0f),
		glm::vec3(0.0f, -10.0f, 0.0f),
		glm::vec3(1.0f, -10.0f, 0.0f),
		glm::vec3(-1.0f, -10.0f, 0.0f),

		glm::vec3(0.0f, -8.0f, 1.0f),
		glm::vec3(1.0f, -8.0f, 1.0f),
		glm::vec3(-1.0f, -8.0f, 1.0f),
		glm::vec3(0.0f, -9.0f, 1.0f),
		glm::vec3(1.0f, -9.0f, 1.0f),
		glm::vec3(-1.0f, -9.0f, 1.0f),
		glm::vec3(0.0f, -10.0f, 1.0f),
		glm::vec3(1.0f, -10.0f, 1.0f),
		glm::vec3(-1.0f, -10.0f, 1.0f),

		glm::vec3(0.0f, -8.0f, 2.0f),
		glm::vec3(1.0f, -8.0f, 2.0f),
		glm::vec3(-1.0f, -8.0f, 2.0f),
		glm::vec3(0.0f, -9.0f, 2.0f),
		glm::vec3(1.0f, -9.0f, 2.0f),
		glm::vec3(-1.0f, -9.0f, 2.0f),
		glm::vec3(0.0f, -10.0f, 2.0f),
		glm::vec3(1.0f, -10.0f, 2.0f),
		glm::vec3(-1.0f, -10.0f, 2.0f) },

		{ glm::vec3(3.0f, -5.0f, 0.0f),
		glm::vec3(4.0f, -5.0f, 0.0f),
		glm::vec3(2.0f, -5.0f, 0.0f),
		glm::vec3(3.0f, -6.0f, 0.0f),
		glm::vec3(4.0f, -6.0f, 0.0f),
		glm::vec3(2.0f, -6.0f, 0.0f),
		glm::vec3(3.0f, -7.0f, 0.0f),
		glm::vec3(4.0f, -7.0f, 0.0f),
		glm::vec3(2.0f, -7.0f, 0.0f),

		glm::vec3(3.0f, -5.0f, 1.0f),
		glm::vec3(4.0f, -5.0f, 1.0f),
		glm::vec3(2.0f, -5.0f, 1.0f),
		glm::vec3(3.0f, -6.0f, 1.0f),
		glm::vec3(4.0f, -6.0f, 1.0f),
		glm::vec3(2.0f, -6.0f, 1.0f),
		glm::vec3(3.0f, -7.0f, 1.0f),
		glm::vec3(4.0f, -7.0f, 1.0f),
		glm::vec3(2.0f, -7.0f, 1.0f),

		glm::vec3(3.0f, -5.0f, 2.0f),
		glm::vec3(4.0f, -5.0f, 2.0f),
		glm::vec3(2.0f, -5.0f, 2.0f),
		glm::vec3(3.0f, -6.0f, 2.0f),
		glm::vec3(4.0f, -6.0f, 2.0f),
		glm::vec3(2.0f, -6.0f, 2.0f),
		glm::vec3(3.0f, -7.0f, 2.0f),
		glm::vec3(4.0f, -7.0f, 2.0f),
		glm::vec3(2.0f, -7.0f, 2.0f) },

		////////////////////////////////////////////////////// + ////////////////////////////////////////////

		{ glm::vec3(9.0f, -1.0f, 0.0f),
		glm::vec3(10.0f, -1.0f, 0.0f),
		glm::vec3(8.0f, -1.0f, 0.0f),
		glm::vec3(9.0f, -2.0f, 0.0f),
		glm::vec3(10.0f, -2.0f, 0.0f),
		glm::vec3(8.0f, -2.0f, 0.0f),
		glm::vec3(9.0f, -3.0f, 0.0f),
		glm::vec3(10.0f, -3.0f, 0.0f),
		glm::vec3(8.0f, -3.0f, 0.0f),

		glm::vec3(9.0f, -1.0f, 1.0f),
		glm::vec3(10.0f, -1.0f, 1.0f),
		glm::vec3(8.0f, -1.0f, 1.0f),
		glm::vec3(9.0f, -2.0f, 1.0f),
		glm::vec3(10.0f, -2.0f, 1.0f),
		glm::vec3(8.0f, -2.0f, 1.0f),
		glm::vec3(9.0f, -3.0f, 1.0f),
		glm::vec3(10.0f, -3.0f, 1.0f),
		glm::vec3(8.0f, -3.0f, 1.0f),

		glm::vec3(9.0f, -1.0f, 2.0f),
		glm::vec3(10.0f, -1.0f, 2.0f),
		glm::vec3(8.0f, -1.0f, 2.0f),
		glm::vec3(9.0f, -2.0f, 2.0f),
		glm::vec3(10.0f, -2.0f, 2.0f),
		glm::vec3(8.0f, -2.0f, 2.0f),
		glm::vec3(9.0f, -3.0f, 2.0f),
		glm::vec3(10.0f, -3.0f, 2.0f),
		glm::vec3(8.0f, -3.0f, 2.0f) },

		{ glm::vec3(12.0f, -4.0f, 0.0f),
		glm::vec3(13.0f, -4.0f, 0.0f),
		glm::vec3(11.0f, -4.0f, 0.0f),
		glm::vec3(12.0f, -5.0f, 0.0f),
		glm::vec3(13.0f, -5.0f, 0.0f),
		glm::vec3(11.0f, -5.0f, 0.0f),
		glm::vec3(12.0f, -6.0f, 0.0f),
		glm::vec3(13.0f, -6.0f, 0.0f),
		glm::vec3(11.0f, -6.0f, 0.0f),

		glm::vec3(12.0f, -4.0f, 1.0f),
		glm::vec3(13.0f, -4.0f, 1.0f),
		glm::vec3(11.0f, -4.0f, 1.0f),
		glm::vec3(12.0f, -5.0f, 1.0f),
		glm::vec3(13.0f, -5.0f, 1.0f),
		glm::vec3(11.0f, -5.0f, 1.0f),
		glm::vec3(12.0f, -6.0f, 1.0f),
		glm::vec3(13.0f, -6.0f, 1.0f),
		glm::vec3(11.0f, -6.0f, 1.0f),

		glm::vec3(12.0f, -4.0f, 2.0f),
		glm::vec3(13.0f, -4.0f, 2.0f),
		glm::vec3(11.0f, -4.0f, 2.0f),
		glm::vec3(12.0f, -5.0f, 2.0f),
		glm::vec3(13.0f, -5.0f, 2.0f),
		glm::vec3(11.0f, -5.0f, 2.0f),
		glm::vec3(12.0f, -6.0f, 2.0f),
		glm::vec3(13.0f, -6.0f, 2.0f),
		glm::vec3(11.0f, -6.0f, 2.0f) },

		{ glm::vec3(12.0f, -1.0f, 0.0f),
		glm::vec3(13.0f, -1.0f, 0.0f),
		glm::vec3(11.0f, -1.0f, 0.0f),
		glm::vec3(12.0f, -2.0f, 0.0f),
		glm::vec3(13.0f, -2.0f, 0.0f),
		glm::vec3(11.0f, -2.0f, 0.0f),
		glm::vec3(12.0f, -3.0f, 0.0f),
		glm::vec3(13.0f, -3.0f, 0.0f),
		glm::vec3(11.0f, -3.0f, 0.0f),

		glm::vec3(12.0f, -1.0f, 1.0f),
		glm::vec3(13.0f, -1.0f, 1.0f),
		glm::vec3(11.0f, -1.0f, 1.0f),
		glm::vec3(12.0f, -2.0f, 1.0f),
		glm::vec3(13.0f, -2.0f, 1.0f),
		glm::vec3(11.0f, -2.0f, 1.0f),
		glm::vec3(12.0f, -3.0f, 1.0f),
		glm::vec3(13.0f, -3.0f, 1.0f),
		glm::vec3(11.0f, -3.0f, 1.0f),

		glm::vec3(12.0f, -1.0f, 2.0f),
		glm::vec3(13.0f, -1.0f, 2.0f),
		glm::vec3(11.0f, -1.0f, 2.0f),
		glm::vec3(12.0f, -2.0f, 2.0f),
		glm::vec3(13.0f, -2.0f, 2.0f),
		glm::vec3(11.0f, -2.0f, 2.0f),
		glm::vec3(12.0f, -3.0f, 2.0f),
		glm::vec3(13.0f, -3.0f, 2.0f),
		glm::vec3(11.0f, -3.0f, 2.0f) },

		{ glm::vec3(12.0f, 2.0f, 0.0f),
		glm::vec3(13.0f, 2.0f, 0.0f),
		glm::vec3(11.0f, 2.0f, 0.0f),
		glm::vec3(12.0f, 1.0f, 0.0f),
		glm::vec3(13.0f, 1.0f, 0.0f),
		glm::vec3(11.0f, 1.0f, 0.0f),
		glm::vec3(12.0f, 0.0f, 0.0f),
		glm::vec3(13.0f, 0.0f, 0.0f),
		glm::vec3(11.0f, 0.0f, 0.0f),

		glm::vec3(12.0f, 2.0f, 1.0f),
		glm::vec3(13.0f, 2.0f, 1.0f),
		glm::vec3(11.0f, 2.0f, 1.0f),
		glm::vec3(12.0f, 1.0f, 1.0f),
		glm::vec3(13.0f, 1.0f, 1.0f),
		glm::vec3(11.0f, 1.0f, 1.0f),
		glm::vec3(12.0f, 0.0f, 1.0f),
		glm::vec3(13.0f, 0.0f, 1.0f),
		glm::vec3(11.0f, 0.0f, 1.0f),

		glm::vec3(12.0f, 2.0f, 2.0f),
		glm::vec3(13.0f, 2.0f, 2.0f),
		glm::vec3(11.0f, 2.0f, 2.0f),
		glm::vec3(12.0f, 1.0f, 2.0f),
		glm::vec3(13.0f, 1.0f, 2.0f),
		glm::vec3(11.0f, 1.0f, 2.0f),
		glm::vec3(12.0f, 0.0f, 2.0f),
		glm::vec3(13.0f, 0.0f, 2.0f),
		glm::vec3(11.0f, 0.0f, 2.0f) },

		{ glm::vec3(15.0f, -1.0f, 0.0f),
		glm::vec3(16.0f, -1.0f, 0.0f),
		glm::vec3(14.0f, -1.0f, 0.0f),
		glm::vec3(15.0f, -2.0f, 0.0f),
		glm::vec3(16.0f, -2.0f, 0.0f),
		glm::vec3(14.0f, -2.0f, 0.0f),
		glm::vec3(15.0f, -3.0f, 0.0f),
		glm::vec3(16.0f, -3.0f, 0.0f),
		glm::vec3(14.0f, -3.0f, 0.0f),

		glm::vec3(15.0f, -1.0f, 1.0f),
		glm::vec3(16.0f, -1.0f, 1.0f),
		glm::vec3(14.0f, -1.0f, 1.0f),
		glm::vec3(15.0f, -2.0f, 1.0f),
		glm::vec3(16.0f, -2.0f, 1.0f),
		glm::vec3(14.0f, -2.0f, 1.0f),
		glm::vec3(15.0f, -3.0f, 1.0f),
		glm::vec3(16.0f, -3.0f, 1.0f),
		glm::vec3(14.0f, -3.0f, 1.0f),

		glm::vec3(15.0f, -1.0f, 2.0f),
		glm::vec3(16.0f, -1.0f, 2.0f),
		glm::vec3(14.0f, -1.0f, 2.0f),
		glm::vec3(15.0f, -2.0f, 2.0f),
		glm::vec3(16.0f, -2.0f, 2.0f),
		glm::vec3(14.0f, -2.0f, 2.0f),
		glm::vec3(15.0f, -3.0f, 2.0f),
		glm::vec3(16.0f, -3.0f, 2.0f),
		glm::vec3(14.0f, -3.0f, 2.0f) },

		/////////////////////////////////////////////////////// + ////////////////////////////////////////////

		{ glm::vec3(21.0f, -1.0f, 0.0f),
		glm::vec3(22.0f, -1.0f, 0.0f),
		glm::vec3(20.0f, -1.0f, 0.0f),
		glm::vec3(21.0f, -2.0f, 0.0f),
		glm::vec3(22.0f, -2.0f, 0.0f),
		glm::vec3(20.0f, -2.0f, 0.0f),
		glm::vec3(21.0f, -3.0f, 0.0f),
		glm::vec3(22.0f, -3.0f, 0.0f),
		glm::vec3(20.0f, -3.0f, 0.0f),

		glm::vec3(21.0f, -1.0f, 1.0f),
		glm::vec3(22.0f, -1.0f, 1.0f),
		glm::vec3(20.0f, -1.0f, 1.0f),
		glm::vec3(21.0f, -2.0f, 1.0f),
		glm::vec3(22.0f, -2.0f, 1.0f),
		glm::vec3(20.0f, -2.0f, 1.0f),
		glm::vec3(21.0f, -3.0f, 1.0f),
		glm::vec3(22.0f, -3.0f, 1.0f),
		glm::vec3(20.0f, -3.0f, 1.0f),

		glm::vec3(21.0f, -1.0f, 2.0f),
		glm::vec3(22.0f, -1.0f, 2.0f),
		glm::vec3(20.0f, -1.0f, 2.0f),
		glm::vec3(21.0f, -2.0f, 2.0f),
		glm::vec3(22.0f, -2.0f, 2.0f),
		glm::vec3(20.0f, -2.0f, 2.0f),
		glm::vec3(21.0f, -3.0f, 2.0f),
		glm::vec3(22.0f, -3.0f, 2.0f),
		glm::vec3(20.0f, -3.0f, 2.0f) },

		{ glm::vec3(24.0f, -4.0f, 0.0f),
		glm::vec3(25.0f, -4.0f, 0.0f),
		glm::vec3(23.0f, -4.0f, 0.0f),
		glm::vec3(24.0f, -5.0f, 0.0f),
		glm::vec3(25.0f, -5.0f, 0.0f),
		glm::vec3(23.0f, -5.0f, 0.0f),
		glm::vec3(24.0f, -6.0f, 0.0f),
		glm::vec3(25.0f, -6.0f, 0.0f),
		glm::vec3(23.0f, -6.0f, 0.0f),

		glm::vec3(24.0f, -4.0f, 1.0f),
		glm::vec3(25.0f, -4.0f, 1.0f),
		glm::vec3(23.0f, -4.0f, 1.0f),
		glm::vec3(24.0f, -5.0f, 1.0f),
		glm::vec3(25.0f, -5.0f, 1.0f),
		glm::vec3(23.0f, -5.0f, 1.0f),
		glm::vec3(24.0f, -6.0f, 1.0f),
		glm::vec3(25.0f, -6.0f, 1.0f),
		glm::vec3(23.0f, -6.0f, 1.0f),

		glm::vec3(24.0f, -4.0f, 2.0f),
		glm::vec3(25.0f, -4.0f, 2.0f),
		glm::vec3(23.0f, -4.0f, 2.0f),
		glm::vec3(24.0f, -5.0f, 2.0f),
		glm::vec3(25.0f, -5.0f, 2.0f),
		glm::vec3(23.0f, -5.0f, 2.0f),
		glm::vec3(24.0f, -6.0f, 2.0f),
		glm::vec3(25.0f, -6.0f, 2.0f),
		glm::vec3(23.0f, -6.0f, 2.0f) },

		{ glm::vec3(24.0f, -1.0f, 0.0f),
		glm::vec3(25.0f, -1.0f, 0.0f),
		glm::vec3(23.0f, -1.0f, 0.0f),
		glm::vec3(24.0f, -2.0f, 0.0f),
		glm::vec3(25.0f, -2.0f, 0.0f),
		glm::vec3(23.0f, -2.0f, 0.0f),
		glm::vec3(24.0f, -3.0f, 0.0f),
		glm::vec3(25.0f, -3.0f, 0.0f),
		glm::vec3(23.0f, -3.0f, 0.0f),

		glm::vec3(24.0f, -1.0f, 1.0f),
		glm::vec3(25.0f, -1.0f, 1.0f),
		glm::vec3(23.0f, -1.0f, 1.0f),
		glm::vec3(24.0f, -2.0f, 1.0f),
		glm::vec3(25.0f, -2.0f, 1.0f),
		glm::vec3(23.0f, -2.0f, 1.0f),
		glm::vec3(24.0f, -3.0f, 1.0f),
		glm::vec3(25.0f, -3.0f, 1.0f),
		glm::vec3(23.0f, -3.0f, 1.0f),

		glm::vec3(24.0f, -1.0f, 2.0f),
		glm::vec3(25.0f, -1.0f, 2.0f),
		glm::vec3(23.0f, -1.0f, 2.0f),
		glm::vec3(24.0f, -2.0f, 2.0f),
		glm::vec3(25.0f, -2.0f, 2.0f),
		glm::vec3(23.0f, -2.0f, 2.0f),
		glm::vec3(24.0f, -3.0f, 2.0f),
		glm::vec3(25.0f, -3.0f, 2.0f),
		glm::vec3(23.0f, -3.0f, 2.0f) },

		{ glm::vec3(24.0f, 2.0f, 0.0f),
		glm::vec3(25.0f, 2.0f, 0.0f),
		glm::vec3(23.0f, 2.0f, 0.0f),
		glm::vec3(24.0f, 1.0f, 0.0f),
		glm::vec3(25.0f, 1.0f, 0.0f),
		glm::vec3(23.0f, 1.0f, 0.0f),
		glm::vec3(24.0f, 0.0f, 0.0f),
		glm::vec3(25.0f, 0.0f, 0.0f),
		glm::vec3(23.0f, 0.0f, 0.0f),

		glm::vec3(24.0f, 2.0f, 1.0f),
		glm::vec3(25.0f, 2.0f, 1.0f),
		glm::vec3(23.0f, 2.0f, 1.0f),
		glm::vec3(24.0f, 1.0f, 1.0f),
		glm::vec3(25.0f, 1.0f, 1.0f),
		glm::vec3(23.0f, 1.0f, 1.0f),
		glm::vec3(24.0f, 0.0f, 1.0f),
		glm::vec3(25.0f, 0.0f, 1.0f),
		glm::vec3(23.0f, 0.0f, 1.0f),

		glm::vec3(24.0f, 2.0f, 2.0f),
		glm::vec3(25.0f, 2.0f, 2.0f),
		glm::vec3(23.0f, 2.0f, 2.0f),
		glm::vec3(24.0f, 1.0f, 2.0f),
		glm::vec3(25.0f, 1.0f, 2.0f),
		glm::vec3(23.0f, 1.0f, 2.0f),
		glm::vec3(24.0f, 0.0f, 2.0f),
		glm::vec3(25.0f, 0.0f, 2.0f),
		glm::vec3(23.0f, 0.0f, 2.0f) },

		{ glm::vec3(27.0f, -1.0f, 0.0f),
		glm::vec3(28.0f, -1.0f, 0.0f),
		glm::vec3(26.0f, -1.0f, 0.0f),
		glm::vec3(27.0f, -2.0f, 0.0f),
		glm::vec3(28.0f, -2.0f, 0.0f),
		glm::vec3(26.0f, -2.0f, 0.0f),
		glm::vec3(27.0f, -3.0f, 0.0f),
		glm::vec3(28.0f, -3.0f, 0.0f),
		glm::vec3(26.0f, -3.0f, 0.0f),

		glm::vec3(27.0f, -1.0f, 1.0f),
		glm::vec3(28.0f, -1.0f, 1.0f),
		glm::vec3(26.0f, -1.0f, 1.0f),
		glm::vec3(27.0f, -2.0f, 1.0f),
		glm::vec3(28.0f, -2.0f, 1.0f),
		glm::vec3(26.0f, -2.0f, 1.0f),
		glm::vec3(27.0f, -3.0f, 1.0f),
		glm::vec3(28.0f, -3.0f, 1.0f),
		glm::vec3(26.0f, -3.0f, 1.0f),

		glm::vec3(27.0f, -1.0f, 2.0f),
		glm::vec3(28.0f, -1.0f, 2.0f),
		glm::vec3(26.0f, -1.0f, 2.0f),
		glm::vec3(27.0f, -2.0f, 2.0f),
		glm::vec3(28.0f, -2.0f, 2.0f),
		glm::vec3(26.0f, -2.0f, 2.0f),
		glm::vec3(27.0f, -3.0f, 2.0f),
		glm::vec3(28.0f, -3.0f, 2.0f),
		glm::vec3(26.0f, -3.0f, 2.0f) },
	};

	/*glm::vec3 cubePositions[] = {//Posiciones para un solo cubo
		glm::vec3(0.0f,  1.0f,  0.0f),//edge arriba back
		glm::vec3(1.0f,  1.0f, 0.0f),//corner arriba derecha back
		glm::vec3(-1.0f, 1.0f, 0.0f),//corner arriba izquierda back
		glm::vec3(0.0f, 0.0f, 0.0f),//center back
		glm::vec3(1.0f, 0.0f, 0.0f),//edge back derecha
		glm::vec3(-1.0f,  0.0f, 0.0f),//edge back izquierda
		glm::vec3(0.0f, -1.0f, 0.0f),//edge abajo back
		glm::vec3(1.0f, -1.0f, 0.0f),//corner abajo derecha back
		glm::vec3(-1.0f, -1.0f, 0.0f),//corner abajo izquierda back

		glm::vec3(0.0f,  1.0f,  1.0f),//center arriba
		glm::vec3(1.0f,  1.0f, 1.0f),//edge arriba right
		glm::vec3(-1.0f, 1.0f, 1.0f),//edge arriba left
		glm::vec3(0.0f, 0.0f, 1.0f),//center_fake
		glm::vec3(1.0f, 0.0f, 1.0f),//center right
		glm::vec3(-1.0f,  0.0f, 1.0f),//center left
		glm::vec3(0.0f, -1.0f, 1.0f),//center abajo
		glm::vec3(1.0f, -1.0f, 1.0f),//edge abajo right
		glm::vec3(-1.0f, -1.0f, 1.0f),//edge abajo left

		glm::vec3(0.0f,  1.0f,  2.0f),//edge arriba front
		glm::vec3(1.0f,  1.0f, 2.0f),//corner arriba derecha front
		glm::vec3(-1.0f, 1.0f, 2.0f),//corner arriba izquierda front
		glm::vec3(0.0f, 0.0f, 2.0f),//center front
		glm::vec3(1.0f, 0.0f, 2.0f),//edge front derecha
		glm::vec3(-1.0f,  0.0f, 2.0f),//edge front izquierda
		glm::vec3(0.0f, -1.0f, 2.0f),//edge abajo front
		glm::vec3(1.0f, -1.0f, 2.0f),//corner abajo derecha front
		glm::vec3(-1.0f, -1.0f, 2.0f)//corner abajo izquierda front
	};*/

	std::vector<std::vector<int>>pintar = {//Forma en que se pinta cada cara del cubo
		{0,6,6,6,6,1},{0,6,6,3,6,1},{0,6,2,6,6,1},
		{0,6,6,6,6,6},{0,6,6,3,6,6},{0,6,2,6,6,6},
		{0,6,6,6,4,6},{0,6,6,3,4,6},{0,6,2,6,4,6},
		
		{6,6,6,6,6,1},{6,6,6,3,6,1},{6,6,2,6,6,1},
		{6,6,6,6,6,6},{6,6,6,3,6,6},{6,6,2,6,6,6},
		{6,6,6,6,4,6},{6,6,6,3,4,6},{6,6,2,6,4,6},
		
		{6,5,6,6,6,1},{6,5,6,3,6,1},{6,5,2,6,6,1},
		{6,5,6,6,6,6},{6,5,6,3,6,6},{6,5,2,6,6,6},
		{6,5,6,6,4,6},{6,5,6,3,4,6},{6,5,2,6,4,6},
	};

	///////////////////////////MENU//////////////////////////////////

	std::cout << "Movimiento | Letra Asociada" << std::endl;
	std::cout << "F          | F" << std::endl;
	std::cout << "F'         | G" << std::endl;
	std::cout << "R          | R" << std::endl;
	std::cout << "R'         | T" << std::endl;
	std::cout << "U          | U" << std::endl;
	std::cout << "U'         | I" << std::endl;
	std::cout << "B          | B" << std::endl;
	std::cout << "B'         | N" << std::endl;
	std::cout << "L          | L" << std::endl;
	std::cout << "L'         | K" << std::endl;
	std::cout << "D          | D" << std::endl;
	std::cout << "D'         | S" << std::endl << std::endl;
	std::cout << "Scramble   | Z" << std::endl;
	std::cout << "Solve      | X" << std::endl << std::endl;

	std::cout << "Acercar Camara           | UP" << std::endl;
	std::cout << "Alejar Camara            | DOWN" << std::endl;
	std::cout << "Camara a la Izquierda    | LEFT" << std::endl;
	std::cout << "Camara a la Derecha      | RIGHT" << std::endl;
	std::cout << "Cambiar Posición Camara  | Mouse" << std::endl << std::endl;

	/////////////////////////////////////////////////////////////////

	char* pattern0 = "./";
	std::vector<std::vector<cubo*>> c_cubes(20);
	for (int j = 0; j < 20; ++j) {
		for (int i = 0; i < 27; i++) {
			c_cubes[j].push_back(new cubo(worldPostions[j][i], pintar[i]));
		}
	}

	//===================================================================================================================//
	while (!glfwWindowShouldClose(window)) {
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//MOVEMENTS SWITCH
		for (int k = 0; k < 12; k++) {
			if (MOVES[k]) {
				switch (k) {
				case 0: {//F
						int nums[9] = { 18,19,20,21,22,23,24,25,26 };
						int aux[9] = { 22,25,19,21,24,18,23,26,20 };
						int order[9][3] = { {0,1,-1},{0,2,1},{0,2,1},
											{0,-1,-1},{0,1,-1},{0,1,-1},
											{0,1,-1},{0,2,1},{0,2,1}};

						for (int i = 0; i < 20; ++i) {
							for (int j = 0; j < 9; ++j) {
								c_cubes[i][nums[j]]->update_pos(worldPostions[i][aux[j]]);
								c_cubes[i][nums[j]]->update_order(pintar[aux[j]], order[j]);
							}
							updates_cubes(c_cubes[i], nums, aux);
						}
						MOVES[k] = 0;
					}
					break;
				case 1:{//F'
						int nums[9] = { 18,19,20,21,22,23,24,25,26 };
						int aux[9] = { 23,20,26,21,18,24,22,19,25 };
						int order[9][3] = { {0,1,-1},{0,2,1},{0,2,1},
											{0,-1,-1},{0,1,-1},{0,1,-1},
											{0,1,-1},{0,2,1},{0,2,1} };

						for (int i = 0; i < 20; ++i) {
							for (int j = 0; j < 9; ++j) {
								c_cubes[i][nums[j]]->update_pos(worldPostions[i][aux[j]]);
								c_cubes[i][nums[j]]->update_order(pintar[aux[j]], order[j]);
							}
							updates_cubes(c_cubes[i], nums, aux);
						}
						MOVES[k] = 0;
					}
					break;
				case 2:{//R
						int nums[9] = { 1,4,7,10,13,16,19,22,25 };
						int aux[9] = { 7,16,25,4,13,22,1,10,19 };
						int order[9][3] = { {2,1,0},{1,0,-1},{2,1,0},
											{1,0,-1},{0,-1,-1},{1,0,-1},
											{2,1,0},{1,0,-1},{2,1,0} };

						for (int i = 0; i < 20; ++i) {
							for (int j = 0; j < 9; ++j) {
								c_cubes[i][nums[j]]->update_pos(worldPostions[i][aux[j]]);
								c_cubes[i][nums[j]]->update_order(pintar[aux[j]], order[j]);
							}
							updates_cubes(c_cubes[i], nums, aux);
						}
						MOVES[k] = 0;
					}
					break;
				case 3:{//R'
						int nums[9] = { 1,4,7,10,13,16,19,22,25 };
						int aux[9] = { 19,10,1,22,13,4,25,16,7 };
						int order[9][3] = { {2,1,0},{1,0,-1},{2,1,0},
											{1,0,-1},{0,-1,-1},{1,0,-1},
											{2,1,0},{1,0,-1},{2,1,0} };

						for (int i = 0; i < 20; ++i) {
							for (int j = 0; j < 9; ++j) {
								c_cubes[i][nums[j]]->update_pos(worldPostions[i][aux[j]]);
								c_cubes[i][nums[j]]->update_order(pintar[aux[j]], order[j]);
							}
							updates_cubes(c_cubes[i], nums, aux);
						}
						MOVES[k] = 0;
					}
					break;
				case 4:{//U
						int nums[9] = { 0,1,2,9,10,11,18,19,20 };
						int aux[9] = { 10,19,1,9,18,0,11,20,2 };
						int order[9][3] = { {0,1,-1},{1,0,2},{1,0,2},
											{0,-1,-1},{0,1,-1},{0,1,-1},
											{0,1,-1},{1,0,2},{1,0,2} };

						for (int i = 0; i < 20; ++i) {
							for (int j = 0; j < 9; ++j) {
								c_cubes[i][nums[j]]->update_pos(worldPostions[i][aux[j]]);
								c_cubes[i][nums[j]]->update_order(pintar[aux[j]], order[j]);
							}
							updates_cubes(c_cubes[i], nums, aux);
						}
						MOVES[k] = 0;
					}
					break;
				case 5:{//U'
						int nums[9] = { 0,1,2,9,10,11,18,19,20 };
						int aux[9] = { 11,2,20,9,0,18,10,1,19 };
						int order[9][3] = { {0,1,-1},{1,0,2},{1,0,2},
											{0,-1,-1},{0,1,-1},{0,1,-1},
											{0,1,-1},{1,0,2},{1,0,2} };

						for (int i = 0; i < 20; ++i) {
							for (int j = 0; j < 9; ++j) {
								c_cubes[i][nums[j]]->update_pos(worldPostions[i][aux[j]]);
								c_cubes[i][nums[j]]->update_order(pintar[aux[j]], order[j]);
							}
							updates_cubes(c_cubes[i], nums, aux);
						}
						MOVES[k] = 0;
					}
					break;
				case 6:{//B
						int nums[9] = { 0,1,2,3,4,5,6,7,8 };
						int aux[9] = { 5,2,8,3,0,6,4,1,7 };
						int order[9][3] = { {0,1,-1},{0,2,1},{0,2,1},
											{0,-1,-1},{0,1,-1},{0,1,-1},
											{0,1,-1},{0,2,1},{0,2,1} };

						for (int i = 0; i < 20; ++i) {
							for (int j = 0; j < 9; ++j) {
								c_cubes[i][nums[j]]->update_pos(worldPostions[i][aux[j]]);
								c_cubes[i][nums[j]]->update_order(pintar[aux[j]], order[j]);
							}
							updates_cubes(c_cubes[i], nums, aux);
						}
						MOVES[k] = 0;
					}
					break;
				case 7:{//B'
						int nums[9] = { 0,1,2,3,4,5,6,7,8 };
						int aux[9] = { 4,7,1,3,6,0,5,8,2 };
						int order[9][3] = { {0,1,-1},{0,2,1},{0,2,1},
											{0,-1,-1},{0,1,-1},{0,1,-1},
											{0,1,-1},{0,2,1},{0,2,1} };

						for (int i = 0; i < 20; ++i) {
							for (int j = 0; j < 9; ++j) {
								c_cubes[i][nums[j]]->update_pos(worldPostions[i][aux[j]]);
								c_cubes[i][nums[j]]->update_order(pintar[aux[j]], order[j]);
							}
							updates_cubes(c_cubes[i], nums, aux);
						}
						MOVES[k] = 0;
					}
					break;
				case 8:{//L
						int nums[9] = { 2,5,8,11,14,17,20,23,26 };
						int aux[9] = { 8,17,26,5,14,23,2,11,20 };
						int order[9][3] = { {2,1,0},{1,0,-1},{2,1,0},
											{1,0,-1},{0,-1,-1},{1,0,-1},
											{2,1,0},{1,0,-1},{2,1,0} };

						for (int i = 0; i < 20; ++i) {
							for (int j = 0; j < 9; ++j) {
								c_cubes[i][nums[j]]->update_pos(worldPostions[i][aux[j]]);
								c_cubes[i][nums[j]]->update_order(pintar[aux[j]], order[j]);
							}
							updates_cubes(c_cubes[i], nums, aux);
						}
						MOVES[k] = 0;
					}
					break;
				case 9:{//L'
						int nums[9] = { 2,5,8,11,14,17,20,23,26 };
						int aux[9] = { 20,11,2,23,14,5,26,17,8 };
						int order[9][3] = { {2,1,0},{1,0,-1},{2,1,0},
											{1,0,-1},{0,-1,-1},{1,0,-1},
											{2,1,0},{1,0,-1},{2,1,0} };

						for (int i = 0; i < 20; ++i) {
							for (int j = 0; j < 9; ++j) {
								c_cubes[i][nums[j]]->update_pos(worldPostions[i][aux[j]]);
								c_cubes[i][nums[j]]->update_order(pintar[aux[j]], order[j]);
							}
							updates_cubes(c_cubes[i], nums, aux);
						}
						MOVES[k] = 0;
					}
					break;
				case 10:{//D
						int nums[9] = { 6,7,8,15,16,17,24,25,26 };
						int aux[9] = { 17,8,26,15,6,24,16,7,25 };
						int order[9][3] = { {0,1,-1},{1,0,2},{1,0,2},
											{0,-1,-1},{0,1,-1},{0,1,-1},
											{0,1,-1},{1,0,2},{1,0,2} };

						for (int i = 0; i < 20; ++i) {
							for (int j = 0; j < 9; ++j) {
								c_cubes[i][nums[j]]->update_pos(worldPostions[i][aux[j]]);
								c_cubes[i][nums[j]]->update_order(pintar[aux[j]], order[j]);
							}
							updates_cubes(c_cubes[i], nums, aux);
						}
						MOVES[k] = 0;
					}
					break;
				case 11:{//D'
						int nums[9] = { 6,7,8,15,16,17,24,25,26 };
						int aux[9] = { 16,25,7,15,24,6,17,26,8 };
						int order[9][3] = { {0,1,-1},{1,0,2},{1,0,2},
											{0,-1,-1},{0,1,-1},{0,1,-1},
											{0,1,-1},{1,0,2},{1,0,2} };

						for (int i = 0; i < 20; ++i) {
							for (int j = 0; j < 9; ++j) {
								c_cubes[i][nums[j]]->update_pos(worldPostions[i][aux[j]]);
								c_cubes[i][nums[j]]->update_order(pintar[aux[j]], order[j]);
							}
							updates_cubes(c_cubes[i], nums, aux);
						}
						MOVES[k] = 0;
					}
					break;
				default:
					break;
				}
				break;
			}
		}

		/////////////////////////////////////////////////COMPRESION ANIMATION///////////////////////////////////////////////////////////////////////////

		if (!flag_1 && !flag_2 && !flag_3 && !flag_4 && !flag_5 && !flag_6 && !flag_7 && !flag_8 && !flag_9 && !flag_10 && !flag_11 && !flag_12 && is_solve) {
			flag_scramble = true;
			flag_solve = true;
			flag_12 = true;
		}
		if (!flag_solve) {
			if (limitss >= 0.103) {
				flag_1 = false;
				flag_2 = true;
				limitss = 0.0;
			}
			if (limit_side <= -0.103) {
				flag_1 = false;
				flag_2 = false;
				flag_3 = true;
				limit_side = 0.0;
			}
			if (limit_up >= 0.103) {
				flag_1 = flag_2 = flag_3 = false;
				flag_4 = true;
				limit_up = 0.0;
			}
			if (limit_side_left >= 0.103) {
				flag_1 = flag_2 = flag_3 = flag_4 = false;
				flag_5 = true;
				limit_side_left = 0.0;
			}
			if (limit_side_left_u >= 0.103) {
				flag_1 = flag_2 = flag_3 = flag_4 = flag_5 = false;
				flag_6 = true;
				limit_side_left_u = 0.0;
			}
			if (limit_side_left_d >= 0.103) {
				flag_1 = flag_2 = flag_3 = flag_4 = flag_5 = flag_6 = false;
				flag_7 = true;
				limit_side_left_d = 0.0;
			}
			if (limit_side_right_d <= -0.103) {
				flag_1 = flag_2 = flag_3 = flag_4 = flag_5 = flag_6 = flag_7 = false;
				flag_8 = true;
				limit_side_right_d = 0.0;
			}
			if (limit_mid_d > 0.103) {
				flag_1 = flag_2 = flag_3 = flag_4 = flag_5 = flag_6 = flag_7 = flag_8 = false;
				flag_9 = true;
				limit_mid_d = 0.0;
			}
			if (limit_side_left_d_2 >= 0.103) {
				flag_1 = flag_2 = flag_3 = flag_4 = flag_5 = flag_6 = flag_7 = flag_8 = flag_9 = false;
				flag_10 = true;
				limit_side_left_d_2 = 0.0;
			}
			if (limit_side_right_d_2 <= -0.103) {
				flag_1 = flag_2 = flag_3 = flag_4 = flag_5 = flag_6 = flag_7 = flag_8 = flag_9 = flag_10 = false;
				flag_11 = true;
				limit_side_right_d_2 = 0.0;
			}
			if (limit_mid_up > 0.103) {
				flag_1 = flag_2 = flag_3 = flag_4 = flag_5 = flag_6 = flag_7 = flag_8 = flag_9 = flag_10 = flag_11 = false;
				flag_12 = true;
				limit_mid_up = 0.0;
			}
			if (limit_mid_down < -0.103) {
				flag_1 = flag_2 = flag_3 = flag_4 = flag_5 = flag_6 = flag_7 = flag_8 = flag_9 = flag_10 = flag_11 = flag_12 = false;
				limit_mid_down = 0.0;
			}
			if (flag_1) {//mid->DOWN
				if (limitss < 0.1) {
					limitss = limitss + 0.0052;
				}
				for (int i = 0; i < 20; i++) {
					for (int j = 0; j < 27; j++) {
						if (j == 3 || j == 4 || j == 5) {
							if (limitss < 0.1)
								c_cubes[i][j]->move(glm::vec3(0.0, 0.0, limitss));
						}
					}
				}
			}
			if (flag_2) {//Left -> MID
				if (limit_side > -0.1) {
					limit_side = limit_side - 0.0052;
				}
				for (int i = 0; i < 20; i++) {
					for (int j = 0; j < 27; j++) {
						if (j == 0 || j == 1 || j == 2) {
							if (limit_side > -0.1)
								c_cubes[i][j]->move(glm::vec3(0.0, limit_side, 0.0));
						}
					}
				}
			}
			if (flag_3) {//left->mid->DOWN
				if (limit_up < 0.1) {
					limit_up = limit_up + 0.0052;
				}
				for (int i = 0; i < 20; i++) {
					for (int j = 0; j < 27; j++) {
						if (j == 0 || j == 1 || j == 2) {
							if (limit_up < 0.1)
								c_cubes[i][j]->move(glm::vec3(0.0, 0.0, limit_up));
						}
					}
				}
			}
			if (flag_4) {//Right->LEFT
				if (limit_side_left <= 0.1) {
					limit_side_left = limit_side_left + 0.0052;
				}
				for (int i = 0; i < 20; i++) {
					for (int j = 0; j < 27; j++) {
						if (j == 6 || j == 7 || j == 8) {
							if (limit_side_left <= 0.1)
								c_cubes[i][j]->move(glm::vec3(0.0, limit_side_left, 0.0));
						}
					}
				}
			}
			if (flag_5) {//right->left->DOWN
				if (limit_side_left_u < 0.1) {
					limit_side_left_u = limit_side_left_u + 0.0052;
				}
				for (int i = 0; i < 20; i++) {
					for (int j = 0; j < 27; j++) {
						if (j == 6 || j == 7 || j == 8) {
							if (limit_side_left_u < 0.1)
								c_cubes[i][j]->move(glm::vec3(0.0, 0.0, limit_side_left_u));
						}
					}
				}
			}
			if (flag_6) {//left_d -> MID
				if (limit_side_left_d < 0.1) {
					limit_side_left_d = limit_side_left_d + 0.0052;
				}
				for (int i = 0; i < 20; i++) {
					for (int j = 0; j < 27; j++) {
						if (j == 15 || j == 16 || j == 17) {
							if (limit_side_left_d < 0.1)
								c_cubes[i][j]->move(glm::vec3(0.0, limit_side_left_d, 0.0));
						}
					}
				}
			}
			if (flag_7) {//rigth_d-> MID
				if (limit_side_right_d > -0.1) {
					limit_side_right_d = limit_side_right_d - 0.0052;
				}
				for (int i = 0; i < 20; i++) {
					for (int j = 0; j < 27; j++) {
						if (j == 9 || j == 10 || j == 11) {
							if (limit_side_right_d > -0.1)
								c_cubes[i][j]->move(glm::vec3(0.0, limit_side_right_d, 0.0));
						}
					}
				}
			}
			if (flag_8) {//mid_d -> DOWN
				if (limit_mid_d < 0.1) {
					limit_mid_d = limit_mid_d + 0.0052;
				}
				for (int i = 0; i < 20; i++) {
					for (int j = 0; j < 27; j++) {
						if (j >= 0 && j <= 17) {
							if (limit_mid_d < 0.1)
								c_cubes[i][j]->move(glm::vec3(0.0, 0.0, limit_mid_d));
						}
					}
				}
			}
			if (flag_9) {
				if (limit_side_left_d_2 < 0.1) {
					limit_side_left_d_2 = limit_side_left_d_2 + 0.0052;
				}
				for (int i = 0; i < 20; i++) {
					for (int j = 0; j < 27; j++) {
						if (j == 24 || j == 25 || j == 26) {
							if (limit_side_left_d_2 < 0.1)
								c_cubes[i][j]->move(glm::vec3(0.0, limit_side_left_d_2, 0.0));
						}
					}
				}
			}
			if (flag_10) {
				if (limit_side_right_d_2 > -0.1) {
					limit_side_right_d_2 = limit_side_right_d_2 - 0.0052;
				}
				for (int i = 0; i < 20; i++) {
					for (int j = 0; j < 27; j++) {
						if (j == 18 || j == 19 || j == 20) {
							if (limit_side_right_d_2 > -0.1)
								c_cubes[i][j]->move(glm::vec3(0.0, limit_side_right_d_2, 0.0));
						}
					}
				}
			}
			if (flag_11) {
				if (limit_mid_up < 0.1) {
					limit_mid_up = limit_mid_up + 0.0052;
				}
				for (int i = 0; i < 20; i++) {
					for (int j = 0; j < 27; j++) {
						if (j == 23 || j == 2 || j == 5 || j == 8 || j == 11 || j == 14 || j == 17 || j == 20 || j == 26) {
							if (limit_mid_up < 0.1)
								c_cubes[i][j]->move(glm::vec3(limit_mid_up, 0.0, 0.0));
						}
					}
				}
			}
			if (flag_12) {
				if (limit_mid_down > -0.1) {
					limit_mid_down = limit_mid_down - 0.0052;
				}
				for (int i = 0; i < 20; i++) {
					for (int j = 0; j < 27; j++) {
						if (j == 1 || j == 4 || j == 7 || j == 10 || j == 13 || j == 16 || j == 19 || j == 22 || j == 25) {
							if (limit_mid_down > -0.1)
								c_cubes[i][j]->move(glm::vec3(limit_mid_down, 0.0, 0.0));
						}
					}
				}
			}
		}
		if (flag_scramble && flag_solve) {
			if (limit_mid_down > 0.103) {
				flag_1 = flag_2 = flag_3 = flag_4 = flag_5 = flag_6 = flag_7 = flag_8 = flag_9 = flag_10 = flag_12 = false;
				flag_11 = true;
				limit_mid_down = 0.0;
			}
			if (limit_mid_up < -0.103) {
				flag_1 = flag_2 = flag_3 = flag_4 = flag_5 = flag_6 = flag_7 = flag_8 = flag_9 = flag_12 = flag_11 = false;
				flag_10 = true;
				limit_mid_up = 0.0;
			}
			if (limit_side_right_d_2 >= 0.103) {
				flag_1 = flag_2 = flag_3 = flag_4 = flag_5 = flag_6 = flag_7 = flag_8 = flag_12 = flag_10 = flag_11 = false;
				flag_9 = true;
				limit_side_right_d_2 = 0.0;
			}
			if (limit_side_left_d_2 <= -0.103) {
				flag_1 = flag_2 = flag_3 = flag_4 = flag_5 = flag_6 = flag_7 = flag_12 = flag_9 = flag_10 = flag_11 = false;
				flag_8 = true;
				limit_side_left_d_2 = 0.0;
			}
			if (limit_mid_d < -0.103) {
				flag_1 = flag_2 = flag_3 = flag_4 = flag_5 = flag_6 = flag_9 = flag_8 = flag_10 = flag_11 = flag_12 = false;
				flag_7 = true;
				limit_mid_d = 0.0;
			}
			if (limit_side_right_d >= 0.103) {
				flag_1 = flag_2 = flag_3 = flag_4 = flag_5 = flag_7 = flag_8 = flag_9 = flag_10 = flag_11 = flag_12 = false;
				flag_6 = true;
				limit_side_right_d = 0.0;
			}
			if (limit_side_left_d <= -0.103) {
				flag_1 = flag_2 = flag_3 = flag_4 = flag_6 = flag_7 = flag_8 = flag_9 = flag_10 = flag_11 = flag_12 = false;
				flag_5 = true;
				limit_side_left_d = 0.0;
			}
			if (limit_side_left_u <= -0.103) {
				flag_1 = flag_2 = flag_3 = flag_5 = flag_6 = flag_7 = flag_8 = flag_9 = flag_10 = flag_11 = flag_12 = false;
				flag_4 = true;
				limit_side_left_u = 0.0;
			}
			if (limit_side_left <= -0.103) {
				flag_1 = flag_2 = flag_4 = flag_5 = flag_6 = flag_7 = flag_8 = flag_9 = flag_10 = flag_11 = flag_12 = false;
				flag_3 = true;
				limit_side_left = 0.0;
			}
			if (limit_up <= -0.103) {
				flag_1 = flag_3 = flag_4 = flag_5 = flag_6 = flag_7 = flag_8 = flag_9 = flag_10 = flag_11 = flag_12 = false;
				flag_2 = true;
				limit_up = 0.0;
			}
			if (limit_side >= 0.103) {
				flag_2 = flag_3 = flag_4 = flag_5 = flag_6 = flag_7 = flag_8 = flag_9 = flag_10 = flag_11 = flag_12 = false;
				flag_1 = true;
				limit_side = 0.0;
			}
			//**********************************************************************************************************//
			if (flag_12) {
				if (limit_mid_down < 0.1) {
					limit_mid_down = limit_mid_down + 0.0052;
				}
				for (int i = 0; i < 20; i++) {
					for (int j = 0; j < 27; j++) {
						if (j == 1 || j == 4 || j == 7 || j == 10 || j == 13 || j == 16 || j == 19 || j == 22 || j == 25) {
							if (limit_mid_down < 0.1)
								c_cubes[i][j]->move(glm::vec3(limit_mid_down, 0.0, 0.0));
						}
					}
				}
			}
			if (flag_11) {
				if (limit_mid_up > -0.1) {
					limit_mid_up = limit_mid_up - 0.0052;
				}
				for (int i = 0; i < 20; i++) {
					for (int j = 0; j < 27; j++) {
						if (j == 23 || j == 2 || j == 5 || j == 8 || j == 11 || j == 14 || j == 17 || j == 20 || j == 26) {
							if (limit_mid_up > -0.1)
								c_cubes[i][j]->move(glm::vec3(limit_mid_up, 0.0, 0.0));
						}
					}
				}
			}
			if (flag_10) {
				if (limit_side_right_d_2 < 0.1) {
					limit_side_right_d_2 = limit_side_right_d_2 + 0.0052;
				}
				for (int i = 0; i < 20; i++) {
					for (int j = 0; j < 27; j++) {
						if (j == 18 || j == 19 || j == 20) {
							if (limit_side_right_d_2 < 0.1)
								c_cubes[i][j]->move(glm::vec3(0.0, limit_side_right_d_2, 0.0));
						}
					}
				}
			}
			if (flag_9) {
				if (limit_side_left_d_2 > -0.1) {
					limit_side_left_d_2 = limit_side_left_d_2 - 0.0052;
				}
				for (int i = 0; i < 20; i++) {
					for (int j = 0; j < 27; j++) {
						if (j == 24 || j == 25 || j == 26) {
							if (limit_side_left_d_2 > -0.1)
								c_cubes[i][j]->move(glm::vec3(0.0, limit_side_left_d_2, 0.0));
						}
					}
				}
			}
			if (flag_8) {//mid_d -> DOWN
				if (limit_mid_d > -0.1) {
					limit_mid_d = limit_mid_d - 0.0052;
				}
				for (int i = 0; i < 20; i++) {
					for (int j = 0; j < 27; j++) {
						if (j >= 0 && j <= 17) {
							if (limit_mid_d > -0.1)
								c_cubes[i][j]->move(glm::vec3(0.0, 0.0, limit_mid_d));
						}
					}
				}
			}
			if (flag_7) {//rigth_d-> MID
				if (limit_side_right_d < 0.1) {
					limit_side_right_d = limit_side_right_d + 0.0052;
				}
				for (int i = 0; i < 20; i++) {
					for (int j = 0; j < 27; j++) {
						if (j == 9 || j == 10 || j == 11) {
							if (limit_side_right_d < 0.1)
								c_cubes[i][j]->move(glm::vec3(0.0, limit_side_right_d, 0.0));
						}
					}
				}
			}
			if (flag_6) {//left_d -> MID
				if (limit_side_left_d > -0.1) {
					limit_side_left_d = limit_side_left_d - 0.0052;
				}
				for (int i = 0; i < 20; i++) {
					for (int j = 0; j < 27; j++) {
						if (j == 15 || j == 16 || j == 17) {
							if (limit_side_left_d > -0.1)
								c_cubes[i][j]->move(glm::vec3(0.0, limit_side_left_d, 0.0));
						}
					}
				}
			}
			if (flag_5) {//right->left->DOWN
				if (limit_side_left_u > -0.1) {
					limit_side_left_u = limit_side_left_u - 0.0052;
				}
				for (int i = 0; i < 20; i++) {
					for (int j = 0; j < 27; j++) {
						if (j == 6 || j == 7 || j == 8) {
							if (limit_side_left_u > -0.1)
								c_cubes[i][j]->move(glm::vec3(0.0, 0.0, limit_side_left_u));
						}
					}
				}
			}
			if (flag_4) {//Right->LEFT
				if (limit_side_left >= -0.1) {
					limit_side_left = limit_side_left - 0.0052;
				}
				for (int i = 0; i < 20; i++) {
					for (int j = 0; j < 27; j++) {
						if (j == 6 || j == 7 || j == 8) {
							if (limit_side_left >= -0.1)
								c_cubes[i][j]->move(glm::vec3(0.0, limit_side_left, 0.0));
						}
					}
				}
			}
			if (flag_3) {//left->mid->DOWN
				if (limit_up > -0.1) {
					limit_up = limit_up - 0.0052;
				}
				for (int i = 0; i < 20; i++) {
					for (int j = 0; j < 27; j++) {
						if (j == 0 || j == 1 || j == 2) {
							if (limit_up > -0.1)
								c_cubes[i][j]->move(glm::vec3(0.0, 0.0, limit_up));
						}
					}
				}
			}
			if (flag_2) {//Left -> MID
				if (limit_side < 0.1) {
					limit_side = limit_side + 0.0052;
				}
				for (int i = 0; i < 20; i++) {
					for (int j = 0; j < 27; j++) {
						if (j == 0 || j == 1 || j == 2) {
							if (limit_side < 0.1)
								c_cubes[i][j]->move(glm::vec3(0.0, limit_side, 0.0));
						}
					}
				}
			}
			if (flag_1) {//mid->DOWN
				if (limitss > -0.1) {
					limitss = limitss - 0.0052;
				}
				for (int i = 0; i < 20; i++) {
					for (int j = 0; j < 27; j++) {

						if (j == 3 || j == 4 || j == 5) {
							if (limitss > -0.1)
								c_cubes[i][j]->move(glm::vec3(0.0, 0.0, limitss));
						}
					}
				}
			}
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		for (int j = 0; j < 20; ++j) {
			for (int i = 0; i < 27; i++) {
				c_cubes[j][i]->anim(camera);
				c_cubes[j][i]->draw_cube(shader_t, textures);
				c_cubes[j][i]->reset_position();
				//std::cout << c_cubes[j][i]->pos_x<<" "<< c_cubes[j][i]->pos_y << " "<< c_cubes[j][i]->pos_z<< std::endl;
			}
		}

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (CHECK_SOLUTION) {
			char* aux1 = obtain_colors(c_cubes[0]);
			char* aux2 = "UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB";
			CHECK_SOLUTION = 0;

			if (strcmp(aux1, aux2) != 1) {
				SOLVE = 1;
			}
			else {
				is_solve = true;
			}
		}

		if (SCRAMBLE_CONT < 20 && SOLVING==0) {
			scramble();
			SCRAMBLE_CONT++;
		}

		if (SOLVE && SCRAMBLE_CONT>=20) {
			char* pattern1 = obtain_colors(c_cubes[0]);
			std::cout << "Obtained Faces: ";
			for (int i = 0; i < 54; ++i) {
				std::cout << pattern1[i];
			}std::cout << std::endl;

			char** pattern_ptr = new char*;
			pattern_ptr[0] = pattern0;
			pattern_ptr[1] = pattern1;
			std::string solved = solution(2, pattern_ptr);

			if (solved.compare("Unsolvable cube!") == 0){
				std::cout << "Unsolvable cube!" << std::endl;
				SOLVE = 0;
			}
			else {
				std::vector<char*> parsed = split_line(solved);
				for (int i = 0; i < parsed.size(); ++i) {
					resolution.push(parsed[i]);
				}
				SOLVE = 0;
				SOLVING = 1;
			}
		}

		if (SCRAMBLE_CONT == 20) {
			flag_solve = false;
			SCRAMBLE_CONT++;
		}

		if (SOLVING) {
			solution_application();
		}

		for (int j = 0; j < 20; ++j) {
			for (int i = 0; i < 27; i++) {
				c_cubes[j][i]->del();
			}
		}		
	}

	for (int j = 0; j < 20; ++j) {
		for (int i = 0; i < 27; i++) {
			delete c_cubes[j][i];
		}
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos){
	if (firstMouse){
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
	camera.ProcessMouseScroll(yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		for (int i = 0; i < 12; ++i) {
			if (i != 0) {
				MOVES[i] = 0;
			}
			else {
				MOVES[i] = 1;
			}
		}
	}

	else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		for (int i = 0; i < 12; ++i) {
			if (i != 1) {
				MOVES[i] = 0;
			}
			else {
				MOVES[i] = 1;
			}
		}
	}

	else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		for (int i = 0; i < 12; ++i) {
			if (i != 2) {
				MOVES[i] = 0;
			}
			else {
				MOVES[i] = 1;
			}
		}
	}

	else if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
		for (int i = 0; i < 12; ++i) {
			if (i != 3) {
				MOVES[i] = 0;
			}
			else {
				MOVES[i] = 1;
			}
		}
	}

	else if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
		for (int i = 0; i < 12; ++i) {
			if (i != 4) {
				MOVES[i] = 0;
			}
			else {
				MOVES[i] = 1;
			}
		}
	}

	else if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		for (int i = 0; i < 12; ++i) {
			if (i != 5) {
				MOVES[i] = 0;
			}
			else {
				MOVES[i] = 1;
			}
		}
	}

	else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
		for (int i = 0; i < 12; ++i) {
			if (i != 6) {
				MOVES[i] = 0;
			}
			else {
				MOVES[i] = 1;
			}
		}
	}

	else if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
		for (int i = 0; i < 12; ++i) {
			if (i != 7) {
				MOVES[i] = 0;
			}
			else {
				MOVES[i] = 1;
			}
		}
	}

	else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		for (int i = 0; i < 12; ++i) {
			if (i != 8) {
				MOVES[i] = 0;
			}
			else {
				MOVES[i] = 1;
			}
		}
	}

	else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		for (int i = 0; i < 12; ++i) {
			if (i != 9) {
				MOVES[i] = 0;
			}
			else {
				MOVES[i] = 1;
			}
		}
	}

	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		for (int i = 0; i < 12; ++i) {
			if (i != 10) {
				MOVES[i] = 0;
			}
			else {
				MOVES[i] = 1;
			}
		}
	}

	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		for (int i = 0; i < 12; ++i) {
			if (i != 11) {
				MOVES[i] = 0;
			}
			else {
				MOVES[i] = 1;
			}
		}
	}

	else if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		SCRAMBLE_CONT = 0;
	}

	else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		SOLVE = 1;
	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}