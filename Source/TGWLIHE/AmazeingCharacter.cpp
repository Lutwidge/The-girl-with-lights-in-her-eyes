// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "AmazeingCharacter.h"
#include "AmazeingProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/PostProcessVolume.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "AmazeingGameMode.h"
#include "EndTriggerVolume.h"
#include "UObject/ConstructorHelpers.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Maze.h"
#include "AudioManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AAmazeingCharacter

AAmazeingCharacter::AAmazeingCharacter()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// We are not closing eyes at the beginning
	bClosingEyes = false;
	ClosingFactor = 1.0f;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Set TeamId for the AI Perception to recognize the player as an Enemy
	TeamId = FGenericTeamId(0);

	// Initially, the character is frozen
	IsMovementEnabled = false;

	// Grab the classes of the UMG
	static ConstructorHelpers::FObjectFinder<UClass> UMGClassFinder(TEXT("Class'/Game/Blueprints/UI/Pause.Pause_C'"));
	if (UMGClassFinder.Object)
	{
		UMGPauseWidget = UMGClassFinder.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: Pause UMG not found"));
	}

	CurrentDistance = 0.0f;
}

void AAmazeingCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// We create a dynamic instance of the closing eyes postprocess material & we add it to the postprocess volume
	if (PostProcessVolume != nullptr)
	{
		DynamicEyesMaterial = UMaterialInstanceDynamic::Create(EyesBlendable, this);
		if (DynamicEyesMaterial)
		{
			PostProcessVolume->Settings.AddBlendable(DynamicEyesMaterial, 1);
		}
		// We initialize the weight of the exit blendable to 0 : not visible initially
		PostProcessVolume->Settings.WeightedBlendables.Array[0].Weight = 0;
	}

	/// Subscribe to the necessary events
	if (GetWorld())
	{
		// Unfreeze character when the fade in of the level is finished, in each transition
		AAmazeingGameMode* GameMode = (AAmazeingGameMode*)GetWorld()->GetAuthGameMode();
		GameMode->OnFadeInFinished().AddUFunction(this, FName("UnfreezeCharacter"));

		// Unfreeze character when the monster "kill" animation is finished
		GameMode->OnMonsterKillFadeInFinished().AddUFunction(this, FName("UnfreezeCharacter"));
	}
	// Freeze character when the level transition is launched
	EndTriggerVolume->OnFadeOutLaunched().AddUFunction(this, FName("FreezeCharacter"));

	// Freeze character when the monster "kills" the player
	Maze->OnMonsterKill().AddUFunction(this, FName("FreezeCharacter"));

	// Added so that if the player finishes the level with the eyes closed, they start reopened for the next level & after a Monster Kill
	Maze->OnTransitionFinished().AddUFunction(this, FName("ReopenEyes"));
	Maze->OnMonsterKill().AddUFunction(this, FName("ReopenEyes"));
}

void AAmazeingCharacter::Tick(float DeltaTime)
{
	// Call the base class
	Super::Tick(DeltaTime);

	//Close eyes if CloseEyes button is down, open eyes if it's not
	{
		if (!bClosingEyes)
		{
			ClosingFactor += DeltaTime / 0.2f;         //Close eyes over a quarter of a second
		}
		else
		{
			ClosingFactor -= DeltaTime / 0.2f;        //Open eyes over a quarter of a second

			if (ClosingFactor <= 0.0f)
			{
				EyesClosedEvent.Broadcast();
			}
		}

		ClosingFactor = FMath::Clamp<float>(ClosingFactor, 0.0f, 1.0f);

		//Change the dynamic material instance R parameter
		DynamicEyesMaterial->SetScalarParameterValue(TEXT("R"), ClosingFactor);

		// We change the weight of the blendable, in order to make it appear or disappear depending on where we are of the closing/opening of the eyes
		if (ClosingFactor == 0.0f)
		{
			PostProcessVolume->Settings.WeightedBlendables.Array[0].Weight = 1;
		}
		else
		{
			PostProcessVolume->Settings.WeightedBlendables.Array[0].Weight = 0;
		}

		// https://answers.unrealengine.com/questions/238373/2-post-processing-materials-at-once.html pour avoir un postprocess au-dessus d'un autre
	}

	// Management of the footsteps sounds
	if (IsMovementEnabled) // Only if movement is enabled, to avoid sounds during transitions and monter kills
	{
		FVector MovementVector = GetActorLocation() - PreviousLocation;
		CurrentDistance += MovementVector.Size2D();
		
		if (CurrentDistance >= 1500.0f)
		{
			AudioManager->PlayFootstep();
			CurrentDistance = 0.0f;
			PreviousLocation = GetActorLocation();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AAmazeingCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind closing eyes event
	PlayerInputComponent->BindAction("CloseEyes", IE_Pressed, this, &AAmazeingCharacter::OnCloseEyesPressed);
	PlayerInputComponent->BindAction("CloseEyes", IE_Released, this, &AAmazeingCharacter::OnCloseEyesReleased);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AAmazeingCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AAmazeingCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AAmazeingCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AAmazeingCharacter::LookUpAtRate);

	// For the pause system
	PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &AAmazeingCharacter::OnPaused).bExecuteWhenPaused = true;
}

