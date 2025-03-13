using DevourX.NET.Core;
using Il2CppInterop.Runtime.Injection;
using UnityEngine;

[assembly: MelonLoader.VerifyLoaderVersion(0, 6, 0, true)] // Minimum MelonLoader version is V6.0.0, sanity check for people who use 5.7 and wonder why it crashes :)
[assembly: MelonLoader.MelonInfo(typeof(DevourX.NET.Init), "DevourX", "4", "Jadis0x")]
[assembly: MelonLoader.MelonGame("Straight Back Games", "DEVOUR")]

namespace DevourX.NET
{
    public class Init: MelonLoader.MelonMod
    {
        public static MelonMain MelonMainInstance { get; private set; } = default!;
        public static GameObject DevourX_GameObject { get; private set; } = default!;
        public static void InitializeMod()
        {
            ClassInjector.RegisterTypeInIl2Cpp<Core.MelonMain>();

            DevourX_GameObject = new GameObject("DevourX");
            UnityEngine.Object.DontDestroyOnLoad(DevourX_GameObject);
            DevourX_GameObject.hideFlags |= HideFlags.HideAndDontSave;

            MelonMainInstance = DevourX_GameObject.AddComponent<MelonMain>();
        }

        public override void OnInitializeMelon()
        {
            InitializeMod();
        }
    }
}
