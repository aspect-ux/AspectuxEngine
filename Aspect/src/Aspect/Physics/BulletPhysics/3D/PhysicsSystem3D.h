#pragma once

#include "Aspect/Renderer/Renderer2D.h"

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

namespace Aspect
{
	namespace Utils
	{
		inline glm::vec3 BtVec3ToGlm(const btVector3& src)
		{
			return glm::vec3(src.x(), src.y(), src.z());
		}

		inline btVector3 GlmToBtVec3(const glm::vec3& src)
		{
			return btVector3(src.x, src.y, src.z);
		}

		class BulletDrawer : public btIDebugDraw
		{
		public:
			virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override
			{
				//Renderer2D::DrawLine(BtVec3ToGlm(from), BtVec3ToGlm(to), { BtVec3ToGlm(color), 1.0f });
				Renderer2D::DrawLine(BtVec3ToGlm(from), BtVec3ToGlm(to), glm::vec4(0, 1, 0, 1));
			}

			virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override
			{
				// TODO
				//Renderer2D::DrawLine(BtVec3ToGlm(PointOnB), BtVec3ToGlm(normalOnB), { BtVec3ToGlm(color), 1.0f });
				Renderer2D::DrawLine(BtVec3ToGlm(PointOnB), BtVec3ToGlm(normalOnB), glm::vec4(0, 1, 0, 1));
			}

			virtual void reportErrorWarning(const char* warningString) { AS_CORE_ERROR(warningString); };

			virtual void draw3dText(const btVector3& location, const char* textString) {};

			virtual void setDebugMode(int debugMode) { mDebugMode = debugMode; };

			virtual int getDebugMode() const override { return mDebugMode; }

			int mDebugMode = DebugDrawModes::DBG_DrawWireframe;
		};
	}

}
