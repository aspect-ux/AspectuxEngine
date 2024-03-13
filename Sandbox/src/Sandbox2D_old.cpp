#include "Sandbox2D.h"

// -------Entry Point---------
//#include <Aspect/Core/EntryPoint.h> // �����ű�ʾֱ�Ӵ�ϵͳ��������ļ���Ч�ʸ��ߣ�  " "������Χ���㣬�������Ȱ�������Ŀ¼�е��ļ�

#include "Platform/OpenGL/OpenGLShader.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Example of Timer
template<typename Fn>
class Timer
{
public:
	/*
	 *@ class Timer
	 *@ description steady_clock���ڼ�¼ʱ�� 
	*/
	Timer(const char* name,Fn&& func)
		: m_Name(name),m_Stopped(false),m_Func(func)
	{
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}

	~Timer()
	{
		if (!m_Stopped)
			Stop();
	}

	void Stop()
	{
		auto endTimepoint = std::chrono::high_resolution_clock::now();

		// microseconds΢��
		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

		m_Stopped = true;

		float duration = (end - start) * 0.001f;

		//std::cout << m_Name << " Duration: " << duration << "ms" << std::endl;

		m_Func({ m_Name,duration });
	}
private:
	const char* m_Name;
	std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
	bool m_Stopped;
	Fn m_Func;
};

// []��ʾ����capture�������ı�����[=]��ֵ��[&]�����ã�[]ʲôҲ������
#define PROFILE_SCOPE(name) Timer timer##__LINE__(name,[&](ProfileResult profileResult) {m_ProfileResults.push_back(profileResult); })


/*
0������
1��ǽ��
2�ǲݵ�
3������
*/
static const uint32_t s_MapWidth = 20;
static const char* s_MapTiles =
"00000000000000000000"
"01111111111111111100"
"01222312332312323100"
"01213111121111213100"
"01212222222222212100"
"01211211121112113100"
"01213233333332213100"
"01331212313212133100"
"01111232111232311100"
"01113212313212132100"
"01311233333332112100"
"01321212121312333100"
"01313211121112113100"
"01311222222222212100"
"01211131121111312100"
"01213131121111212100"
"01223333233333322100"
"01111111111111111100"
"00000000000000000000"
"00000000000000000000"
;

Sandbox2D::Sandbox2D()
	:Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f, true), m_SquarePosition(0.0f)
{

}

void Sandbox2D::OnAttach()
{
	m_Checkerboard = (Aspect::Texture2D::Create("assets/textures/Checkerboard.png"));
	m_SpriteSheet = (Aspect::Texture2D::Create("assets/game/textures/RPGpack_sheet_2X.png"));

	//m_TextureStairs, m_TextureTree, m_TextureBush
	s_TextureMap['1'] = Aspect::SubTexture2D::CreateFromCoords(m_SpriteSheet, {10, 8}, {128, 128});
	s_TextureMap['2'] = Aspect::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 1, 11 }, { 128, 128 });
	s_TextureMap['3'] = Aspect::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 11, 11 }, { 128, 128 }, { 1,2 });

	m_MapWidth = s_MapWidth;
	m_MapHeight = strlen(s_MapTiles) / s_MapWidth;

	// Init here
	m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
	m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	m_Particle.SizeBegin = 0.5f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
	m_Particle.LifeTime = 5.0f;
	m_Particle.Velocity = { 0.0f, 0.0f };
	m_Particle.VelocityVariation = { 3.0f, 1.0f };
	m_Particle.Position = { 0.0f, 0.0f };

	m_CameraController.SetZoomLevel(2.5f);

	Aspect::FrameBufferSpecification fbSpec;
	fbSpec.Width = 1280;
	fbSpec.Height = 720;
	m_FrameBuffer = Aspect::FrameBuffer::Create(fbSpec);
}

void Sandbox2D::OnDetach()
{

}

