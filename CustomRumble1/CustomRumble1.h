#pragma once

#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include <RLSDK/Utils.hpp>
#include <random>

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);


class CustomRumble1: public BakkesMod::Plugin::BakkesModPlugin, public BakkesMod::Plugin::PluginSettingsWindow
{
private:
	bool ClassesSafe = false;

private:
	std::shared_ptr<int32_t> Enabled;
	std::shared_ptr<std::mt19937> RandomDevice;

public:
	bool AreGObjectsValid();
	bool AreGNamesValid();
	bool LoadClasses();

	virtual void onLoad();
	virtual void onUnload();

	ServerWrapper getSW();

	void RenderSettings() override;
	std::string GetPluginName() override;
	void SetImGuiContext(uintptr_t ctx) override;

	enum powerUpEnum {
		Haymaker,
		TheBoot,
		Magnetizer,
		Swapper,
		Spike,
		GrapplingHook,
		PowerHitter,
		Plunger,
		Freezer,
		Disruptor,
		Tornado,
		NumPowerups
	};

	std::string powerUpStrings[NumPowerups] = {
		"BallSpring",
		"CarSpring",
		"BallMagnet",
		"EnemySwapper",
		"BallVelcro",
		"GrapplingHook",
		"Powerhitter",
		"BallLasso",
		"BallFreeze",
		"EnemyBooster",
		"Tornado"
	};

	std::string powerUpEnglishStrings[NumPowerups] = {
		"Haymaker",
		"The Boot",
		"Magnetizer",
		"Swapper",
		"Spike",
		"Grappling Hook",
		"Power Hitter",
		"Plunger",
		"Freezer",
		"Disruptor",
		"Tornado"
	};

	/*
	[20:34:10] [bakkesmod] [class CustomRumble1] object name = SpecialPickup_BallSpring
	[20:34:10] [bakkesmod] [class CustomRumble1] object name = SpecialPickup_CarSpring
	[20:34:10] [bakkesmod] [class CustomRumble1] object name = SpecialPickup_GravityWell
	[20:34:10] [bakkesmod] [class CustomRumble1] object name = SpecialPickup_Swapper
	[20:34:10] [bakkesmod] [class CustomRumble1] object name = SpecialPickup_BallVelcro
	[20:34:10] [bakkesmod] [class CustomRumble1] object name = SpecialPickup_BallGrapplingHook
	[20:34:10] [bakkesmod] [class CustomRumble1] object name = SpecialPickup_StrongHit
	[20:34:10] [bakkesmod] [class CustomRumble1] object name = SpecialPickup_BallLasso
	[20:34:10] [bakkesmod] [class CustomRumble1] object name = SpecialPickup_BallFreeze
	[20:34:10] [bakkesmod] [class CustomRumble1] object name = SpecialPickup_BoostOverride
	[20:34:10] [bakkesmod] [class CustomRumble1] object name = SpecialPickup_Tornado
	*/

	void onPowerupGive(ActorWrapper caller, void* params);
	void onGiveItem(ActorWrapper caller, void* params);
	std::string generateNextPower(int teamNum);

	// Inherited via PluginWindow
	/*

	bool isWindowOpen_ = false;
	bool isMinimized_ = false;
	std::string menuTitle_ = "CustomRumble1";

	virtual void Render() override;
	virtual std::string GetMenuName() override;
	virtual std::string GetMenuTitle() override;
	virtual void SetImGuiContext(uintptr_t ctx) override;
	virtual bool ShouldBlockInput() override;
	virtual bool IsActiveOverlay() override;
	virtual void OnOpen() override;
	virtual void OnClose() override;
	
	*/
};

