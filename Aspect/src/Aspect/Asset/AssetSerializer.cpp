#include "aspch.h"
#include "AssetSerializer.h"
#include "Aspect/Project/Project.h"

namespace Aspect
{
//////////////////////////////////////////////////////////////////////////////////
// ScriptFileSerializer
//////////////////////////////////////////////////////////////////////////////////

	void ScriptFileSerializer::Serialize(const AssetMetadata& metadata, const AspectRef<Asset>& asset) const
	{
		std::ofstream stream(Project::GetEditorAssetManager()->GetFileSystemPath(metadata));
		AS_CORE_VERIFY(stream.is_open());

		std::ifstream templateStream("Resources/Templates/NewClassTemplate.cs");
		AS_CORE_VERIFY(templateStream.is_open());

		std::stringstream templateStrStream;
		templateStrStream << templateStream.rdbuf();
		std::string templateString = templateStrStream.str();

		templateStream.close();

		auto replaceTemplateToken = [&templateString](const char* token, const std::string& value)
			{
				size_t pos = 0;
				while ((pos = templateString.find(token, pos)) != std::string::npos)
				{
					templateString.replace(pos, strlen(token), value);
					pos += strlen(token);
				}
			};

		auto scriptFileAsset = asset.As<ScriptFileAsset>();
		replaceTemplateToken("$NAMESPACE_NAME$", scriptFileAsset->GetClassNamespace());
		replaceTemplateToken("$CLASS_NAME$", scriptFileAsset->GetClassName());

		stream << templateString;
		stream.close();
	}

	bool ScriptFileSerializer::TryLoadData(const AssetMetadata& metadata, AspectRef<Asset>& asset) const
	{
		asset = AspectRef<ScriptFileAsset>::Create();
		asset->Handle = metadata.Handle;
		return true;
	}
}
