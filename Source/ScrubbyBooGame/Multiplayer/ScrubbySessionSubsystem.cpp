// Copyright Epic Games, Inc. All Rights Reserved.

#include "Multiplayer/ScrubbySessionSubsystem.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Online/OnlineSessionNames.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "ScrubbyBooGame.h"

const FName UScrubbySessionSubsystem::SettingGameKey(TEXT("SCRUBBY_GAME"));
const FName UScrubbySessionSubsystem::SettingRoomNameKey(TEXT("SCRUBBY_ROOM_NAME"));
const FName UScrubbySessionSubsystem::SettingHostNameKey(TEXT("SCRUBBY_HOST_NAME"));
const FString UScrubbySessionSubsystem::GameIdentifierValue(TEXT("ScrubbyBoo"));

void UScrubbySessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (GEngine)
	{
		OnNetworkFailureHandle = GEngine->OnNetworkFailure().AddUObject(this, &UScrubbySessionSubsystem::HandleNetworkFailure);
		OnTravelFailureHandle = GEngine->OnTravelFailure().AddUObject(this, &UScrubbySessionSubsystem::HandleTravelFailure);
	}
}

void UScrubbySessionSubsystem::Deinitialize()
{
	if (IOnlineSessionPtr Sessions = GetSessionInterface())
	{
		Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteHandle);
		Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteHandle);
		Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteHandle);
		Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteHandle);
		Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteHandle);
	}

	if (GEngine)
	{
		GEngine->OnNetworkFailure().Remove(OnNetworkFailureHandle);
		GEngine->OnTravelFailure().Remove(OnTravelFailureHandle);
	}

	OnNetworkFailureHandle.Reset();
	OnTravelFailureHandle.Reset();

	Super::Deinitialize();
}

IOnlineSessionPtr UScrubbySessionSubsystem::GetSessionInterface() const
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	if (!Subsystem)
	{
		Subsystem = IOnlineSubsystem::Get();
	}
	return Subsystem ? Subsystem->GetSessionInterface() : nullptr;
}

bool UScrubbySessionSubsystem::IsUsingSteam() const
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	if (!Subsystem)
	{
		Subsystem = IOnlineSubsystem::Get();
	}
	return Subsystem && Subsystem->GetSubsystemName() == FName(TEXT("Steam"));
}

bool UScrubbySessionSubsystem::IsInActiveSession() const
{
	const IOnlineSessionPtr Sessions = GetSessionInterface();
	return Sessions.IsValid() && Sessions->GetNamedSession(NAME_GameSession) != nullptr;
}

void UScrubbySessionSubsystem::CreateSession(const FScrubbySessionCreateParams& Params)
{
	const IOnlineSessionPtr Sessions = GetSessionInterface();
	if (!Sessions.IsValid())
	{
		OnSessionCreated.Broadcast(false, TEXT("OnlineSession interface is unavailable."));
		return;
	}

	PendingCreateParams = Params;
	PendingCreateParams.MaxPlayers = FMath::Clamp(Params.MaxPlayers, 1, 16);

	if (Sessions->GetNamedSession(NAME_GameSession) != nullptr)
	{
		bCreateSessionAfterDestroy = true;
		bJoinSessionAfterDestroy = false;
		bReturnToMenuAfterDestroy = false;

		Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteHandle);
		OnDestroySessionCompleteHandle = Sessions->AddOnDestroySessionCompleteDelegate_Handle(
			FOnDestroySessionCompleteDelegate::CreateUObject(this, &UScrubbySessionSubsystem::HandleDestroySessionComplete));

		if (!Sessions->DestroySession(NAME_GameSession))
		{
			bCreateSessionAfterDestroy = false;
			Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteHandle);
			OnSessionCreated.Broadcast(false, TEXT("Failed to clean up previous session."));
		}
		return;
	}

	StartCreateSessionInternal(PendingCreateParams);
}

