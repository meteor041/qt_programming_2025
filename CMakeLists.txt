cmake_minimum_required(VERSION 3.28)
project(qt_programming_2024)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(qt_programming_2024 src/main.cpp
        src/MyGame.cpp
        src/MyGame.h
        src/Items/Item.cpp
        src/Items/Item.h
        src/Items/Characters/Character.cpp
        src/Items/Characters/Character.h
        src/Items/Characters/Link.cpp
        src/Items/Characters/Link.h
        src/Items/Armors/Armor.cpp
        src/Items/Armors/Armor.h
        src/Items/HeadEquipments/HeadEquipment.cpp
        src/Items/HeadEquipments/HeadEquipment.h
        src/Items/LegEquipments/LegEquipment.cpp
        src/Items/LegEquipments/LegEquipment.h
        src/Items/HeadEquipments/CapOfTheHero.cpp
        src/Items/HeadEquipments/CapOfTheHero.h
        src/Items/Armors/OldShirt.cpp
        src/Items/Armors/OldShirt.h
        src/Items/LegEquipments/WellWornTrousers.cpp
        src/Items/LegEquipments/WellWornTrousers.h
        src/Scenes/Scene.cpp
        src/Scenes/Scene.h
        src/Scenes/BattleScene.cpp
        src/Scenes/BattleScene.h
        res/res.qrc
        src/Items/Maps/Map.cpp
        src/Items/Maps/Map.h
        src/Items/Maps/Battlefield.cpp
        src/Items/Maps/Battlefield.h
        src/Items/Mountable.cpp
        src/Items/Mountable.h
        src/Items/Armors/FlamebreakerArmor.cpp
        src/Items/Armors/FlamebreakerArmor.h
        src/Items/Maps/platform/Platform.cpp 
        src/Items/Maps/platform/Platform.h
        src/Items/Maps/platform/EarthPlatform.h
        src/Items/Maps/platform/EarthPlatform.cpp
        src/Items/Maps/platform/GrassPlatform.h
        src/Items/Maps/platform/GrassPlatform.cpp
        src/Items/Maps/platform/IcePlatform.h
        src/Items/Maps/platform/IcePlatform.cpp
        src/Items/Weapon/Weapon.cpp
        src/Items/Weapon/Weapon.h
        src/Items/Weapon/Fist.cpp
        src/Items/Weapon/Fist.h
        src/Items/Weapon/Knife.cpp
        src/Items/Weapon/Knife.h
        src/Items/Weapon/ShotPut.cpp
        src/Items/Weapon/ShotPut.h
        src/Items/Consumables/Consumable.cpp
        src/Items/Consumables/Consumable.h
        src/Items/Consumables/Bandage.cpp
        src/Items/Consumables/Bandage.h
        src/Items/Consumables/Medkit.cpp
        src/Items/Consumables/Medkit.h
        src/Items/Consumables/Adrenaline.cpp
        src/Items/Consumables/Adrenaline.h

)
target_link_libraries(qt_programming_2024
        Qt::Core
        Qt::Gui
        Qt::Widgets
)
