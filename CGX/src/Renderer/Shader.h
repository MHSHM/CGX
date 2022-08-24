#pragma once

#include <string>

#include <mat4x4.hpp>
#include <gtc/type_ptr.hpp>


class Shader
{
public:
	Shader();

	void Create_Shader_Program(const std::string& compute_shader_path, const std::string& _tag);
	void Create_Shader_Program(const std::string& _vertex_shader_path, const std::string& _fragment_shader_path, const std::string& _tag);
	void Create_Shader_Program(const std::string& _vertex_shader_path, const std::string& _fragment_shader_path, const std::string& _geometry_shader_path, const std::string& _tag);
	void Create_Shader_Program(const std::string& vertex_shader_path, const std::string& fragment_shader_path,
		const std::string& tes_control_shader_path, const std::string& tes_evaluation_shader_path, const std::string& geometry_shader_path, const std::string& _tag);

	unsigned int& Get_Shader_program() { return shader_program_id; }

	void Set_Matrix4_Uniform(const std::string& uniform, const glm::mat4& matrix);
	void Set_Vec2_Uniform(const std::string& uniform, const glm::vec2& vector);
	void Set_Vec3_Uniform(const std::string& uniform, const glm::vec3& vector);
	void Set_Vec4_Uniform(const std::string& uniform, const glm::vec4& vector);
	void Set_Float_Uniform(const std::string& uniform, const float& val);
	void Set_Int_Uniform(const std::string& uniform, const int& val);

	void Re_Compile();

	void Bind() const;
	void Un_Bind() const;

	void Clear();

private:
	std::string Parse_Source(const std::string& path);
	unsigned int Compile(unsigned int type, const std::string& source);
	void Re_Compile_Vertex_Shader();
	void Re_Compile_Fragment_Shader();

private:
	unsigned int shader_program_id;
	unsigned int vertex_shader;
	unsigned int fragment_shader;
	unsigned int geometry_shader;

	std::string vertex_shader_path;
	std::string fragment_shader_path;
	std::string geometry_shader_path;

	std::string tag = "";
};