void UScrubbySessionSubsystem::StartCreateSessionInternal(const FScrubbySessionCreateParams& Params)
{
	const IOnlineSessionPtr Sessions = GetSessionInterface();
	if (!Sessions.IsValid())
	{
		OnSessionCreated.Broadcast(false, TEXT("OnlineSession interface is unavailable."));
		return;
	}

	const bool bUseLAN = Params.bForceLAN || !IsUsingSteam();
	const FString RoomName = Params.RoomName.TrimStartAndEnd().IsEmpty() ? TEXT("ScrubbyBoo Lobby") : Params.RoomName.TrimStartAndEnd();
	const int32 ClampedMaxPlayers = FMath::Clamp(Params.MaxPlayers, 1, 16);

	FString HostDisplayName = TEXT("Host");
	const ULocalPlayer* LocalPlayer = GetGameInstance() ? GetGameInstance()->GetFirstGamePlayer() : nullptr;
	if (LocalPlayer && !LocalPlayer->GetNickname().IsEmpty())
	{
		HostDisplayName = LocalPlayer->GetNickname();
	}

	LastSessionSettings = MakeShared<FOnlineSessionSettings>();
	LastSessionSettings->NumPublicConnections = ClampedMaxPlayers;
	LastSessionSettings->NumPrivateConnections = 0;
	LastSessionSettings->bIsLANMatch = bUseLAN;
	LastSessionSettings->bShouldAdvertise = !Params.bIsPrivate;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->bUseLobbiesIfAvailable = !bUseLAN;
	LastSessionSettings->bAllowInvites = true;
	LastSessionSettings->BuildUniqueId = GetBuildUniqueId();

	LastSessionSettings->Set(SettingGameKey, GameIdentifierValue, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	LastSessionSettings->Set(SettingRoomNameKey, RoomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	LastSessionSettings->Set(SettingHostNameKey, HostDisplayName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	PendingCreateParams = Params;
	PendingCreateParams.RoomName = RoomName;
	PendingCreateParams.MaxPlayers = ClampedMaxPlayers;

	Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteHandle);
	OnCreateSessionCompleteHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(
		FOnCreateSessionCompleteDelegate::CreateUObject(this, &UScrubbySessionSubsystem::HandleCreateSessionComplete));

	bool bStarted = false;
	if (LocalPlayer && LocalPlayer->GetPreferredUniqueNetId().IsValid())
	{
		bStarted = Sessions->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings);
	}
	else
	{
		bStarted = Sessions->CreateSession(0, NAME_GameSession, *LastSessionSettings);
	}

	if (!bStarted)
	{
		Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteHandle);
		OnSessionCreated.Broadcast(false, TEXT("CreateSession call rejected by OnlineSubsystem."));
	}
}

void UScrubbySessionSubsystem::HandleCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (IOnlineSessionPtr Sessions = GetSessionInterface())
	{
		Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteHandle);
	}

	if (!bWasSuccessful)
	{
		UE_LOG(LogScrubbyBooGame, Warning, TEXT("CreateSession failed for session %s"), *SessionName.ToString());
		OnSessionCreated.Broadcast(false, TEXT("Failed to create multiplayer session."));
		return;
	}

	ActiveRoomName = PendingCreateParams.RoomName;
	ActiveMaxPlayers = PendingCreateParams.MaxPlayers;
	LastDisconnectReason.Empty();

	UE_LOG(LogScrubbyBooGame, Log, TEXT("Session created: '%s' (MaxPlayers=%d). Opening lobby map: %s"),
		*ActiveRoomName, ActiveMaxPlayers, *PendingCreateParams.LobbyMapPath);

	OnSessionCreated.Broadcast(true, TEXT(""));

	if (!PendingCreateParams.LobbyMapPath.IsEmpty())
	{
		OpenMapClean(PendingCreateParams.LobbyMapPath, TEXT("listen"));
	}
}

void UScrubbySessionSubsystem::FindSessions(int32 MaxResults, bool bForceLAN)
{
	const IOnlineSessionPtr Sessions = GetSessionInterface();
	if (!Sessions.IsValid())
	{
		OnSessionsFound.Broadcast(false, {});
		return;
	}

	if (LastSessionSearch.IsValid() && LastSessionSearch->SearchState == EOnlineAsyncTaskState::InProgress)
	{
		UE_LOG(LogScrubbyBooGame, Log, TEXT("FindSessions ignored: a search is already in progress."));
		return;
	}

	const bool bUseLAN = bForceLAN || !IsUsingSteam();

	LastSessionSearch = MakeShared<FOnlineSessionSearch>();
	LastSessionSearch->MaxSearchResults = FMath::Clamp(MaxResults, 1, 100);
	LastSessionSearch->bIsLanQuery = bUseLAN;

	if (!bUseLAN)
	{
		LastSessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	}

	Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteHandle);
	OnFindSessionsCompleteHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(
		FOnFindSessionsCompleteDelegate::CreateUObject(this, &UScrubbySessionSubsystem::HandleFindSessionsComplete));

	const ULocalPlayer* LocalPlayer = GetGameInstance() ? GetGameInstance()->GetFirstGamePlayer() : nullptr;
	bool bStarted = false;
	if (LocalPlayer && LocalPlayer->GetPreferredUniqueNetId().IsValid())
	{
		bStarted = Sessions->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef());
	}
	else
	{
		bStarted = Sessions->FindSessions(0, LastSessionSearch.ToSharedRef());
	}

	if (!bStarted)
	{
		Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteHandle);
		OnSessionsFound.Broadcast(false, {});
	}
}

