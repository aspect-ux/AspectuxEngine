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
		// Extract name from file: �������һ��slash�����һ����׺dot,��ȡ�ļ���
		auto lastSlash = filepath.find_last_of("/\\"); // �鿴�Ƿ�������б�ܣ�slash),��б����Ҫת��
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1; // npos�����ȣ���� == npos,��ζ��û���ҵ�slash
		auto lastDot = filepath.rfind('.'); // �ҵ����һ��.��λ��
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
		// ��ָ��·�����ļ���ȡshader
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
		std::array<GLenum,2> glShaderIDs; // array�����̶�

		int glShaderIndex = 0;

		// С֪ʶ
		// capacity: ָ���ǿ����ɵ�Ԫ��������size:ָ���ǵ�ǰ��Ԫ�ظ���
		// reserve: ����Ԥ����ռ�capacity��ֻ�����ݣ�resize������Ԫ�ظ��������n���ڵ�ǰcapacity,�����ݣ����nС�ڵ�ǰsize��ɾ������
		// ע�⣡������reserveֻ��Ԥ����ռ䣬�����ܴ������󣬻���Ҫʹ��push_back��insert
		//glShaderIDs.reserve(shaderSource.size()); // ���·����ڴ�ǿ�н�������Ϊn��n��С�ڵ�ǰ

		for (auto &kv : shaderSource)
		{
			GLenum type = kv.first;					// ��ɫ������
			const std::string source = kv.second;   // ��ɫ��Դ��

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
			size_t curlineEndPos = source.find_first_of("\r\n", findCurPos);///r/nд��Ϊ/r/n
			AS_CORE_ASSERT(curlineEndPos != std::string::npos, "����shaderʧ��");
			size_t begin = findCurPos + typeTokenLen + 1;

			std::string type = source.substr(begin, curlineEndPos - begin);// ��ȡ����vertex����fragment
			AS_CORE_ASSERT(ShaderTypeFromString(type), "��Ч��shader������	");

			size_t nextLinePos = source.find_first_not_of("\r\n", curlineEndPos);
			findNextPos = source.find(typeToken, nextLinePos);
			// ��ȡ�������shader����
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
