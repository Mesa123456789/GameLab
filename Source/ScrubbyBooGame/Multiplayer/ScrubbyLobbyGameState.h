// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Multiplayer/ScrubbySessionTypes.h"
#include "ScrubbyLobbyGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScrubbyLobbyStateUpdated);

/**
 * Replicated lobby state shared with all connected clients for UMG lobby lists and Ready checks.
 */
UCLASS(Blueprintable)
class SCRUBBYBOOGAME_API AScrubbyLobbyGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AScrubbyLobbyGameState();

	/** Fires whenever players join, leave, rename, or change Ready/Host status. */
	UPROPERTY(BlueprintAssignable, Category = "ScrubbyBoo|Lobby")
	FOnScrubbyLobbyStateUpdated OnLobbyPlayersChanged;

	/** Fires whenever room name, max player count, or solo-start settings replicate. */
	UPROPERTY(BlueprintAssignable, Category = "ScrubbyBoo|Lobby")
	FOnScrubbyLobbyStateUpdated OnLobbyInfoChanged;

	/** Fires on all clients and host when the match starts and travel is imminent. */
	UPROPERTY(BlueprintAssignable, Category = "ScrubbyBoo|Lobby")
	FOnScrubbyLobbyStateUpdated OnMatchStarting;

	/** Server-only setup called by LobbyGameMode to publish room metadata to clients. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "ScrubbyBoo|Lobby")
	void ConfigureLobbyInfo(const FString& InRoomName, int32 InMaxPlayers, bool bInAllowSoloHostStart);

	/** Server-only setter marking that match travel has begun. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "ScrubbyBoo|Lobby")
	void SetMatchStarting(bool bInStarting);

	/** Broadcasts OnLobbyPlayersChanged; called internally when PlayerStates update. */
	void NotifyLobbyPlayersChanged();

	/** Returns a sorted snapshot of all players in the lobby (Host first, then PlayerId). */
	UFUNCTION(BlueprintPure, Category = "ScrubbyBoo|Lobby")
	TArray<FScrubbyLobbyPlayerInfo> GetLobbyPlayers() const;

	/** Returns the number of players currently marked Ready. */
	UFUNCTION(BlueprintPure, Category = "ScrubbyBoo|Lobby")
	int32 GetReadyPlayerCount() const;

	/** Returns the total number of players currently in the lobby. */
	UFUNCTION(BlueprintPure, Category = "ScrubbyBoo|Lobby")
	int32 GetTotalPlayerCount() const;

	/** Returns true when at least one player is present and every joined player is Ready. */
	UFUNCTION(BlueprintPure, Category = "ScrubbyBoo|Lobby")
	bool AreAllPlayersReady() const;

	/** Returns true if the host is currently allowed to start the match. */
	UFUNCTION(BlueprintPure, Category = "ScrubbyBoo|Lobby")
	bool CanHostStartMatch() const;

	/** Returns the advertised room name. */
	UFUNCTION(BlueprintPure, Category = "ScrubbyBoo|Lobby")
	const FString& GetLobbyRoomName() const { return LobbyRoomName; }

	/** Returns the maximum player limit for this lobby. */
	UFUNCTION(BlueprintPure, Category = "ScrubbyBoo|Lobby")
	int32 GetMaxLobbyPlayers() const { return MaxLobbyPlayers; }

	/** Returns true when a solo host is allowed to start without toggling Ready first. */
	UFUNCTION(BlueprintPure, Category = "ScrubbyBoo|Lobby")
	bool IsSoloHostStartAllowed() const { return bAllowSoloHostStart; }

	/** Returns true once match start / ServerTravel has been triggered. */
	UFUNCTION(BlueprintPure, Category = "ScrubbyBoo|Lobby")
	bool IsMatchStarting() const { return bMatchStarting; }

protected:
	/** Room name shown on the lobby HUD. */
	UPROPERTY(ReplicatedUsing = OnRep_LobbyInfoChanged, BlueprintReadOnly, Category = "ScrubbyBoo|Lobby")
	FString LobbyRoomName = TEXT("ScrubbyBoo Lobby");

	/** Maximum player slots for this lobby. */
	UPROPERTY(ReplicatedUsing = OnRep_LobbyInfoChanged, BlueprintReadOnly, Category = "ScrubbyBoo|Lobby")
	int32 MaxLobbyPlayers = 6;

	/** Allows a single player on the listen server to start immediately for quick testing. */
	UPROPERTY(ReplicatedUsing = OnRep_LobbyInfoChanged, BlueprintReadOnly, Category = "ScrubbyBoo|Lobby")
	bool bAllowSoloHostStart = true;

	/** True once the host has initiated match start. */
	UPROPERTY(ReplicatedUsing = OnRep_MatchStarting, BlueprintReadOnly, Category = "ScrubbyBoo|Lobby")
	bool bMatchStarting = false;

	UFUNCTION()
	void OnRep_LobbyInfoChanged();

	UFUNCTION()
	void OnRep_MatchStarting();

	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
