# ScrubbyBooGame — Multiplayer Architecture

> Living reference for C++ networking architecture and the C++ ↔ Blueprint contract. Update this document whenever core classes, components, or replication contracts are added or changed.

---

## 1. Current Project Baseline

- **Engine**: Unreal Engine 5.6
- **Primary Module**: `ScrubbyBooGame` (`Source/ScrubbyBooGame/ScrubbyBooGame.Build.cs`)
- **Enabled Plugins** (`ScrubbyBooGame.uproject`):
  - `ModelingToolsEditorMode`
  - `StateTree`
  - `GameplayStateTree`
- **Current Source Layout** (under `Source/ScrubbyBooGame/`):
  - `Core/` *(protected)* — Holds `AScrubbyBooGameCharacter`, `AScrubbyBooGameGameMode`, `AScrubbyBooGamePlayerController`, `AScrubbyBooGameCameraManager`
  - `Multiplayer/` — Holds `ScrubbySessionTypes.h`, `UScrubbySessionSubsystem`, `AScrubbyLobbyGameMode`, `AScrubbyLobbyGameState`, `AScrubbyLobbyPlayerState`, `AScrubbyLobbyPlayerController`
  - `Shared/` — Scaffolded and ready for thin, additive `UInterface`s, `GameplayTags`, and shared structs/enums
  - `Systems/<Feature>/` — Scaffolded (`Systems/`) and ready for self-contained feature folders (e.g., `Systems/Grab/`, `Systems/Extraction/`, `Systems/Health/`, `Systems/Inventory/`, `Systems/AI/`). Always `#include` relative to module root (e.g., `#include "Core/..."` or `#include "Systems/Grab/GrabComponent.h"`) so `ScrubbyBooGame.Build.cs` does not require edits per folder.
  - Reference variants: `Variant_Horror/`, `Variant_Shooter/`
- **Active Networking Modules** (`ScrubbyBooGame.Build.cs` + `Config/DefaultEngine.ini`):
  - `NetCore`
  - `OnlineSubsystem` (`DefaultPlatformService=Null` active for fast solo/LAN PIE testing; Steam AppID `480` block commented in `DefaultEngine.ini`)
  - `OnlineSubsystemUtils`
  - *(Later)* Enable `OnlineSubsystemSteam` plugin (or EOS) and uncomment Steam config when playtesting over the internet.

---

## 2. Planned Core Class & BP Contract Map

