#pragma once

#include "Aspect/Core/Ref.h"

#include "Pipeline.h"

namespace Aspect {

	class RenderCommandBuffer : public RefCounted
	{
	public:
		virtual ~RenderCommandBuffer() = default;

		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual void Submit() = 0;

		virtual float GetExecutionGPUTime(uint32_t frameIndex, uint32_t queryIndex = 0) const = 0;
		virtual const PipelineStatistics& GetPipelineStatistics(uint32_t frameIndex) const = 0;

		virtual uint32_t BeginTimestampQuery() = 0;
		virtual void EndTimestampQuery(uint32_t queryID) = 0;

		static Ref<RenderCommandBuffer> Create(uint32_t count = 0, const std::string& debugName = "");
		static Ref<RenderCommandBuffer> CreateFromSwapChain(const std::string& debugName = "");
	};

}
