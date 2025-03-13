using DevourX.NET.Core.Utility;
using Il2Cpp;
using Il2CppPhoton.Bolt;
using Il2CppUdpKit.Platform.Photon;
using MelonLoader;
using UnityEngine;
using static Il2Cpp.IngameStatsTracker;

namespace DevourX.NET.Core
{
    public class MelonMain : MonoBehaviour
    {
        public MelonMain(IntPtr ptr)
            : base(ptr)
        {
        }

        #region MENU
        private string[] tabs = { "Settings", "Gameplay", "Object Pool", "Players", "Stats Manager" };
        private int selectedTab = 0;

        private string menuToggleKeyInput;
        private string feedbackMessage = "";

        private Vector2 playerTABScrollPosition;
        private float statsMultiplier = 1f;
        #endregion

        #region COMPONENTS
        static Il2Cpp.GameUI _GameUI;
        #endregion

        private List<ObjectPool> objectPools;
        private ObjectPool? selectedPool = null; // SELECTED PREFAB
        private Vector2 poolScrollPosition;

        private int lobbyLimitInput = 4;
        private string itemKey = "";
        private PhotonRegion.Regions selectedRegion = PhotonRegion.Regions.BEST_REGION; // default region
        private bool bestRegionChecked = true; // default selected
        private bool euChecked = false;
        private bool usChecked = false;
        private bool asiaChecked = false;

        public void Start()
        {
            MelonLogger.Warning("Made with by Jadis0x");
            MelonLogger.Warning("Github : https://github.com/jadis0x/DevourX");

            _GameUI = UnityEngine.Object.FindObjectOfType<Il2Cpp.GameUI>();

            InitializeObjectPools();
        }

        private void InitializeObjectPools()
        {
            objectPools = new List<ObjectPool>()
            {
                new ObjectPool(BoltPrefabs.Crow, "Crow (Bird)"),
                new ObjectPool(BoltPrefabs.Cage, "Cage (Animal Cage)"),
                new ObjectPool(BoltPrefabs.TV, "TV (Television)"),
                new ObjectPool(BoltPrefabs.AzazelApril, "Azazel April (Character)"),
                new ObjectPool(BoltPrefabs.AzazelNathan, "Azazel Nathan (Character)"),
                new ObjectPool(BoltPrefabs.AzazelSam, "Azazel Sam (Character)"),
                new ObjectPool(BoltPrefabs.AzazelZara, "Azazel Zara (Character)"),
                new ObjectPool(BoltPrefabs.SurvivalAzazelMolly, "Survival Azazel Molly (Character)"),
                new ObjectPool(BoltPrefabs.Animal_Gate, "Animal Gate"),
                new ObjectPool(BoltPrefabs.SurvivalBone, "Bone (Survival Item)"),
                new ObjectPool(BoltPrefabs.SurvivalBleach, "Bleach (Survival Item)"),
                new ObjectPool(BoltPrefabs.SurvivalBattery, "Battery (Survival Item)"),
                new ObjectPool(BoltPrefabs.SurvivalPig, "Pig (Animal)"),
                new ObjectPool(BoltPrefabs.AsylumDoor, "Asylum Door"),
                new ObjectPool(BoltPrefabs.Spider, "Spider (Creature)"),
                new ObjectPool(BoltPrefabs.SurvivalAltar, "Altar (Survival Item)"),
                new ObjectPool(BoltPrefabs.SurvivalAltarMolly, "Molly's Altar (Survival Item)"),
                new ObjectPool(BoltPrefabs.SurvivalCake, "Cake (Survival Item)"),
                new ObjectPool(BoltPrefabs.SurvivalFirstAid, "First Aid Kit"),
                new ObjectPool(BoltPrefabs.SurvivalFuse, "Fuse (Survival Item)"),
                new ObjectPool(BoltPrefabs.TrashCan, "Trash Can"),
                new ObjectPool(BoltPrefabs.TownPentagram, "Pentagram (Town)"),
                new ObjectPool(BoltPrefabs.SurvivalSpade, "Spade (Survival Item)"),
                new ObjectPool(BoltPrefabs.SurvivalRottenFood, "Rotten Food (Survival Item)"),
                new ObjectPool(BoltPrefabs.Elevator_Door, "Elevator Door"),
                new ObjectPool(BoltPrefabs.DoorNumber, "Door with Number"),
                new ObjectPool(BoltPrefabs.DevourDoorMain, "Devour Main Door"),
                new ObjectPool(BoltPrefabs.DevourDoorBack, "Devour Back Door"),
                new ObjectPool(BoltPrefabs.Corpse, "Corpse (Dead Body)"),
                new ObjectPool(BoltPrefabs.Freezer_Room, "Freezer Room"),
                new ObjectPool(BoltPrefabs.Ghost, "Ghost (Spirit)"),
                new ObjectPool(BoltPrefabs.InnDoor, "Inn Door"),
                new ObjectPool(BoltPrefabs.ManorGate, "Manor Gate"),
                new ObjectPool(BoltPrefabs.ManorMausoleumDoor, "Mausoleum Door"),
                new ObjectPool(BoltPrefabs.ManorShrine, "Shrine (Manor)"),
                new ObjectPool(BoltPrefabs.SurvivalGoat, "Goat"),
                new ObjectPool(BoltPrefabs.SurvivalHay, "Hay"),
                new ObjectPool(BoltPrefabs.SurvivalMatchbox, "Matchbox"),
                new ObjectPool(BoltPrefabs.SurvivalMatchbox_02, "Matchbox 2"),
                new ObjectPool(BoltPrefabs.SurvivalRitualBook, "Ritual Book"),
                new ObjectPool(BoltPrefabs.SurvivalRose, "Rose"),
                new ObjectPool(BoltPrefabs.SurvivalRat, "Rat"),
                new ObjectPool(BoltPrefabs.InnMaze_B, "InnMaze_B"),
                new ObjectPool(BoltPrefabs.InnFountain, "Inn Fountain")
            };

            MelonLogger.Msg($"Initialized {objectPools.Count} Object Pools.");
        }

