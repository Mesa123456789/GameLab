// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Engine/EngineBaseTypes.h"
#include "Multiplayer/ScrubbySessionTypes.h"
#include "ScrubbySessionSubsystem.generated.h"

class FOnlineSessionSearch;
class FOnlineSessionSettings;
class UNetDriver;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScrubbySessionResult, bool, bSuccess, const FString&, ErrorMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScrubbySessionsFound, bool, bSuccess, const TArray<FScrubbySessionSearchResult>&, Results);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScrubbySessionDestroyed, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScrubbyNetworkError, const FString&, ErrorMessage);

/**
 * Persistent multiplayer session subsystem managing Create/Find/Join/Leave/Lock across map travel.
 * Automatically adapts between LAN (Null) and Steam Lobbies based on the active OnlineSubsystem.
 */
UCLASS()
class SCRUBBYBOOGAME_API UScrubbySessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Fires after CreateSession finishes (just before opening the lobby map on success). */
	UPROPERTY(BlueprintAssignable, Category = "ScrubbyBoo|Multiplayer")
	FOnScrubbySessionResult OnSessionCreated;

	/** Fires when FindSessions completes with the list of discovered ScrubbyBoo lobbies. */
	UPROPERTY(BlueprintAssignable, Category = "ScrubbyBoo|Multiplayer")
	FOnScrubbySessionsFound OnSessionsFound;

	/** Fires when JoinSessionByIndex finishes (or fails to resolve connect string). */
	UPROPERTY(BlueprintAssignable, Category = "ScrubbyBoo|Multiplayer")
	FOnScrubbySessionResult OnSessionJoined;

	/** Fires when an active session finishes destroying. */
	UPROPERTY(BlueprintAssignable, Category = "ScrubbyBoo|Multiplayer")
	FOnScrubbySessionDestroyed OnSessionDestroyed;

	/** Fires on network or travel errors (e.g., host disconnected, connection lost). */
	UPROPERTY(BlueprintAssignable, Category = "ScrubbyBoo|Multiplayer")
	FOnScrubbyNetworkError OnNetworkError;

	/** Creates a hosted session and opens the configured lobby map as a Listen Server. */
	UFUNCTION(BlueprintCallable, Category = "ScrubbyBoo|Multiplayer")
	void CreateSession(const FScrubbySessionCreateParams& Params);

	/** Searches for available ScrubbyBoo sessions (LAN or Steam lobbies). */
	UFUNCTION(BlueprintCallable, Category = "ScrubbyBoo|Multiplayer")
	void FindSessions(int32 MaxResults = 20, bool bForceLAN = false);

	/** Joins a discovered session by its SessionIndex from OnSessionsFound. */
	UFUNCTION(BlueprintCallable, Category = "ScrubbyBoo|Multiplayer")
	void JoinSessionByIndex(int32 SessionIndex);

	/** Directly connects to a Listen Server IP/port (e.g. "127.0.0.1" or "192.168.1.50:7777"). */
	UFUNCTION(BlueprintCallable, Category = "ScrubbyBoo|Multiplayer")
	void JoinByIP(const FString& IPAddress);

	/** Destroys the active session (if any) and returns to the specified Main Menu map. */
	UFUNCTION(BlueprintCallable, Category = "ScrubbyBoo|Multiplayer")
	void DestroySessionAndReturnToMenu(const FString& MainMenuMapPath = TEXT(""));

	/** Locks the active session when the match begins so late-joiners cannot enter mid-run. */
	UFUNCTION(BlueprintCallable, Category = "ScrubbyBoo|Multiplayer")
	void LockSessionForMatchStart();

	/** Sets the fallback Main Menu map path used on disconnects and DestroySessionAndReturnToMenu. */
	UFUNCTION(BlueprintCallable, Category = "ScrubbyBoo|Multiplayer")
	void SetDefaultMainMenuMap(const FString& MapPath);

	/** Returns the room name cached for the current/last created session. */
	UFUNCTION(BlueprintPure, Category = "ScrubbyBoo|Multiplayer")
	const FString& GetActiveRoomName() const { return ActiveRoomName; }

	/** Returns the max player count cached for the current/last created session. */
	UFUNCTION(BlueprintPure, Category = "ScrubbyBoo|Multiplayer")
	int32 GetActiveMaxPlayers() const { return ActiveMaxPlayers; }

	/** Returns true if the active OnlineSubsystem is Steam (false for Null/LAN). */
	UFUNCTION(BlueprintPure, Category = "ScrubbyBoo|Multiplayer")
	bool IsUsingSteam() const;

	/** Returns true if NAME_GameSession currently exists on the OnlineSubsystem. */
	UFUNCTION(BlueprintPure, Category = "ScrubbyBoo|Multiplayer")
	bool IsInActiveSession() const;

	/** Returns the last disconnect/network error message (persisted across return to menu). */
	UFUNCTION(BlueprintPure, Category = "ScrubbyBoo|Multiplayer")
	const FString& GetLastDisconnectReason() const { return LastDisconnectReason; }

	/** Clears the stored disconnect reason after the UI has displayed it. */
	UFUNCTION(BlueprintCallable, Category = "ScrubbyBoo|Multiplayer")
	void ClearLastDisconnectReason();

private:
	static const FName SettingGameKey;
	static const FName SettingRoomNameKey;
	static const FName SettingHostNameKey;
	static const FString GameIdentifierValue;

	IOnlineSessionPtr GetSessionInterface() const;
	void StartCreateSessionInternal(const FScrubbySessionCreateParams& Params);
	void StartJoinSessionInternal(int32 SessionIndex);
	void OpenMapClean(const FString& MapPath, const FString& Options) const;
	FString StripMapOptions(const FString& MapPath) const;

	void HandleCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void HandleStartSessionComplete(FName SessionName, bool bWasSuccessful);
	void HandleFindSessionsComplete(bool bWasSuccessful);
	void HandleJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void HandleDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
	void HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorString);

	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

	FDelegateHandle OnCreateSessionCompleteHandle;
	FDelegateHandle OnStartSessionCompleteHandle;
	FDelegateHandle OnFindSessionsCompleteHandle;
	FDelegateHandle OnJoinSessionCompleteHandle;
	FDelegateHandle OnDestroySessionCompleteHandle;
	FDelegateHandle OnNetworkFailureHandle;
	FDelegateHandle OnTravelFailureHandle;

	FScrubbySessionCreateParams PendingCreateParams;
	bool bCreateSessionAfterDestroy = false;

	int32 PendingJoinIndex = INDEX_NONE;
	bool bJoinSessionAfterDestroy = false;

	bool bReturnToMenuAfterDestroy = false;
	FString PendingMenuMapPath;

	FString ActiveRoomName = TEXT("ScrubbyBoo Lobby");
	int32 ActiveMaxPlayers = 6;
	FString DefaultMainMenuMapPath = TEXT("/Game/ScrubbyBoo/Maps/Lvl_MainMenu");
	FString LastDisconnectReason;
};
