#include <vector>
#include <iostream>
#include <string>
#include "colors.h"
#include "camera.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

class figurita{
public:
	unsigned int VBA, VBO;
	std::vector<float> vertices;
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection;
	float timeValue = glfwGetTime();

	figurita() {}
	void animate(std::vector<float> vertices, glm::vec3 cube_position, Camera camara) {
		timeValue = glfwGetTime();

		this->vertices = vertices;
		glGenVertexArrays(1, &VBA);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VBA);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * 4, &vertices.front(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);


		model = glm::translate(model, glm::vec3(0.0, 0.0, 0.0));
		//model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1.0f, 1.0f, 0.0f));
		model = glm::translate(model, cube_position);
		//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -7.0f));
		//projection = glm::perspective(glm::radians(60.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		view = camara.GetViewMatrix();
		projection = glm::perspective(glm::radians(camara.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	}

	void draw(Shader shader_v, Textures texture_v) {
		texture_v.bind(GL_TEXTURE_2D);
		shader_v.use();
		shader_v.setMatrix("view", &view[0][0]);
		shader_v.setMatrix("projection", glm::value_ptr(projection));
		shader_v.setMatrix("model", glm::value_ptr(model));
		shader_v.setresolution("resolution", SCR_WIDTH, SCR_HEIGHT);
		shader_v.setFloat("time", timeValue);
		glBindVertexArray(VBA);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 8);
	}

	void delbuf() {
		glDeleteVertexArrays(1, &VBA);
		glDeleteBuffers(1, &VBO);

		vertices.clear();
		model = glm::mat4(1.0f);
		view = glm::mat4(1.0f);
		projection = glm::mat4(1.0f);
	}

	~figurita() {
		delbuf();
	}
};
//=================Caras del Cubo=================//
std::vector<float> cara1 = {
		-0.5f, -0.5f, -0.5f, HEX_COLOR(orangered),  0.0,0.0,
		 0.5f, -0.5f, -0.5f, HEX_COLOR(orangered),  1.0,0.0,
		 0.5f,  0.5f, -0.5f, HEX_COLOR(orangered),  1.0,1.0,
		 0.5f,  0.5f, -0.5f, HEX_COLOR(orangered),  1.0,1.0,
		-0.5f,  0.5f, -0.5f, HEX_COLOR(orangered),  0.0,1.0,
		-0.5f, -0.5f, -0.5f, HEX_COLOR(orangered),  0.0,0.0,
};
std::vector<float> cara2 = {
	-0.5f, -0.5f,  0.5f, HEX_COLOR(lightcyan),  0.0,0.0,
	 0.5f, -0.5f,  0.5f, HEX_COLOR(lightcyan),  1.0,0.0,
	 0.5f,  0.5f,  0.5f, HEX_COLOR(lightcyan),  1.0,1.0,
	 0.5f,  0.5f,  0.5f, HEX_COLOR(lightcyan),  1.0,1.0,
	-0.5f,  0.5f,  0.5f, HEX_COLOR(lightcyan),  0.0,1.0,
	-0.5f, -0.5f,  0.5f, HEX_COLOR(lightcyan),  0.0,0.0
};
std::vector<float> cara3 = {
	-0.5f,  0.5f,  0.5f, HEX_COLOR(lightblue),  0.0,0.0,
	-0.5f,  0.5f, -0.5f, HEX_COLOR(lightblue),  1.0,0.0,
	-0.5f, -0.5f, -0.5f, HEX_COLOR(lightblue),  1.0,1.0,
	-0.5f, -0.5f, -0.5f, HEX_COLOR(lightblue),  1.0,1.0,
	-0.5f, -0.5f,  0.5f, HEX_COLOR(lightblue),  0.0,1.0,
	-0.5f,  0.5f,  0.5f, HEX_COLOR(lightblue),  0.0,0.0
};
std::vector<float> cara4 = {
	 0.5f,  0.5f,  0.5f, HEX_COLOR(lightseagreen), 0.0,0.0,
	 0.5f,  0.5f, -0.5f, HEX_COLOR(lightseagreen), 1.0,0.0,
	 0.5f, -0.5f, -0.5f, HEX_COLOR(lightseagreen), 1.0,1.0,
	 0.5f, -0.5f, -0.5f, HEX_COLOR(lightseagreen), 1.0,1.0,
	 0.5f, -0.5f,  0.5f, HEX_COLOR(lightseagreen), 0.0,1.0,
	 0.5f,  0.5f,  0.5f, HEX_COLOR(lightseagreen), 0.0,0.0
};
std::vector<float> cara5 = {
	-0.5f, -0.5f, -0.5f,HEX_COLOR(purple), 0.0,0.0,
	 0.5f, -0.5f, -0.5f,HEX_COLOR(purple), 1.0,0.0,
	 0.5f, -0.5f,  0.5f,HEX_COLOR(purple), 1.0,1.0,
	 0.5f, -0.5f,  0.5f,HEX_COLOR(purple), 1.0,1.0,
	-0.5f, -0.5f,  0.5f,HEX_COLOR(purple), 0.0,1.0,
	-0.5f, -0.5f, -0.5f,HEX_COLOR(purple), 0.0,0.0
};
std::vector<float> cara6 = {
	-0.5f,  0.5f, -0.5f,HEX_COLOR(lightgreen), 0.0,0.0,
	 0.5f,  0.5f, -0.5f,HEX_COLOR(lightgreen), 1.0,0.0,
	 0.5f,  0.5f,  0.5f,HEX_COLOR(lightgreen), 1.0,1.0,
	 0.5f,  0.5f,  0.5f,HEX_COLOR(lightgreen), 1.0,1.0,
	-0.5f,  0.5f,  0.5f,HEX_COLOR(lightgreen), 0.0,1.0,
	-0.5f,  0.5f, -0.5f,HEX_COLOR(lightgreen), 0.0,0.0
};

