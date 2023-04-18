#ifndef UNIFORMMEMORY_HPP
#define UNIFORMMEMORY_HPP

#include <glad/glad.h>
#include <glm.hpp>

#define NUM_LIGHTS 1
#define MAX_LIGHTS 200 // breaks after going beyond 32 because ubo info is passed in incorrectly, pls fix later
// ^^ note: breaks when the shader ubo info is not correct, or goes beyond 16KB (16k bytes)

namespace DEL10
{
	template <typename T>
	class UniformBuffer
	{
	public:
		UniformBuffer(unsigned idx)
		{
			glGenBuffers(1, &id);

			glBindBuffer(GL_UNIFORM_BUFFER, id);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(T), nullptr, GL_STATIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			glBindBufferBase(GL_UNIFORM_BUFFER, idx, id);
		}

		void SetData()
		{
			glBindBuffer(GL_UNIFORM_BUFFER, id);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T), &data);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		void UpdateData(GLintptr offset)
		{
			glBindBuffer(GL_UNIFORM_BUFFER, id);
			glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(T) - offset, static_cast<char*>(&data) + offset);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		T& GetData()
		{
			return data;
		}

	private:
		GLuint id;
		T data;
	};

	class Light
	{
		glm::vec4 position;
		glm::vec4 colorRadius; // xyz = color, w = radius
	};

	class VisibilityIdx
	{
		glm::ivec4 idx;
	};
}

#endif