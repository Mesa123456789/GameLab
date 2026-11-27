// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ScrubbyLobbyPlayerState.generated.h"

class AScrubbyLobbyPlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScrubbyLobbyReadyChanged, AScrubbyLobbyPlayerState*, PlayerState, bool, bNewReady);

/**
 * Replicated player state used inside the walkable 3D lobby to sync Ready and Host status.
 */
UCLASS(Blueprintable)
class SCRUBBYBOOGAME_API AScrubbyLobbyPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AScrubbyLobbyPlayerState();

	/** Fires whenever this player's Ready flag changes (on both server and clients). */
	UPROPERTY(BlueprintAssignable, Category = "ScrubbyBoo|Lobby")
	FOnScrubbyLobbyReadyChanged OnLobbyReadyChanged;

	/** Server-only setter for whether this player is ready to start the match. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "ScrubbyBoo|Lobby")
	void SetLobbyReady(bool bNewReady);

	/** Server-only setter marking whether this player is the listen-server host. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "ScrubbyBoo|Lobby")
	void SetLobbyHost(bool bNewHost);

	/** Returns true when this player has marked themselves Ready in the lobby. */
	UFUNCTION(BlueprintPure, Category = "ScrubbyBoo|Lobby")
	bool IsLobbyReady() const { return bIsLobbyReady; }

	/** Returns true when this player is the listen-server host. */
	UFUNCTION(BlueprintPure, Category = "ScrubbyBoo|Lobby")
	bool IsLobbyHost() const { return bIsLobbyHost; }

protected:
	/** True when the player has confirmed Ready in the lobby. */
	UPROPERTY(ReplicatedUsing = OnRep_IsLobbyReady, BlueprintReadOnly, Category = "ScrubbyBoo|Lobby")
	bool bIsLobbyReady = false;

	/** True when this player is the listen-server host. */
	UPROPERTY(ReplicatedUsing = OnRep_IsLobbyHost, BlueprintReadOnly, Category = "ScrubbyBoo|Lobby")
	bool bIsLobbyHost = false;

	/** Blueprint cosmetic hook triggered on both server and clients when Ready state changes. */
	UFUNCTION(BlueprintImplementableEvent, Category = "ScrubbyBoo|Lobby")
	void BP_OnLobbyReadyChanged(bool bNewReady);

	UFUNCTION()
	void OnRep_IsLobbyReady();

	UFUNCTION()
	void OnRep_IsLobbyHost();

	virtual void OnRep_PlayerName() override;
	virtual void CopyProperties(APlayerState* PlayerState) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void NotifyGameStateLobbyChanged() const;
};
