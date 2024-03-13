#include "aspch.h"

#include "OpenGLShader.h"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

namespace Aspect
{
	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;

		AS_CORE_ASSERT(false, "Unknown shader type!");
	}

	OpenGLShader::OpenGLShader(const std::string& filepath)
	{
		// Extract name from file: 根据最后一个slash和最后一个后缀dot,提取文件名
		auto lastSlash = filepath.find_last_of("/\\"); // 查看是否有左右斜杠（slash),右斜杠需要转义
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1; // npos代表长度，如果 == npos,意味着没有找到slash
		auto lastDot = filepath.rfind('.'); // 找到最后一个.的位置
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		m_Name = filepath.substr(lastSlash, count);

		std::string source = ReadFile(filepath);
		auto shaderSource = PreProcess(source);

		Compile(shaderSource);

		
	}

	OpenGLShader::OpenGLShader(const std::string& name,const std::string& vertexSrc, const std::string& fragmentSrc)
		: m_Name(name)
	{
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;
		Compile(sources);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RendererID);
	}

	std::string OpenGLShader::ReadFile(const std::string& filepath)
	{
		std::string result;
		// 从指定路径的文件读取shader
		std::ifstream in(filepath, std::ios::in ,std::ios::binary);

		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else
		{
			AS_CORE_ERROR("Could not open file '{0}'", filepath);
		}

		return result;
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSource)
	{
		GLuint program = glCreateProgram();

		AS_CORE_ASSERT(shaderSource.size() <= 2, "only support 2 shaders");
		std::array<GLenum,2> glShaderIDs; // array容量固定

		int glShaderIndex = 0;

		// 小知识
		// capacity: 指的是可容纳的元素数量；size:指的是当前的元素个数
		// reserve: 重新预分配空间capacity，只可拓容；resize：更改元素个数，如果n大于当前capacity,则拓容，如果n小于当前size，删除多余
		// 注意！！由于reserve只是预分配空间，并不能创建对象，还是要使用push_back或insert
		//glShaderIDs.reserve(shaderSource.size()); // 重新分配内存强行将容量改为n，n不小于当前

		for (auto &kv : shaderSource)
		{
			GLenum type = kv.first;					// 着色器类型
			const std::string source = kv.second;   // 着色器源码

			// Create an empty vertex shader handle
			GLuint shader = glCreateShader(type);
			// Send the vertex shader source code to GL
			// Note that std::string's .c_str is NULL character terminated.
			const GLchar* sourceCstr = (const GLchar*)source.c_str();
			glShaderSource(shader, 1, &sourceCstr, 0);

			// Compile the vertex shader
			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				// We don't need the shader anymore.
				glDeleteShader(shader);

				AS_CORE_ERROR("{0}", infoLog.data());
				AS_CORE_ASSERT(false, "Shader compilation failure!");
				break;
			}

			// Attach our shaders to our program
			glAttachShader(program, shader);

			//glShaderIDs.push_back(shader);
			glShaderIDs[glShaderIndex++] = shader;
		}

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			glDeleteProgram(program);
			// Don't leak shaders either.
			for (auto id : glShaderIDs)
			{
				glDeleteShader(id);
			}

			AS_CORE_ERROR("{0}", infoLog.data());
			AS_CORE_ASSERT(false, "Shader link failure!");
			return;
		}

		// Always detach shaders after a successful link.
		for (auto id : glShaderIDs)
			glDetachShader(program, id);

		m_RendererID = program;
	}

	std::unordered_map<GLenum,std::string> OpenGLShader::PreProcess(const std::string & source)
	{
		std::unordered_map<GLenum, std::string> shaderSources;

		std::string typeToken = "#type";
		size_t typeTokenLen = typeToken.size();
		size_t findCurPos = source.find(typeToken, 0);
		size_t findNextPos = findCurPos;
		while (findNextPos != std::string::npos) {
			size_t curlineEndPos = source.find_first_of("\r\n", findCurPos);///r/n写错为/r/n
			AS_CORE_ASSERT(curlineEndPos != std::string::npos, "解析shader失败");
			size_t begin = findCurPos + typeTokenLen + 1;

			std::string type = source.substr(begin, curlineEndPos - begin);// 获取到是vertex还是fragment
			AS_CORE_ASSERT(ShaderTypeFromString(type), "无效的shader的类型	");

			size_t nextLinePos = source.find_first_not_of("\r\n", curlineEndPos);
			findNextPos = source.find(typeToken, nextLinePos);
			// 获取到具体的shader代码
			shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos, findNextPos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));

			findCurPos = findNextPos;
		}
		return shaderSources;
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::SetBool(const std::string& name, bool value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::SetInt(const std::string& name, int value)
	{
		UploadUniformInt(name, value);
	}

	void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count)
	{
		UploadUniformIntArray(name, values, count);
	}

	void OpenGLShader::SetFloat(const std::string& name, float value)
	{
		UploadUniformFloat(name, value);
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		UploadUniformFloat3(name, value);
	}

	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		UploadUniformFloat4(name, value);
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		UploadUniformMat4(name, value);
	}

	void OpenGLShader::UploadUniformInt(const std::string& name, int value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1iv(location, count, values);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2f(location, value.x, value.y);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(location,value.x, value.y, value.z);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
}
