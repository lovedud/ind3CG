#include "Shader.h"

std::vector<std::string> tex_Paths;
std::string tex_Path_2;

bool mix = false;
//bool vert = false;
bool mix_color = false;
//bool light_on = false;

double mix_value = 0;
double move = 0;

double rotate_xx = 0;
double rotate_yy = 0;
double rotate_zz = 0;

std::vector<GLShader> shaders;
std::vector<Mesh> meshes;
std::vector<Athens> athens;
std::vector<Material> materials;
std::vector<GLuint> textures;
GLuint texture_2;

std::vector<GLuint> VBO_verteces;
std::vector<GLuint>  VBO_textures;
std::vector<GLuint>  VBO_normals;
std::vector<GLuint> VBO_colors;

const char* vsSource = "#version 330 core\n"
"layout(location = 0) in vec3 coord;\n"
"layout(location = 1) in vec2 vertexUV;\n"
"layout(location = 3) in vec3 vertexColor;\n"
"uniform mat4 MVP;\n"
"out vec2 UV;\n"
"out vec3 fragmentColor;\n"
"void main() {\n"
"	gl_Position = MVP * vec4(coord, 1.0);\n"
"	fragmentColor = vertexColor;\n"
"	UV = vertexUV;\n"
"}\n";

const char* fsSource_1 = "#version 330 core\n"
"in vec2 UV;\n"
"uniform sampler2D myTextureSampler;\n"
"void main() {\n"
"	gl_FragColor = texture(myTextureSampler, UV);\n"
"}\n";

const char* fsSource_2 = "#version 330 core\n"
"in vec2 UV;\n"
"uniform sampler2D myTextureSampler;\n"
"in vec3 fragmentColor;\n"
"void main() {\n"
"	gl_FragColor = mix(texture(myTextureSampler, UV), vec4(fragmentColor, 1.0), 0.3);\n"
"}\n";

const char* fsSource_3 = "#version 330 core\n"
"in vec2 UV;\n"
"uniform sampler2D myTextureSampler;\n"
"uniform sampler2D myTextureSampler1;\n"
"in vec3 fragmentColor;\n"
"uniform float mix_f;\n"
"void main() {\n"
"	gl_FragColor = mix(texture(myTextureSampler, UV), texture(myTextureSampler1, UV), mix_f);\n"
"}\n";


const char* vsSource_2 = "#version 330 core\n"
"#define VERT_POSITION 0\n"
"#define VERT_TEXCOORD 1\n"
"#define VERT_NORMAL 2\n"
"layout(location = VERT_POSITION) in vec3 position;\n"
"layout(location = VERT_TEXCOORD) in vec2 texcoord;\n"
"layout(location = VERT_NORMAL) in vec3 normal;\n"
"uniform struct Transform {\n"
"	mat4 model;\n"
"	mat4 viewProjection;\n"
"	mat3 normal;\n"
"	vec3 viewPosition;\n"
"} transform;\n"
"uniform struct PointLight{\n"
"	vec4 position;\n"
"	vec4 ambient;\n"
"	vec4 diffuse;\n"
"	vec4 specular;\n"
"	vec3 attenuation;\n"
"	vec3 direction;\n"
"	float angle;\n"
"} light;\n"
"uniform struct PointLight{\n"
"	vec4 position;\n"
"	vec4 ambient;\n"
"	vec4 diffuse;\n"
"	vec4 specular;\n"
"	vec3 attenuation;\n"
"	vec3 direction;\n"
"	float angle;\n"
"} light2;\n"
"out struct Vertex {\n"
"	vec2 texcoord;\n"
"	vec3 normal;\n"
"	vec3 lightDir;\n"
"	vec3 light2Dir;\n"
"	vec3 viewDir;\n"
"	float distance;\n"
"} Vert;\n"
"void main(void){\n"
"	vec4 vertex = transform.model * vec4(position, 1.0);\n"
"	vec4 lightDir = light.position - vertex;\n"
"	vec4 light2Dir = light2.position - vertex;\n"
"	gl_Position = transform.viewProjection * vertex;\n"
"	Vert.texcoord = texcoord;\n"
"	Vert.normal = transform.normal * normal;\n"
"	Vert.lightDir = vec3(lightDir);\n"
"	Vert.light2Dir = vec3(light2Dir);\n"
"	Vert.viewDir = transform.viewPosition - vec3(vertex);\n"
"	Vert.distance = length(lightDir);\n"
"}\n";

