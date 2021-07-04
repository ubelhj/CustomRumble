#include "pch.h"
#include "CustomRumble1.h"

BAKKESMOD_PLUGIN(CustomRumble1, "Custom rumble", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void CustomRumble1::onLoad()
{
	_globalCvarManager = cvarManager;

	GObjects = reinterpret_cast<TArray<UObject*>*>(Utils::FindPattern(GetModuleHandleA("RocketLeague.exe"), GObjects_Pattern, GObjects_Mask));
	GNames = reinterpret_cast<TArray<FNameEntry*>*>(Utils::FindPattern(GetModuleHandleA("RocketLeague.exe"), GNames_Pattern, GNames_Mask));

	if (AreGObjectsValid() && AreGNamesValid()) {
		gameWrapper->HookEventWithCaller<ActorWrapper>("Function TAGame.ItemPoolCycle_TA.ApplyItemToCar",
			[this](ActorWrapper caller, void* params, std::string eventname) {
				if (params == nullptr || &caller == nullptr) {
					return;
				}

				if (!getSW()) return;

				UItemPoolCycle_TA_execApplyItemToCar_Params* paramValues = (UItemPoolCycle_TA_execApplyItemToCar_Params*)params;

				UItemPoolCycle_TA* itemPool = (UItemPoolCycle_TA*)caller.memory_address;

				if (itemPool == nullptr) {
					return;
				}

				TArray<class ASpecialPickup_TA*> remItems = itemPool->RemainingItems;

				while (true) {
					for (ASpecialPickup_TA* remItem : remItems) {
						if (remItem->PickupName.IsValid()) {
							std::string name = remItem->PickupName.ToString();
							cvarManager->log(name);

							if (name == "BallLasso") {
								paramValues->Item = remItem;

								itemPool->RefillPool();
								return;
							}
						}
					}

					itemPool->RefillPool();
					remItems = itemPool->RemainingItems;
				}
			});
	} else {
		cvarManager->log("(onLoad) Error: RLSDK classes are wrong, please contact JerryTheBee");
	}

	for (int i = 0; i < NumPowerups; i++) {
		std::string powerup = powerUpStrings[i];
		std::string powerupEnglish = powerUpEnglishStrings[i];
		cvarManager->registerCvar("rumble_enable_blue_" + powerup, "0", "Enables " + powerupEnglish + " for blue", true, true, 0, true, 1)
			.addOnValueChanged([this, i](std::string, CVarWrapper cvar) {
				if (cvar.getBoolValue()) {

				}
				});

		cvarManager->registerCvar("rumble_enable_orange_" + powerup, "0", "Enables " + powerupEnglish + " for orange", true, true, 0, true, 1)
			.addOnValueChanged([this, i](std::string, CVarWrapper cvar) {
			if (cvar.getBoolValue()) {

			}
				});
	}
}

void CustomRumble1::onUnload()
{
}

bool CustomRumble1::AreGObjectsValid() {
	if (UObject::GObjObjects()->Num() > 0 && UObject::GObjObjects()->Max() > UObject::GObjObjects()->Num())
	{
		if (UObject::GObjObjects()->At(0)->GetFullName() == "Class Core.Config_ORS")
		{
			return true;
		}
	}

	return false;
}

bool CustomRumble1::AreGNamesValid() {
	if (FName::Names()->Num() > 0 && FName::Names()->Max() > FName::Names()->Num())
	{
		if (FName(0).ToString() == "None")
		{
			return true;
		}
	}

	return false;
}

bool CustomRumble1::LoadClasses() {
	UGameData_TA* gameData = Utils::GetDefaultInstanceOf<UGameData_TA>();

	return ClassesSafe;
}

ServerWrapper CustomRumble1::getSW() {
	if (gameWrapper->IsInOnlineGame()) {
		auto server = gameWrapper->GetOnlineGame();

		if (server.IsNull()) {
			cvarManager->log("null server");
			return NULL;
		}

		auto playlist = server.GetPlaylist();

		if (!playlist) {
			return NULL;
		}

		// playlist 24 is a LAN match for a client
		if (playlist.GetPlaylistId() != 24) {
			return NULL;
		}

		return server;
	}
	if (gameWrapper->IsInGame()) {
		auto server = gameWrapper->GetGameEventAsServer();

		if (server.IsNull()) {
			cvarManager->log("null server");
			return NULL;
		}

		return server;
	}
	//cvarManager->log("no server");
	return NULL;
}

std::string CustomRumble1::GetPluginName() {
	return "Custom Rumble";
}

void CustomRumble1::SetImGuiContext(uintptr_t ctx) {
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

void CustomRumble1::RenderSettings() {

	ImGui::TextUnformatted("Blue team items");
	for (int i = 0; i < NumPowerups; i++) {
		std::string powerup = powerUpStrings[i];
		std::string powerupEnglish = powerUpEnglishStrings[i];
		CVarWrapper enablePowerupCvar = cvarManager->getCvar("rumble_enable_blue_" + powerup);

		if (!enablePowerupCvar) {
			return;
		}

		bool enabledPowerup = enablePowerupCvar.getBoolValue();

		/*if (ImGui::Checkbox("Enable plugin", &enabled)) {
			gameWrapper->Execute([this, enableCvar, enabled](...) mutable {
				enableCvar.setValue(enabled);
				});
		}*/

		if (ImGui::Selectable(powerupEnglish.c_str(), &enabledPowerup, ImGuiSelectableFlags_DontClosePopups)) {
			enablePowerupCvar.setValue(std::to_string(enabledPowerup));
		}
	}

	ImGui::TextUnformatted("Orange team items");
	for (int i = 0; i < NumPowerups; i++) {
		std::string powerup = powerUpStrings[i];
		std::string powerupEnglish = powerUpEnglishStrings[i];
		CVarWrapper enablePowerupCvar = cvarManager->getCvar("rumble_enable_orange_" + powerup);

		if (!enablePowerupCvar) {
			return;
		}

		bool enabledPowerup = enablePowerupCvar.getBoolValue();

		/*if (ImGui::Checkbox("Enable plugin", &enabled)) {
			gameWrapper->Execute([this, enableCvar, enabled](...) mutable {
				enableCvar.setValue(enabled);
				});
		}*/

		if (ImGui::Selectable(powerupEnglish.c_str(), &enabledPowerup, ImGuiSelectableFlags_DontClosePopups)) {
			enablePowerupCvar.setValue(std::to_string(enabledPowerup));
		}
	}

	ImGui::Separator();

	ImGui::TextUnformatted("Plugin made by JerryTheBee#1117 - DM me on discord for custom plugin commissions!");
}
