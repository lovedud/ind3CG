#include <SOIL.h>
#include <gl/glew.h> 
#include <gl/GL.h>
#include <gl/GLU.h>
#include <gl/freeglut.h>
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <regex>

class GLShader
{
private:
	GLuint vertex_shader;
	GLuint fragment_shader;

	GLuint compileSource(const char* source, GLuint shader_type)
	{
		GLuint shader = glCreateShader(shader_type);
		glShaderSource(shader, 1, &source, NULL);
		glCompileShader(shader);
		return shader;
	}

	void linkProgram()
	{
		ShaderProgram = glCreateProgram();
		glAttachShader(ShaderProgram, vertex_shader);
		glAttachShader(ShaderProgram, fragment_shader);
		glLinkProgram(ShaderProgram);
	}

public:
	GLuint ShaderProgram;

	GLShader() :ShaderProgram(0)
	{
	}

	~GLShader()
	{
		glUseProgram(0);
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		glDeleteProgram(ShaderProgram);
	}

	void load(const char* vertext_src, const char* fragment_src)
	{
		vertex_shader = compileSource(vertext_src, GL_VERTEX_SHADER);
		fragment_shader = compileSource(fragment_src, GL_FRAGMENT_SHADER);
		linkProgram();
	}

	void load_vertex_shader(const char* vertext_src, const char* fragment_src)
	{
		vertex_shader = compileSource(vertext_src, GL_VERTEX_SHADER);
		fragment_shader = compileSource(fragment_src, GL_FRAGMENT_SHADER);
		linkProgram();
	}

	GLuint getIDProgram()
	{
		return ShaderProgram;
	}

	GLint getAttribLocation(const char* name)const
	{
		GLint t = glGetAttribLocation(ShaderProgram, name);

		if (t == -1)
		{
			std::cout << "could not bind attrib " << name << std::endl;
			return -1;
		}

		return t;
	}

	GLuint getUniformLocation(const char* name)const
	{
		GLint t = glGetUniformLocation(ShaderProgram, name);

		if (t == -1)
		{
			std::cout << "could not bind uniform " << name << std::endl;
			return -1;
		}

		return t;
	}

	void use()
	{
		glUseProgram(ShaderProgram);
	}
};

class Athens
{
public:
	glm::mat4 matrix;
	glm::mat4 rotate;

	Athens(double m_x, double m_y, double m_z, double s_x, double s_y, double s_z, double r_x, double r_y, double r_z)
	{
		glm::mat4 move =
		{
			1.0f,  0.0f, 0.0f, m_x,
			0.0f, 1.0f, 0.0f, m_y,
			0.0f, 0.0f, 1.0f, m_z,
			0, 0, 0, 1.0f
		};

		glm::mat4 scale =
		{
			s_x,  0.0f, 0.0f, 0.0f,
			0.0f, s_y, 0.0f, 0.0f,
			0.0f, 0.0f, s_z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		glm::mat4 rotate_x =
		{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, glm::cos(r_x), -glm::sin(r_x), 0.0f,
			0.0f, glm::sin(r_x), glm::cos(r_x), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		glm::mat4 rotate_y =
		{
			glm::cos(r_y), 0.0f, glm::sin(r_y), 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			-glm::sin(r_y), 0.0f, glm::cos(r_y), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		glm::mat4 rotate_z =
		{
			glm::cos(r_z),  -glm::sin(r_z), 0.0f, 0.0f,
			glm::sin(r_z), glm::cos(r_z), 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		rotate = glm::mat4(1.0f) * rotate_x * rotate_y * rotate_z;
		matrix = glm::mat4(1.0f) * scale * rotate_x * rotate_y * rotate_z * move;
	}
};

class Point_2D
{

public:
	float x;
	float y;

	Point_2D(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	Point_2D()
	{
		x = 0;
		y = 0;
	}
};

class Point_3D
{

public:
	float x;
	float y;
	float z;

	Point_3D(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Point_3D()
	{
		x = 0;
		y = 0;
		z = 0;
	}
};

class Mesh
{

public:
	std::vector<Point_2D> texturePoint;

	std::vector<Point_3D> pointList;
	std::vector<Point_3D> normalList;

	std::vector<std::vector<std::tuple<int, int, int>>> polygons;
	std::vector<int> indicesList;

	Mesh()
	{
		this->texturePoint = std::vector<Point_2D>();
		this->normalList = std::vector<Point_3D>();
		this->pointList = std::vector<Point_3D>();
		this->polygons = std::vector<std::vector<std::tuple<int, int, int>>>();
		this->indicesList = std::vector<int>();
	}

	Mesh(std::vector<Point_3D> points)
	{
		this->pointList = std::vector<Point_3D>();

		for (int i = 0; i < points.size(); i++)
		{
			this->pointList.push_back(points[i]);
		}
	}
};

struct Transform
{
	glm::mat3 normal;
	glm::vec3 view_position;
	glm::mat4 view_projection;
	glm::mat4 model;
};

struct Light
{
	float angle;
	glm::vec3 attenuation;
	glm::vec3 direction;
	glm::vec4 position;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
};

struct Material
{
	float shininess;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::vec4 emission;
};
