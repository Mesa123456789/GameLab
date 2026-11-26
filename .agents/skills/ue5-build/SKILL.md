---
name: ue5-build
description: >-
  Use this skill whenever compiling C++ code, verifying C++ edits, or regenerating
  Visual Studio project files for ScrubbyBooGame on Unreal Engine 5.6.
---

# UE 5.6 Build & Verification Workflow (`ScrubbyBooGame`)

Use these exact commands on Windows (PowerShell) from the workspace root `e:\!_Chotayakorn_ICAT\GameLab`.

## Commands

> Note: Invoke `UnrealBuildTool.exe` directly from PowerShell rather than `Build.bat` because `cmd.exe` delayed expansion strips the `!` in `e:\!_Chotayakorn_ICAT\...`. Pass `-NoUBA -NoUBALocal` on Editor builds because the Unreal Build Accelerator (UBA) local executor can hang `cl.exe` in this environment.

### 1. Build Editor Target (Default Verification)
Run after any C++ `.h` or `.cpp` edit to verify UHT (UnrealHeaderTool) and MSVC compilation:

```powershell
& "C:\Program Files\Epic Games\UE_5.6\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" ScrubbyBooGameEditor Win64 Development -Project="e:\!_Chotayakorn_ICAT\GameLab\ScrubbyBooGame.uproject" -WaitMutex -FromMsBuild -NoUBA -NoUBALocal
```

### 2. Regenerate Visual Studio Project Files
Run whenever you add or remove `.h`/`.cpp` files or edit `Source/ScrubbyBooGame/ScrubbyBooGame.Build.cs`:

```powershell
& "C:\Program Files\Epic Games\UE_5.6\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" -ProjectFiles -Project="e:\!_Chotayakorn_ICAT\GameLab\ScrubbyBooGame.uproject" -Game -Rocket -Progress
```

## Live Coding vs. Full Build Guidance
- **Live Coding (`.cpp`-only edits)**: If the Unreal Editor is open with Live Coding enabled, `UnrealBuildTool.exe` may fail due to DLL or build-mutex locks. For `.cpp`-only body changes, the user can press `Ctrl+Alt+F11` inside the Unreal Editor instead.
- **Full Build Required**: Whenever any header (`.h`), `UCLASS`, `USTRUCT`, `UENUM`, `UPROPERTY`, `UFUNCTION`, delegate signature, or `.Build.cs` changes, **Live Coding is unsafe/unsupported**—remind the user to close the Unreal Editor and run the `UnrealBuildTool.exe` Editor build above (and reopen the Editor afterward).

## Validation & Error Handling
1. Confirm the command exits with code `0` (`Command exited with code 0`).
2. If the build fails:
   - Summarize every UHT or MSVC error with exact file paths and line numbers.
   - Fix **all** reported errors before re-running the build.
   - If failure is caused by a locked DLL / Live Coding mutex while the Editor is running, inform the user to close the Unreal Editor and re-run.