const char* fsSource_4 = "#version 330 core\n"
"#define FRAG_OUTPUT0 0\n"
"layout(location = FRAG_OUTPUT0) out vec4 color;\n"
"uniform struct PointLight{\n"
"	vec4 position;\n"
"	vec4 ambient;\n"
"	vec4 diffuse;\n"
"	vec4 specular;\n"
"	vec3 attenuation;\n"
"	vec3 direction;\n"
"	float angle;\n"
"} light;\n"
"uniform struct PointLight{\n"
"	vec4 position;\n"
"	vec4 ambient;\n"
"	vec4 diffuse;\n"
"	vec4 specular;\n"
"	vec3 attenuation;\n"
"	vec3 direction;\n"
"	float angle;\n"
"} light2;\n"
"uniform struct Material {\n"
"	sampler2D texture;\n"
"	vec4 ambient;\n"
"	vec4 diffuse;\n"
"	vec4 specular;\n"
"	vec4 emission;\n"
"	float shininess;\n"
"} material;\n"
"in struct Vertex {\n"
"	vec2 texcoord;\n"
"	vec3 normal;\n"
"	vec3 lightDir;\n"
"	vec3 light2Dir;\n"
"	vec3 viewDir;\n"
"	float distance;\n"
"} Vert;\n"
"void main(void){\n"
"	vec3 normal = normalize(Vert.normal);\n"
"	vec3 lightDir = normalize(Vert.lightDir);\n"
"	vec3 light2Dir = normalize(Vert.light2Dir);\n"
"	vec3 viewDir = normalize(Vert.viewDir);\n"
"	float attenuation = 1.0 / (light.attenuation[0] + light.attenuation[1] * Vert.distance + light.attenuation[2] * Vert.distance * Vert.distance);\n"
"	float attenuation2 = 1.0 / (light2.attenuation[0] + light2.attenuation[1] * Vert.distance + light2.attenuation[2] * Vert.distance * Vert.distance);\n"
"	color = material.emission;\n"
"	float inLight = max(1.0 - dot(lightDir, light.direction), 0) * 0.5;\n"
"	float inLight2 = max(1.0 - dot(light2Dir, light2.direction), 0) * 0.5;\n"
"	if (inLight < (1.0 - light.angle)){\n;"
"		inLight = 0;}\n"
"	else {\n"
"	inLight = 1;}\n"
"	if (inLight2 < (1.0 - light2.angle)){\n;"
"		inLight2 = 0;}\n"
"	else {\n"
"	inLight2 = 1;}\n"
"	color += material.ambient * light.ambient * attenuation * inLight;\n"
"	color += material.ambient * light2.ambient * attenuation * inLight2;\n"
"	float Ndot = max(dot(normal, lightDir), 0.0);\n"
"	float Ndot2 = max(dot(normal, light2Dir), 0.0);\n"
"	color += material.diffuse * light.diffuse * Ndot * attenuation * inLight;\n"
"	color += material.diffuse * light2.diffuse * Ndot2 * attenuation * inLight2;\n"
"	vec3 H = normalize(lightDir + viewDir);\n"
"	vec3 H2 = normalize(light2Dir + viewDir);\n"
"	float RdotVpow = max(pow(dot(normal, H), material.shininess), 0.0);\n"
"	float RdotVpow2 = max(pow(dot(normal, H2), material.shininess), 0.0);\n"
"	color += material.specular * light.specular * RdotVpow * attenuation * inLight;\n"
"	color += material.specular * light2.specular * RdotVpow2 * attenuation * inLight2;\n"
"	color *= texture(material.texture, Vert.texcoord);\n"
"}\n";