        public void Update()
        {
            if (UnityEngine.Input.GetKeyDown(Settings.Settings.bMenuToggleKey))
            {
                Settings.Settings.bShowMenu = !Settings.Settings.bShowMenu;

                if (_GameUI == null)
                {
                    _GameUI = UnityEngine.Object.FindObjectOfType<Il2Cpp.GameUI>();
                }
                else
                {
                    if (Settings.Settings.bShowMenu)
                    {
                        _GameUI.ShowMouseCursor();
                    }
                    else
                    {
                        _GameUI.HideMouseCursor();
                    }
                }
            }
        }

        public void OnGUI()
        {
            if (!Settings.Settings.bShowMenu) return;

            float menuWidth = Screen.width * Settings.Settings.menuWidthPercentage;
            float menuHeight = Screen.height * Settings.Settings.menuHeightPercentage;
            float menuX = (Screen.width - menuWidth) / 2;
            float menuY = (Screen.height - menuHeight) / 2;


            GUILayout.BeginArea(new Rect(menuX, menuY, menuWidth, menuHeight), GUI.skin.box);
            GUILayout.Label("<color=#fff>DevourX.NET by Jadis0x - v4.0.1 [Preview]</color>");

            GUILayout.BeginHorizontal();
            for (int i = 0; i < tabs.Length; i++)
            {
                if (i == selectedTab)
                {
                    if (GUILayout.Button(tabs[i], GUILayout.Height(30)))
                    {
                        selectedTab = i;
                    }
                }
                else
                {
                    if (GUILayout.Button(tabs[i], GUILayout.Height(30)))
                    {
                        selectedTab = i;
                    }
                }
            }
            GUILayout.EndHorizontal();

            GUILayout.Space(10);


            switch (selectedTab)
            {
                case 0:
                    ShowSettingsTab();
                    break;

                case 1:
                    ShowGameplayTab();
                    break;

                case 2:
                    ShowObjectPoolTab();
                    break;

                case 3:
                    ShowPlayersTab();
                    break;

                case 4:
                    ShowStatsManagerTab();
                    break;
            }

            GUILayout.EndArea();
        }

