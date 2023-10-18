#include "BlasterPlayerController.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/Components/BlasterComponents/CombatComponent.h"
#include "Blaster/GameMode/BlasterGameMode.h"
#include "Blaster/GameState/BlasterGameState.h"
#include "Blaster/HUD/Announcement.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/HUD/CharacterOverlay.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetBlasterHUD();

	if(HasAuthority())
	{
		BlasterGameMode = BlasterGameMode == nullptr ? Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this)) : BlasterGameMode;
		if (BlasterGameMode)
		{
			if (BlasterGameMode->bHasBeenBeginPlay)
			{
				ServerCheckMatchState();
			}
			else
			{
				BlasterGameMode->OnBlasterGameModeBeginPlay.AddDynamic(this, &ThisClass::OnBlasterGameModeBeginPlay);
			}
		}
	}
	else
	{
		ServerCheckMatchState();
	}
	SetHUDRoundProgressBars();

	InitializeHUD();
}

void ABlasterPlayerController::SetBlasterHUD()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
}

void ABlasterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerController, MatchState);
	DOREPLIFETIME(ABlasterPlayerController, bDisableGameplay);
}

void ABlasterPlayerController::OnBlasterGameModeBeginPlay()
{
	ServerCheckMatchState();
}

void ABlasterPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDTime();
	CheckTimeSync(DeltaTime);
	PollInit();
	CheckPing(DeltaTime);
	
}

void ABlasterPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

void ABlasterPlayerController::CheckPing(float DeltaTime)
{
	HighPingRunningTime += DeltaTime;
	if (HighPingRunningTime > CheckPingFrequency)
	{
		PlayerState = PlayerState == nullptr ? GetPlayerState<APlayerState>() : PlayerState;
		if (PlayerState)
		{
			UE_LOG(LogTemp, Warning, TEXT("Ping: %d"), PlayerState->GetCompressedPing() * 4)
			if (PlayerState->GetCompressedPing() * 4 > HighPingThreshold) // Ping is compressed; it's actually ping divided by 4
			{
				HighPingWarning();
				PingAnimationRunningTime = 0.f;
				ServerReportPingStatus(true);
			}
			else
			{
				ServerReportPingStatus(false);
			}
		}
		HighPingRunningTime = 0.f;
	}
	bool bHighPingAnimationPlaying =
		BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->HighPingAnimation &&
		BlasterHUD->CharacterOverlay->IsAnimationPlaying(BlasterHUD->CharacterOverlay->HighPingAnimation);

	if (bHighPingAnimationPlaying)
	{
		PingAnimationRunningTime += DeltaTime;
		if (PingAnimationRunningTime > HighPingDuration)
		{
			StopHighPingWarning();
		}
	}
}

void ABlasterPlayerController::HighPingWarning()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->HighPingImage &&
		BlasterHUD->CharacterOverlay->HighPingAnimation;

	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->HighPingImage->SetOpacity(1.f);
		BlasterHUD->CharacterOverlay->PlayAnimation(BlasterHUD->CharacterOverlay->HighPingAnimation, 0.f, 5);
	}
}

void ABlasterPlayerController::StopHighPingWarning()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->HighPingImage &&
		BlasterHUD->CharacterOverlay->HighPingAnimation;

	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->HighPingImage->SetOpacity(0.f);
		if (BlasterHUD->CharacterOverlay->IsAnimationPlaying(BlasterHUD->CharacterOverlay->HighPingAnimation))
		{
			BlasterHUD->CharacterOverlay->StopAnimation(BlasterHUD->CharacterOverlay->HighPingAnimation);
		}
	}
}

// Client report to the server whether his ping too high or not
void ABlasterPlayerController::ServerReportPingStatus_Implementation(bool bHighPing)
{
	HighPingDelegate.Broadcast(bHighPing);
}

