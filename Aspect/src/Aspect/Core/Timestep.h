#pragma once

namespace Aspect
{
	class Timestep
	{
	public:
		Timestep(float time = 0.0f) : m_Time(time) {}

		operator float() const { return m_Time; } //可以直接把对象当作float变量来读取

		float GetSeconds() const { return m_Time; }
		float GetMilliseconds() const { return m_Time * 1000.0f; }
	private:
		float m_Time;
	};
}
