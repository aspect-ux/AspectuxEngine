#include "aspch.h"
#include "Project.h"

#include "ProjectSerializer.h"
#include "Aspect/Script/ScriptBuilder.h"

namespace Aspect {

	Ref<Project> Project::New()
	{
		s_ActiveProject = CreateRef<Project>();
		return s_ActiveProject;
	}

	Ref<Project> Project::Load(const std::filesystem::path& path)
	{
		Ref<Project> project = CreateRef<Project>();

		ProjectSerializer serializer(project);
		if (serializer.Deserialize(path))
		{
			AS_CORE_WARN("Project::Load at <{0}:{1}> "+path.string(),__FILE__,__LINE__);
			//TODO: m_ProjectDirectory is to be replaced by GetConfig()
			project->m_ProjectDirectory = path.parent_path();
			project->GetConfig().ProjectDirectory = path.parent_path().string();
			s_ActiveProject = project;

			//TODO: Load Project Assets after project loaded!!
			s_AssetManager = AspectRef<EditorAssetManager>::Create();
			return s_ActiveProject;
		}

		return nullptr;
	}

	bool Project::SaveActive(const std::filesystem::path& path)
	{
		ProjectSerializer serializer(s_ActiveProject);
		if (serializer.Serialize(path))
		{
			s_ActiveProject->m_ProjectDirectory = path.parent_path();
			AS_CORE_WARN("ProjectDirectory:" + s_ActiveProject->m_ProjectDirectory.string());
			return true;
		}

		return false;
	}

}
