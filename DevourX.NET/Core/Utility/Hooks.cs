using HarmonyLib;
using Il2Cpp;
using MelonLoader;
using UnityEngine;

namespace DevourX.NET.Core.Utility
{
    public class Hooks
    {
        [HarmonyPatch(typeof(Il2Cpp.NolanBehaviour), "OnAttributeUpdateValue")]
        static class NolanBehaviour_UV
        {
            [HarmonyPrefix]
            static void Prefix(ref Il2CppOpsive.UltimateCharacterController.Traits.Attribute attribute)
            {
                if (Settings.Settings.unlimitedUVLight && attribute.m_Name == "Battery")
                {
                    attribute.m_Value = 100.0f;
                    return;
                }
            }
        }

        [HarmonyPatch(typeof(LockedInteractable), "CanInteract")]
        static class LockedInteractable_CanInteract_Patch
        {
            [HarmonyPrefix]
            static bool Prefix(LockedInteractable __instance, GameObject character, ref bool __result)
            {
                if (Settings.Settings.unlockDoors)
                {
                    __instance.isLocked = false;
                    __result = true;
                    return false;
                }

                return true;
            }
        }


        [HarmonyPatch(typeof(DevourInput), "GetLongPress")]
        static class DevourInput_GetLongPress_Patch
        {
            [HarmonyPrefix]
            static bool Prefix(DevourInput __instance, String name, ref float duration, bool waitForRelease)
            {
                if (Settings.Settings.disableLongInteract)
                {
                    duration = 0.0f;
                }

                return true;
            }
        }

        [HarmonyPatch(typeof(Il2CppHorror.Menu), "SetupOutfit")]
        static class Menu_SetupOutfit_Patch
        {
            [HarmonyPrefix]
            static bool Prefix(Il2CppHorror.Menu __instance, CharacterOutfit outfit)
            {
                if (outfit != null)
                {
                    if (!outfit.isOwned)
                    {
                        UnlockOutfit(outfit);
                    }
                }
                return true;
            }

            static void UnlockOutfit(CharacterOutfit outfit)
            {
                outfit.isOwned = true;
                outfit.isSupporter = true;
            }
        }


        [HarmonyPatch(typeof(Il2Cpp.SurvivalLobbyController), "OnSelectOutfit")]
        static class SurvivalLobbyController_OnSelectOutfit_Patch
        {
            [HarmonyPrefix]
            static bool Prefix(Il2Cpp.SurvivalLobbyController __instance, CharacterOutfit outfit)
            {
                if (outfit != null)
                {
                    if (!outfit.isOwned)
                    {
                        UnlockOutfit(outfit);
                    }
                }
                return true;
            }

            static void UnlockOutfit(CharacterOutfit outfit)
            {
                MelonLogger.Warning($"[lobby] Unlocking outfit: {outfit.addressableName}");

                outfit.isOwned = true;
                outfit.isSupporter = true;
            }
        }

        [HarmonyPatch(typeof(Il2Cpp.SurvivalLobbyController), "LobbyIsFull")]
        static class SurvivalLobbyController_LobbyIsFull_Patch
        {
            [HarmonyPrefix]
            static bool Prefix(Il2Cpp.SurvivalLobbyController __instance, ref bool __result)
            {
                __result = false;
                return false;
            }
        }

        [HarmonyPatch(typeof(NolanBehaviour), "Update")]
        static class NolanBehaviour_Update_Patch
        {
            [HarmonyPrefix]
            static bool Prefix(NolanBehaviour __instance)
            {
                if (__instance.entity.IsOwner)
                {
                    if (Settings.Settings.fullbright)
                    {
                        Misc.EnableFullBright(__instance);
                    }

                    if (Settings.Settings.fastMovement)
                    {
                        __instance.speedChangeAbility.m_MaxSpeedChangeValue = Settings.Settings.fastMovementMultiplier;
                        __instance.speedChangeAbility.m_SpeedChangeMultiplier = Settings.Settings.fastMovementMultiplier;
                    }

                    if (Settings.Settings.fly)
                    {
                        Misc.Fly(__instance, Settings.Settings.flyMultiplier);
                    }
                }

                return true;
            }
        }

        [HarmonyPatch(typeof(Il2Cpp.RankHelpers))]
        [HarmonyPatch(nameof(Il2Cpp.RankHelpers.CalculateExpGain))]
        static class RankHelpers_CalculateExpGain
        {
            static void Postfix(ref Il2Cpp.RankHelpers.ExpGainInfo __result)
            {
                if (Settings.Settings.expModifier)
                {
                    __result.totalExp = Settings.Settings.expModifierValue;
                }

                return;
            }
        }


        [HarmonyPatch(typeof(Il2Cpp.RankHelpers))]
        [HarmonyPatch(nameof(Il2Cpp.RankHelpers.GetRankName))]
        static class RankHelpers_GetRankName
        {
            static bool Prefix(ref string __result, int rank)
            {
                if (Settings.Settings.expModifier && rank >= 1)
                {
                    __result = "<color=#66ffcc>✦ Signal Injected by Jadis0x ✦</color>";
                    return false;
                }

                return true;
            }
        }
    }
}
