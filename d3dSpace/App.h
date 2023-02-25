#pragma once
#include "Window.h"
#include "myTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "Sun.h"
#include "Vehicle.h"
#include <set>

class App
{
public:
	enum CameraStat
	{
		OVERALL_VIEW,
		FPP_FOLLOW,
		TPP_FOLLOW,
	};
	App();
	// master frame & message loop
	int Go();
	~App();
private:
	void DoFrame();
	void SpawnSimulationWindow() noexcept;
private:
	ImguiManager imgui;
	Window wnd;
	myTimer timer;
	float speed_factor = 1.0f;
	// cameras
	CameraStat cst = OVERALL_VIEW;
	std::unique_ptr<OvlCamera> overallCamera;
	std::unique_ptr<FPPCamera> FppCamera;
	std::unique_ptr<TPPCamera> TppCamera;
	// TPPCamera TppCamera;
	// stars
	bool starMove = false;
	std::vector<std::unique_ptr<class Drawable>> drawables;
	std::unique_ptr<class Vehicle> v;
	Sun sun;
};