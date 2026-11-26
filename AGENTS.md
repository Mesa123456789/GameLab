# ScrubbyBooGame — Agent Rules & C++/Blueprint Contract

## Project Context
- **Project**: `ScrubbyBooGame` (`ScrubbyBooGame.uproject`), **Unreal Engine 5.6** (`C:\Program Files\Epic Games\UE_5.6`) on Windows.
- **Genre**: R.E.P.O.-style co-op horror (server-authoritative physics grabbing of fragile valuables, multi-player shared carry, extraction/quota loop, monsters, multiplayer lobbies/sessions).
- **Goal**: Build a rock-solid, server-authoritative C++ multiplayer foundation with clean, designer-friendly Blueprint hooks so the C++ developer and Blueprint developer can work in parallel without blocking or breaking each other.

---

## 2-Dev + Artist Workflow & Folder Ownership
- **Multiplayer Core Dev & Git Manager (User)** owns:
  - `Source/ScrubbyBooGame/Core/`, `Source/ScrubbyBooGame/Multiplayer/`, `Source/ScrubbyBooGame/Shared/`
  - `Config/`, `ScrubbyBooGame.uproject`, `ScrubbyBooGame.Build.cs`, `Docs/`
  - PR reviews and merges into `main`.
- **Gameplay Dev (Co-worker — Blueprint + AI-assisted C++)** owns:
  - `Content/ScrubbyBoo/` (`BP_*` subclasses, maps/levels, UMG widgets, meshes, animations, VFX, SFX, DataAsset instances).
  - Their assigned feature folders under `Source/ScrubbyBooGame/Systems/<Feature>/` (e.g., `Systems/Inventory/`, `Systems/Flashlight/`).
- **Artist** works outside Git via Shared Cloud (`02_Ready_For_Unreal/`), and devs reimport assets into `Content/ScrubbyBoo/`.
- **Strict Agent Boundary**:
  - Never create, edit, or delete binary `.uasset` or `.umap` files directly.
  - Always expose clean, documented C++ properties, delegates, and events so Blueprints can handle visuals, audio, UI, and gameplay tuning inside `Content/`.

---

## Modular C++ Folder Layout & Merge-Conflict Prevention
Both developers (and their AI assistants) write C++ in parallel. Enforce this modular layout to keep Git merges conflict-free:

```text
Source/ScrubbyBooGame/
├── Core/             # GameMode, GameState, PlayerState, PlayerController, Character base (Protected)
├── Multiplayer/      # Sessions, lobby, replication utilities (Multiplayer Dev)
├── Shared/           # Thin, additive UInterfaces, GameplayTags, shared structs/enums
└── Systems/          # 1 self-contained folder per major gameplay system
    ├── Player/
    ├── AI/
    ├── Grab/
    └── Extraction/   # Add new Systems/<Feature>/ folders per major system as needed
```

Mirror the same system boundaries in `Content/ScrubbyBoo/Blueprints/` (`Core/`, `Player/`, `AI/`, `Valuables/`, `Multiplayer/`).

- **1 System = 1 Short Branch & 1 Folder**: Branch from up-to-date `main` as `feat/<system>` (1–3 days max, merge back to `main`, delete branch, pull `main` before starting the next system). Never edit files inside another dev's active feature folder; wait for their branch to merge to `main` or decouple through a `UInterface` in `Shared/` (or built-in `UGameplayStatics::ApplyDamage`).
- **Zero `.Build.cs` Edits for New Folders**: `"ScrubbyBooGame"` is already in `PublicIncludePaths` in `ScrubbyBooGame.Build.cs`. Always use module-root relative includes such as `#include "Systems/Grab/GrabComponent.h"` instead of adding every new subfolder to `ScrubbyBooGame.Build.cs`.
- **Component & Subsystem First**: Build new features as modular `UActorComponent`s (`SetIsReplicatedByDefault(true)` when networked), `UGameInstanceSubsystem`/`UWorldSubsystem`s (auto-instantiated by Unreal without editing `GameInstance`/`GameMode`), or standalone `AActor`s + `UDataAsset`s. Never dump feature logic directly into shared `Character`, `GameMode`, or `PlayerController` files.
- **Interface & Delegate Decoupling**: Use thin `UInterface`s in `Shared/` and `BlueprintAssignable` delegates instead of tightly coupling two `Systems/<Feature>/` folders together.
- **Protected Shared Files** (coordinate with the Git Manager before modifying; keep changes minimal and additive):
  - `ScrubbyBooGame.uproject`, `Source/ScrubbyBooGame/ScrubbyBooGame.Build.cs`, `Config/*.ini`, `Source/ScrubbyBooGame/Core/*` (and existing template root files until migrated), plus shared `Content/ScrubbyBoo/` core Blueprints and main maps.
