---
name: mp-bp-handover
description: >-
  Use this skill when designing, implementing, or updating any multiplayer system,
  replicated Actor or ActorComponent, or Blueprint-exposed C++ API in ScrubbyBooGame.
---

# Multiplayer Implementation & Blueprint Handover

Follow these 5 steps for every multiplayer feature so C++ networking remains server-authoritative and the Blueprint developer gets a clean, conflict-free interface.

## Step 1: Define Replication Design
Before writing code, specify:
1. **Authority & Ownership**: Which side owns the Actor/Component (Server, Owning Client, or Autonomous Proxy) and where `HasAuthority()` / `IsLocallyControlled()` guards apply.
2. **Replicated Properties**: Exact properties, replication conditions (`COND_None`, `COND_OwnerOnly`, `COND_SkipOwner`, `COND_InitialOnly`), and `OnRep_*` handlers.
3. **Listen-Server Parity**: Identify every `OnRep_*` that must be called explicitly on the server after mutating state so the listen-server host sees the same UI/cosmetic updates as clients.
4. **RPCs**: Direction (`Server_`, `Client_`, `Multicast_`), reliability (`Reliable` for gameplay state/requests, `Unreliable` for one-shot cosmetics), and server validation rules.

## Step 2: Design the Blueprint Surface
- Prefer `UCLASS(Abstract, Blueprintable)` bases, modular `UActorComponent`s, and `UDataAsset`s for variants/tuning.
- Group all exposed members under `Category = "ScrubbyBoo|<System>"` with concise `/** ... */` doc comments.
- Expose UI/state changes via `BlueprintAssignable` dynamic multicast delegates (`On<Event>`).
- Expose VFX/SFX/animation hooks via `BlueprintImplementableEvent` or `BlueprintNativeEvent` (`BP_On<Event>`).
- Never break existing Blueprint symbols without flagging the change and providing a `[CoreRedirects]` entry for `Config/DefaultEngine.ini`.

## Step 3: Implement C++ & Verify Compilation
1. Implement headers and `.cpp` files following `AGENTS.md` conventions (`TObjectPtr<>`, `"Net/UnrealNetwork.h"`, `GetLifetimeReplicatedProps`, `Super::`).
2. Update `ScrubbyBooGame.Build.cs` if new modules are required.
3. Follow `.agents/skills/ue5-build/SKILL.md` to regenerate project files (if files/modules were added) and run the `ScrubbyBooGameEditor Win64 Development` build until it passes with exit code `0`.

## Step 4: Update Architecture Documentation
- Update `Docs/MultiplayerArchitecture.md` with any newly added C++ classes, components, `DataAsset`s, module dependencies, or Blueprint contract changes, and append an entry to the **BP Handover Log** if appropriate.

## Step 5: Produce the BP Handover Note
At the end of your response, always include a concise **BP Handover Note** for the co-worker containing:
- **C++ Class / Component**: Exact class name(s) to subclass (`BP_*`) or attach.
- **Exposed Properties / DataAssets**: `EditDefaultsOnly` / `EditAnywhere` knobs or `UDataAsset` fields to configure.
- **Events & Delegates to Bind**: `BlueprintAssignable` delegates (for UMG/gameplay) and `BlueprintImplementableEvent`s (for VFX/SFX/Anim).
- **PIE Test Instructions**: Recommended Play-In-Editor setup (e.g., Net Mode: `Play As Listen Server`, Number of Players: `2+`, specific actions to verify host & client parity).
- **Breaking-Change / CoreRedirects Note**: Explicit warning + `Config/DefaultEngine.ini` redirect snippet (only if any existing BP-exposed symbol changed; otherwise state "None").