void Sandbox2D::OnUpdate(Aspect::Timestep ts)
{
	//PROFILE_SCOPE("Sandbox2D::OnUpdate");
	
	{
		//PROFILE_SCOPE("Sandbox2D::CameraController.OnUpdate");
		/// Update
		m_CameraController.OnUpdate(ts);

		// ��Ⱦ��Ϣ��ʼ��
		Aspect::Renderer2D::ResetStats();
	}

	{
		PROFILE_SCOPE("Sandbox2D::OnUpdate.RendererPrep:");

		// 1.���Զ����֡���壨���ڽ����Ⱦ��Ĭ�ϵ�֡�����У�
		m_FrameBuffer->Bind();
		// Renderer Prep
		Aspect::RenderCommand::SetClearColor(glm::vec4({ 0.1f, 0.1f, 0.1f, 1.0f }));
		Aspect::RenderCommand::Clear();
	}

#if 0
	{
		// PROFILE_SCOPE("Sandbox2D::OnUpdate.RendererDraw:");
		// Renderer Draw

		 // ��������ϻ���ת��Quad//
		static float rotation = 0.0f;
		rotation += ts * 50.0f;

		Aspect::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Aspect::Renderer2D::DrawRotatedQuad({ 1.0f, 0.0f }, { 0.8f, 0.8f }, -45.0f, { 0.2,0.6,0.2,1.0 });
		Aspect::Renderer2D::DrawQuad({ -1.0f,0.0f }, { 0.8f,0.8f }, { 0.8f,0.2f,0.1f,1.0f });
		Aspect::Renderer2D::DrawQuad({ 0.5f,-0.5f }, { 0.5f,0.75f }, { 0.2f,0.2f,0.8f,1.0f });
		Aspect::Renderer2D::DrawQuad({ 0.0f,0.0f,-0.1f }, { 20.0f,20.0f }, m_Checkerboard,10.0f);
		Aspect::Renderer2D::DrawRotatedQuad({ -0.2f, -0.0f, 0.0f }, { 1.0f, 1.0f }, rotation, m_Checkerboard, 20.0f);
		Aspect::Renderer2D::EndScene();


		// �����µĻ��ƣ������û����ڴ�
		Aspect::Renderer2D::BeginScene(m_CameraController.GetCamera());
		for (float y = -5.0f; y < 5.0f; y += 0.5f) {
			for (float x = -5.0f; x < 5.0f; x += 0.5f)
			{
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f , (y + 5.0f) / 10.0f , 1.0f };
				Aspect::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
			}
		}
		Aspect::Renderer2D::EndScene();
	}
