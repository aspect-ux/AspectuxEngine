#include "aspch.h"
#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>
namespace Aspect
{
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)),m_ViewMatrix(1.0f)
	{
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::RecaculateViewMatrix()
	{
		// 1. glm::mat4(1.0f)创建4x4单位矩阵
		// 2. OpenGL开发中，eigen数学库是按列（column)存储,glm则是按行（row）存储
		//    正常学习是按列存储，这里所有矩阵乘积顺序需调换一下（根据矩阵转置公式）
		// 3. 视图变换（View Transform)流程为先将相机平移回原点，然后旋转到轴对齐

		// 平移回原点+轴对齐， transform = 将glm::mat4(1.0f)变换到相机当前相机的position&rotation
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) * 
			glm::rotate(glm::mat4(1.0f),glm::radians(m_Rotation),glm::vec3(0,0,1));

		// 取逆后，m_ViewMatrix反过来变成将相机变换到glm::mat4(1.0f)； 这个过程也就是"摆好相机"的视图变换
		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix; //因为glm按行存储，注意顺序

	}


}