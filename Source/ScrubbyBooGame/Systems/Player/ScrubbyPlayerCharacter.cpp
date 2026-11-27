// Copyright Epic Games, Inc. All Rights Reserved.

#include "Systems/Player/ScrubbyPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"

AScrubbyPlayerCharacter::AScrubbyPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(0.0, 0.0, BaseEyeHeight));
	FirstPersonCamera->bUsePawnControlRotation = true;

	// The body is visible to other players and stays out of its owner's camera.
	GetMesh()->SetOwnerNoSee(true);
}

void AScrubbyPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInput || !IsLocallyControlled())
	{
		return;
	}

	if (MoveAction)
	{
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AScrubbyPlayerCharacter::Move);
	}
	if (LookAction)
	{
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AScrubbyPlayerCharacter::Look);
	}
	if (MouseLookAction && MouseLookAction != LookAction)
	{
		EnhancedInput->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AScrubbyPlayerCharacter::Look);
	}
}

void AScrubbyPlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	AddLocalInputMappings();
}

void AScrubbyPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	APlayerController* PlayerController = InputController.Get();
	if (EndPlayReason == EEndPlayReason::Destroyed && PlayerController && PlayerController->GetPawn() != this)
	{
		HandlePossessedPawnChanged(this, PlayerController->GetPawn());
	}
	else
	{
		RemoveLocalInputMappings();
	}
	Super::EndPlay(EndPlayReason);
}

void AScrubbyPlayerCharacter::Move(const FInputActionValue& Value)
{
	const AController* PlayerController = GetController();
	if (!IsLocallyControlled() || !PlayerController || PlayerController->GetPawn() != this)
	{
		return;
	}

	const FVector2D Axis = Value.Get<FVector2D>();
	const FRotator YawRotation(0.0, PlayerController->GetControlRotation().Yaw, 0.0);
	AddMovementInput(YawRotation.RotateVector(FVector::ForwardVector), Axis.Y);
	AddMovementInput(YawRotation.RotateVector(FVector::RightVector), Axis.X);
}

void AScrubbyPlayerCharacter::Look(const FInputActionValue& Value)
{
	const AController* PlayerController = GetController();
	if (!IsLocallyControlled() || !PlayerController || PlayerController->GetPawn() != this)
	{
		return;
	}

	const FVector2D Axis = Value.Get<FVector2D>();
	AddControllerYawInput(Axis.X);
	AddControllerPitchInput(Axis.Y);
}

void AScrubbyPlayerCharacter::AddLocalInputMappings()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController || !PlayerController->IsLocalController() || PlayerController->GetPawn() != this)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	if (!Subsystem)
	{
		return;
	}

	if (InputController.Get() != PlayerController || InputSubsystem.Get() != Subsystem)
	{
		RemoveLocalInputMappings();
		InputController = PlayerController;
		InputSubsystem = Subsystem;
		PlayerController->OnPossessedPawnChanged.AddUniqueDynamic(this, &AScrubbyPlayerCharacter::HandlePossessedPawnChanged);
	}

	for (UInputMappingContext* Context : InputMappingContexts)
	{
		if (Context && !Subsystem->HasMappingContext(Context))
		{
			Subsystem->AddMappingContext(Context, 0);
			AddedMappingContexts.AddUnique(Context);
		}
	}
}

void AScrubbyPlayerCharacter::RemoveLocalInputMappings()
{
	if (APlayerController* PlayerController = InputController.Get())
	{
		PlayerController->OnPossessedPawnChanged.RemoveDynamic(this, &AScrubbyPlayerCharacter::HandlePossessedPawnChanged);
	}
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = InputSubsystem.Get())
	{
		for (const TWeakObjectPtr<UInputMappingContext>& Context : AddedMappingContexts)
		{
			if (UInputMappingContext* MappingContext = Context.Get())
			{
				Subsystem->RemoveMappingContext(MappingContext);
			}
		}
	}

	AddedMappingContexts.Reset();
	InputSubsystem.Reset();
	InputController.Reset();
}

void AScrubbyPlayerCharacter::HandlePossessedPawnChanged(APawn* /*OldPawn*/, APawn* NewPawn)
{
	if (NewPawn == this)
	{
		return;
	}

	RemoveLocalInputMappings();

	// ClientRestart may have run on the new pawn before the pawn-change notification.
	if (AScrubbyPlayerCharacter* NextPawn = Cast<AScrubbyPlayerCharacter>(NewPawn))
	{
		NextPawn->AddLocalInputMappings();
	}
}