void ABlasterPlayerController::ServerCheckMatchState_Implementation()
{
	BlasterGameMode = BlasterGameMode == nullptr ? Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this)) : BlasterGameMode;
	if (BlasterGameMode)
	{
		WarmupTime = BlasterGameMode->WarmupTime;
		MatchTime = BlasterGameMode->MatchTime;
		CooldownTime = BlasterGameMode->CooldownTime;
		LevelStartingTime = BlasterGameMode->LevelStartingTime;
		MatchState = BlasterGameMode->GetMatchState();
		ClientJoinMidGame(MatchState, WarmupTime, MatchTime, LevelStartingTime, CooldownTime);
	}
}

void ABlasterPlayerController::ClientJoinMidGame_Implementation(FName StateOfMatch, float Warmup, float Match, float StartingTime, float Cooldown)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch;
	OnMatchStateSet(MatchState);

	if (BlasterHUD && MatchState == MatchState::WaitingToStart)
	{
		BlasterHUD->AddAnnouncement();
	}

}

void ABlasterPlayerController::InitializeHUD()
{
	if (ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(AController::GetPawn()))
	{
		if(!BlasterCharacter->HasAuthority())
		{
			SetHUDHealth(BlasterCharacter->GetHealth(), BlasterCharacter->GetMaxHealth());
			SetHUDShield(BlasterCharacter->GetShield(), BlasterCharacter->GetMaxShield());
			
			if (BlasterCharacter->GetCombat())
			{
				BlasterCharacter->GetCombat()->UpdateCarriedAmmo();
				BlasterCharacter->GetCombat()->UpdatePrimaryWeaponIcon();
				BlasterCharacter->GetCombat()->UpdateWeaponAmmo();
			}
		}
	}
}

void ABlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(InPawn))
	{
		SetHUDRoundProgressBars();
		SetHUDHealth(BlasterCharacter->GetHealth(), BlasterCharacter->GetMaxHealth());
		SetHUDShield(BlasterCharacter->GetShield(), BlasterCharacter->GetMaxShield());
		BlasterCharacter->SetDisableCharacterGameplay(bDisableGameplay);
		if (BlasterCharacter->GetCombat())
		{
			SetHUDGrenades(BlasterCharacter->GetCombat()->GetGrenades());
			BlasterCharacter->GetCombat()->UpdateCarriedAmmo();
			BlasterCharacter->GetCombat()->UpdatePrimaryWeaponIcon();
			BlasterCharacter->GetCombat()->UpdateWeaponAmmo();
		}
	}
}

void ABlasterPlayerController::SetHUDRoundProgressBars()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->RoundHealthBar;

	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->RoundHealthBar->SetProgressBar();		
		BlasterHUD->CharacterOverlay->RoundShieldBar->SetProgressBar();
		BlasterHUD->CharacterOverlay->RoundSpeedBuffBar->SetProgressBar(false);
		BlasterHUD->CharacterOverlay->RoundJumpBuffBar->SetProgressBar(false);
	}
}

void ABlasterPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	
	//bool bHUDValid = BlasterHUD &&
	//	BlasterHUD->CharacterOverlay &&
	//	BlasterHUD->CharacterOverlay->RoundHealthBar;

	if(BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->RoundHealthBar)
	{
		const float HealthPercent = Health / MaxHealth;
		if (!BlasterHUD->CharacterOverlay->IsInViewport())
		{
			BlasterHUD->AddCharacterOverlay();
		}
		BlasterHUD->CharacterOverlay->RoundHealthBar->SetPercentage(HealthPercent);
	}
	//if (bHUDValid)
	//{
	//	const float HealthPercent = Health / MaxHealth;
	//	BlasterHUD->CharacterOverlay->RoundHealthBar->SetPercentage(HealthPercent);
	//}
	
	/*else
	{
	//Instead of this and "PollInit()" there are initialisation in BlasterHUD::BeginPlay()

		bInitializeCharacterOverlay = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}*/
}

