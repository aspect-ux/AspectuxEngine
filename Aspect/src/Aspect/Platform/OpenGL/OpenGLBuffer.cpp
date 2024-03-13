
#include "aspch.h"
#include "OpenGLBuffer.h"
#include <glad/glad.h>
namespace Aspect
{

	/*OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
	{
		// 创建缓冲对象
		glCreateBuffers(1, &m_RendererID);
		// 绑定对象
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		// 将顶点数据保存到GPU顶点缓冲供OpenGL使用(动态数据)
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
	{
		// 创建缓冲对象
		glCreateBuffers(1, &m_RendererID);
		// 绑定对象
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		// 将顶点数据保存到GPU顶点缓冲供OpenGL使用
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}*/

	static GLenum OpenGLUsage(VertexBufferUsage usage)
	{
		switch (usage)
		{
		case VertexBufferUsage::Static:    return GL_STATIC_DRAW;
		case VertexBufferUsage::Dynamic:   return GL_DYNAMIC_DRAW;
		}
		AS_CORE_ASSERT(false, "Unknown vertex buffer usage");
		return 0;
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size, VertexBufferUsage usage)
		: m_Usage(usage)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, OpenGLUsage(m_Usage));
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(void* vertices, uint32_t size, VertexBufferUsage usage)
		: m_Usage(usage)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, OpenGLUsage(m_Usage));
	}


	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID); // 绑定顶点buffer
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data); // 更新已有缓存的数据
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count) : m_Count(count)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t count)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * count, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(void* data, uint32_t size)
		: m_Size(size)
	{
		/*
		m_LocalData = Buffer::Copy(data, size);

		Ref<VulkanIndexBuffer> instance = this;
		Renderer::Submit([instance]() mutable
			{
				auto device = VulkanContext::GetCurrentDevice();
				VulkanAllocator allocator("IndexBuffer");

#define USE_STAGING 1
#if USE_STAGING
				VkBufferCreateInfo bufferCreateInfo{};
				bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				bufferCreateInfo.size = instance->m_Size;
				bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
				bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
				VkBuffer stagingBuffer;
				VmaAllocation stagingBufferAllocation = allocator.AllocateBuffer(bufferCreateInfo, VMA_MEMORY_USAGE_CPU_TO_GPU, stagingBuffer);

				// Copy data to staging buffer
				uint8_t* destData = allocator.MapMemory<uint8_t>(stagingBufferAllocation);
				memcpy(destData, instance->m_LocalData.Data, instance->m_LocalData.Size);
				allocator.UnmapMemory(stagingBufferAllocation);

				VkBufferCreateInfo indexBufferCreateInfo = {};
				indexBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				indexBufferCreateInfo.size = instance->m_Size;
				indexBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
				instance->m_MemoryAllocation = allocator.AllocateBuffer(indexBufferCreateInfo, VMA_MEMORY_USAGE_GPU_ONLY, instance->m_VulkanBuffer);

				VkCommandBuffer copyCmd = device->GetCommandBuffer(true);

				VkBufferCopy copyRegion = {};
				copyRegion.size = instance->m_LocalData.Size;
				vkCmdCopyBuffer(
					copyCmd,
					stagingBuffer,
					instance->m_VulkanBuffer,
					1,
					&copyRegion);

				device->FlushCommandBuffer(copyCmd);

				allocator.DestroyBuffer(stagingBuffer, stagingBufferAllocation);
#else
				VkBufferCreateInfo indexbufferCreateInfo = {};
				indexbufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				indexbufferCreateInfo.size = instance->m_Size;
				indexbufferCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

				auto bufferAlloc = allocator.AllocateBuffer(indexbufferCreateInfo, VMA_MEMORY_USAGE_CPU_TO_GPU, instance->m_VulkanBuffer);

				void* dstBuffer = allocator.MapMemory<void>(bufferAlloc);
				memcpy(dstBuffer, instance->m_LocalData.Data, instance->m_Size);
				allocator.UnmapMemory(bufferAlloc);
#endif
			});*/
	}


	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void OpenGLIndexBuffer::SetData(const void* data, uint32_t count)
	{
		m_Count = count;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint32_t) * count, data);
	}
}