const char* vsSource_3 = "#version 330 core\n"
"#define VERT_POSITION 0\n"
"#define VERT_TEXCOORD 1\n"
"#define VERT_NORMAL 2\n"
"layout(location = VERT_POSITION) in vec3 position;\n"
"layout(location = VERT_TEXCOORD) in vec2 texcoord;\n"
"layout(location = VERT_NORMAL) in vec3 normal;\n"
"uniform struct Transform {\n"
"	mat4 model;\n"
"	mat4 viewProjection;\n"
"	mat3 normal;\n"
"	vec3 viewPosition;\n"
"} transform;\n"
"uniform struct PointLight{\n"
"	vec4 position;\n"
"	vec4 ambient;\n"
"	vec4 diffuse;\n"
"	vec4 specular;\n"
"	vec3 attenuation;\n"
"	vec3 direction;\n"
"	float angle;\n"
"} light;\n"
"uniform struct PointLight{\n"
"	vec4 position;\n"
"	vec4 ambient;\n"
"	vec4 diffuse;\n"
"	vec4 specular;\n"
"	vec3 attenuation;\n"
"	vec3 direction;\n"
"	float angle;\n"
"} light2;\n"
"out float lightness;\n"
"out float lightness2;\n"
"out vec2 UV;\n"
"out float inLight;\n"
"out float inLight2;\n"
"void main(void){\n"
"	vec4 vertex = transform.model * vec4(position, 1.0);\n"
"	vec4 lightDir = normalize(light.position - vertex);\n"
"	vec4 light2Dir = normalize(light2.position - vertex);\n"
"	inLight = (1.0 - dot(vec3(lightDir.r, lightDir.g, lightDir.b), light.direction)) * 0.5;\n"
"	if (inLight < (1.0 - light.angle)){\n;"
"		inLight = 0;}\n"
"	else {\n"
"	inLight = 1;}\n"
"	inLight2 = (1.0 - dot(vec3(light2Dir.r, light2Dir.g, light2Dir.b), light2.direction)) * 0.5;\n"
"	if (inLight2 < (1.0 - light2.angle)){\n;"
"		inLight2 = 0;}\n"
"	else {\n"
"	inLight2 = 1;}\n"
"	float t = dot(vec3(lightDir.r, lightDir.g, lightDir.b), normalize(transform.normal * normal));\n"
"	float t2 = dot(vec3(light2Dir.r, light2Dir.g, light2Dir.b), normalize(transform.normal * normal));\n"
"	lightness = t;\n"
"	lightness2 = t;\n"
"	UV = texcoord;\n"
"	gl_Position = transform.viewProjection * vertex;\n"
"}\n";



std::vector<GLfloat> triangulate(std::vector<Point_3D> verts, int size1, std::vector<int> inds, int size2, int& out_size)
{
	int n1 = size1;
	int n2 = size2;
	int ind = 0;
	out_size = n2 * 3;
	std::vector<GLfloat> ans;
	for (size_t i = 0; i < n2 / 3; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			ans.push_back(verts[(int)inds[i * 3 + j]].x);
			ind++;
			ans.push_back(verts[(int)inds[i * 3 + j]].y);
			ind++;
			ans.push_back(verts[(int)inds[i * 3 + j]].z);
			ind++;
		}
	}
	return ans;
}

void Init_Athens()
{
	athens.push_back(Athens(0, 0, 0, 1, 1, 1, -2.8, -2.5, -0.1));
	athens.push_back(Athens(-2, -0.1, 0, 0.8, 1, 0.8, 0, 0, 0));
	athens.push_back(Athens(0, 0.2, 0, 0.09, 0.09, 0.09, 0, 1.8, 0));
	athens.push_back(Athens(1, 1.5, 0, 0.2, 0.2, 0.2, -3, -3.5, 3.5));
	athens.push_back(Athens(2, 0.6, 0, 0.09, 0.09, 0.09, 0, -1.5, 0));
}

