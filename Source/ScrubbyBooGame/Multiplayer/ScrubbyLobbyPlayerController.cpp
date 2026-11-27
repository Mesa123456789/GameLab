// Copyright Epic Games, Inc. All Rights Reserved.

#include "Multiplayer/ScrubbyLobbyPlayerController.h"
#include "Engine/World.h"
#include "Multiplayer/ScrubbyLobbyGameMode.h"
#include "Multiplayer/ScrubbyLobbyGameState.h"
#include "Multiplayer/ScrubbyLobbyPlayerState.h"
#include "ScrubbyBooGame.h"

void AScrubbyLobbyPlayerController::RequestSetLobbyReady(bool bReady)
{
	if (!IsLocalController())
	{
		return;
	}

	Server_SetLobbyReady(bReady);
}

void AScrubbyLobbyPlayerController::RequestToggleLobbyReady()
{
	if (!IsLocalController())
	{
		return;
	}

	bool bNextReady = true;
	if (const AScrubbyLobbyPlayerState* LobbyPS = GetPlayerState<AScrubbyLobbyPlayerState>())
	{
		bNextReady = !LobbyPS->IsLobbyReady();
	}

	Server_SetLobbyReady(bNextReady);
}

void AScrubbyLobbyPlayerController::RequestStartMatch(const FString& OverrideMatchMapPath)
{
	if (!IsLocalController())
	{
		return;
	}

	Server_RequestStartMatch(OverrideMatchMapPath);
}

bool AScrubbyLobbyPlayerController::IsLocalLobbyHost() const
{
	if (const AScrubbyLobbyPlayerState* LobbyPS = GetPlayerState<AScrubbyLobbyPlayerState>())
	{
		return LobbyPS->IsLobbyHost();
	}
	return HasAuthority() && IsLocalController();
}

void AScrubbyLobbyPlayerController::Server_SetLobbyReady_Implementation(bool bReady)
{
	const UWorld* World = GetWorld();
	if (const AScrubbyLobbyGameState* LobbyGS = World ? World->GetGameState<AScrubbyLobbyGameState>() : nullptr)
	{
		if (LobbyGS->IsMatchStarting())
		{
			return;
		}
	}

	if (AScrubbyLobbyPlayerState* LobbyPS = GetPlayerState<AScrubbyLobbyPlayerState>())
	{
		LobbyPS->SetLobbyReady(bReady);
	}
}

void AScrubbyLobbyPlayerController::Server_RequestStartMatch_Implementation(const FString& OverrideMatchMapPath)
{
	const AScrubbyLobbyPlayerState* LobbyPS = GetPlayerState<AScrubbyLobbyPlayerState>();
	const bool bIsHost = (LobbyPS && LobbyPS->IsLobbyHost()) || IsLocalController();
	if (!bIsHost)
	{
		UE_LOG(LogScrubbyBooGame, Warning, TEXT("Non-host player %s attempted to start match."), *GetName());
		return;
	}

	if (AScrubbyLobbyGameMode* LobbyGM = GetWorld() ? GetWorld()->GetAuthGameMode<AScrubbyLobbyGameMode>() : nullptr)
	{
		LobbyGM->TryStartMatch(OverrideMatchMapPath);
	}
}
