using Il2Cpp;
using Il2CppOpsive.UltimateCharacterController.Character;
using Il2CppPhoton.Bolt;
using Il2CppUdpKit.Platform.Photon;
using MelonLoader;
using UnityEngine;


namespace DevourX.NET.Core.Utility
{
    public class Misc
    {
        public static void ForceLobbyStart()
        {
            if (!GameHelper.IsLobbyOwner()) return;

            Il2CppHorror.Menu HorrorMenu = UnityEngine.Object.FindObjectOfType<Il2CppHorror.Menu>();
            if (HorrorMenu == null) return;

            HorrorMenu.OnLobbyStartButtonClick();
        }

        public static void EnableMovement(bool _enabled)
        {
            UnityEngine.GameObject LocalPlayer = PlayerManager.Instance.GetLocalPlayer();

            if (LocalPlayer == null) return;

            Il2CppOpsive.UltimateCharacterController.Character.UltimateCharacterLocomotionHandler LocomotionHandler = LocalPlayer.GetComponent<UltimateCharacterLocomotionHandler>();

            if (LocomotionHandler == null)
            {
                LocalPlayer.AddComponent<UltimateCharacterLocomotionHandler>();
            }
            else
            {
                LocomotionHandler.enabled = _enabled;
            }
        }

        public static void EnableFullBright(NolanBehaviour playerNolan)
        {
            if (playerNolan == null) return;

            Light light = playerNolan.flashlightSpot;

            if (light == null) return;

            light.intensity = 1.3f;
            light.spotAngle = 290f;
            light.range = 370f;

            light.shadows = LightShadows.None;
        }

        public static void Fly(NolanBehaviour playerNolanBehaviour, float speed)
        {
            Vector3 playerPosition = playerNolanBehaviour.transform.position;

            RewiredHelpers _RewiredHelpers = UnityEngine.Object.FindObjectOfType<RewiredHelpers>();
            if (_RewiredHelpers == null) return;

            if (Input.GetKey((KeyCode)System.Enum.Parse(typeof(KeyCode), _RewiredHelpers.GetCurrentBinding("Move Up").ToString().Replace(" ", ""))))
            {
                playerPosition += playerNolanBehaviour.transform.forward * speed * Time.deltaTime;
            }
            if (Input.GetKey((KeyCode)System.Enum.Parse(typeof(KeyCode), _RewiredHelpers.GetCurrentBinding("Move Down").ToString().Replace(" ", ""))))
            {
                playerPosition += -playerNolanBehaviour.transform.forward * speed * Time.deltaTime;
            }
            if (Input.GetKey((KeyCode)System.Enum.Parse(typeof(KeyCode), _RewiredHelpers.GetCurrentBinding("Move Right").ToString().Replace(" ", ""))))
            {
                playerPosition += playerNolanBehaviour.transform.right * speed * Time.deltaTime;
            }
            if (Input.GetKey((KeyCode)System.Enum.Parse(typeof(KeyCode), _RewiredHelpers.GetCurrentBinding("Move Left").ToString().Replace(" ", ""))))
            {
                playerPosition += -playerNolanBehaviour.transform.right * speed * Time.deltaTime;
            }
            if (Input.GetKey(KeyCode.Space))
            {
                playerPosition += playerNolanBehaviour.transform.up * speed * Time.deltaTime;
            }
            if (Input.GetKey(KeyCode.LeftControl))
            {
                playerPosition += -playerNolanBehaviour.transform.up * speed * Time.deltaTime;
            }

            playerNolanBehaviour.locomotion.SetPosition(playerPosition, false);
        }

        public static void KillPlayer(GameObject player)
        {
            /*if (player == null) return;

            BoltEntity component = player.GetComponent<BoltEntity>();
            IPlayerState state = component.GetState<IPlayerState>();

            state.Poison = 10000;
            */

            NolanBehaviour component = player.GetComponent<NolanBehaviour>();

            BoltEntity component2 = component.gameObject.GetComponent<BoltEntity>();
            KnockoutBoxEvent obj = KnockoutBoxEvent.Create();
            obj.KilledBy = player.name;
            obj.Player = component2;
            obj.Send();
        }

        public static void RevivePlayer(GameObject player)
        {
            BoltEntity cachedComponent = player.GetComponent<BoltEntity>();

            IPlayerState state = cachedComponent.GetState<IPlayerState>();

            BoltEntity player2 = cachedComponent;
            ReviveEvent obj = ReviveEvent.Create();
            obj.Player = player2;
            obj.Reviver = cachedComponent;
            obj.Send();

            // local only
            //Il2CppOpsive.Shared.Events.EventHandler.ExecuteEvent("OnResetGame");
        }

