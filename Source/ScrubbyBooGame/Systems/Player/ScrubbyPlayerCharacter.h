// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ScrubbyPlayerCharacter.generated.h"

class APlayerController;
class UCameraComponent;
class UEnhancedInputLocalPlayerSubsystem;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

/** First-person player pawn with configurable presentation and standard Character Movement. */
UCLASS(Abstract, Blueprintable)
class SCRUBBYBOOGAME_API AScrubbyPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AScrubbyPlayerCharacter();

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PawnClientRestart() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** First-person view attached to the capsule. Set its relative transform in the Blueprint. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ScrubbyBoo|Player", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	/** Mapping contexts applied at priority zero for the player who possesses this pawn. */
	UPROPERTY(EditDefaultsOnly, Category = "ScrubbyBoo|Player")
	TArray<TObjectPtr<UInputMappingContext>> InputMappingContexts;

	/** Axis2D movement input. X is right and Y is forward. */
	UPROPERTY(EditDefaultsOnly, Category = "ScrubbyBoo|Player")
	TObjectPtr<UInputAction> MoveAction;

	/** Optional Axis2D look input, typically mapped to a gamepad. */
	UPROPERTY(EditDefaultsOnly, Category = "ScrubbyBoo|Player")
	TObjectPtr<UInputAction> LookAction;

	/** Optional Axis2D look input mapped to a mouse. */
	UPROPERTY(EditDefaultsOnly, Category = "ScrubbyBoo|Player")
	TObjectPtr<UInputAction> MouseLookAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void AddLocalInputMappings();
	void RemoveLocalInputMappings();

	UFUNCTION()
	void HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	TWeakObjectPtr<APlayerController> InputController;
	TWeakObjectPtr<UEnhancedInputLocalPlayerSubsystem> InputSubsystem;
	TArray<TWeakObjectPtr<UInputMappingContext>> AddedMappingContexts;
};
