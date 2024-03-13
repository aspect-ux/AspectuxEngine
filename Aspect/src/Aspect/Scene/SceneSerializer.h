#pragma once

#include "Scene.h"

namespace Aspect {

	class SceneSerializer
	{
	public:
		SceneSerializer(const AspectRef<Scene>& scene);

		void Serialize(const std::string& filepath);
		void SerializeRuntime(const std::string& filepath);

		bool Deserialize(const std::string& filepath);
		bool DeserializeRuntime(const std::string& filepath);
	private:
		AspectRef<Scene> m_Scene;
	};

}
