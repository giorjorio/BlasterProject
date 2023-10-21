#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"


class APlayerController;
class UAnnouncement;
class UCharacterOverlay;
class UElimAnnouncement;
class UTexture2D;
class UUSerwidget;




USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()

public:

	UTexture2D* CrosshairsCenter;
	UTexture2D* CrosshairsLeft;
	UTexture2D* CrosshairsRight;
	UTexture2D* CrosshairsTop;
	UTexture2D* CrosshairsBottom;
	float CrosshairSpread;
	FLinearColor CrosshairColor;
	
};

UCLASS()
class BLASTER_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<UUserWidget> CharacterOverlayClass;

	UPROPERTY()
	UCharacterOverlay* CharacterOverlay;

	UPROPERTY(EditAnywhere, Category = "Announcements")
	TSubclassOf<UUserWidget> AnnouncementClass;

	UPROPERTY()
	UAnnouncement* Announcement;

	void AddAnnouncement();
	void AddCharacterOverlay();
	void AddElimAnnouncement(FString Attacker, FString Victim);


protected:
	virtual void BeginPlay() override;
	
private:
	FHUDPackage HUDPackage;

	UPROPERTY()
	APlayerController* OwningPlayer;

	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor);

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;

	UPROPERTY(EditAnywhere, Category = "Announcements")
	TSubclassOf<UElimAnnouncement> ElimAnnouncementClass;
	
	UPROPERTY(EditAnywhere, Category = "Announcements")
	float ElimMsgTime = 1.5f;

	UFUNCTION()
	void ElimAnnouncementTimerFinished(UElimAnnouncement* MsgToRemove);

	UPROPERTY()
	TArray<UElimAnnouncement*> ElimMessages;

public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; }


};
