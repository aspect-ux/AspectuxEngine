#pragma once
#include <glm/glm.hpp>
namespace Aspect {
	class Camera {
	public:
		Camera() = default;
		virtual ~Camera() = default;

		Camera(const glm::mat4& projection)
			: m_Projection(projection) {
			//TODO: to be fixed
			m_ProjectionMatrix = m_Projection;
		}
		const glm::mat4& GetProjection() const { return m_Projection; }

		//TODO:
		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetUnReversedProjectionMatrix() const { return m_UnReversedProjectionMatrix; }
	protected:
		glm::mat4 m_Projection = glm::mat4(1.0f);

		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
		//Currently only needed for shadow maps and ImGuizmo
		glm::mat4 m_UnReversedProjectionMatrix = glm::mat4(1.0f);
	};
}