void ABlasterPlayerController::SetHUDShield(float Shield, float MaxShield)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		//BlasterHUD->CharacterOverlay->ShieldBar &&
		//BlasterHUD->CharacterOverlay->ShieldText &&
		BlasterHUD->CharacterOverlay->RoundShieldBar;

	if (bHUDValid)
	{
		const float ShieldPercent = Shield / MaxShield;
		//BlasterHUD->CharacterOverlay->ShieldBar->SetPercent(ShieldPercent);
		//FString ShieldText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Shield), FMath::CeilToInt(MaxShield));
		//BlasterHUD->CharacterOverlay->ShieldText->SetText(FText::FromString(ShieldText));
		BlasterHUD->CharacterOverlay->RoundShieldBar->SetPercentage(ShieldPercent);

	}
}

void ABlasterPlayerController::UpdateSpeedBuffBar(float TimeLeft, float BuffTime)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->RoundSpeedBuffBar;

	bool bShow = true;
	const float BuffPercent1 = TimeLeft / BuffTime;

	if (TimeLeft <= 0.05) { bShow = false; }

	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->RoundSpeedBuffBar->SetPercentage(BuffPercent1, bShow);
	}
}

void ABlasterPlayerController::UpdateJumpBuffBar(float TimeLeft, float BuffTime)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->RoundJumpBuffBar;

	bool bShow1 = true;
	const float BuffPercent2 = TimeLeft / BuffTime;

	if (TimeLeft <= 0.05) { bShow1 = false; }

	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->RoundJumpBuffBar->SetPercentage(BuffPercent2, bShow1);
	}
}

void ABlasterPlayerController::SetHUDScore(float Score)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->ScoreAmount;

	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		BlasterHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
	/*else
	{
		bInitializeCharacterOverlay = true;
		HUDScore = Score;
	}*/
}

void ABlasterPlayerController::SetHUDDefeats(int32 Defeats)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->DefeatsAmount;

	if (bHUDValid)
	{
		FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);
		BlasterHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}
	/*else
	{
		bInitializeCharacterOverlay = true;
		HUDDefeats = Defeats;
	}*/
}

void ABlasterPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->WeaponAmmoAmount;

	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		BlasterHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
	else
	{
		bInitializeWeaponAmmo = true;
		HUDWeaponAmmo = Ammo;
	}
}

void ABlasterPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->CarriedAmmoAmount;

	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		BlasterHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void ABlasterPlayerController::SetHUDEquippedWeaponIcon(float Opacity, bool IsSecondary, UTexture2D* WeaponIcon)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->PrimaryWeaponImage &&
		BlasterHUD->CharacterOverlay->SecondaryWeaponImage;

	if (bHUDValid)
	{
		UTexture2D* WeaponTexture = WeaponIcon;
		if(IsSecondary)
		{
			BlasterHUD->CharacterOverlay->SecondaryWeaponImage->SetBrushFromTexture(WeaponTexture);
			BlasterHUD->CharacterOverlay->SecondaryWeaponImage->SetOpacity(Opacity);
		}
		else
		{
			BlasterHUD->CharacterOverlay->PrimaryWeaponImage->SetBrushFromTexture(WeaponTexture);
			BlasterHUD->CharacterOverlay->PrimaryWeaponImage->SetOpacity(Opacity);
		}
	}
}

void ABlasterPlayerController::SetHUDGrenades(int32 Grenades)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->GrenadesText;

	if (bHUDValid)
	{
		FString GrenadesText = FString::Printf(TEXT("%d"), Grenades);
		BlasterHUD->CharacterOverlay->GrenadesText->SetText(FText::FromString(GrenadesText));
	}
}

void ABlasterPlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->MatchCountdownText;

	if (bHUDValid)
	{
		if (CountdownTime < 0.f)
		{
			BlasterHUD->CharacterOverlay->MatchCountdownText->SetText(FText());
			return;
		}

		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;
		
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		BlasterHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}

void ABlasterPlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->Announcement &&
		BlasterHUD->Announcement->WarmupTime;

	if (bHUDValid)
	{
		if (CountdownTime < 0.f)
		{
			BlasterHUD->Announcement->WarmupTime->SetText(FText());
			return;
		}

		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;

		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		BlasterHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
	}
}

