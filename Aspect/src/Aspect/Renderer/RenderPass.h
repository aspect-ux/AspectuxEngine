#pragma once

#include "Aspect/Core/Base/Base.h"

#include "FrameBuffer.h"
#include "PostProcessing.h"

namespace Aspect {

	struct RenderPassSpecification
	{
		Ref<FrameBuffer> TargetFramebuffer = nullptr;
		std::string DebugName = "";
		glm::vec4 MarkerColor = glm::vec4(0.0,0.0,0.0,1.0);
	};

	class RenderPass 
	{
	public:
		RenderPass(RenderPassSpecification Spec) : mSpecification(Spec) {};
		virtual ~RenderPass() = default;

		virtual RenderPassSpecification& GetSpecification() { return mSpecification; };
		virtual const RenderPassSpecification& GetSpecification() const { return mSpecification; };

		//TODO:static Ref<RenderPass> Create(const RenderPassSpecification& spec);

		void AddPostProcessing(PostProcessingType type);

		uint32_t ExcuteAndReturnFinalTex();
	public:
		std::vector<Scope<PostProcessing>> mPostProcessings;
	private:
		RenderPassSpecification mSpecification;
	};

}
