#pragma once

#include "Event.h"
#include "Aspect/Scene/Scene.h"
#include "Aspect/Scene/Entity.h"
#include "Aspect/Editor/SelectionManager.h"

#include <sstream>

namespace Aspect {

	class SceneEvent : public Event
	{
	public:
		const AspectRef<Scene>& GetScene() const { return m_Scene; }
		AspectRef<Scene> GetScene() { return m_Scene; }

		EVENT_CLASS_CATEGORY(EventCategoryApplication | EventCategoryScene)
	protected:
		SceneEvent(const AspectRef<Scene>& scene)
			: m_Scene(scene) {}

		AspectRef<Scene> m_Scene;
	};

	class ScenePreStartEvent : public SceneEvent
	{
	public:
		ScenePreStartEvent(const AspectRef<Scene>& scene)
			: SceneEvent(scene) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "ScenePreStartEvent: " << m_Scene->GetName();
			return ss.str();
		}

		EVENT_CLASS_TYPE(ScenePreStart)
	};

	class ScenePostStartEvent : public SceneEvent
	{
	public:
		ScenePostStartEvent(const AspectRef<Scene>& scene)
			: SceneEvent(scene) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "ScenePostStartEvent: " << m_Scene->GetName();
			return ss.str();
		}

		EVENT_CLASS_TYPE(ScenePostStart)
	};

	class ScenePreStopEvent : public SceneEvent
	{
	public:
		ScenePreStopEvent(const AspectRef<Scene>& scene)
			: SceneEvent(scene) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "ScenePreStopEvent: " << m_Scene->GetName();
			return ss.str();
		}

		EVENT_CLASS_TYPE(ScenePreStop)
	};

	class ScenePostStopEvent : public SceneEvent
	{
	public:
		ScenePostStopEvent(const AspectRef<Scene>& scene)
			: SceneEvent(scene) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "ScenePostStopEvent: " << m_Scene->GetName();
			return ss.str();
		}

		EVENT_CLASS_TYPE(ScenePostStop)
	};

	// TODO(Peter): Probably move this somewhere else...
	class SelectionChangedEvent : public Event
	{
	public:
		SelectionChangedEvent(SelectionContext contextID, UUID selectionID, bool selected)
			: m_Context(contextID), m_SelectionID(selectionID), m_Selected(selected)
		{
		}
		
		SelectionContext GetContextID() const { return m_Context; }
		UUID GetSelectionID() const { return m_SelectionID; }
		bool IsSelected() const { return m_Selected; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "EntitySelectionChangedEvent: Context(" << (int32_t)m_Context << "), Selection(" << m_SelectionID << "), " << m_Selected;
			return ss.str();
		}

		EVENT_CLASS_CATEGORY(EventCategoryScene)
		EVENT_CLASS_TYPE(SelectionChanged)
	private:
		SelectionContext m_Context;
		UUID m_SelectionID;
		bool m_Selected;
	};

}
