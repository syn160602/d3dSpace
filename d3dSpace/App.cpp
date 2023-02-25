#include "App.h"
#include "SkinnedBox.h"
#include "Earth.h"
#include "Moon.h"
#include <memory>
#include <algorithm>
#include "GDIPlusManager.h"
#include "imgui/imgui.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace dx = DirectX;

GDIPlusManager gdipm;

App::App()
	:
	wnd(1600, 900, "newWnd"),
	sun( wnd.Gfx() )
{
	drawables.emplace_back( std::make_unique<Earth>(wnd.Gfx(), 10.0f, 300.0f) );
	drawables.emplace_back( std::make_unique<Moon>( wnd.Gfx(), dynamic_cast<Earth*>(drawables[0].get()) ) );
	drawables.emplace_back( std::make_unique<SkinnedBox>( wnd.Gfx(), 10000.0f ) );
	
	v = std::make_unique<Vehicle>(wnd, dynamic_cast<Earth*>(drawables[0].get()), dynamic_cast<Moon*>(drawables[1].get()), 0.05);

	overallCamera = std::make_unique<OvlCamera>();
	FppCamera = std::make_unique<FPPCamera>(*v);
	TppCamera = std::make_unique<TPPCamera>(*v);
}

int App::Go()
{
	while (true) {
		// Process all messages pending, but not block for new messages
		if (const auto ecode = Window::ProcessMessages())
			//if return optional has value, means to quit so return exit code
			return *ecode;

		DoFrame();
	}
}

App::~App()
{}

void App::DoFrame()
{
	while (const auto e = wnd.kbd.ReadKey())
	{
		if (!e->IsPress())
		{
			continue;
		}
		switch (e->GetCode())
		{
		case VK_F1:
			if (wnd.CursorEnabled())
			{
				wnd.DisableCursor();
				wnd.mouse.EnableRaw();
			}
			else
			{
				wnd.EnableCursor();
				wnd.mouse.DisableRaw();
			}
			break;
		case VK_F2:
			if (cst == OVERALL_VIEW)
				cst = FPP_FOLLOW;
			else if (cst == FPP_FOLLOW)
				cst = TPP_FOLLOW;
			else
				cst = OVERALL_VIEW;
			break;
		case VK_F3:
			starMove = !starMove;
			break;
		}
	}
	// reproj Camera
	switch (cst)
	{
	case OVERALL_VIEW:
		wnd.Gfx().SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 1.0f, 18000.0f));
		break;
	case FPP_FOLLOW:
		wnd.Gfx().SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.3f, 13700.0f));
		break;
	case TPP_FOLLOW:
		wnd.Gfx().SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.8f, 14000.0f));
		break;
	}
	Camera* cam = nullptr;
	switch (cst) {
	case OVERALL_VIEW:
		cam = overallCamera.get();
		break;
	case FPP_FOLLOW:
		cam = FppCamera.get();
		break;
	case TPP_FOLLOW:
		cam = TppCamera.get();
		break;
	}
	wnd.Gfx().SetCamera(cam->GetMatrix());

	// set star move
	if (starMove)
	{
		dynamic_cast<Earth*>(drawables[0].get())->EnableMove();
		dynamic_cast<Moon*>(drawables[1].get())->EnableMove();
	}
	else
	{
		dynamic_cast<Earth*>(drawables[0].get())->DisableMove();
		dynamic_cast<Moon*>(drawables[1].get())->DisableMove();
	}

	auto dt = timer.Mark() ;
	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);

	sun.Bind(wnd.Gfx(), cam->GetMatrix());

	// render geometry
	for (auto& b : drawables)
	{
		b->Update(wnd.kbd.KeyIsPressed(VK_SPACE) ? 0.0f : dt * speed_factor);
		b->Draw(wnd.Gfx());
	}
	v->Update(wnd.kbd.KeyIsPressed(VK_SPACE) ? 0.0f : dt * speed_factor);
	v->Draw(wnd.Gfx());
	sun.Draw(wnd.Gfx());

	// operation responds
	cam->Update(wnd, dt);
	
	// imgui windows
	SpawnSimulationWindow();
	cam->SpawnControlWindow();
	
	// present
	wnd.Gfx().EndFrame();
}

void App::SpawnSimulationWindow() noexcept
{
	if (ImGui::Begin("Simulation"))
	{
		ImGui::Text("Cursor: %s, press F1 to %s", wnd.CursorEnabled() ? "enabled" : "disabled", wnd.CursorEnabled() ? "disable" : "enable");
		if (ImGui::TreeNode("Simulation Speed"))
		{
			ImGui::SliderFloat("Speed Factor", &speed_factor, 0.0f, 200.0f, "%.4f", 5.0f);
			ImGui::Text("Application immediate %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Text("Status: %s", wnd.kbd.KeyIsPressed(VK_SPACE) ? "PAUSED" : "RUNNING (hold Space to pause)");
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Choose Camera"))
		{
			ImGui::Text("Choose below or press F2 to switch cameras");
			const std::vector<std::string> cameraNames = { "overall view", "fpp follow", "tpp follow" };
			const auto preview = cameraNames[cst];
			if (ImGui::BeginCombo("", preview.c_str()))
			{
				for (int i = 0; i < 3; i++) 
				{
					const bool selected = cst == i;
					if (ImGui::Selectable(cameraNames[i].c_str(), selected))
						cst = CameraStat(i);
					if (selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Star State"))
		{
			ImGui::Text("Stars %s moving,", starMove ? "" : "not");
			ImGui::Text("Press F3 or click the box below to let stars %s", starMove ? "stop" : "move");
			ImGui::Checkbox("Stars Move", &starMove);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Vehicle State"))
		{
			v->ImguiShow();
			ImGui::TreePop();
		}
	}
	ImGui::End();
}