void UScrubbySessionSubsystem::HandleFindSessionsComplete(bool bWasSuccessful)
{
	if (IOnlineSessionPtr Sessions = GetSessionInterface())
	{
		Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteHandle);
	}

	TArray<FScrubbySessionSearchResult> Results;
	if (!bWasSuccessful || !LastSessionSearch.IsValid())
	{
		OnSessionsFound.Broadcast(false, Results);
		return;
	}

	for (int32 Index = 0; Index < LastSessionSearch->SearchResults.Num(); ++Index)
	{
		const FOnlineSessionSearchResult& SearchResult = LastSessionSearch->SearchResults[Index];
		if (!SearchResult.IsValid())
		{
			continue;
		}

		FString GameTag;
		if (SearchResult.Session.SessionSettings.Get(SettingGameKey, GameTag) && GameTag != GameIdentifierValue)
		{
			continue;
		}

		FScrubbySessionSearchResult Entry;
		Entry.SessionIndex = Index;
		Entry.PingMs = SearchResult.PingInMs;
		Entry.bIsLAN = SearchResult.Session.SessionSettings.bIsLANMatch;
		Entry.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
		Entry.CurrentPlayers = FMath::Clamp(Entry.MaxPlayers - SearchResult.Session.NumOpenPublicConnections, 0, Entry.MaxPlayers);

		if (!SearchResult.Session.SessionSettings.Get(SettingRoomNameKey, Entry.RoomName) || Entry.RoomName.IsEmpty())
		{
			Entry.RoomName = FString::Printf(TEXT("%s's Lobby"), *SearchResult.Session.OwningUserName);
		}

		if (!SearchResult.Session.SessionSettings.Get(SettingHostNameKey, Entry.HostName) || Entry.HostName.IsEmpty())
		{
			Entry.HostName = SearchResult.Session.OwningUserName;
		}

		Results.Add(Entry);
	}

	UE_LOG(LogScrubbyBooGame, Log, TEXT("FindSessions completed: %d matching lobbies found."), Results.Num());
	OnSessionsFound.Broadcast(true, Results);
}

void UScrubbySessionSubsystem::JoinSessionByIndex(int32 SessionIndex)
{
	const IOnlineSessionPtr Sessions = GetSessionInterface();
	if (!Sessions.IsValid())
	{
		OnSessionJoined.Broadcast(false, TEXT("OnlineSession interface is unavailable."));
		return;
	}

	if (!LastSessionSearch.IsValid() || !LastSessionSearch->SearchResults.IsValidIndex(SessionIndex))
	{
		OnSessionJoined.Broadcast(false, TEXT("Invalid session selection. Please refresh the browser."));
		return;
	}

	if (Sessions->GetNamedSession(NAME_GameSession) != nullptr)
	{
		PendingJoinIndex = SessionIndex;
		bJoinSessionAfterDestroy = true;
		bCreateSessionAfterDestroy = false;
		bReturnToMenuAfterDestroy = false;

		Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteHandle);
		OnDestroySessionCompleteHandle = Sessions->AddOnDestroySessionCompleteDelegate_Handle(
			FOnDestroySessionCompleteDelegate::CreateUObject(this, &UScrubbySessionSubsystem::HandleDestroySessionComplete));

		if (!Sessions->DestroySession(NAME_GameSession))
		{
			bJoinSessionAfterDestroy = false;
			Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteHandle);
			OnSessionJoined.Broadcast(false, TEXT("Failed to clean up previous session before joining."));
		}
		return;
	}

	StartJoinSessionInternal(SessionIndex);
}

void UScrubbySessionSubsystem::StartJoinSessionInternal(int32 SessionIndex)
{
	const IOnlineSessionPtr Sessions = GetSessionInterface();
	if (!Sessions.IsValid() || !LastSessionSearch.IsValid() || !LastSessionSearch->SearchResults.IsValidIndex(SessionIndex))
	{
		OnSessionJoined.Broadcast(false, TEXT("Session is no longer available."));
		return;
	}

	Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteHandle);
	OnJoinSessionCompleteHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(
		FOnJoinSessionCompleteDelegate::CreateUObject(this, &UScrubbySessionSubsystem::HandleJoinSessionComplete));

	const ULocalPlayer* LocalPlayer = GetGameInstance() ? GetGameInstance()->GetFirstGamePlayer() : nullptr;
	bool bStarted = false;
	if (LocalPlayer && LocalPlayer->GetPreferredUniqueNetId().IsValid())
	{
		bStarted = Sessions->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, LastSessionSearch->SearchResults[SessionIndex]);
	}
	else
	{
		bStarted = Sessions->JoinSession(0, NAME_GameSession, LastSessionSearch->SearchResults[SessionIndex]);
	}

	if (!bStarted)
	{
		Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteHandle);
		OnSessionJoined.Broadcast(false, TEXT("JoinSession call rejected by OnlineSubsystem."));
	}
}