#endif
	// ���λ��ת��������ռ�
	if (Aspect::Input::IsMouseButtonPressed(Aspect::Mouse::Button0))
	{
		auto [x, y] = Aspect::Input::GetMousePosition();
		auto width = Aspect::Application::Get().GetWindow().GetWidth();
		auto height = Aspect::Application::Get().GetWindow().GetHeight();

		auto bounds = m_CameraController.GetBounds();
		auto pos = m_CameraController.GetCamera().GetPosition();

		/* ///��δ���Ļ���굽��������///
		* �ܹ��̣�viewport��任��ndc��任��VP��任
		* 
		* 1. viewport: ������ͨ��������Ļ����ӡ�0��1���任����0��width(height��,���Ե�һ����x,y�ֱ���Գ���
		* 2. ndc��������ͨ����һ���õ������������ڶ������Ǽ�ȥ0.5Ȼ�����2. 
		*         ע�⣺��1�������Bound��t,b,l,r�Ľṹ�壬ԭ��ndc�任����[-t,t]��[0,1]�Ĺ�һ������,��TΪ��λ�����������,���Կ���[-1,1]
		*               �������ս�����ǳ���2������bounds.GetWidth()...
		*               (2) ��������yֵ�������ȡ���ˣ���ΪOpenGL����������ϵ����y+,��x+������£�z+ָ����Ļ�ڡ�
		*               ��ʵ�ϣ�OpenGL���½�Ϊԭ�㣬����ƽ��Ҳ�Ǵ����½ǿ�ʼ���Ʊػᵼ�»��Ƴ����������µߵ�������������Ҫ�����y���еߵ�һ�¡�
		*               Ȼ��z+ָ����Ļ��
		* 3. VP�任: Ŀǰֻʵ��2D��������������任��Ҫ������ƻ�ԭ��(view) �����ŵ���-1��1��^3(projection);���������������ԭ�㣬����ǰ������Ҳ�Ѿ�
		* �ڡ�-1��1�����������ڣ�����Ҳ������Ϊʲô 2��1���г���bounds.GetWidth()������2���������Ѿ������������任�ˡ�
		* 
		* ���䣺���ۿ���������������ϵ�����۲�����������֣�OpenGL����)
		*/
		x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
		y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
		m_Particle.Position = { x + pos.x, y + pos.y };
		for (int i = 0; i < 5; i++)
			m_ParticleSystem.Emit(m_Particle);
	}

	m_ParticleSystem.OnUpdate(ts);
	m_ParticleSystem.OnRender(m_CameraController.GetCamera());


	Aspect::Renderer2D::BeginScene(m_CameraController.GetCamera());
	//Aspect::Renderer2D::DrawQuad({ 0.0f,0.0f,0.f }, { 1.0f,1.0f }, m_SpriteSheet,1.0f);
	Aspect::Renderer2D::EndScene();


	Aspect::Renderer2D::BeginScene(m_CameraController.GetCamera());
	for (uint32_t y = 0; y < m_MapHeight; y++) {
		for (uint32_t x = 0; x < m_MapWidth; x++) {
			// x + y * m_MapWidth; �и��2ά����
			char titleType = s_MapTiles[x + y * m_MapWidth];
			if (titleType == '0') { // 0�Ķ�������
				continue;
			}
			Aspect::Ref<Aspect::SubTexture2D> texture;
			if (s_TextureMap.find(titleType) != s_TextureMap.end()) {
				texture = s_TextureMap[titleType];
			}
			else {
				texture = m_TextureBush;
			}
			Aspect::Renderer2D::DrawQuad({ x - m_MapWidth / 2.0f, m_MapHeight / 2.0f - y, 0.5f }, { 1.0f, 1.0f }, texture,1.0f); // x - m_MapWidth / 2.0f,  y - m_MapHeight / 2.0f // �ᵼ��y���෴�滭
		}
	}
	Aspect::Renderer2D::EndScene();


	// 3.���֡����
	m_FrameBuffer->UnBind();
}

void Sandbox2D::OnImGuiRender()
{
	/* ͨ��imgui�鿴ĳ�������ĺ�ʱ
	for (auto& result : m_ProfileResults)
	{
		char label[50];
		strcpy(label, "%.3fms ");
		strcat(label, result.Name);
		ImGui::Text(label, result.Time);
	}
	m_ProfileResults.clear(); // size = 0,capacity����*/

	static bool dockspaceOpen = true;
	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
	if (!opt_padding)
		ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// Submit the DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Options"))
		{
			if (ImGui::MenuItem("Exit")) Aspect::Application::Get().Close();
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	ImGui::End();

	ImGui::Begin("Settings");
	auto stats = Aspect::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

	// imgui��Ⱦһ����������
	//uint32_t textureID = m_Checkerboard->GetRendererID();
	//ImGui::Image((void*)textureID, ImVec2(256.0f, 256.0f));

	// imgui��Ⱦ֡�����еĶ���(����֡�����ϵ���ɫ������)
	uint32_t textureID = m_FrameBuffer->GetColorAttachmentRendererID();
	ImGui::Image((void*)textureID, ImVec2{ 1280, 720 });

	ImGui::End();
}

void Sandbox2D::OnEvent(Aspect::Event& e)
{
	m_CameraController.OnEvent(e);
}
