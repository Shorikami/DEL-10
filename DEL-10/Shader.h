#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    unsigned int m_ID;

    Shader(const char* computePath)
    {
        GenerateCompute(computePath);
    }

    Shader(const char* vertexPath, const char* fragmentPath)
    {
        Generate(vertexPath, fragmentPath);
    }

    void Shader::Generate(const char* vPath, const char* fPath)
    {
        int success;
        char infoLog[512];

        std::string m_VertSrc, m_FragSrc;

        GLuint vShader = Compile(vPath, GL_VERTEX_SHADER, m_VertSrc);
        GLuint fShader = Compile(fPath, GL_FRAGMENT_SHADER, m_FragSrc);

        m_ID = glCreateProgram();
        glAttachShader(m_ID, vShader);
        glAttachShader(m_ID, fShader);

        glLinkProgram(m_ID);

        glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(m_ID, 512, NULL, infoLog);
            std::cout << "Program linking error: " << std::endl << infoLog << std::endl;
        }

        glDeleteShader(vShader);
        glDeleteShader(fShader);
    }

    void Shader::GenerateCompute(const char* cmptPath)
    {
        int success;
        char infoLog[512];

        GLuint vShader = Compile(cmptPath, GL_COMPUTE_SHADER, std::string());

        m_ID = glCreateProgram();
        glAttachShader(m_ID, vShader);
        glLinkProgram(m_ID);

        glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(m_ID, 512, NULL, infoLog);
            std::cout << "Program linking error: " << std::endl << infoLog << std::endl;
        }

        glDeleteShader(vShader);
    }

    GLuint Shader::Compile(const char* path, GLenum type, std::string& source)
    {
        int success;
        char infoLog[512];

        GLuint res = glCreateShader(type);
        std::string src = LoadShaderSrc(path);
        source = src;
        const GLchar* shader = src.c_str();

        glShaderSource(res, 1, &shader, NULL);
        glCompileShader(res);

        glGetShaderiv(res, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(res, 512, NULL, infoLog);
            std::cout << "Error with shader from " << path << ": " << std::endl << infoLog << std::endl;
        }

        return res;
    }

    std::string Shader::LoadShaderSrc(const char* name)
    {
        std::ifstream file;
        std::stringstream buf;
        std::string res = "";

        file.open(name);

        if (file.is_open())
        {
            buf << file.rdbuf();
            res = buf.str();
        }
        else
        {
            std::cout << "Could not open " << name << std::endl;
        }

        file.close();

        return res;
    }

    void Activate() const
    {
        glUseProgram(m_ID);
    }

    void SetBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(m_ID, name.c_str()), (int)value);
    }

    void SetInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
    }

    void SetFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
    }

    void SetVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);
    }

    void SetVec2I(const std::string& name, const glm::ivec2& value) const
    {
        glUniform2iv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);
    }

    void SetVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(m_ID, name.c_str()), x, y);
    }

    void SetVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);
    }
    void SetVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(m_ID, name.c_str()), x, y, z);
    }

    void SetVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);
    }
    void SetVec4(const std::string& name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(m_ID, name.c_str()), x, y, z, w);
    }

    void SetMat2(const std::string& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void SetMat3(const std::string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void SetMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
};

#endif