#include <VertexArrayObject.h>
#include <glew.h>
#include <Model.h>

void VertexArrayObject::Send_Data_To_GPU(const RenderData* render_data)
{
	glGenVertexArrays(1, &vertex_array_id);
	glBindVertexArray(vertex_array_id); 

	glGenBuffers(1, &indices_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, render_data->indices.size() * sizeof(unsigned int), render_data->indices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &positions_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, positions_buffer); 
	glBufferData(GL_ARRAY_BUFFER, render_data->positions.size() * sizeof(glm::vec3), render_data->positions.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	glGenBuffers(1, &normals_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, normals_buffer); 
	glBufferData(GL_ARRAY_BUFFER, render_data->normals.size() * sizeof(glm::vec3), render_data->normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1); 
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	glGenBuffers(1, &uvs_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvs_buffer);
	glBufferData(GL_ARRAY_BUFFER, render_data->uvs.size() * sizeof(glm::vec2), render_data->uvs.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);

	glGenBuffers(1, &tangents_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, tangents_buffer);
	glBufferData(GL_ARRAY_BUFFER, render_data->tangents.size() * sizeof(glm::vec3), render_data->tangents.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	glGenBuffers(1, &bitangents_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, bitangents_buffer);
	glBufferData(GL_ARRAY_BUFFER, render_data->bitangents.size() * sizeof(glm::vec3), render_data->bitangents.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
}

void VertexArrayObject::Bind()
{
	glBindVertexArray(vertex_array_id);
}

void VertexArrayObject::Un_Bind()
{
	glBindVertexArray(0);
}

void VertexArrayObject::Clear()
{
	glDeleteBuffers(1, &positions_buffer); 
	glDeleteBuffers(1, &normals_buffer); 
	glDeleteBuffers(1, &uvs_buffer);
	glDeleteBuffers(1, &indices_buffer);
	glDeleteVertexArrays(1, &vertex_array_id); 
}
