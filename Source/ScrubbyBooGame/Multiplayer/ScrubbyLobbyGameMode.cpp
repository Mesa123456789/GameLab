// Copyright Epic Games, Inc. All Rights Reserved.

#include "Multiplayer/ScrubbyLobbyGameMode.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "Multiplayer/ScrubbyLobbyGameState.h"
#include "Multiplayer/ScrubbyLobbyPlayerController.h"
#include "Multiplayer/ScrubbyLobbyPlayerState.h"
#include "Multiplayer/ScrubbySessionSubsystem.h"
#include "ScrubbyBooGame.h"

AScrubbyLobbyGameMode::AScrubbyLobbyGameMode()
{
	GameStateClass = AScrubbyLobbyGameState::StaticClass();
	PlayerStateClass = AScrubbyLobbyPlayerState::StaticClass();
	PlayerControllerClass = AScrubbyLobbyPlayerController::StaticClass();
	bUseSeamlessTravel = true;
}

void AScrubbyLobbyGameMode::BeginPlay()
{
	Super::BeginPlay();

	bUseSeamlessTravel = bUseSeamlessTravelForMatch;

	FString RoomName = TEXT("ScrubbyBoo Lobby");
	const int32 EffectiveMax = ResolveEffectiveMaxPlayers();

	if (const UGameInstance* GI = GetGameInstance())
	{
		if (const UScrubbySessionSubsystem* SessionSub = GI->GetSubsystem<UScrubbySessionSubsystem>())
		{
			if (!SessionSub->GetActiveRoomName().IsEmpty())
			{
				RoomName = SessionSub->GetActiveRoomName();
			}
		}
	}

	if (AScrubbyLobbyGameState* LobbyGS = GetGameState<AScrubbyLobbyGameState>())
	{
		LobbyGS->ConfigureLobbyInfo(RoomName, EffectiveMax, bAllowSoloHostStart);
	}
}

void AScrubbyLobbyGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(StartMatchTimerHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void AScrubbyLobbyGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	if (!ErrorMessage.IsEmpty())
	{
		return;
	}

	if (const AScrubbyLobbyGameState* LobbyGS = GetGameState<AScrubbyLobbyGameState>())
	{
		if (LobbyGS->IsMatchStarting())
		{
			ErrorMessage = TEXT("Match has already started.");
			return;
		}
	}

	if (GetNumPlayers() >= ResolveEffectiveMaxPlayers())
	{
		ErrorMessage = TEXT("Lobby is full.");
	}
}

void AScrubbyLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (NewPlayer)
	{
		if (AScrubbyLobbyPlayerState* LobbyPS = NewPlayer->GetPlayerState<AScrubbyLobbyPlayerState>())
		{
			if (NewPlayer->IsLocalController())
			{
				LobbyPS->SetLobbyHost(true);
			}
		}
	}

	if (AScrubbyLobbyGameState* LobbyGS = GetGameState<AScrubbyLobbyGameState>())
	{
		LobbyGS->NotifyLobbyPlayersChanged();
	}

	BP_OnPlayerJoinedLobby(NewPlayer);
}

void AScrubbyLobbyGameMode::Logout(AController* Exiting)
{
	BP_OnPlayerLeftLobby(Exiting);

	Super::Logout(Exiting);

	if (AScrubbyLobbyGameState* LobbyGS = GetGameState<AScrubbyLobbyGameState>())
	{
		LobbyGS->NotifyLobbyPlayersChanged();
	}
}

bool AScrubbyLobbyGameMode::TryStartMatch(const FString& OverrideMatchMapPath)
{
	if (!HasAuthority())
	{
		return false;
	}

	AScrubbyLobbyGameState* LobbyGS = GetGameState<AScrubbyLobbyGameState>();
	if (!LobbyGS || !LobbyGS->CanHostStartMatch())
	{
		UE_LOG(LogScrubbyBooGame, Log, TEXT("TryStartMatch declined: not all players are ready or match already starting."));
		return false;
	}

	FString TargetMap = OverrideMatchMapPath.TrimStartAndEnd().IsEmpty() ? DefaultMatchMapPath : OverrideMatchMapPath.TrimStartAndEnd();
	FString CleanMap;
	if (TargetMap.Split(TEXT("?"), &CleanMap, nullptr))
	{
		TargetMap = CleanMap;
	}

	if (TargetMap.IsEmpty())
	{
		UE_LOG(LogScrubbyBooGame, Warning, TEXT("TryStartMatch failed: DefaultMatchMapPath is empty."));
		return false;
	}

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UScrubbySessionSubsystem* SessionSub = GI->GetSubsystem<UScrubbySessionSubsystem>())
		{
			SessionSub->LockSessionForMatchStart();
		}
	}

	LobbyGS->SetMatchStarting(true);
	PendingTravelMapPath = TargetMap;

	UWorld* World = GetWorld();
	if (StartTravelDelaySeconds > 0.0f && World)
	{
		World->GetTimerManager().SetTimer(
			StartMatchTimerHandle,
			this,
			&AScrubbyLobbyGameMode::ExecuteServerTravel,
			StartTravelDelaySeconds,
			false);
	}
	else
	{
		ExecuteServerTravel();
	}

	return true;
}

void AScrubbyLobbyGameMode::ExecuteServerTravel()
{
	if (!HasAuthority() || PendingTravelMapPath.IsEmpty())
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		UE_LOG(LogScrubbyBooGame, Log, TEXT("Lobby starting ServerTravel to: %s"), *PendingTravelMapPath);
		World->ServerTravel(PendingTravelMapPath, false);
	}
}

int32 AScrubbyLobbyGameMode::ResolveEffectiveMaxPlayers() const
{
	if (const UGameInstance* GI = GetGameInstance())
	{
		if (const UScrubbySessionSubsystem* SessionSub = GI->GetSubsystem<UScrubbySessionSubsystem>())
		{
			if (SessionSub->IsInActiveSession() && SessionSub->GetActiveMaxPlayers() > 0)
			{
				return FMath::Clamp(SessionSub->GetActiveMaxPlayers(), 1, 16);
			}
		}
	}

	return FMath::Clamp(MaxLobbyPlayers, 1, 16);
}