        private void ShowPlayersTab()
        {
            List<GameObject> players = PlayerManager.Instance.GetAllPlayers();

            GUILayout.Label($"<color=#fff>Players ({players.Count.ToString()})</color>", new GUIStyle { richText = true });
            GUILayout.Space(5);

            playerTABScrollPosition = GUILayout.BeginScrollView(playerTABScrollPosition);

            GUILayout.Label("<color=#53ff79>Item Key:</color>");
            itemKey = GUILayout.TextField(itemKey, GUILayout.Width(150));

            if (GUILayout.Button("Carry Item From Key"))
            {
                Misc.CarryObject(itemKey);
            }

            if (BoltNetwork.IsServer)
            {
                if (GUILayout.Button("[FORCE] Attach pet to players"))
                {
                    Misc.AttachPetToPlayers("PetCat", players);
                }
            }

            if (players.Count == 0)
            {
                GUILayout.Label("No players found.");
            }
            else
            {
                foreach (var player in players)
                {
                    if (player != null)
                    {
                        if (GameHelper.GetActiveSceneName() == "Menu")
                        {
                            GUILayout.Label($"Name: {player.name}");
                        }
                        else
                        {
                            Il2Cpp.DissonancePlayerTracking dpt = player.GetComponent<Il2Cpp.DissonancePlayerTracking>();

                            if (dpt != null)
                            {
                                GUILayout.Label($"Name: <color=#7cd5d5>{dpt.state.PlayerName}</color> | Carry Object Name: <color=#7cd5d5>{dpt.state.CarryObjectName}</color> | Moving: <color=#7cd5d5>{dpt.state.Moving}</color> | Poison: <color=#7cd5d5>{dpt.state.Poison}</color>");
                            }
                            else
                            {
                                GUILayout.Label($"Name: {player.name}");
                            }


                            // Butonlar
                            GUILayout.BeginHorizontal(); // Butonları yatay bir satıra koymak için
                            if (GUILayout.Button("Kill", GUILayout.Width(50)))
                            {
                                Misc.KillPlayer(player);
                            }
                            if (GUILayout.Button("Revive", GUILayout.Width(60)))
                            {
                                Misc.RevivePlayer(player);
                            }
                            if (GUILayout.Button("Jumpscare", GUILayout.Width(80)))
                            {
                                Misc.JumpscarePlayer(player);
                            }
                            if (GUILayout.Button("Cage", GUILayout.Width(50)))
                            {
                                Misc.CagePlayer(player);
                            }
                            if (GUILayout.Button("TP to player", GUILayout.Width(95)))
                            {
                                Misc.TeleportPlayer(player);
                            }
                            if (GUILayout.Button("TP azazel to player", GUILayout.Width(130)))
                            {
                                Misc.TeleportAzazel(player);
                            }
                            GUILayout.EndHorizontal();
                        }

                        GUILayout.Space(10);
                    }
                }
            }

            GUILayout.EndScrollView();
        }

        private void ShowObjectPoolTab()
        {
            GUILayout.Label("<color=#788686>Object Pool</color>");
            GUILayout.Space(5);

            poolScrollPosition = GUILayout.BeginScrollView(poolScrollPosition, GUILayout.Height(300));
            foreach (var pool in objectPools)
            {
                if (GUILayout.Button(pool.DisplayName, GUILayout.Width(350)))
                {
                    selectedPool = pool;
                    Debug.Log($"Selected: {pool.DisplayName}");
                }
            }
            GUILayout.EndScrollView();

            GUILayout.Space(20);

            if (selectedPool.HasValue)
            {
                GUILayout.Label($"<color=white>Selected Prefab: </color> <color=orange>{selectedPool.Value.DisplayName}</color>");
                if (GUILayout.Button("Instantiate", GUILayout.Height(40)))
                {
                    if (GameHelper.IsLobbyOwner())
                    {
                        Misc.SpawnPrefab(selectedPool.Value.Prefab, 3f);
                    }
                    else
                    {
                        GUILayout.Label("<color=red>You're not lobby owner!</color>");
                    }
                }
            }
        }