        public static void JumpscarePlayer(GameObject player)
        {
            if (!GameHelper.IsLobbyOwner())
            {
                MelonLogger.Error("You're not lobby owner!");
                return;
            }

            if (!player) return;

            Il2Cpp.SurvivalAzazelBehaviour SurvivalAzazelBehaviour = Il2Cpp.SurvivalAzazelBehaviour.FindObjectOfType<Il2Cpp.SurvivalAzazelBehaviour>();

            if (!SurvivalAzazelBehaviour) return;

            SurvivalAzazelBehaviour.OnPickedUpPlayer(SurvivalAzazelBehaviour.gameObject, player, false);
        }

        public static void CagePlayer(GameObject player)
        {
            if (player == null) return;

            BoltNetwork.Instantiate(BoltPrefabs.Cage, player.transform.position, Quaternion.identity);
        }

        public static void TeleportPlayer(GameObject player)
        {
            GameObject localPlayer = PlayerManager.Instance.GetLocalPlayer();

            if (localPlayer == null || player == null) return;

            localPlayer.GetComponent<NolanBehaviour>().TeleportTo(player.transform.position, Quaternion.identity, false);
        }

        public static void TeleportAzazel(GameObject player)
        {
            if (!player) return;

            SurvivalAzazelBehaviour azazel = SurvivalAzazelBehaviour.FindObjectOfType<Il2Cpp.SurvivalAzazelBehaviour>();

            if (!azazel) return;

            UltimateCharacterLocomotion azazelLocomotion = azazel.GetComponent<UltimateCharacterLocomotion>();

            if (!azazelLocomotion) return;

            azazelLocomotion.SetPosition(player.transform.position);
        }

        public static void SpawnPrefab(PrefabId prefab, float spawnDistance)
        {
            GameObject localPlayer = PlayerManager.Instance.GetLocalPlayer();
            if (localPlayer != null)
            {
                SpawnAtTarget(prefab, localPlayer, spawnDistance);
            }
        }

        public static void SpawnPrefab(PrefabId prefab, GameObject target, float spawnDistance)
        {
            if (target != null)
            {
                SpawnAtTarget(prefab, target, spawnDistance);
            }
        }

        private static void SpawnAtTarget(PrefabId prefab, GameObject target, float spawnDistance)
        {
            Vector3 origin = target.transform.position;
            Vector3 forwardDirection = target.transform.forward;

            Vector3 spawnPosition = origin + forwardDirection * spawnDistance;

            BoltNetwork.Instantiate(prefab, spawnPosition, Quaternion.identity);
        }

        public static void CreateCustomizedLobby(PhotonRegion.Regions region, int serverConnectionLimit = 4, bool isPrivate = false, string password = "")
        {
            Il2CppHorror.Menu HorrorMenu = UnityEngine.Object.FindObjectOfType<Il2CppHorror.Menu>();
            if (HorrorMenu == null) return;

            GameObject localPlayer = PlayerManager.Instance.GetLocalPlayer();

            if (!localPlayer)
            {
                PhotonRegion _region = PhotonRegion.GetRegion(region);
                HorrorMenu.SetUDPPlatform(_region);

                BoltConfig __config = HorrorMenu.boltConfig;

                UnityEngine.UI.Toggle __toggle = HorrorMenu.hostPrivateServer;
                HorrorMenu.hostServerPasswordInputField.text = password;

                __toggle.isOn = isPrivate;
                __config.serverConnectionLimit = serverConnectionLimit;

                MelonLogger.Warning("Server created with the password: " + HorrorMenu.hostServerPasswordInputField.text.ToString());

                BoltLauncher.StartServer(__config, null);

                HorrorMenu.ShowCanvasGroup(HorrorMenu.loadingCanvasGroup, true);
                HorrorMenu.ShowCanvasGroup(HorrorMenu.hostCanvasGroup, false);
                HorrorMenu.ShowCanvasGroup(HorrorMenu.mainMenuCanvasGroup, false);
            }
        }

        public static void AttachPetToPlayers(string petName, List<GameObject> players)
        {
            for (int i = 0; i < players.Count; i++)
            {
                if (players[i] == null) return;

                // PetSpider
                // PetCat
                // ..

                PetLoader petLoader = players[i].GetComponent<NolanBehaviour>().petLoader;

                if (petLoader.GetPet() == null)
                {
                    petLoader.LoadPet(petName);
                    petLoader.ResetPet();
                    petLoader.GetPet();
                }
            }
        }

