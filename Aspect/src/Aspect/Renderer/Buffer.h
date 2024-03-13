#pragma once
#include "Aspect/Core/Assert.h"

namespace Aspect
{
	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:  return 4;
			case ShaderDataType::Float2: return 4 * 2;
			case ShaderDataType::Float3: return 4 * 3;
			case ShaderDataType::Float4: return 4 * 4;
			case ShaderDataType::Mat3:   return 4 * 3 * 3;
			case ShaderDataType::Mat4:   return 4 * 4 * 4;
			case ShaderDataType::Int:    return 4;
			case ShaderDataType::Int2:   return 4 * 2;
			case ShaderDataType::Int3:   return 4 * 3;
			case ShaderDataType::Int4:   return 4 * 4;
			case ShaderDataType::Bool:   return 1;

			AS_CORE_ASSERT(false, "Unknown ShaderDataType!");
			return 0;
		}
	}

	struct BufferElements
	{
		std::string Name;
		ShaderDataType Type;
		uint32_t Size;
		uint32_t Offset;
		bool Normalized;

		BufferElements(){}

		BufferElements(ShaderDataType type,const std::string& name,bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0),Normalized(normalized)
		{

		}

		uint32_t GetComponentCount(ShaderDataType type) const
		{
			switch (type)
			{
				case ShaderDataType::Float:  return 1;
				case ShaderDataType::Float2: return 2;
				case ShaderDataType::Float3: return 3;
				case ShaderDataType::Float4: return 4;
				case ShaderDataType::Mat3:   return 3 * 3;
				case ShaderDataType::Mat4:   return 4 * 4;
				case ShaderDataType::Int:    return 1;
				case ShaderDataType::Int2:   return 2;
				case ShaderDataType::Int3:   return 3;
				case ShaderDataType::Int4:   return 4;
				case ShaderDataType::Bool:   return 1;

				AS_CORE_ASSERT(false, "Unknown ShaderDataType!");
				return 0;
			}
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout(){}

		BufferLayout(const std::initializer_list<BufferElements>& elements) : m_Elements(elements)
		{
			CaculateOffsetAndStride();
		}

		inline uint32_t GetStride() const { return m_Stride; }
		inline const std::vector<BufferElements>& GetElements() const { return m_Elements; }

		std::vector<BufferElements>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElements>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElements>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElements>::const_iterator end() const { return m_Elements.end(); }
	private:
		void CaculateOffsetAndStride()
		{
			uint32_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}
	private:
		std::vector<BufferElements> m_Elements;
		uint32_t m_Stride = 0;

	};

	//TODO:
	enum class VertexBufferUsage
	{
		None = 0, Static = 1, Dynamic = 2
	};

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void Bind()const = 0;
		virtual void Unbind()const = 0;

		//virtual void SetData(void* buffer, uint32_t size,uint32_t offset) = 0;
		//virtual void RT_SetData(void* buffer, uint32_t size, uint32_t offset = 0) = 0;

		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;

		virtual void SetData(const void* data, uint32_t size) = 0;

		//static Ref<VertexBuffer> Create(uint32_t size); error: repeat with the last one
		static Ref<VertexBuffer> Create(float* vertices, uint32_t size);

		// added...
		static Ref<VertexBuffer> Create(void* data, uint32_t size, VertexBufferUsage usage = VertexBufferUsage::Static);
		static Ref<VertexBuffer> Create(uint32_t size, VertexBufferUsage usage = VertexBufferUsage::Dynamic);
	};

	// support only for 32-bit buffers
	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void Bind()const = 0;
		virtual void Unbind()const = 0;

		virtual void SetData(const void* data, uint32_t count) = 0;

		virtual uint32_t GetCount() const = 0;
		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);

		//TODO:
		static Ref<IndexBuffer> Create(uint32_t size);
		static Ref<IndexBuffer> Create(void* data, uint32_t size = 0);
	};
}

