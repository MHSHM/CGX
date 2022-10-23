#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <glew.h>
#include <vector>
#include <Game.h>

Shader::Shader() :
	shader_program_id(0)
{

}


std::string Shader::Parse_Source(const std::string& path)
{
	std::stringstream source_code;
	std::ifstream shader_file(path);
	std::string line;

	if (!shader_file.is_open())
	{
		LOG_ERR("failed to load shader with path " + path + '\n');
		return "";
	}

	while (std::getline(shader_file, line))
	{
		if (line == "") 
		{
			continue; 
		}

		source_code << line << '\n';
	}

	shader_file.close();

	return source_code.str();
}

unsigned int Shader::Compile(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);

	const char* src = source.c_str();

	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int success = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE)
	{
		int maxLength = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<char> errorLog(maxLength);
		glGetShaderInfoLog(id, maxLength, &maxLength, &errorLog[0]);
		for (auto& c : errorLog) std::cout << c;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

void Shader::Re_Compile_Vertex_Shader()
{
	// Get Previous Source Code
	int len;
	char prev_source[512];
	glGetShaderiv(vertex_shader, GL_SHADER_SOURCE_LENGTH, &len);
	glGetShaderSource(vertex_shader, len, &len, prev_source);

	// Get Current Source Code
	const std::string current_source = Parse_Source(vertex_shader_path);

	// Compare the two source codes
	int res = strcmp(prev_source, current_source.c_str()); 

	if (res != 0) 
	{
		std::cout << "Compilling Vertex Shader with tag " + tag + "...\n";

		const char* src = current_source.c_str();

		glShaderSource(vertex_shader, 1, &src, nullptr);
		glCompileShader(vertex_shader);

		int success = 0;
		glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

		if (success == GL_FALSE)
		{
			int maxLength = 0;
			glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<char> errorLog(maxLength);
			glGetShaderInfoLog(vertex_shader, maxLength, &maxLength, &errorLog[0]);
			for (auto& c : errorLog) std::cout << c;
			return; 
		}

		glLinkProgram(shader_program_id);
	}
}

void Shader::Re_Compile_Fragment_Shader()
{
	// Get Previous Source Code
	int len;
	char prev_source[4096];
	glGetShaderiv(fragment_shader, GL_SHADER_SOURCE_LENGTH, &len);
	glGetShaderSource(fragment_shader, len, &len, prev_source);

	// Get Current Source Code
	const std::string current_source = Parse_Source(fragment_shader_path);

	// Compare the two source codes
	int res = strcmp(prev_source, current_source.c_str());

	if (res != 0)
	{
		std::cout << "Compilling Fragment Shader with tag " + tag + "...\n";

		const char* src = current_source.c_str();

		glShaderSource(fragment_shader, 1, &src, nullptr);
		glCompileShader(fragment_shader);

		int success = 0;
		glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

		if (success == GL_FALSE)
		{
			int maxLength = 0;
			glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<char> errorLog(maxLength);
			glGetShaderInfoLog(fragment_shader, maxLength, &maxLength, &errorLog[0]);
			for (auto& c : errorLog) std::cout << c;
			return;
		}
		
		std::cout << "Compiled Successfully\n";

		glLinkProgram(shader_program_id);
	}
}

void Shader::Re_Compile_Compute_Shader()
{
	// Get Previous Source Code
	int len;
	char prev_source[4096];
	glGetShaderiv(compute_shader, GL_SHADER_SOURCE_LENGTH, &len);
	glGetShaderSource(compute_shader, len, &len, prev_source);

	// Get Current Source Code
	const std::string current_source = Parse_Source(compute_shader_path);

	// Compare the two source codes
	int res = strcmp(prev_source, current_source.c_str());

	if (res != 0)
	{
		std::cout << "Compilling compute Shader with tag " + tag + "...\n";

		const char* src = current_source.c_str();

		glShaderSource(compute_shader, 1, &src, nullptr);
		glCompileShader(compute_shader);

		int success = 0;
		glGetShaderiv(compute_shader, GL_COMPILE_STATUS, &success);

		if (success == GL_FALSE)
		{
			int maxLength = 0;
			glGetShaderiv(compute_shader, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<char> errorLog(maxLength);
			glGetShaderInfoLog(compute_shader, maxLength, &maxLength, &errorLog[0]);
			for (auto& c : errorLog) std::cout << c;
			return;
		}

		std::cout << "Compiled Successfully\n";

		glLinkProgram(shader_program_id);
	}
}

void Shader::Re_Compile()
{
	Re_Compile_Vertex_Shader();
	Re_Compile_Fragment_Shader();
}

void Shader::Re_Compile_Compute()
{
	Re_Compile_Compute_Shader();
}

void Shader::Create_Shader_Program(const std::string& _compute_shader_path, const std::string& _tag)
{
	compute_shader_path = _compute_shader_path;

	tag = _tag; 
	std::string compute_source = Parse_Source(compute_shader_path);

	compute_shader = Compile(GL_COMPUTE_SHADER, compute_source);

	shader_program_id = glCreateProgram();
	glAttachShader(shader_program_id, compute_shader);

	glLinkProgram(shader_program_id);
}

void Shader::Create_Shader_Program(const std::string& _vertex_shader_path, const std::string& _fragment_shader_path, const std::string& _tag)
{
	tag = _tag; 
	vertex_shader_path = _vertex_shader_path; 
	fragment_shader_path = _fragment_shader_path;

	std::string vertex_source = Parse_Source(vertex_shader_path);
	std::string fragment_source = Parse_Source(fragment_shader_path);

	vertex_shader = Compile(GL_VERTEX_SHADER, vertex_source);
	fragment_shader = Compile(GL_FRAGMENT_SHADER, fragment_source);

	shader_program_id = glCreateProgram();
	glAttachShader(shader_program_id, vertex_shader);
	glAttachShader(shader_program_id, fragment_shader);

	glLinkProgram(shader_program_id);
}

void Shader::Create_Shader_Program(const std::string& _vertex_shader_path, const std::string& _fragment_shader_path, const std::string& _geometry_shader_path, const std::string& _tag)
{
	tag = _tag;
	
	vertex_shader_path = _vertex_shader_path;
	fragment_shader_path = _fragment_shader_path; 
	geometry_shader_path = _geometry_shader_path;

	std::string vertex_source   = Parse_Source(vertex_shader_path);
	std::string fragment_source = Parse_Source(fragment_shader_path);
	std::string geometry_source = Parse_Source(geometry_shader_path); 

	vertex_shader = Compile(GL_VERTEX_SHADER, vertex_source);
	fragment_shader = Compile(GL_FRAGMENT_SHADER, fragment_source);
	geometry_shader = Compile(GL_GEOMETRY_SHADER, geometry_source); 

	shader_program_id = glCreateProgram();
	glAttachShader(shader_program_id, vertex_shader);
	glAttachShader(shader_program_id, fragment_shader);
	glAttachShader(shader_program_id, geometry_shader);

	glLinkProgram(shader_program_id);
}

void Shader::Create_Shader_Program(const std::string& vertex_shader_path, const std::string& fragment_shader_path,
	 const std::string& tes_control_shader_path, const std::string& tes_evaluation_shader_path, const std::string& geometry_shader_path, const std::string& _tag)
{
	tag = _tag;

	std::string vertex_source = Parse_Source(vertex_shader_path);
	std::string tes_control_source = Parse_Source(tes_control_shader_path);
	std::string tes_evaluation_source = Parse_Source(tes_evaluation_shader_path);
	std::string geometry_source = Parse_Source(geometry_shader_path);
	std::string fragment_source = Parse_Source(fragment_shader_path);

	unsigned int vertex_shader = Compile(GL_VERTEX_SHADER, vertex_source);
	unsigned int tesslation_control_shader = Compile(GL_TESS_CONTROL_SHADER, tes_control_source);
	unsigned int tesslation_evaluation_shader = Compile(GL_TESS_EVALUATION_SHADER, tes_evaluation_source);
	unsigned int geometry_shader = Compile(GL_GEOMETRY_SHADER, geometry_source);
	unsigned int fragment_shader = Compile(GL_FRAGMENT_SHADER, fragment_source);

	shader_program_id = glCreateProgram();
	glAttachShader(shader_program_id, vertex_shader);
	glAttachShader(shader_program_id, tesslation_control_shader);
	glAttachShader(shader_program_id, tesslation_evaluation_shader);
	glAttachShader(shader_program_id, geometry_shader);
	glAttachShader(shader_program_id, fragment_shader);

	glLinkProgram(shader_program_id);
}


void Shader::Set_Matrix4_Uniform(const std::string& uniform, const glm::mat4& matrix)
{
	GLuint location = glGetUniformLocation(shader_program_id, uniform.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::Set_Vec2_Uniform(const std::string& uniform, const glm::vec2& vector)
{
	GLuint location = glGetUniformLocation(shader_program_id, uniform.c_str());
	glUniform2f(location, vector.x, vector.y);
}

void Shader::Set_Vec3_Uniform(const std::string& uniform, const glm::vec3& vector)
{
	GLuint location = glGetUniformLocation(shader_program_id, uniform.c_str());
	glUniform3f(location, vector.x, vector.y, vector.z);
}

void Shader::Set_Vec4_Uniform(const std::string& uniform, const glm::vec4& vector)
{
	GLuint location = glGetUniformLocation(shader_program_id, uniform.c_str());
	glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
}

void Shader::Set_Float_Uniform(const std::string& uniform, const float& val)
{
	GLuint location = glGetUniformLocation(shader_program_id, uniform.c_str());
	glUniform1f(location, val);
}

void Shader::Set_Int_Uniform(const std::string& uniform, const int& val)
{
	GLuint location = glGetUniformLocation(shader_program_id, uniform.c_str());
	glUniform1i(location, val);
}

void Shader::Bind() const 
{
	glUseProgram(shader_program_id);
}

void Shader::Un_Bind() const
{
	glUseProgram(0);
}

void Shader::Clear()
{
	glDeleteShader(shader_program_id);
}