void AAmazeingCharacter::OnCloseEyesPressed()
{
	if (IsMovementEnabled)
	{
		bClosingEyes = true;
		//EyesClosedEvent.Broadcast();
	}
}

void AAmazeingCharacter::OnCloseEyesReleased()
{
	if (IsMovementEnabled)
	{
		ReopenEyes();
	}
}

void AAmazeingCharacter::ReopenEyes()
{
	bClosingEyes = false;
	EyesOpenedEvent.Broadcast();
}

void AAmazeingCharacter::MoveForward(float Value)
{
	if (IsMovementEnabled)
	{
		if (Value != 0.0f)
		{
			if (!bClosingEyes)
			{
				// add movement in that direction
				AddMovementInput(GetActorForwardVector(), Value);

				UGameplayStatics::PlayWorldCameraShake(GetWorld(), HeadBob, GetActorLocation(), 0.0f, 500.0f);
			}
			else
			{
				// add movement in that direction
				AddMovementInput(GetActorForwardVector(), Value / 3.0f);
			}
		}
	}
}

void AAmazeingCharacter::MoveRight(float Value)
{
	if (IsMovementEnabled)
	{
		if (Value != 0.0f)
		{
			if (!bClosingEyes)
			{
				// add movement in that direction
				AddMovementInput(GetActorRightVector(), Value);

				UGameplayStatics::PlayWorldCameraShake(GetWorld(), HeadBob, GetActorLocation(), 0.0f, 500.0f);
			}
			else
			{
				// add movement in that direction
				AddMovementInput(GetActorRightVector(), Value / 3.0f);
			}
		}
	}
}

void AAmazeingCharacter::TurnAtRate(float Rate)
{
	if (IsMovementEnabled)
	{
		// calculate delta for this frame from the rate information
		AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void AAmazeingCharacter::LookUpAtRate(float Rate)
{
	if (IsMovementEnabled)
	{
		// calculate delta for this frame from the rate information
		AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}

FGenericTeamId AAmazeingCharacter::GetGenericTeamId() const
{
	return TeamId;
}

void AAmazeingCharacter::FreezeCharacter()
{
	IsMovementEnabled = false;
}

void AAmazeingCharacter::UnfreezeCharacter()
{
	IsMovementEnabled = true;
}

void AAmazeingCharacter::OnPaused()
{
	// TO DO : On rouvre les yeux si TP avec yeux fermés ou si fini avec yeux fermés

	// If we are paused, unpause
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController->IsPaused())
	{
		if (PauseWidget)
		{
			PauseWidget->RemoveFromParent();
		}
		PlayerController->SetPause(false);
		PlayerController->bShowMouseCursor = false;
		PlayerController->SetInputMode(FInputModeGameOnly());
	}
	// If we are not paused, pause
	else
	{
		PlayerController->SetPause(true);
		PlayerController->bShowMouseCursor = true;
		PlayerController->SetInputMode(FInputModeGameAndUI());

		// If the widget is not created, create it
		if (PauseWidget == nullptr)
		{
			PauseWidget = CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), UMGPauseWidget);
		}

		// Show the UMG
		PauseWidget->AddToViewport(); // The constructor is run here, so we set here the variables of the widget
		PauseWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AAmazeingCharacter::InitializeLocation(FVector StartLocation)
{
	// Initialize the location for the footstep sounds
	PreviousLocation = StartLocation + FVector(0.0f, 0.0f, 98.150101);

	// Set the character location at the right place
	SetActorLocation(PreviousLocation);
}

void AAmazeingCharacter::Rotate(FRotator Rotation)
{
	// Rotate using the control rotation
	GetController()->SetControlRotation(Rotation);
}