        private void ShowStatsManagerTab()
        {
            GUILayout.Label("<color=#788686>Stats Manager</color>");
            GUILayout.Space(5);

            IngameStatsTracker StatsTracker = IngameStatsTracker.singleton;

            string mapName = GameHelper.GetActiveSceneName();
            if (mapName == "Menu" && StatsTracker == null)
            {
                GUILayout.Label("<color=#788686>The game has not started.</color>");
                return;
            }
            else
            {
                IPlayerState state = GameHelper.GetLocalPlayerState();

                if (state == null)
                {
                    GUILayout.Label("<color=#788686>The game has not started.</color>");
                    return;
                }
                else
                {
                    int localPlayerConnectionId = state.ConnectionId;
                    foreach (StatsPlayer playerStat in StatsTracker.playerStats)
                    {
                        if (playerStat.playerInfo.connectionId == localPlayerConnectionId)
                        {
                            int revives = playerStat.stats.revives;
                            int deaths = playerStat.stats.deaths;
                            int staggers = playerStat.stats.staggers;

                            GUILayout.Label("Current State");
                            GUILayout.Label($"Revives: {revives}");
                            GUILayout.Label($"Deaths: {deaths}");
                            GUILayout.Label($"Staggers: {staggers}");

                            GUILayout.Space(10);
                        }
                    }
                }
            }
        }

        private void ShowSettingsTab()
        {
            GUILayout.Label("<color=#788686>General Information</color>");
            GUILayout.Label($"FPS: <color=#56da7b>{1.0f / Time.deltaTime:F2}</color>");

            GUILayout.Space(5);

            GUILayout.Label($"Total Memory: <color=#56da7b>{System.GC.GetTotalMemory(false) / 1024 / 1024} MB</color>");

            GUILayout.Space(10);

            GUILayout.Label("<color=#788686>Settings</color>");

            GUILayout.Space(10);

            // show current toggle key
            GUILayout.Label($"<b>Menu Toggle Key (Current):</b> <color=#e8ff91>{Settings.Settings.bMenuToggleKey.ToString()}</color>");

            menuToggleKeyInput = GUILayout.TextField(menuToggleKeyInput ?? Settings.Settings.bMenuToggleKey.ToString().ToUpper());

            if (GUILayout.Button("Set Menu Toggle Key"))
            {
                if (System.Enum.TryParse(menuToggleKeyInput, out KeyCode newKey))
                {
                    Settings.Settings.bMenuToggleKey = newKey;
                    feedbackMessage = $"<color=#56da7b>Menu toggle key updated to: {newKey.ToString().ToUpper()}</color>";
                }
                else
                {
                    feedbackMessage = "<color=#cb5b5f>Invalid KeyCode entered. Try again!</color>";
                }
            }

            if (!string.IsNullOrEmpty(feedbackMessage))
            {
                GUILayout.Label(feedbackMessage, new GUIStyle
                {
                    richText = true,
                    fontSize = 14
                });
            }
        }

