using UnityEngine;

namespace DevourX.NET.Core.Settings
{
    public class Settings
    {
        public static bool bShowMenu { get; set; } = false;
        public static KeyCode bMenuToggleKey { get; set; } = KeyCode.Q;
        public static readonly float menuWidthPercentage = 0.4f;  // Menu width is 30% of the screen width
        public static readonly float menuHeightPercentage = 0.8f; // Menu height is 80% of the screen height

        #region MENU_FEATURES
        public static bool unlimitedUVLight = false;
        public static bool unlockDoors = false;
        public static bool enableMovement = false;
        public static bool disableLongInteract = false;
        public static bool fullbright = false;

        public static bool fastMovement = false;
        public static float fastMovementMultiplier = 1.1f;

        public static bool fly = false;
        public static float flyMultiplier = 1.5f;

        public static bool expModifier = false;
        public static int expModifierValue = 1000; // max. 3k
        #endregion
    }
}
