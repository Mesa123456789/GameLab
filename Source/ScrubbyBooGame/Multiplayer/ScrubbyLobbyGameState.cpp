// Copyright Epic Games, Inc. All Rights Reserved.

#include "Multiplayer/ScrubbyLobbyGameState.h"
#include "Multiplayer/ScrubbyLobbyPlayerState.h"
#include "Net/UnrealNetwork.h"

AScrubbyLobbyGameState::AScrubbyLobbyGameState()
{
	bReplicates = true;
}

void AScrubbyLobbyGameState::ConfigureLobbyInfo(const FString& InRoomName, int32 InMaxPlayers, bool bInAllowSoloHostStart)
{
	if (!HasAuthority())
	{
		return;
	}

	LobbyRoomName = InRoomName.IsEmpty() ? TEXT("ScrubbyBoo Lobby") : InRoomName;
	MaxLobbyPlayers = FMath::Clamp(InMaxPlayers, 1, 16);
	bAllowSoloHostStart = bInAllowSoloHostStart;
	OnRep_LobbyInfoChanged();
}

void AScrubbyLobbyGameState::SetMatchStarting(bool bInStarting)
{
	if (!HasAuthority() || bMatchStarting == bInStarting)
	{
		return;
	}

	bMatchStarting = bInStarting;
	OnRep_MatchStarting();
}

void AScrubbyLobbyGameState::NotifyLobbyPlayersChanged()
{
	OnLobbyPlayersChanged.Broadcast();
}

TArray<FScrubbyLobbyPlayerInfo> AScrubbyLobbyGameState::GetLobbyPlayers() const
{
	TArray<FScrubbyLobbyPlayerInfo> Result;
	Result.Reserve(PlayerArray.Num());

	for (APlayerState* PS : PlayerArray)
	{
		if (!IsValid(PS))
		{
			continue;
		}

		FScrubbyLobbyPlayerInfo Info;
		Info.PlayerId = PS->GetPlayerId();
		Info.PlayerName = PS->GetPlayerName();

		if (const AScrubbyLobbyPlayerState* LobbyPS = Cast<AScrubbyLobbyPlayerState>(PS))
		{
			Info.bIsReady = LobbyPS->IsLobbyReady();
			Info.bIsHost = LobbyPS->IsLobbyHost();
		}

		Result.Add(Info);
	}

	Result.Sort([](const FScrubbyLobbyPlayerInfo& A, const FScrubbyLobbyPlayerInfo& B)
	{
		if (A.bIsHost != B.bIsHost)
		{
			return A.bIsHost;
		}
		return A.PlayerId < B.PlayerId;
	});

	return Result;
}

int32 AScrubbyLobbyGameState::GetReadyPlayerCount() const
{
	int32 ReadyCount = 0;
	for (APlayerState* PS : PlayerArray)
	{
		if (const AScrubbyLobbyPlayerState* LobbyPS = Cast<AScrubbyLobbyPlayerState>(PS))
		{
			if (LobbyPS->IsLobbyReady())
			{
				++ReadyCount;
			}
		}
	}
	return ReadyCount;
}

int32 AScrubbyLobbyGameState::GetTotalPlayerCount() const
{
	return PlayerArray.Num();
}

bool AScrubbyLobbyGameState::AreAllPlayersReady() const
{
	if (PlayerArray.Num() == 0)
	{
		return false;
	}

	for (APlayerState* PS : PlayerArray)
	{
		const AScrubbyLobbyPlayerState* LobbyPS = Cast<AScrubbyLobbyPlayerState>(PS);
		if (!LobbyPS || !LobbyPS->IsLobbyReady())
		{
			return false;
		}
	}

	return true;
}

bool AScrubbyLobbyGameState::CanHostStartMatch() const
{
	if (bMatchStarting || PlayerArray.Num() <= 0)
	{
		return false;
	}

	if (PlayerArray.Num() == 1 && bAllowSoloHostStart)
	{
		return true;
	}

	return AreAllPlayersReady();
}

void AScrubbyLobbyGameState::OnRep_LobbyInfoChanged()
{
	OnLobbyInfoChanged.Broadcast();
}

void AScrubbyLobbyGameState::OnRep_MatchStarting()
{
	OnMatchStarting.Broadcast();
}

void AScrubbyLobbyGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
	NotifyLobbyPlayersChanged();
}

void AScrubbyLobbyGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
	NotifyLobbyPlayersChanged();
}

void AScrubbyLobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AScrubbyLobbyGameState, LobbyRoomName);
	DOREPLIFETIME(AScrubbyLobbyGameState, MaxLobbyPlayers);
	DOREPLIFETIME(AScrubbyLobbyGameState, bAllowSoloHostStart);
	DOREPLIFETIME(AScrubbyLobbyGameState, bMatchStarting);
}