        private void ShowGameplayTab()
        {
            GUILayout.Label("<color=#788686>Gameplay</color>");

            GUILayout.Space(10);

            if (GUILayout.Button("Force Lobby Start"))
            {
                Utility.Misc.ForceLobbyStart();
            }

           

            GUILayout.Space(20);
            GUILayout.BeginVertical(GUILayout.Width(250));

            GUILayout.Label("<color=#53ff79>Connection Limit:</color>");
            string input = GUILayout.TextField(lobbyLimitInput.ToString(), GUILayout.Width(90));

            GUILayout.Space(10);
            if (int.TryParse(input, out int parsedLimit))
            {
                lobbyLimitInput = parsedLimit;
            }

            GUILayout.Label("<color=#53ff79>Select Region:</color>");
            GUILayout.BeginVertical();

            // checkbox control
            if (GUILayout.Toggle(bestRegionChecked, "Best Region") != bestRegionChecked)
            {
                bestRegionChecked = !bestRegionChecked;
                if (bestRegionChecked)
                {
                    selectedRegion = PhotonRegion.Regions.BEST_REGION;
                    euChecked = usChecked = asiaChecked = false; // reset others
                }
            }

            if (GUILayout.Toggle(euChecked, "EU") != euChecked)
            {
                euChecked = !euChecked;
                if (euChecked)
                {
                    selectedRegion = PhotonRegion.Regions.EU;
                    bestRegionChecked = usChecked = asiaChecked = false; // reset others
                }
            }

            if (GUILayout.Toggle(usChecked, "US") != usChecked)
            {
                usChecked = !usChecked;
                if (usChecked)
                {
                    selectedRegion = PhotonRegion.Regions.US;
                    bestRegionChecked = euChecked = asiaChecked = false; // reset others
                }
            }

            if (GUILayout.Toggle(asiaChecked, "Asia") != asiaChecked)
            {
                asiaChecked = !asiaChecked;
                if (asiaChecked)
                {
                    selectedRegion = PhotonRegion.Regions.ASIA;
                    bestRegionChecked = euChecked = usChecked = false; // reset others
                }
            }

            GUILayout.EndVertical();

            // Custom lobby oluşturma butonu
            if (GUILayout.Button("Create Customized Lobby", GUILayout.Width(190)))
            {
                Utility.Misc.CreateCustomizedLobby(selectedRegion, lobbyLimitInput);
                MelonLogger.Warning($"Custom lobby created! Region: {selectedRegion}, Limit: {lobbyLimitInput}");
            }

            GUILayout.EndVertical();
            GUILayout.Space(20);

            if (GUILayout.Button($"Enable Movement: <color={(Settings.Settings.enableMovement ? "green" : "#cb5b5f")}>{Settings.Settings.enableMovement}</color>"))
            {
                Settings.Settings.enableMovement = !Settings.Settings.enableMovement;

                Utility.Misc.EnableMovement(Settings.Settings.enableMovement);
            }

            if (GUILayout.Button("Burn one"))
            {
                Utility.Misc.InstantWin(GameHelper.GetMapName(GameHelper.GetActiveSceneName()), false);
            }

            if (GUILayout.Button("Burn all (Instant Win)"))
            {
                Utility.Misc.InstantWin(GameHelper.GetMapName(GameHelper.GetActiveSceneName()), true);
            }

            GUILayout.Space(20);

            Settings.Settings.unlimitedUVLight = GUILayout.Toggle(Settings.Settings.unlimitedUVLight, "Unlimited UV Light");
            Settings.Settings.unlockDoors = GUILayout.Toggle(Settings.Settings.unlockDoors, "Unlock Doors");
            Settings.Settings.fullbright = GUILayout.Toggle(Settings.Settings.fullbright, "Fullbright");
            Settings.Settings.disableLongInteract = GUILayout.Toggle(Settings.Settings.disableLongInteract, "Disable Long Interact");

            GUILayout.Space(15);

            Settings.Settings.fastMovement = GUILayout.Toggle(Settings.Settings.fastMovement, $"Fast Movement: <color={(Settings.Settings.fastMovement ? "green" : "#cb5b5f")}>{Settings.Settings.fastMovement}</color>");
            GUILayout.Label($"<b>Movement Speed Multiplier:</b> <color=#fff>{Settings.Settings.fastMovementMultiplier:F0}</color>");
            Settings.Settings.fastMovementMultiplier = GUILayout.HorizontalSlider(Settings.Settings.fastMovementMultiplier, 1.1f, 15f, GUILayout.Width(300));

            GUILayout.Space(15);

            Settings.Settings.fly = GUILayout.Toggle(Settings.Settings.fly, $"Fly: <color={(Settings.Settings.fly ? "green" : "#cb5b5f")}>{Settings.Settings.fly}</color>");
            GUILayout.Label($"<b>Fly Speed Multiplier:</b> <color=#fff>{Settings.Settings.flyMultiplier:F0}</color>");
            Settings.Settings.flyMultiplier = GUILayout.HorizontalSlider(Settings.Settings.flyMultiplier, 1.5f, 25f, GUILayout.Width(300));
        }
    }
}
