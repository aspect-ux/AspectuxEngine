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
		// 1. glm::mat4(1.0f)����4x4��λ����
		// 2. OpenGL�����У�eigen��ѧ���ǰ��У�column)�洢,glm���ǰ��У�row���洢
		//    ����ѧϰ�ǰ��д洢���������о���˻�˳�������һ�£����ݾ���ת�ù�ʽ��
		// 3. ��ͼ�任��View Transform)����Ϊ�Ƚ����ƽ�ƻ�ԭ�㣬Ȼ����ת�������

		// ƽ�ƻ�ԭ��+����룬 transform = ��glm::mat4(1.0f)�任�������ǰ�����position&rotation
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) * 
			glm::rotate(glm::mat4(1.0f),glm::radians(m_Rotation),glm::vec3(0,0,1));

		// ȡ���m_ViewMatrix��������ɽ�����任��glm::mat4(1.0f)�� �������Ҳ����"�ں����"����ͼ�任
		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix; //��Ϊglm���д洢��ע��˳��

	}


}