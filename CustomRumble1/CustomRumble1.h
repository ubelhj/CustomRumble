#pragma once

#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include <RLSDK/Utils.hpp>
#include <random>
#include "RLSDK/GameDefines.hpp"

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
		"SpecialPickup_BallSpring",
		"SpecialPickup_CarSpring",
		"SpecialPickup_GravityWell",
		"SpecialPickup_Swapper",
		"SpecialPickup_BallVelcro",
		"SpecialPickup_BallGrapplingHook",
		"SpecialPickup_StrongHit",
		"SpecialPickup_BallLasso",
		"SpecialPickup_BallFreeze",
		"SpecialPickup_BoostOverride",
		"SpecialPickup_Tornado"
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