void UScrubbySessionSubsystem::HandleJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSessionPtr Sessions = GetSessionInterface();
	if (Sessions.IsValid())
	{
		Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteHandle);
	}

	if (Result != EOnJoinSessionCompleteResult::Success || !Sessions.IsValid())
	{
		const FString ErrorMsg = FString::Printf(TEXT("Failed to join session (code %d)."), static_cast<int32>(Result));
		UE_LOG(LogScrubbyBooGame, Warning, TEXT("%s"), *ErrorMsg);
		OnSessionJoined.Broadcast(false, ErrorMsg);
		return;
	}

	FString ConnectString;
	if (!Sessions->GetResolvedConnectString(SessionName, ConnectString) || ConnectString.IsEmpty())
	{
		OnSessionJoined.Broadcast(false, TEXT("Could not resolve host connection address."));
		return;
	}

	APlayerController* PC = GetGameInstance() ? GetGameInstance()->GetFirstLocalPlayerController(GetWorld()) : nullptr;
	if (!PC)
	{
		OnSessionJoined.Broadcast(false, TEXT("Local PlayerController not found for ClientTravel."));
		return;
	}

	LastDisconnectReason.Empty();
	UE_LOG(LogScrubbyBooGame, Log, TEXT("Joining session via ClientTravel: %s"), *ConnectString);
	OnSessionJoined.Broadcast(true, TEXT(""));
	PC->ClientTravel(ConnectString, ETravelType::TRAVEL_Absolute);
}

void UScrubbySessionSubsystem::JoinByIP(const FString& IPAddress)
{
	FString TargetAddress = IPAddress.TrimStartAndEnd();
	if (TargetAddress.IsEmpty())
	{
		TargetAddress = TEXT("127.0.0.1");
	}

	APlayerController* PC = GetGameInstance() ? GetGameInstance()->GetFirstLocalPlayerController(GetWorld()) : nullptr;
	if (!PC)
	{
		OnSessionJoined.Broadcast(false, TEXT("Local PlayerController not found for direct IP join."));
		return;
	}

	LastDisconnectReason.Empty();
	UE_LOG(LogScrubbyBooGame, Log, TEXT("Direct joining IP via ClientTravel: %s"), *TargetAddress);
	PC->ClientTravel(TargetAddress, ETravelType::TRAVEL_Absolute);
}

void UScrubbySessionSubsystem::DestroySessionAndReturnToMenu(const FString& MainMenuMapPath)
{
	const FString TargetMenuMap = !MainMenuMapPath.TrimStartAndEnd().IsEmpty() ? MainMenuMapPath : DefaultMainMenuMapPath;
	const IOnlineSessionPtr Sessions = GetSessionInterface();

	if (!Sessions.IsValid() || Sessions->GetNamedSession(NAME_GameSession) == nullptr)
	{
		OnSessionDestroyed.Broadcast(true);
		if (!TargetMenuMap.IsEmpty())
		{
			OpenMapClean(TargetMenuMap, TEXT(""));
		}
		return;
	}

	bCreateSessionAfterDestroy = false;
	bJoinSessionAfterDestroy = false;
	bReturnToMenuAfterDestroy = !TargetMenuMap.IsEmpty();
	PendingMenuMapPath = TargetMenuMap;

	Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteHandle);
	OnDestroySessionCompleteHandle = Sessions->AddOnDestroySessionCompleteDelegate_Handle(
		FOnDestroySessionCompleteDelegate::CreateUObject(this, &UScrubbySessionSubsystem::HandleDestroySessionComplete));

	if (!Sessions->DestroySession(NAME_GameSession))
	{
		Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteHandle);
		bReturnToMenuAfterDestroy = false;
		OnSessionDestroyed.Broadcast(false);
		if (!TargetMenuMap.IsEmpty())
		{
			OpenMapClean(TargetMenuMap, TEXT(""));
		}
	}
}