void Init_Materials()
{
	materials = std::vector<Material>();

	Material material = Material();

	material.ambient = glm::vec4(0.2, 0.2, 0.2, 1);
	material.diffuse = glm::vec4(2, 2, 2, 1);
	material.emission = glm::vec4(0, 0, 0, 1);
	material.specular = glm::vec4(1, 1, 1, 1);
	material.shininess = 4;
	materials.push_back(material);

	material.ambient = glm::vec4(0.1, 0.1, 0.1, 1);
	material.diffuse = glm::vec4(2, 2, 2, 1);
	material.emission = glm::vec4(0, 0, 0, 1);
	material.specular = glm::vec4(1, 1, 1, 1);
	material.shininess = 2;
	materials.push_back(material);

	material.ambient = glm::vec4(0.1, 0.1, 0.1, 1);
	material.diffuse = glm::vec4(2, 2, 2, 1);
	material.emission = glm::vec4(0, 0, 0, 1);
	material.specular = glm::vec4(1, 1, 1, 1);
	material.shininess = 1;
	materials.push_back(material);

	material.ambient = glm::vec4(0.1, 0.1, 0.1, 1);
	material.diffuse = glm::vec4(2, 2, 2, 1);
	material.emission = glm::vec4(0, 0, 0, 1);
	material.specular = glm::vec4(1, 1, 1, 1);
	material.shininess = 2;

	materials.push_back(material);

	material.ambient = glm::vec4(.1, .1, .1, 1);
	material.diffuse = glm::vec4(2, 2, 2, 1);
	material.emission = glm::vec4(0, 0, 0, 1);
	material.specular = glm::vec4(1, 1, 1, 1);
	material.shininess = 0.1;
	materials.push_back(material);
}

void checkOpenGLerror()
{
	GLenum errCode;

	if ((errCode = glGetError()) != GL_NO_ERROR)
	{
		std::cout << "OpenGl error! - " << gluErrorString(errCode);
	}
}