class cubo {
public:
	figurita c1;
	figurita c2;
	figurita c3;
	figurita c4;
	figurita c5;
	figurita c6;
	float pos_x;
	float pos_y;
	float pos_z;
	glm::vec3 pos;
	std::vector<int> color;
	glm::mat4 rot = glm::mat4(1.0f);

	cubo(glm::vec3 position,std::vector<int> _color){
		pos_x = position.x;
		pos_y = position.y;
		pos_z = position.z;
		pos = position;

		for (int i = 0; i < _color.size(); ++i) {
			color.push_back(_color[i]);
		}
	}

	void update_pos(glm::vec3 position) {
		pos = position;
		pos_x = position.x;
		pos_y = position.y;
		pos_z = position.z;
	}

	void update_order(std::vector<int> new_color, int* order) {
		std::vector<int> real_colors;
		int cont = 0;
		for (int i = 0; i < color.size(); ++i) {
			if (color[i] != 6) {
				real_colors.push_back(color[i]);
			}
		}

		for (int i = 0; i < new_color.size(); ++i) {
			if (new_color[i] != 6) {
				color[i] = real_colors[order[cont]];
				cont++;
			}
			else {
				color[i] = 6;
			}
		}
	}

	void anim(Camera camara) {
		c1.animate(cara1, pos, camara);
		c2.animate(cara2, pos, camara);
		c3.animate(cara3, pos, camara);
		c4.animate(cara4, pos, camara);
		c5.animate(cara5, pos, camara);
		c6.animate(cara6, pos, camara);
	}

	void draw_cube(Shader shader_v, std::vector<Textures> texture_v){
		c1.draw(shader_v, texture_v[color[0]]);
		c2.draw(shader_v, texture_v[color[1]]);
		c3.draw(shader_v, texture_v[color[2]]);
		c4.draw(shader_v, texture_v[color[3]]);
		c5.draw(shader_v, texture_v[color[4]]);
		c6.draw(shader_v, texture_v[color[5]]);
	}

	void del() {
		c1.delbuf();
		c2.delbuf();
		c3.delbuf();
		c4.delbuf();
		c5.delbuf();
		c6.delbuf();
	}

	void move(glm::vec3 new_position) {
		glm::vec4 as(pos, 1.0);
		rot = glm::translate(rot, new_position);
		as = rot * as;
		update_pos(as);
	}

	void reset_position() {
		rot = glm::mat4(1.0f);
	}

	~cubo() {

	}
};