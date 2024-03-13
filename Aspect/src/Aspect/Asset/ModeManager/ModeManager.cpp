#include "aspch.h"
#include "Aspect/Asset/ModeManager/ModeManager.h"

namespace Aspect
{
	int ModeManager::b3DMode = 1;
	bool ModeManager::bHdrUse = false;
	bool ModeManager::bEditState = true;
	SceneMode ModeManager::mSceneMode = SceneMode::None;
	EditMode ModeManager::mEditMode = EditMode::Select;
	bool ModeManager::bShowPhysicsColliders = false;
	PhysicsDebugDrawModeFlag ModeManager::mPhysicsDebugDrawModeFlag = PhysicsDebugDrawModeFlag::Aspect_DrawWireframe;
}
