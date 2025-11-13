#pragma once

#include <string>
#include <vector>

namespace SceneExplorer {
        struct FieldEntry {
                std::string name;
                std::string typeName;
                std::string value;
                std::string modifiers;
        };

        struct ComponentEntry {
                std::string typeName;
                std::vector<FieldEntry> fields;
        };

        struct GameObjectEntry {
                std::string name;
                std::vector<ComponentEntry> components;
                std::vector<GameObjectEntry> children;
        };

        struct Snapshot {
                std::string sceneName;
                std::string timestamp;
                std::vector<GameObjectEntry> roots;
                bool valid = false;
        };

        Snapshot CaptureActiveScene();
        void LogSnapshot(const Snapshot& snapshot);
}
