#include "aspch.h"
#include "UICore.h"

#include "Aspect/Renderer/RendererAPI.h"

#include "Aspect/Platform/OpenGL/OpenGLTexture.h"
//#include "Aspect/Platform/OpenGL/OpenGLImage.h"

//#include "imgui/backends/imgui_impl_OpenGL_with_textures.h"

namespace ImGui {
	extern bool ImageButtonEx(ImGuiID id, ImTextureID texture_id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec2& padding, const ImVec4& bg_col, const ImVec4& tint_col);
}

namespace Aspect::UI {
	/*
	ImTextureID GetTextureID(Ref<Texture2D> texture)
	{
		if (RendererAPI::Current() == RendererAPIType::OpenGL)
		{
			Ref<OpenGLTexture2D> OpenGLTexture = texture.As<OpenGLTexture2D>();
			const VkDescriptorImageInfo& imageInfo = OpenGLTexture->GetOpenGLDescriptorInfo();
			if (!imageInfo.imageView)
				return nullptr;

			return ImGui_ImplOpenGL_AddTexture(imageInfo.sampler, imageInfo.imageView, imageInfo.imageLayout);
		}

		return (ImTextureID)0;
	}

	void Image(const Ref<Image2D>& image, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
	{
		if (RendererAPI::Current() == RendererAPIType::OpenGL)
		{
			Ref<OpenGLImage2D> OpenGLImage = image.As<OpenGLImage2D>();
			const auto& imageInfo = OpenGLImage->GetImageInfo();
			if (!imageInfo.ImageView)
				return;
			const auto textureID = ImGui_ImplOpenGL_AddTexture(imageInfo.Sampler, imageInfo.ImageView, OpenGLImage->GetDescriptorInfo().imageLayout);
			ImGui::Image(textureID, size, uv0, uv1, tint_col, border_col);
		}
	}

	void Image(const Ref<Image2D>& image, uint32_t imageLayer, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
	{
		if (RendererAPI::Current() == RendererAPIType::OpenGL)
		{
			Ref<OpenGLImage2D> OpenGLImage = image.As<OpenGLImage2D>();
			auto imageInfo = OpenGLImage->GetImageInfo();
			imageInfo.ImageView = OpenGLImage->GetLayerImageView(imageLayer);
			if (!imageInfo.ImageView)
				return;
			const auto textureID = ImGui_ImplOpenGL_AddTexture(imageInfo.Sampler, imageInfo.ImageView, OpenGLImage->GetDescriptorInfo().imageLayout);
			ImGui::Image(textureID, size, uv0, uv1, tint_col, border_col);
		}
	}

	void ImageMip(const Ref<Image2D>& image, uint32_t mip, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
	{
		Ref<OpenGLImage2D> OpenGLImage = image.As<OpenGLImage2D>();
		auto imageInfo = OpenGLImage->GetImageInfo();
		imageInfo.ImageView = OpenGLImage->GetMipImageView(mip);
		if (!imageInfo.ImageView)
			return;

		const auto textureID = ImGui_ImplOpenGL_AddTexture(imageInfo.Sampler, imageInfo.ImageView, OpenGLImage->GetDescriptorInfo().imageLayout);
		ImGui::Image(textureID, size, uv0, uv1, tint_col, border_col);
	}*/

	void Image(const AspectRef<Texture2D>& texture, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
	{
		if (RendererAPI::Current() == RendererAPIType::OpenGL)
		{
			auto OpenGLTexture = texture.As<OpenGLTexture2D>();
			//const VkDescriptorImageInfo& imageInfo = OpenGLTexture->GetOpenGLDescriptorInfo();
			//if (!imageInfo.imageView)
			//	return;
			ImGui::Image((ImTextureID)OpenGLTexture->GetRendererID(), size, uv0, uv1);
			//const auto textureID = ImGui_ImplOpenGL_AddTexture(imageInfo.sampler, imageInfo.imageView, imageInfo.imageLayout);
			//ImGui::Image(textureID, size, uv0, uv1, tint_col, border_col);
		}
	}
	/*
	bool ImageButton(const Ref<Image2D>& image, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
	{
		return ImageButton(nullptr, image, size, uv0, uv1, frame_padding, bg_col, tint_col);
	}

	bool ImageButton(const char* stringID, const Ref<Image2D>& image, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
	{
		if (RendererAPI::Current() == RendererAPIType::OpenGL)
		{
			Ref<OpenGLImage2D> OpenGLImage = image.As<OpenGLImage2D>();
			const auto& imageInfo = OpenGLImage->GetImageInfo();
			if (!imageInfo.ImageView)
				return false;
			const auto textureID = ImGui_ImplOpenGL_AddTexture(imageInfo.Sampler, imageInfo.ImageView, OpenGLImage->GetDescriptorInfo().imageLayout);
			ImGuiID id = (ImGuiID)((((uint64_t)imageInfo.ImageView) >> 32) ^ (uint32_t)(uint64_t)imageInfo.ImageView);
			if (stringID)
			{
				const ImGuiID strID = ImGui::GetID(stringID);
				id = id ^ strID;
			}
			return ImGui::ImageButtonEx(id, textureID, size, uv0, uv1, ImVec2{ (float)frame_padding, (float)frame_padding }, bg_col, tint_col);
		}

		AS_CORE_VERIFY(false, "Not supported");
		return false;
	}

	bool ImageButton(const Ref<Texture2D>& texture, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
	{
		return ImageButton(nullptr, texture, size, uv0, uv1, frame_padding, bg_col, tint_col);
	}

	bool ImageButton(const char* stringID, const Ref<Texture2D>& texture, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
	{
		AS_CORE_VERIFY(texture);
		if (!texture)
			return false;

		if (RendererAPI::Current() == RendererAPIType::OpenGL)
		{
			Ref<OpenGLTexture2D> OpenGLTexture = texture.As<OpenGLTexture2D>();

			// This is technically okay, could mean that GPU just hasn't created the texture yet
			AS_CORE_VERIFY(OpenGLTexture->GetImage());
			if (!OpenGLTexture->GetImage())
				return false;

			const VkDescriptorImageInfo& imageInfo = OpenGLTexture->GetOpenGLDescriptorInfo();
			const auto textureID = ImGui_ImplOpenGL_AddTexture(imageInfo.sampler, imageInfo.imageView, imageInfo.imageLayout);
			ImGuiID id = (ImGuiID)((((uint64_t)imageInfo.imageView) >> 32) ^ (uint32_t)(uint64_t)imageInfo.imageView);
			if (stringID)
			{
				const ImGuiID strID = ImGui::GetID(stringID);
				id = id ^ strID;
			}
			return ImGui::ImageButtonEx(id, textureID, size, uv0, uv1, ImVec2{ (float)frame_padding, (float)frame_padding }, bg_col, tint_col);
		}

		AS_CORE_VERIFY(false, "Not supported");
		return false;
	}
	*/
}
