#include "pch.h"
#include "CustomRumble1.h"

BAKKESMOD_PLUGIN(CustomRumble1, "Custom rumble", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

std::list<int> enabledBlue;
std::list<int> enabledOrange;


void CustomRumble1::onLoad()
{
    _globalCvarManager = cvarManager;

    std::random_device dev;
    std::mt19937 rng(dev());

    RandomDevice = std::make_shared<std::mt19937>(rng);

    GObjects = reinterpret_cast<TArray<UObject*>*>(Memory::FindPattern(GetModuleHandleA(nullptr), GObjects_Pattern, GObjects_Mask));
    GNames = reinterpret_cast<TArray<FNameEntry*>*>(Memory::FindPattern(GetModuleHandleA(nullptr), GNames_Pattern, GNames_Mask));

    if (!GObjects || !GNames) {
        cvarManager->log("(onLoad) Error: RLSDK classes are wrong, please contact JerryTheBee");
        cvarManager->log(std::to_string(!GObjects) + ", " + std::to_string(!GNames));
        ClassesSafe = false;
        return;
    }

    if (AreGObjectsValid() && AreGNamesValid()) {
        ClassesSafe = true;
        gameWrapper->HookEventWithCaller<ActorWrapper>("Function TAGame.ItemPoolCycle_TA.GiveItem",
            [this](ActorWrapper caller, void* params, std::string eventname) {
                onGiveItem(caller, params);
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
                    enabledBlue.push_front(i);
                } else {
                    enabledBlue.remove(i);
                }
                });

        cvarManager->registerCvar("rumble_enable_orange_" + powerup, "0", "Enables " + powerupEnglish + " for orange", true, true, 0, true, 1)
            .addOnValueChanged([this, i](std::string, CVarWrapper cvar) {
                if (cvar.getBoolValue()) {
                    enabledOrange.push_front(i);
                } else {
                    enabledOrange.remove(i);
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
    //UGameData_TA* gameData = Memory::GetDefaultInstanceOf<UGameData_TA>();
    ClassesSafe = true;
    return ClassesSafe;
}

ServerWrapper CustomRumble1::getSW() {
    if (gameWrapper->IsInOnlineGame()) {
        return NULL;
    }
    if (gameWrapper->IsInGame()) {
        auto server = gameWrapper->GetGameEventAsServer();

        if (server.IsNull()) {
            //cvarManager->log("null server");
            return NULL;
        }

        return server;
    }
    //cvarManager->log("no server");
    return NULL;
}

void CustomRumble1::onGiveItem(ActorWrapper caller, void* params) {
    if (params == nullptr || &caller == nullptr) {
        return;
    }

    if (!getSW()) return;

    UItemPoolCycle_TA_execGiveItem_Params* paramValues = (UItemPoolCycle_TA_execGiveItem_Params*)params;

    UItemPoolCycle_TA* itemPool = (UItemPoolCycle_TA*)caller.memory_address;

    if (itemPool == nullptr || paramValues == nullptr) {
        return;
    }

    CarWrapper wrappedCar((uintptr_t)paramValues->Car);

    if (!wrappedCar) {
        //cvarManager->log("wrapped car broke");
        return;
    }

    std::string nextPowerup = generateNextPower(wrappedCar.GetTeamNum2());

    //cvarManager->log("generated powerup " + nextPowerup);

    TArray<FRandomWeight> items = itemPool->Items;

    for (FRandomWeight& itemtype : items) {
        UObject* object = itemtype.Obj;

        if (object == nullptr) {
            //cvarManager->log("null object");
            continue;
        }

        //cvarManager->log("object name = " + object->Name.ToString());
        //cvarManager->log("object wt   = " + std::to_string(itemtype.Weight));

        if (object->Name.ToString() == nextPowerup) {
            itemtype.Weight = 2.0;

        }
        else {
            itemtype.Weight = 0.0;
        }
    }
    
    itemPool->RefillPool();

    //itemPool->GiveItem(paramValues->Car);
}

std::string CustomRumble1::generateNextPower(int teamNum) {
    std::list<int> powerupsList;

    //cvarManager->log("team = " + std::to_string(teamNum));

    if (teamNum == 0) {
        powerupsList = enabledBlue;
    } else {
        powerupsList = enabledOrange;
    }

    /*for (int powerup : powerupsList) {
        cvarManager->log("option " + std::to_string(powerup) + " " + powerUpStrings[powerup]);
    }*/

    if (powerupsList.size() == 0) {
        return "";
    }

    std::uniform_int_distribution<std::mt19937::result_type> dist(0, powerupsList.size() - 1);

    int result = dist(*RandomDevice.get());

    //cvarManager->log("choices list index " + std::to_string(result));

    int i = 0;
    int resultIndex = 0;
    for (int powerup : powerupsList) {
        if (i > result) {
            //cvarManager->log("couldn't find powerup");
            return "";
        }

        //cvarManager->log("comparing i: " + std::to_string(i) + " to result: " + std::to_string(result));

        if (i == result) {
            resultIndex = powerup;
            //cvarManager->log("powerup list index " + std::to_string(resultIndex));

            return powerUpStrings[resultIndex];
        }
        
        i++;
    }
    
    return "";
}

std::string CustomRumble1::GetPluginName() {
    return "Custom Rumble";
}

void CustomRumble1::SetImGuiContext(uintptr_t ctx) {
    ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

void CustomRumble1::RenderSettings() {
    ImGui::Columns(2);
    ImGui::TextUnformatted("Blue team items");
    ImGui::NextColumn();
    ImGui::TextUnformatted("Orange team items");
    ImGui::Columns(1);
    if (!ClassesSafe) {
        ImGui::TextUnformatted("Something went wrong. Try reloading the plugin or rebooting the game");
    }
    ImGui::Separator();
    ImGui::Columns(2);

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

    ImGui::NextColumn();
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

        std::string label = powerupEnglish + "##orange";

        if (ImGui::Selectable(label.c_str(), &enabledPowerup, ImGuiSelectableFlags_DontClosePopups)) {
            enablePowerupCvar.setValue(std::to_string(enabledPowerup));
        }
    }

    ImGui::Columns(1);

    ImGui::Separator();

    ImGui::TextUnformatted("Plugin commissioned by Lethamyr");
    ImGui::TextUnformatted("Plugin made by JerryTheBee#1117 - DM me on discord for custom plugin commissions!");
}
