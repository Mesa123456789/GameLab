// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/ScrubbyBooGamePlayerController.h"
#include "ScrubbyLobbyPlayerController.generated.h"

/**
 * Walkable 3D lobby PlayerController extending the First Person controller with Ready-Up and Host Start RPCs.
 */
UCLASS(Blueprintable)
class SCRUBBYBOOGAME_API AScrubbyLobbyPlayerController : public AScrubbyBooGamePlayerController
{
	GENERATED_BODY()

public:
	/** Client/UMG call to set this player's Ready state in the lobby. */
	UFUNCTION(BlueprintCallable, Category = "ScrubbyBoo|Lobby")
	void RequestSetLobbyReady(bool bReady);

	/** Client/UMG call to toggle this player's current Ready state. */
	UFUNCTION(BlueprintCallable, Category = "ScrubbyBoo|Lobby")
	void RequestToggleLobbyReady();

	/** Host-only UI call to lock the session and ServerTravel everyone into the match map. */
	UFUNCTION(BlueprintCallable, Category = "ScrubbyBoo|Lobby")
	void RequestStartMatch(const FString& OverrideMatchMapPath = TEXT(""));

	/** Returns true if this local controller belongs to the Listen Server lobby host. */
	UFUNCTION(BlueprintPure, Category = "ScrubbyBoo|Lobby")
	bool IsLocalLobbyHost() const;

protected:
	/** Server RPC updating this player's replicated Ready flag on PlayerState. */
	UFUNCTION(Server, Reliable)
	void Server_SetLobbyReady(bool bReady);

	/** Server RPC requesting match start (validated for Lobby Host authority). */
	UFUNCTION(Server, Reliable)
	void Server_RequestStartMatch(const FString& OverrideMatchMapPath);
};
