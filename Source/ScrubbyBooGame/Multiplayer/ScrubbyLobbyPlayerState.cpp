// Copyright Epic Games, Inc. All Rights Reserved.

#include "Multiplayer/ScrubbyLobbyPlayerState.h"
#include "Engine/World.h"
#include "Multiplayer/ScrubbyLobbyGameState.h"
#include "Net/UnrealNetwork.h"

AScrubbyLobbyPlayerState::AScrubbyLobbyPlayerState()
{
	bReplicates = true;
}

void AScrubbyLobbyPlayerState::SetLobbyReady(bool bNewReady)
{
	if (!HasAuthority() || bIsLobbyReady == bNewReady)
	{
		return;
	}

	bIsLobbyReady = bNewReady;
	// Listen server does not auto-trigger OnRep_ when mutating locally.
	OnRep_IsLobbyReady();
}

void AScrubbyLobbyPlayerState::SetLobbyHost(bool bNewHost)
{
	if (!HasAuthority() || bIsLobbyHost == bNewHost)
	{
		return;
	}

	bIsLobbyHost = bNewHost;
	OnRep_IsLobbyHost();
}

void AScrubbyLobbyPlayerState::OnRep_IsLobbyReady()
{
	OnLobbyReadyChanged.Broadcast(this, bIsLobbyReady);
	BP_OnLobbyReadyChanged(bIsLobbyReady);
	NotifyGameStateLobbyChanged();
}

void AScrubbyLobbyPlayerState::OnRep_IsLobbyHost()
{
	NotifyGameStateLobbyChanged();
}

void AScrubbyLobbyPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();
	NotifyGameStateLobbyChanged();
}

void AScrubbyLobbyPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	if (AScrubbyLobbyPlayerState* TargetPS = Cast<AScrubbyLobbyPlayerState>(PlayerState))
	{
		TargetPS->bIsLobbyReady = bIsLobbyReady;
		TargetPS->bIsLobbyHost = bIsLobbyHost;
	}
}

void AScrubbyLobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AScrubbyLobbyPlayerState, bIsLobbyReady);
	DOREPLIFETIME(AScrubbyLobbyPlayerState, bIsLobbyHost);
}

void AScrubbyLobbyPlayerState::NotifyGameStateLobbyChanged() const
{
	if (const UWorld* World = GetWorld())
	{
		if (AScrubbyLobbyGameState* LobbyGS = World->GetGameState<AScrubbyLobbyGameState>())
		{
			LobbyGS->NotifyLobbyPlayersChanged();
		}
	}
}
