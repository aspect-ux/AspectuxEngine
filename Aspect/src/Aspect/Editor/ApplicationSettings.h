#pragma once

namespace Aspect {

	struct ApplicationSettings
	{
		//---------- Scripting ------------
		bool ShowHiddenFields = false;
		int ScriptDebuggerListenPort = 2550;

		//---------- Content Browser ------------
		bool ContentBrowserShowAssetTypes = true;
		int ContentBrowserThumbnailSize = 128;
		
		//---------- Aspectnut ------------
		bool AdvancedMode = false;

		static ApplicationSettings& Get();
	};

	class ApplicationSettingsSerializer
	{
	public:
		static void Init();

		static void LoadSettings();
		static void SaveSettings();
	};

}
