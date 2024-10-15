#pragma once

#define TRACE_LENGTH 80000.f


#define CUSTOM_DEPTH_PINK 245
#define CUSTOM_DEPTH_ORANGE 246
#define CUSTOM_DEPTH_YELLOW 247
#define CUSTOM_DEPTH_GREEN 248
#define CUSTOM_DEPTH_RED 249
#define CUSTOM_DEPTH_PURPLE 250
#define CUSTOM_DEPTH_BLUE 251
#define CUSTOM_DEPTH_TAN 252

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AssaultRifle UMETA(DisplayName = "Assault Rifle"),
	EWT_RocketLauncher UMETA(DisplayName = "Rocket Launcher"),
	EWT_Pistol UMETA(DisplayName = "Pistol"),
	EWT_SubmachineGun UMETA(DisplayName = "Submachine Gun"),
	EWT_Shotgun UMETA(DisplayName = "Shotgun"),
	EWT_SniperRifle UMETA(DisplayName = "Sniper Rifle"),
	EWT_GrenadeLauncher UMETA(DisplayName = "Grenade Launcher"),
	EWT_Grenade UMETA(DisplayName = "Grenade"),
	EWT_Flag UMETA(DisplayName = "Flag"),

	EWT_MAX UMETA(DisplayName = "DefaultMAX")
};