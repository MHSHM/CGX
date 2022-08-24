#pragma once

class VertexArrayObject 
{
public:
	VertexArrayObject() = default; 

public:
	void Send_Data_To_GPU(const struct RenderData* render_data);
	void Bind(); 
	void Un_Bind(); 
	void Clear(); 

	unsigned int vertex_array_id;
	unsigned int positions_buffer; 
	unsigned int normals_buffer; 
	unsigned int uvs_buffer;
	unsigned int tangents_buffer; 
	unsigned int bitangents_buffer; 
	unsigned int indices_buffer; 
};