        public static void InstantWin(string map, bool burnAll)
        {
            if (!GameHelper.IsLobbyOwner())
            {
                MelonLogger.Error("You're not lobby owner!");
                return;
            }

            switch (map)
            {
                case "Inn":
                    var innController = UnityEngine.Object.FindObjectOfType<Il2Cpp.InnMapController>();
                    if (!innController) return;

                    if (burnAll)
                        innController.SetProgressTo(10);
                    else
                        innController.IncreaseProgress();
                    break;

                case "Slaughterhouse":
                    var slaughterhouseController = UnityEngine.Object.FindObjectOfType<Il2Cpp.SlaughterhouseAltarController>();
                    if (!slaughterhouseController) return;

                    if (burnAll)
                        slaughterhouseController.BurnGoat();
                    else
                        slaughterhouseController.SkipToGoat(10);
                    break;

                case "Manor":
                    var manorController = UnityEngine.Object.FindObjectOfType<Il2Cpp.MapController>();
                    if (!manorController) return;

                    if (burnAll)
                        manorController.SetProgressTo(10);
                    else
                        manorController.SetProgressTo(manorController.GetMapProgress() + 1);
                    break;

                default:
                    var altar = UnityEngine.Object.FindObjectOfType<Il2Cpp.SurvivalObjectBurnController>();
                    if (!altar) return;

                    if (burnAll)
                        altar.SkipToGoat(10);
                    else
                        altar.BurnGoat();
                    break;
            }
        }

        public static void CarryObject(string key)
        {
            UnityEngine.GameObject LocalPlayer = PlayerManager.Instance.GetLocalPlayer();
            if (!LocalPlayer || GameHelper.GetActiveSceneName() == "Menu") return;

            NolanBehaviour nolanBehaviour = LocalPlayer.GetComponent<NolanBehaviour>();
            if (!nolanBehaviour) return;

            nolanBehaviour.StartCarry(key);

            /*
            {"Hay", "SurvivalHay"},
		    {"First aid", "SurvivalFirstAid"},
		    {"Spade", "SurvivalSpade"},
		    {"Cake", "SurvivalCake"},
		    {"Bone", "SurvivalBone"},
		    {"Battery", "SurvivalBattery"},
		    {"Gasoline", "SurvivalGasoline"},
		    {"Fuse", "SurvivalFuse"},
		    {"Food", "SurvivalRottenFood"},
		    {"Bleach", "SurvivalBleach"},
		    {"Ritual Book (inactive)", "RitualBook-InActive-1"},
		    {"Ritual Book (active)", "RitualBook-Active-1"},
		    {"Matchbox", "Matchbox-3"},
		    {"Egg 1", "Egg-Clean-1"},
		    {"Egg 2", "Egg-Clean-2"},
		    {"Egg 3", "Egg-Clean-3"},
		    {"Egg 4", "Egg-Clean-4"},
		    {"Egg 5", "Egg-Clean-5"},
		    {"Egg 6", "Egg-Clean-6"},
		    {"Egg 7", "Egg-Clean-7"},
		    {"Egg 8", "Egg-Clean-8"},
		    {"Egg 9", "Egg-Clean-9"},
		    {"Egg 10", "Egg-Clean-10"},
		    {"Head (Dirty/Gardener)", "Head-Dirty-Gardener"},
		    {"Head (Clean/Gardener)", "Head-Clean-Gardener"},
		    {"Head (Dirty/Butler)", "Head-Dirty-Butler"},
		    {"Head (Clean/Butler)", "Head-Clean-Butler)"},
		    {"Head (Dirty/Bridesmaid_01)", "Head-Dirty-Bridesmaid_01"},
		    {"Head (Clean/Bridesmaid_01)", "Head-Clean-Bridesmaid_01"},
		    {"Head (Dirty/Bridesmaid_02)", "Head-Dirty-Bridesmaid_02"},
		    {"Head (Clean/Bridesmaid_02)", "Head-Clean-Bridesmaid_02"},
		    {"Head (Dirty/Cook)", "Head-Dirty-Cook"},
		    {"Head (Clean/Cook)", "Head-Clean-Cook"},
		    {"Head (Dirty/Groomsman_01)", "Head-Dirty-Groomsman_01"},
		    {"Head (Clean/Groomsman_01)", "Head-Clean-Groomsman_01"},
		    {"Head (Dirty/Groomsman_02)", "Head-Dirty-Groomsman_02"},
		    {"Head (Clean/Groomsman_02)", "Head-Clean-Groomsman_02"},
		    {"Head (Dirty/Maid)", "Head-Dirty-Maid"},
		    {"Head (Clean/Maid)", "Head-Clean-Maid"},
		    {"Head (Dirty/Photographer)", "Head-Dirty-Photographer"},
		    {"Head (Clean/Photographer)", "Head-Clean-Photographer"},
		    {"Head (Dirty/Priest)", "Head-Dirty-Priest"},
		    {"Head (Clean/Priest)", "Head-Clean-Priest"}
            */
        }
    }
}
