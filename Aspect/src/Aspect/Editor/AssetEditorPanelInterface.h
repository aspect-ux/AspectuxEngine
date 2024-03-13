#pragma once

#include "Aspect/Asset/Asset.h"

namespace Aspect {

	// NOTE(Peter): Ugly workaround since AssetEditorPanel includes ImGui.h (meaning ImGui.h can't include AssetEditorPanel).
	//				Will rework those includes at a later date...
	class AssetEditorPanelInterface
	{
	public:
		static void OpenEditor(const AspectRef<Asset>& asset);
	};

}
