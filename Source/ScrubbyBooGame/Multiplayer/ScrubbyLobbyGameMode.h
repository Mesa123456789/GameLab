// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/ScrubbyBooGameGameMode.h"
#include "TimerManager.h"
#include "ScrubbyLobbyGameMode.generated.h"

class AController;
class APlayerController;

/**
 * Server-authoritative GameMode for the walkable 3D Lobby.
 * Enforces lobby capacity, blocks late-joiners once the match starts, and coordinates ServerTravel.
 */
UCLASS(Blueprintable)
class SCRUBBYBOOGAME_API AScrubbyLobbyGameMode : public AScrubbyBooGameGameMode
{
	GENERATED_BODY()

public:
	AScrubbyLobbyGameMode();

	/** Server-only attempt to lock the lobby session and ServerTravel all players into the match map. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "ScrubbyBoo|Lobby")
	bool TryStartMatch(const FString& OverrideMatchMapPath = TEXT(""));

protected:
	/** Default match map package path used when the host starts the game. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ScrubbyBoo|Lobby")
	FString DefaultMatchMapPath = TEXT("/Game/ScrubbyBoo/Maps/Lvl_Main");

	/** Fallback max player limit when opened directly in PIE without CreateSession. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ScrubbyBoo|Lobby", meta = (ClampMin = "1", ClampMax = "16"))
	int32 MaxLobbyPlayers = 6;

	/** Allows a solo listen-server host to start immediately for fast testing. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ScrubbyBoo|Lobby")
	bool bAllowSoloHostStart = true;

	/** Enables Seamless Travel when transitioning from the lobby into the match map. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ScrubbyBoo|Lobby")
	bool bUseSeamlessTravelForMatch = true;

	/** Brief delay before ServerTravel so clients receive OnMatchStarting UI cues cleanly. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ScrubbyBoo|Lobby", meta = (ClampMin = "0.0", ClampMax = "10.0"))
	float StartTravelDelaySeconds = 0.5f;

	/** Blueprint hook fired on the server when a player joins the lobby. */
	UFUNCTION(BlueprintImplementableEvent, Category = "ScrubbyBoo|Lobby")
	void BP_OnPlayerJoinedLobby(APlayerController* NewPlayer);

	/** Blueprint hook fired on the server when a player leaves the lobby. */
	UFUNCTION(BlueprintImplementableEvent, Category = "ScrubbyBoo|Lobby")
	void BP_OnPlayerLeftLobby(AController* Exiting);

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

private:
	void ExecuteServerTravel();
	int32 ResolveEffectiveMaxPlayers() const;

	FString PendingTravelMapPath;
	FTimerHandle StartMatchTimerHandle;
};