void ABlasterPlayerController::SetHUDTime()
{
	float TimeLeft = 0.f;

	if (HasAuthority())
	{
		BlasterGameMode = BlasterGameMode == nullptr ? Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this)) : BlasterGameMode;
		if (BlasterGameMode)
		{
			LevelStartingTime = BlasterGameMode->GetLevelStartingTime();
		}
	}

	if (MatchState == MatchState::WaitingToStart)
	{ 
		TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime; 
	}
	else if (MatchState == MatchState::InProgress)
	{
		TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	}
	else if (MatchState == MatchState::Cooldown)
	{
		TimeLeft = CooldownTime + WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	}
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);

	if (HasAuthority())
	{
		BlasterGameMode = BlasterGameMode == nullptr ? Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this)) : BlasterGameMode;
		if (BlasterGameMode)
		{
			SecondsLeft = FMath::CeilToInt(BlasterGameMode->GetCountdownTime() + LevelStartingTime);
		}
	}

	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		if(MatchState==MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
	}
	CountdownInt = SecondsLeft;
}

void ABlasterPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (BlasterHUD && BlasterHUD->CharacterOverlay)
		{
			CharacterOverlay = BlasterHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				if (bInitializeWeaponAmmo) SetHUDWeaponAmmo(HUDWeaponAmmo);
			}
		}
	}
}

void ABlasterPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void ABlasterPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	SingleTripTime = (0.5f * RoundTripTime);
	float CurrentServerTime = TimeServerReceivedClientRequest + SingleTripTime;
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float ABlasterPlayerController::GetServerTime()
{
	if (HasAuthority()) 
	{ 
		return GetWorld()->GetTimeSeconds(); 
	}
	else 
	{ 
		return GetWorld()->GetTimeSeconds() + ClientServerDelta; 
	}
}

void ABlasterPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void ABlasterPlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCountdown();
	}
}

void ABlasterPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCountdown();
	}
}

void ABlasterPlayerController::HandleMatchHasStarted()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast < ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD)
	{
		if (BlasterHUD->CharacterOverlay && !BlasterHUD->CharacterOverlay->IsInViewport())
		{
			BlasterHUD->AddCharacterOverlay();
		}
		if (BlasterHUD->Announcement)
		{
			BlasterHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ABlasterPlayerController::HandleCountdown()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast < ABlasterHUD>(GetHUD()) : BlasterHUD;
	
	if (BlasterHUD)
	{
		BlasterHUD->CharacterOverlay->RemoveFromParent();

		if (BlasterHUD->Announcement == nullptr)
		{
			BlasterHUD->AddAnnouncement();
		}

		bool bHUDValid = BlasterHUD->Announcement &&
			BlasterHUD->Announcement->AnnouncementText &&
			BlasterHUD->Announcement->InfoText;

		if (bHUDValid)
		{
			if (!BlasterHUD->Announcement->IsInViewport())
			{
				BlasterHUD->Announcement->AddToViewport();
			}
			BlasterHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
			FString AnnouncementText("New Match Starts In:");
			BlasterHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));

			ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
			ABlasterPlayerState* BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
			if (BlasterGameState && BlasterPlayerState)
			{
				TArray<ABlasterPlayerState*> TopPlayers = BlasterGameState->TopScoringPlayers;
				FString InfoTextString;
				if (TopPlayers.Num() == 0)
				{
					InfoTextString = FString("There is no winner.");
				}
				else if (TopPlayers.Num() == 1 && TopPlayers[0] == BlasterPlayerState)
				{
					InfoTextString = FString("You are the winner!");
				}
				else if (TopPlayers.Num() == 1)
				{
					InfoTextString = FString::Printf(TEXT("Winner: \n%s"), *TopPlayers[0]->GetPlayerName());
				}
				else if (TopPlayers.Num() > 1)
				{
					InfoTextString = FString("Players tied for the win:\n");
					for (auto TiedPlayer : TopPlayers)
					{
						InfoTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
					}
				}

				BlasterHUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString));
			}
			
		}
	}
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
	if (BlasterCharacter && BlasterCharacter->GetCombat())
	{
		bDisableGameplay = true;
		BlasterCharacter->bDisableCharacterGameplay = true;
		BlasterCharacter->GetCombat()->FireButtonPressed(false);
	}
}