void UScrubbySessionSubsystem::HandleDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (IOnlineSessionPtr Sessions = GetSessionInterface())
	{
		Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteHandle);
	}

	OnSessionDestroyed.Broadcast(bWasSuccessful);

	if (bCreateSessionAfterDestroy)
	{
		bCreateSessionAfterDestroy = false;
		StartCreateSessionInternal(PendingCreateParams);
		return;
	}

	if (bJoinSessionAfterDestroy)
	{
		bJoinSessionAfterDestroy = false;
		const int32 IndexToJoin = PendingJoinIndex;
		PendingJoinIndex = INDEX_NONE;
		StartJoinSessionInternal(IndexToJoin);
		return;
	}

	if (bReturnToMenuAfterDestroy)
	{
		bReturnToMenuAfterDestroy = false;
		const FString MenuMap = PendingMenuMapPath;
		PendingMenuMapPath.Empty();
		if (!MenuMap.IsEmpty())
		{
			OpenMapClean(MenuMap, TEXT(""));
		}
	}
}

void UScrubbySessionSubsystem::LockSessionForMatchStart()
{
	const IOnlineSessionPtr Sessions = GetSessionInterface();
	if (!Sessions.IsValid())
	{
		return;
	}

	FNamedOnlineSession* NamedSession = Sessions->GetNamedSession(NAME_GameSession);
	if (!NamedSession)
	{
		return;
	}

	FOnlineSessionSettings UpdatedSettings = NamedSession->SessionSettings;
	UpdatedSettings.bAllowJoinInProgress = false;
	UpdatedSettings.bShouldAdvertise = false;
	Sessions->UpdateSession(NAME_GameSession, UpdatedSettings, true);

	Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteHandle);
	OnStartSessionCompleteHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(
		FOnStartSessionCompleteDelegate::CreateUObject(this, &UScrubbySessionSubsystem::HandleStartSessionComplete));

	Sessions->StartSession(NAME_GameSession);
	UE_LOG(LogScrubbyBooGame, Log, TEXT("Session locked for match start (join-in-progress disabled)."));
}

void UScrubbySessionSubsystem::HandleStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (IOnlineSessionPtr Sessions = GetSessionInterface())
	{
		Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteHandle);
	}

	if (!bWasSuccessful)
	{
		UE_LOG(LogScrubbyBooGame, Warning, TEXT("StartSession returned false for %s"), *SessionName.ToString());
	}
}

void UScrubbySessionSubsystem::SetDefaultMainMenuMap(const FString& MapPath)
{
	DefaultMainMenuMapPath = StripMapOptions(MapPath);
}

void UScrubbySessionSubsystem::ClearLastDisconnectReason()
{
	LastDisconnectReason.Empty();
}

void UScrubbySessionSubsystem::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	if (World && World->GetGameInstance() != GetGameInstance())
	{
		return;
	}

	LastDisconnectReason = ErrorString.IsEmpty()
		? FString::Printf(TEXT("Network connection lost (%s)."), ENetworkFailure::ToString(FailureType))
		: ErrorString;

	UE_LOG(LogScrubbyBooGame, Warning, TEXT("Network failure: %s"), *LastDisconnectReason);
	OnNetworkError.Broadcast(LastDisconnectReason);

	DestroySessionAndReturnToMenu(DefaultMainMenuMapPath);
}

void UScrubbySessionSubsystem::HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorString)
{
	if (World && World->GetGameInstance() != GetGameInstance())
	{
		return;
	}

	LastDisconnectReason = ErrorString.IsEmpty()
		? FString::Printf(TEXT("Map travel failed (%s)."), ETravelFailure::ToString(FailureType))
		: ErrorString;

	UE_LOG(LogScrubbyBooGame, Warning, TEXT("Travel failure: %s"), *LastDisconnectReason);
	OnNetworkError.Broadcast(LastDisconnectReason);

	DestroySessionAndReturnToMenu(DefaultMainMenuMapPath);
}

void UScrubbySessionSubsystem::OpenMapClean(const FString& MapPath, const FString& Options) const
{
	const UWorld* World = GetWorld();
	const FString CleanMap = StripMapOptions(MapPath);
	if (!World || CleanMap.IsEmpty())
	{
		return;
	}

	UGameplayStatics::OpenLevel(World, FName(*CleanMap), true, Options);
}

FString UScrubbySessionSubsystem::StripMapOptions(const FString& MapPath) const
{
	FString CleanMap = MapPath.TrimStartAndEnd();
	FString LeftPart;
	if (CleanMap.Split(TEXT("?"), &LeftPart, nullptr))
	{
		CleanMap = LeftPart;
	}
	return CleanMap;
}
