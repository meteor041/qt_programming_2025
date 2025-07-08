# CMakeLists.txt

cmake_minimum_required(VERSION 3.28)
project(qt_programming_2024)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 寻找Qt6的必要模块
find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

# [FIXED] 添加所有必需的源文件(.cpp)和资源文件(.qrc)
# 头文件(.h)会自动被CMake处理，不需要写在这里
add_executable(qt_programming_2024
        # --- 核心与入口 ---
        src/main.cpp
        src/MyGame.cpp

        # --- 场景 ---
        src/Scenes/Scene.cpp
        src/Scenes/BattleScene.cpp

        # --- 地图与平台 ---
        src/Items/Maps/Map.cpp
        src/Items/Maps/Battlefield.cpp
        src/Items/Maps/platform/Platform.cpp
        src/Items/Maps/platform/Earthplatform.cpp
        src/Items/Maps/platform/GrassPlatform.cpp
        src/Items/Maps/platform/IcePlatform.cpp

        # --- 基础物品与角色 ---
        src/Items/Item.cpp
        src/Items/Mountable.cpp
        src/Items/Characters/Character.cpp
        src/Items/Characters/Link.cpp

        # --- 装备 ---
        src/Items/HeadEquipments/HeadEquipment.cpp
        src/Items/HeadEquipments/CapOfTheHero.cpp
        src/Items/Armors/Armor.cpp
        src/Items/Armors/OldShirt.cpp
        src/Items/Armors/FlamebreakerArmor.cpp
        src/Items/LegEquipments/LegEquipment.cpp
        src/Items/LegEquipments/WellWornTrousers.cpp

        # --- 资源文件 ---
        res/res.qrc
)

# 链接Qt的库
target_link_libraries(qt_programming_2024 PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
)
