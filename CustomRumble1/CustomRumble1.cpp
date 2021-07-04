
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
		cvarManager->log("(onLoad) Error: RLSDK classes are wrong, please contact ItsBranK if he doesn't already know!");
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