void Load_Textures()
{
	for (size_t i = 0; i < meshes.size(); i++)
	{
		textures.push_back(SOIL_load_OGL_texture(tex_Paths[i].c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT));
	}
	texture_2 = SOIL_load_OGL_texture(tex_Path_2.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
}

Mesh openOBJ(std::string filename)
{
	Point_3D point(1, 2, 3);
	char buff[1000];
	std::ifstream fin(filename);

	std::regex myregex;
	int mode = 0;

	Mesh mesh;

	std::vector<Point_2D> uv_vec;
	std::vector<Point_3D> norm_vec;

	while (!fin.eof())
	{
		fin.getline(buff, 1000);
		std::string s = buff;
		if (s[0] == 'v')
		{
			if (s[1] == ' ')
			{
				myregex = std::regex("v (\-?\\d+,\\d+) (\-?\\d+,\\d+) (\-?\\d+,\\d+)");
				mode = 0;
			}
			else if (s[1] == 'n')
			{
				myregex = std::regex("vn (\-?\\d+,\\d+) (\-?\\d+,\\d+) (\-?\\d+,\\d+)");
				mode = 1;
			}
			else if (s[1] == 't')
			{
				myregex = std::regex("vt (\-?\\d+,\\d+) (\-?\\d+,\\d+)");
				mode = 2;
			}
		}
		else if (s[0] == 'f')
		{
			myregex = std::regex("f (\\d+/\\d+/\\d+) (\\d+/\\d+/\\d+) (\\d+/\\d+/\\d+)");
			mode = 3;
		}
		else
			continue;
		auto words_begin = std::sregex_iterator(s.begin(), s.end(), myregex);
		auto words_end = std::sregex_iterator();
		for (std::sregex_iterator i = words_begin; i != words_end; i++)
		{
			std::smatch match = *i;
			if (mode == 0)
			{
				Point_3D p(stod(match[1]), stod(match[2]), stod(match[3]));
				mesh.pointList.push_back(p);
			}
			else if (mode == 1)
			{
				Point_3D p(stod(match[1]), stod(match[2]), stod(match[3]));
				norm_vec.push_back(p);
			}
			else if (mode == 2)
			{
				Point_2D p(stod(match[1]), stod(match[2]));
				uv_vec.push_back(p);
			}
			else if (mode == 3)
			{
				std::vector<std::tuple<int, int, int>> polygon = std::vector<std::tuple<int, int, int>>();
				for (int j = 1; j < match.size(); j++)
				{
					std::regex point = std::regex("(\\d+)/(\\d+)/(\\d+)");
					std::string s0 = match[j];
					auto matchpoint = std::sregex_iterator(s0.begin(), s0.end(), point);
					polygon.push_back(std::make_tuple(stoi((*matchpoint)[1]) - 1, stoi((*matchpoint)[2]) - 1, stoi((*matchpoint)[3]) - 1));
				}
				mesh.polygons.push_back(polygon);
			}
		}

	}

	mesh.indicesList = std::vector<int>(mesh.polygons.size() * 3);
	mesh.normalList = std::vector<Point_3D>(mesh.polygons.size() * 3);
	mesh.texturePoint = std::vector<Point_2D>(mesh.polygons.size() * 3);
	int pointer = 0;
	for (int i = 0; i < mesh.polygons.size(); i++)
	{
		for (int j = 0; j < mesh.polygons[i].size(); j++)
		{
			try
			{
				int ind1 = std::get<0>(mesh.polygons[i][j]);
				int ind2 = std::get<1>(mesh.polygons[i][j]);
				int ind3 = std::get<2>(mesh.polygons[i][j]);
				mesh.indicesList[pointer] = ind1;
				mesh.texturePoint[pointer] = uv_vec[ind2];
				mesh.normalList[pointer] = norm_vec[ind3];
				++pointer;
			}
			catch (std::exception)
			{
				;
			}
		}
	}

	fin.close();

	return mesh;
}

void Init_Shader()
{
	shaders.clear();
	for (size_t i = 0; i < meshes.size(); i++)
	{
		GLShader t = GLShader();
		shaders.push_back(t);
		if (true)
		{
			if (mix_color)
			{
				shaders[i].load(vsSource, fsSource_2);
			}
			else
			{
				if (mix)
				{
					shaders[i].load(vsSource, fsSource_3);
				}
				else
				{
					shaders[i].load(vsSource_2, fsSource_4);
				}
			}
		}

		checkOpenGLerror();
	}
}

void initVBO()
{
	VBO_verteces = std::vector<GLuint>();
	VBO_textures = std::vector<GLuint>();
	VBO_normals = std::vector<GLuint>();
	VBO_colors = std::vector<GLuint>();
	for (size_t i = 0; i < meshes.size(); i++)
	{
		int vert_size = 0;
		int col_size = 0;
		std::vector<GLfloat> v1 = triangulate(meshes[i].pointList, meshes[i].pointList.size(), meshes[i].indicesList, meshes[i].indicesList.size(), vert_size);

		GLfloat* vertices = new GLfloat[v1.size()];
		for (size_t j = 0; j < v1.size(); j += 3)
		{
			glm::vec4 t = glm::vec4(v1[j], v1[j + 1], v1[j + 2], 1) * athens[i].matrix;
			vertices[j] = t.r;
			vertices[j + 1] = t.g;
			vertices[j + 2] = t.b;
		}

		GLfloat* uvs = new GLfloat[meshes[i].texturePoint.size() * 2];
		int ind = 0;
		for (size_t j = 0; j < meshes[i].texturePoint.size(); j++)
		{
			uvs[ind] = meshes[i].texturePoint[j].x;
			ind++;
			uvs[ind] = meshes[i].texturePoint[j].y;
			ind++;
		}
		int uv_size = ind;

		GLfloat* normals = new GLfloat[meshes[i].normalList.size() * 3];
		ind = 0;
		for (size_t j = 0; j < meshes[i].normalList.size(); j++)
		{
			glm::vec4 t = glm::vec4(meshes[i].normalList[j].x, meshes[i].normalList[j].y, meshes[i].normalList[j].z, 1) * athens[i].rotate;
			normals[ind] = t.r;
			ind++;
			normals[ind] = t.g;
			ind++;
			normals[ind] = t.b;
			ind++;
		}
		int norm_size = ind;

		std::vector<Point_3D> old_colors;
		for (size_t j = 0; j < meshes[i].pointList.size(); j++)
		{
			double r = (rand() % 100) * 0.01;
			double g = (rand() % 100) * 0.01;
			double b = (rand() % 100) * 0.01;
			old_colors.push_back(Point_3D(r, g, b));
		}

		int old_col_size = old_colors.size() * 3;

		std::vector<GLfloat> c1 = triangulate(old_colors, old_col_size, meshes[i].indicesList, meshes[i].indicesList.size(), col_size);
		GLfloat* colors = new GLfloat[c1.size()];
		for (size_t j = 0; j < c1.size(); j++)
		{
			colors[j] = c1[j];
		}
		col_size = c1.size();

		VBO_verteces.push_back(0);
		VBO_textures.push_back(0);
		VBO_normals.push_back(0);
		VBO_colors.push_back(0);

		glGenBuffers(1, &VBO_verteces[i]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_verteces[i]);
		glBufferData(GL_ARRAY_BUFFER, vert_size * 4, vertices, GL_STATIC_DRAW);

		glGenBuffers(1, &VBO_textures[i]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_textures[i]);
		glBufferData(GL_ARRAY_BUFFER, uv_size * 4, uvs, GL_STATIC_DRAW);

		glGenBuffers(1, &VBO_normals[i]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_normals[i]);
		glBufferData(GL_ARRAY_BUFFER, norm_size * 4, normals, GL_STATIC_DRAW);

		glGenBuffers(1, &VBO_colors[i]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_colors[i]);
		glBufferData(GL_ARRAY_BUFFER, col_size * 4, colors, GL_STATIC_DRAW);

		checkOpenGLerror();
	}
}

void specialKeys(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_RIGHT: rotate_yy += 0.1; break;
	case GLUT_KEY_LEFT: rotate_yy -= 0.1; break;
	case GLUT_KEY_F1: mix = !mix; break;
	case GLUT_KEY_F2: mix_color = !mix_color; Init_Shader(); break;

	case GLUT_KEY_F8:
	{
		if (mix_value > 0)
		{
			mix_value -= 0.1f;
		}
		Init_Shader();
		break;
	}
	case GLUT_KEY_F9:
	{
		if (mix_value < 1)
		{
			mix_value += 0.1f;
		}
		Init_Shader();
		break;
	}
	}

	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'w':
	{
		move += 0.1;
		break;
	}
	case 's':
	{
		move -= 0.1;
		break;
	}
	default:
	{
		break;
	}
	}

	athens[0] = Athens(0, 0, move, 0.01, 0.01, 0.01, -2, 2, 0);

	initVBO();
	glutPostRedisplay();
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec3 viewPosition = glm::vec3(4, 3, 3);

	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 View = glm::lookAt(viewPosition, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 Model = glm::mat4(1.0f);

	glm::mat4 rotate_x =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, glm::cos(rotate_xx), -glm::sin(rotate_xx), 0.0f,
		0.0f, glm::sin(rotate_xx), glm::cos(rotate_xx), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	glm::mat4 rotate_y =
	{
		glm::cos(rotate_yy), 0.0f, glm::sin(rotate_yy), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		-glm::sin(rotate_yy), 0.0f, glm::cos(rotate_yy), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	glm::mat4 rotate_z =
	{
		glm::cos(rotate_zz),  -glm::sin(rotate_zz), 0.0f, 0.0f,
		glm::sin(rotate_zz), glm::cos(rotate_zz), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	glm::mat4 MVP = Projection * View * Model * rotate_x * rotate_y * rotate_z;

	Transform transform;
	transform.model = Model * rotate_x * rotate_y * rotate_z;
	transform.view_projection = Projection * View;
	transform.normal = glm::transpose(glm::inverse(transform.model));
	transform.view_position = viewPosition;

	Light light;

	light.position = glm::vec4(0, 1.5, 0, 1);
	light.ambient = glm::vec4(1, 1, 1, 1);
	light.diffuse = glm::vec4(1, 1, 1, 1);
	light.specular = glm::vec4(1, 1, 1, 1);
	light.attenuation = glm::vec3(0.5, 0.5, 0.5);
	light.direction = glm::vec3(0, 0, 1);
	light.angle = 1;

	Light light2;

	glm::vec4 t = glm::vec4(0, 1, move, 1) * Athens(0, 0, 0.2, 1, 1, 1, 0, 0, 0).matrix;
	light2.position = glm::vec4(t.r, t.g, t.b, 1);
	light2.ambient = glm::vec4(1, 1, 1, 1);
	light2.diffuse = glm::vec4(1, 1, 1, 1);
	light2.specular = glm::vec4(1, 1, 1, 1);
	light2.attenuation = glm::vec3(0.5, 0.5, 0.5);
	light2.direction = glm::vec3(0, 0, 1);



	for (size_t i = 0; i < meshes.size(); i++)
	{
		glUseProgram(shaders[i].ShaderProgram);

		GLuint TransformModelID = glGetUniformLocation(shaders[i].ShaderProgram, "transform.model");
		GLuint TransformProjectionID = glGetUniformLocation(shaders[i].ShaderProgram, "transform.viewProjection");
		GLuint TransformNormalID = glGetUniformLocation(shaders[i].ShaderProgram, "transform.normal");
		GLuint TransformPositionID = glGetUniformLocation(shaders[i].ShaderProgram, "transform.viewPosition");

		if (i > 0)
		{
			glUniformMatrix4fv(TransformModelID, 1, GL_FALSE, &transform.model[0][0]);
		}
		else
		{
			glUniformMatrix4fv(TransformModelID, 1, GL_FALSE, &Model[0][0]);
		}

		glUniformMatrix4fv(TransformProjectionID, 1, GL_FALSE, &transform.view_projection[0][0]);
		glUniformMatrix3fv(TransformNormalID, 1, GL_FALSE, &transform.normal[0][0]);
		glUniform3fv(TransformPositionID, 1, &transform.view_position[0]);

		GLuint LightPositionID = glGetUniformLocation(shaders[i].ShaderProgram, "light.position");
		GLuint LightAmbientID = glGetUniformLocation(shaders[i].ShaderProgram, "light.ambient");
		GLuint LightDiffuseID = glGetUniformLocation(shaders[i].ShaderProgram, "light.diffuse");
		GLuint LightSpecularID = glGetUniformLocation(shaders[i].ShaderProgram, "light.specular");
		GLuint LightAttenuationID = glGetUniformLocation(shaders[i].ShaderProgram, "light.attenuation");
		GLuint LightDirectionID = glGetUniformLocation(shaders[i].ShaderProgram, "light.direction");
		GLuint LightAngleID = glGetUniformLocation(shaders[i].ShaderProgram, "light.angle");
		glUniform4fv(LightPositionID, 1, &light.position[0]);
		glUniform4fv(LightAmbientID, 1, &light.ambient[0]);
		glUniform4fv(LightDiffuseID, 1, &light.diffuse[0]);
		glUniform4fv(LightSpecularID, 1, &light.specular[0]);
		glUniform3fv(LightAttenuationID, 1, &light.attenuation[0]);
		glUniform3fv(LightDirectionID, 1, &light.direction[0]);
		glUniform1f(LightAngleID, light.angle);

		GLuint LightPositionID2 = glGetUniformLocation(shaders[i].ShaderProgram, "light2.position");
		GLuint LightAmbientID2 = glGetUniformLocation(shaders[i].ShaderProgram, "light2.ambient");
		GLuint LightDiffuseID2 = glGetUniformLocation(shaders[i].ShaderProgram, "light2.diffuse");
		GLuint LightSpecularID2 = glGetUniformLocation(shaders[i].ShaderProgram, "light2.specular");
		GLuint LightAttenuationID2 = glGetUniformLocation(shaders[i].ShaderProgram, "light2.attenuation");
		GLuint LightDirectionID2 = glGetUniformLocation(shaders[i].ShaderProgram, "light2.direction");
		GLuint LightAngleID2 = glGetUniformLocation(shaders[i].ShaderProgram, "light2.angle");
		glUniform4fv(LightPositionID2, 1, &light2.position[0]);
		glUniform4fv(LightAmbientID2, 1, &light2.ambient[0]);
		glUniform4fv(LightDiffuseID2, 1, &light2.diffuse[0]);
		glUniform4fv(LightSpecularID2, 1, &light2.specular[0]);
		glUniform3fv(LightAttenuationID2, 1, &light2.attenuation[0]);
		glUniform3fv(LightDirectionID2, 1, &light2.direction[0]);
		glUniform1f(LightAngleID2, light2.angle);

		GLuint MaterialAmbientID = glGetUniformLocation(shaders[i].ShaderProgram, "material.ambient");
		GLuint MaterialDiffuseID = glGetUniformLocation(shaders[i].ShaderProgram, "material.diffuse");
		GLuint MaterialEmissionID = glGetUniformLocation(shaders[i].ShaderProgram, "material.emission");
		GLuint MaterialSpecularID = glGetUniformLocation(shaders[i].ShaderProgram, "material.specular");
		GLuint MaterialShininessID = glGetUniformLocation(shaders[i].ShaderProgram, "material.shininess");
		glUniform4fv(MaterialAmbientID, 1, &materials[i].ambient[0]);
		glUniform4fv(MaterialDiffuseID, 1, &materials[i].diffuse[0]);
		glUniform4fv(MaterialEmissionID, 1, &materials[i].emission[0]);
		glUniform4fv(MaterialSpecularID, 1, &materials[i].specular[0]);
		glUniform1f(MaterialShininessID, materials[i].shininess);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glUniform1i(glGetUniformLocation(shaders[i].ShaderProgram, "material.texture"), 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glUniform1i(glGetUniformLocation(shaders[i].ShaderProgram, "myTextureSampler"), 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, texture_2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glUniform1i(glGetUniformLocation(shaders[i].ShaderProgram, "myTextureSampler1"), 3);

		glUniform1f(glGetUniformLocation(shaders[i].ShaderProgram, "mix_f"), mix_value);



		GLuint MatrixID = glGetUniformLocation(shaders[i].ShaderProgram, "MVP");
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_verteces[i]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_textures[i]);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);


		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_normals[i]);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_colors[i]);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, 10000 * 3);
	}

	GLuint MaterialEmissionID = glGetUniformLocation(shaders[0].ShaderProgram, "material.emission");
	glUniform4fv(MaterialEmissionID, 1, &materials[materials.size() - 1].emission[0]);

	checkOpenGLerror();

	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	setlocale(LC_ALL, "rus");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE);
	glutInitWindowSize(1000, 800);
	glutCreateWindow("Indiv");
	glClearColor(0, 0.5, 1, 0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	GLenum glew_status = glewInit();

	tex_Paths.push_back("l1-2.jpg");
	tex_Paths.push_back("dif_2.jpg");
	tex_Paths.push_back("pink.jpg");
	tex_Paths.push_back("4.jpg");
	tex_Paths.push_back("met.jpg");
	tex_Path_2 = "igls.jpg";

	Mesh mesh_1 = openOBJ("plane_4.obj");
	meshes.push_back(mesh_1);

	Mesh mesh_2 = openOBJ("Lamp.obj");
	meshes.push_back(mesh_2);

	Mesh mesh_3 = openOBJ("a_1.obj");
	meshes.push_back(mesh_3);

	Mesh mesh_4 = openOBJ("heart.obj");
	meshes.push_back(mesh_4);

	Mesh mesh_5 = openOBJ("a_1.obj");
	meshes.push_back(mesh_5);

	Init_Materials();
	Init_Athens();
	Init_Shader();
	Load_Textures();
	initVBO();

	glutDisplayFunc(render);
	glutSpecialFunc(specialKeys);
	glutKeyboardFunc(keyboard);

	glutMainLoop();
}