| System | C++ Base Class | Owner & Replication Role | Planned BP Subclass / Usage | Status |
| :--- | :--- | :--- | :--- | :--- |
| **Session Management** | `UScrubbySessionSubsystem` (`UGameInstanceSubsystem`) | Local client/host instance; creates/finds/joins/locks sessions, handles Direct IP join & network failures, opens Lobby `?listen`. | Called from `WBP_MainMenu` & `WBP_Lobby` UMG widgets via `BlueprintCallable` functions & delegates. | Implemented (`feat/multiplayer-session`) |
| **Walkable 3D Lobby** | `AScrubbyLobbyGameMode`, `AScrubbyLobbyGameState`, `AScrubbyLobbyPlayerState`, `AScrubbyLobbyPlayerController` | Server-authoritative lobby slots (default max 6), Ready-Up sync, Host solo/all-ready start gate, `ServerTravel` + late-join lock (`PreLogin`). | `BP_LobbyGameMode`, `BP_LobbyGameState`, `BP_LobbyPlayerState`, `BP_LobbyPlayerController` in `Content/ScrubbyBoo/Blueprints/Multiplayer/`. | Implemented (`feat/multiplayer-session`) |
| **Match Rules & Spawning** | `AScrubbyGameMode` (`AGameModeBase` / `AGameMode`) | Server-only; manages player join/respawn, round transitions, quota evaluation, seamless travel. | `BP_ScrubbyGameMode` sets default Pawn, PlayerController, GameState, PlayerState, and level rules. | Planned |
| **Shared Match State** | `AScrubbyGameState` (`AGameStateBase`) | Server-authoritative, replicated to all; holds current level, match phase, quota target, extracted value, and delegates (`OnRep_*`). | `BP_ScrubbyGameState` / HUD widgets bind to `OnQuotaUpdated`, `OnPhaseChanged`, etc. | Planned |
| **Player Identity & Status** | `AScrubbyPlayerState` (`APlayerState`) | Server-authoritative, replicated to all; holds alive/dead state, health/stamina summary, upgrades, display name. | `BP_ScrubbyPlayerState` / Scoreboard & Spectate UI bind to `OnAliveStateChanged`, `OnUpgradesChanged`. | Planned |
| **Input & UI Routing** | `AScrubbyPlayerController` (`APlayerController`) | Server + Owning Client; routes local Enhanced Input, HUD creation (`IsLocalController()`), and client-to-server requests. | `BP_ScrubbyPlayerController` assigns Input Mapping Contexts and creates `WBP_HUD` / `WBP_PauseMenu`. | Planned |
| **Player Character & Components** | `AScrubbyCharacter` + `UGrabComponent`, `UHealthComponent`, `UStaminaComponent` | Server-authoritative gameplay & physics grab; autonomous proxy movement/input; delegates & cosmetic events on `OnRep_*`. | `BP_ScrubbyCharacter` sets skeletal meshes, animations, SFX/VFX hooks, and tunes component defaults. | Planned |
| **Valuables & Fragility** | `AValuableActor` + `UValuableDataAsset` | Server-authoritative Chaos physics & value tracking; replicates current value, intact/broken state, grabbers; `Multicast` one-shot impact/break VFX. | `BP_Valuable_*` subclasses + `DA_Valuable_*` assets define mesh, mass, base value, fragility threshold, break effects. | Planned |
| **Extraction Zone** | `AExtractionPoint` (`AActor`) | Server-authoritative overlap/volume tracking; counts valuables inside zone, triggers extraction sequence, notifies `AScrubbyGameState`. | `BP_ExtractionPoint` defines trigger mesh/bounds, door/lever animations, extraction SFX/VFX. | Planned |
| **Monsters / AI** | `AMonsterCharacter` + `StateTree` tasks/evaluators | Server-authoritative AI & StateTree execution; replicates aggro/alert state & attack cues via `OnRep_*`. | `BP_Monster_*` + `ST_Monster_*` configure meshes, animations, sounds, and StateTree behaviors. | Planned |

---

## 3. PIE Multiplayer Testing Checklist

Before marking any multiplayer feature done, verify in Unreal Editor Play-In-Editor (PIE):

1. **Net Mode**: Set to **Play As Listen Server**.
2. **Number of Players**: **2** minimum (**3–4** when testing shared multi-player valuable lifting).
3. **Host vs. Client Parity**:
   - Trigger the feature from the **Listen Server (Player 1)** and confirm both Host and Client see identical state, UI, and cosmetics.
   - Trigger the feature from a **Client (Player 2)** and confirm Server validation, replication back to Client, and updates on Host.
4. **Late-Joiner / State Sync** *(when applicable)*: Confirm replicated properties (`OnRep_*`) accurately reflect current state without relying on missed `NetMulticast` RPCs.
5. **Network Emulation** *(for physics/grab & combat checks)*: Enable PIE **Network Emulation** (e.g., *Average* / *Bad* packet lag & loss) to verify smoothness and absence of rubber-banding or stuck grab states.

---

## 4. BP Handover Log

> Record short entries here as C++ multiplayer milestones are handed off to Blueprint.

- **`feat/multiplayer-session`**: Added `UScrubbySessionSubsystem` (Create/Find/Join/Direct IP/Leave/Lock + error delegates) and walkable 3D Lobby framework (`AScrubbyLobbyGameMode`, `AScrubbyLobbyGameState`, `AScrubbyLobbyPlayerState`, `AScrubbyLobbyPlayerController`) with LAN (`Null`) default and Steam-ready hooks.
