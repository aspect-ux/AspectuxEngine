# AspectuxEngine

A game engine based on opengl。

![Language](https://img.shields.io/badge/Language-c++-blue)

## Introduction

I really learned a lot from github and some excellent open source authors or video makers。

This is my first game engine and the last one as well of my undergraduate life.



It is a game engine based on opengl.

I don't know whether will it support DX11/12 or Vulkan in the future or not,it depends...

The engine contents are as follows.

## Inventory

### Log System(日志系统)

Based on open source header-only library **`spdlog`**

We have three loggers for our log system.

```cpp
// 核心层，客户端层，编辑器层
std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
std::shared_ptr<spdlog::logger> Log::s_ClientLogger;
std::shared_ptr<spdlog::logger> Log::s_EditorConsoleLogger;
```

核心层，客户端层的日志有两个输出的地方，一个是带颜色的`stdout`，一个是文件Aspect.log.

编辑器层Editor输出日志的地方也有两个，一个是文件APP.log，一个是控制台`EditorConsoleSink`.

均支持多线程。

日志级别有以下5个

```cpp
Level::Trace
Level::Info:
Level::Warn:
Level::Error
Level::Fatal
```

## My Notes

[Click here](Documentation/如何从零开发游戏引擎.md)

## Contents

### 2d/3d Renderer,Editor

![image-20240320222235545](README/1.png)

console

![image-20240320223455658](README/3.png)

### Resources Management and Serialization System

find Assets directory and write the found files description into .asr file, then serialize from the file to a scene 

subject。 

![image-20240320223659152](README/4.png)

### Script System

**Based on Mono**, just some basic functions are implemented

example of position change

```c#
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Aspect;

namespace Sandbox
{
	public class testMove : Entity
	{
		private TransformComponent m_Transform;
		//private Rigidbody2DComponent m_Rigidbody;

		public float Speed = 10.0f;
		public float Time = 0.0f;

		void OnCreate()
		{
			Console.WriteLine($"Player.OnCreate - {ID}");

			int testvalue = 10;
			Log.Info($"Test For console, testvalue: {0}", testvalue);

			m_Transform = GetComponent<TransformComponent>();
		}

		void OnUpdate(float ts)
		{
			Time += ts;
			Console.WriteLine($"Player.OnUpdate: {ts}");

			float speed = Speed;
			Vector3 velocity = Vector3.Zero;
			/*
			if (Input.IsKeyDown(KeyCode.W))
				velocity.Y = 1.0f;
			else if (Input.IsKeyDown(KeyCode.S))
				velocity.Y = -1.0f;

			if (Input.IsKeyDown(KeyCode.A))
				velocity.X = -1.0f;
			else if (Input.IsKeyDown(KeyCode.D))
				velocity.X = 1.0f;
			
			Entity cameraEntity = FindEntityByName("Camera");
			if (cameraEntity != null)
			{
				Camera camera = cameraEntity.As<Camera>();

				if (Input.IsKeyDown(KeyCode.Q))
					camera.DistanceFromPlayer += speed * 2.0f * ts;
				else if (Input.IsKeyDown(KeyCode.E))
					camera.DistanceFromPlayer -= speed * 2.0f * ts;
			}*/
			velocity.X = speed;
			//m_Rigidbody.ApplyLinearImpulse(velocity.XY, true);

			Vector3 translation = m_Transform.Translation;
			translation += velocity * ts;
			m_Transform.Translation = translation;
		}

	}
}

```

put a script component to this entity

![image-20240320223237185](README/2.png)

### Physics

Based on bullet

## Credits

* [TheCherno/Hazel]([TheCherno/Hazel: Hazel Engine (github.com)](https://github.com/TheCherno/Hazel))
* [hebohang/HEngine]([hebohang/HEngine: HEngine - 2D/3D Game Engine written in C++20 (github.com)](https://github.com/hebohang/HEngine))
* [BoomingTech/Piccolo]([BoomingTech/Piccolo: Piccolo (formerly Pilot) – mini game engine for games104 (github.com)](https://github.com/BoomingTech/Piccolo))
* [adriengivry/Overload]([adriengivry/Overload: 3D Game engine with editor (github.com)](https://github.com/adriengivry/Overload))
* [LearnOpengl](https://learnopengl-cn.github.io/)
* Games101/Games202
