// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ScrubbySessionTypes.generated.h"

/**
 * Parameters for creating and hosting a ScrubbyBoo multiplayer lobby session.
 */
USTRUCT(BlueprintType)
struct SCRUBBYBOOGAME_API FScrubbySessionCreateParams
{
	GENERATED_BODY()

	/** Display name shown in the session browser. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ScrubbyBoo|Multiplayer")
	FString RoomName = TEXT("ScrubbyBoo Lobby");

	/** Maximum player slots allowed in this lobby (clamped 1..16). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ScrubbyBoo|Multiplayer", meta = (ClampMin = "1", ClampMax = "16"))
	int32 MaxPlayers = 6;

	/** Clean map package path opened as a listen server upon session creation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ScrubbyBoo|Multiplayer")
	FString LobbyMapPath = TEXT("/Game/ScrubbyBoo/Maps/Lvl_Lobby");

	/** If true, hides the room from public session searches (joinable via direct IP or invite). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ScrubbyBoo|Multiplayer")
	bool bIsPrivate = false;

	/** Forces LAN mode even if an online service such as Steam is active. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ScrubbyBoo|Multiplayer")
	bool bForceLAN = false;
};

/**
 * Blueprint-friendly summary of a discovered multiplayer session for UI lobby browsers.
 */
USTRUCT(BlueprintType)
struct SCRUBBYBOOGAME_API FScrubbySessionSearchResult
{
	GENERATED_BODY()

	/** Index into the subsystem's cached search results, passed to JoinSessionByIndex. */
	UPROPERTY(BlueprintReadOnly, Category = "ScrubbyBoo|Multiplayer")
	int32 SessionIndex = INDEX_NONE;

	/** Custom room name advertised by the host. */
	UPROPERTY(BlueprintReadOnly, Category = "ScrubbyBoo|Multiplayer")
	FString RoomName;

	/** Host player's display name. */
	UPROPERTY(BlueprintReadOnly, Category = "ScrubbyBoo|Multiplayer")
	FString HostName;

	/** Number of occupied player slots currently in the lobby. */
	UPROPERTY(BlueprintReadOnly, Category = "ScrubbyBoo|Multiplayer")
	int32 CurrentPlayers = 0;

	/** Maximum player capacity configured for the lobby. */
	UPROPERTY(BlueprintReadOnly, Category = "ScrubbyBoo|Multiplayer")
	int32 MaxPlayers = 0;

	/** Round-trip ping in milliseconds reported by the session search. */
	UPROPERTY(BlueprintReadOnly, Category = "ScrubbyBoo|Multiplayer")
	int32 PingMs = 0;

	/** True if this session was found over LAN. */
	UPROPERTY(BlueprintReadOnly, Category = "ScrubbyBoo|Multiplayer")
	bool bIsLAN = false;
};

/**
 * Replicated summary of a player currently inside the lobby, formatted for UMG player lists.
 */
USTRUCT(BlueprintType)
struct SCRUBBYBOOGAME_API FScrubbyLobbyPlayerInfo
{
	GENERATED_BODY()

	/** Unique PlayerState PlayerId for deterministic sorting. */
	UPROPERTY(BlueprintReadOnly, Category = "ScrubbyBoo|Lobby")
	int32 PlayerId = INDEX_NONE;

	/** Player display name from PlayerState. */
	UPROPERTY(BlueprintReadOnly, Category = "ScrubbyBoo|Lobby")
	FString PlayerName;

	/** True when this player has toggled Ready in the lobby. */
	UPROPERTY(BlueprintReadOnly, Category = "ScrubbyBoo|Lobby")
	bool bIsReady = false;

	/** True when this player is the listen-server host. */
	UPROPERTY(BlueprintReadOnly, Category = "ScrubbyBoo|Lobby")
	bool bIsHost = false;
};