- **AI Scope & Multiplayer Guardrail (For Both Devs' AI Assistants)**:
  - Stay strictly inside the assigned feature folder (`Source/ScrubbyBooGame/Systems/<Feature>/` and `Content/ScrubbyBoo/<Feature>/`). Never auto-refactor, reformat, or rename files outside your current task's feature folder.
  - Any AI-generated C++ gameplay code **must** follow the UE 5.6 multiplayer rules below (`HasAuthority()`, replicated properties + `OnRep_` listen-server parity, `Server_`/`Client_`/`Multicast_` RPCs) so it works in multiplayer, not only single-player.

---

## C++ ↔ Blueprint Contract
- **Division of Responsibility**:
  - **C++ owns**: Networking, authority, replication, RPCs, `GameMode`, `GameState`, `PlayerState`, `PlayerController`, `UGameInstanceSubsystem` (sessions), core `UActorComponent`s, and `UDataAsset` schemas.
  - **Blueprint owns**: Subclassing C++ bases (`BP_*`), assigning meshes/materials/animations, spawning VFX/SFX, building UMG widgets, and configuring `UDataAsset` / `EditDefaultsOnly` tuning values.
- **Composability & Conflict Avoidance**:
  - Prefer `UCLASS(Abstract, Blueprintable)` (or `UCLASS(Blueprintable)`) base classes, modular `UActorComponent`s (e.g., `UGrabComponent`, `UHealthComponent`), and `UDataAsset` definitions so the Blueprint dev can create item/enemy variants and tune parameters without editing a single monolithic Character Blueprint or touching C++.
- **Event-Driven UI & Cosmetics**:
  - Expose gameplay/UI state changes via `BlueprintAssignable` dynamic multicast delegates (`DECLARE_DYNAMIC_MULTICAST_DELEGATE_*`).
  - Expose cosmetic hooks (VFX, SFX, camera shake, animation triggers) via `BlueprintImplementableEvent` or `BlueprintNativeEvent`, triggered from `OnRep_*` handlers so both server and clients update consistently.
- **Breaking-Change Guardrail**:
  - **Never** rename, delete, or change the signature/type of any Blueprint-exposed `UPROPERTY` (`BlueprintReadOnly`, `BlueprintReadWrite`, `EditDefaultsOnly`, `EditAnywhere`) or `UFUNCTION` (`BlueprintCallable`, `BlueprintImplementableEvent`, `BlueprintNativeEvent`) without:
    1. Explicitly warning the user before/when making the change, and
    2. Providing the exact `[CoreRedirects]` entry to add in `Config/DefaultEngine.ini` so existing Blueprints in `Content/` do not break on pull.
- **Metadata & Documentation**:
  - Always group Blueprint-exposed members under `Category = "ScrubbyBoo|<System>"` (e.g., `"ScrubbyBoo|Grab"`, `"ScrubbyBoo|Quota"`, `"ScrubbyBoo|Health"`).
  - Add concise `/** ... */` doc comments on every Blueprint-exposed `UCLASS`, `UPROPERTY`, `UFUNCTION`, and delegate.

---

## UE 5.6 Multiplayer & C++ Standards
- **Authority & Local Control**:
  - All gameplay state mutations must be server-authoritative; guard server logic with `HasAuthority()`.
  - Guard local-only input, camera, audio listeners, and HUD creation/updates with `IsLocallyControlled()` (or `IsLocalController()`).
- **Listen-Server Parity (`OnRep_` Behavior)**:
  - Remember that Unreal C++ does **not** automatically invoke `OnRep_*` on the listen server when the server modifies a replicated property. Explicitly call `OnRep_<Prop>()` on the server right after mutating replicated state whenever host visuals, audio, or UI delegates need to update too.
- **Replication Over RPCs for State**:
  - Prefer replicated properties with `ReplicatedUsing = OnRep_<Prop>` for persistent or late-joiner-relevant state.
  - Reserve `NetMulticast` (usually `Unreliable`) strictly for transient, one-shot cosmetic effects (e.g., impact sparks, break sound).
- **RPC Naming & Validation**:
  - Name RPCs consistently: `Server_<Action>`, `Client_<Action>`, `Multicast_<Action>`.
  - Validate client inputs on the server (`WithValidation` when appropriate, plus range/distance/state checks inside `_Implementation`).
- **Replication Boilerplate**:
  - Include `"Net/UnrealNetwork.h"` in `.cpp` files that replicate properties.
  - Implement `GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override` using `DOREPLIFETIME` or `DOREPLIFETIME_CONDITION` (`COND_OwnerOnly`, `COND_SkipOwner`, `COND_InitialOnly`) as appropriate.
- **UE 5.6 C++ Idioms**:
  - Use `TObjectPtr<T>` for `UPROPERTY()` `UObject` member pointers in headers.
  - Forward-declare types in headers whenever possible; include full headers in `.cpp`.
  - Always call `Super::` in overridden virtual engine methods.
  - Keep `PrimaryActorTick.bCanEverTick = false` by default; only enable ticking when per-frame updates (such as physics-grab target smoothing) are genuinely required.

---

## Project Skills & Living Documentation
- **Build & Project File Verification**: Use `.agents/skills/ue5-build/SKILL.md` whenever compiling or regenerating Visual Studio project files.
- **Multiplayer Feature & BP Handover**: Use `.agents/skills/mp-bp-handover/SKILL.md` whenever designing or implementing a multiplayer system, replicated Actor/Component, or Blueprint-exposed C++ API.
- **Architecture Reference**: Keep `Docs/MultiplayerArchitecture.md` updated as core multiplayer systems and Blueprint contracts are added.
