// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
class APostProcessVolume;
#include "Runtime/AIModule/Classes/GenericTeamAgentInterface.h"
#include "AmazeingCharacter.generated.h"
class UInputComponent;

// Declaration of event signature
DECLARE_EVENT(AAmazeingCharacter, FEyesMovement)

// We implement the Team interface for the AI Perception component to recognize the player as en Enemy
UCLASS(config = Game)
class AAmazeingCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AAmazeingCharacter();

	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	/** Deactivates Inputs */
	UFUNCTION()
		void FreezeCharacter();

	/** Reactivates Inputs */
	UFUNCTION()
		void UnfreezeCharacter();

	// Initializes the character location, and gives the first location for the footstep sounds
	void InitializeLocation(FVector StartLocation);

	// Rotates the character using its controller, used for the last level
	void Rotate(FRotator Rotator);

protected:
	virtual void BeginPlay();

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

	/** Activates the closing of the eyes */
	void OnCloseEyesPressed();

	/** Deactivates the closing of the eyes */
	void OnCloseEyesReleased();

	/** Reopen the eyes, used also for an event in order to always have the eyes opened at the start of a level */
	UFUNCTION()
	void ReopenEyes();

	// Actions to conduct when pause is activated
	void OnPaused();

private:
	// Accessors for the team, used for the AI Perception to see the player as an Enemy
	virtual FGenericTeamId GetGenericTeamId() const override;

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	// Accessors to the instances of the signature defined below
	FEyesMovement& OnEyesClosed() { return EyesClosedEvent; }

	FEyesMovement& OnEyesOpened() { return EyesOpenedEvent; }

private:
	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FirstPersonCameraComponent;

	/** Postprocess volume, here used for the postprocess material of eyes closing */
	UPROPERTY(EditAnywhere, Category = Effects)
		APostProcessVolume* PostProcessVolume;

	// Used to activate the blendable for the eyes
	UPROPERTY(EditAnywhere, Category = Effects)
		UMaterialInterface* EyesBlendable;

	/** Used to stock the eyelids dynamic postprocess material instance */
	UPROPERTY()
		UMaterialInstanceDynamic* DynamicEyesMaterial;

	// Pointer to the end trigger volume to subscribe to its event, in order to freeze the player
	UPROPERTY(EditAnywhere, Category = EndTrigger)
		class AEndTriggerVolume* EndTriggerVolume;

	/** Used to determine where we are on the "closing eyes" effect */
	UPROPERTY(VisibleAnywhere)
		float ClosingFactor;

	/** Are we closing the eyes of the character or not ? Also used to lock the character in place. */
	UPROPERTY(VisibleAnywhere)
		bool bClosingEyes;

	// Event launched when the player closes the eyes
	FEyesMovement EyesClosedEvent;

	// Event launched when the player reopens the eyes
	FEyesMovement EyesOpenedEvent;

	// Headbob defined in a Blueprint
	UPROPERTY(EditAnywhere, Category = HeadBob)
		TSubclassOf<class UCameraShake> HeadBob;

	// Used for defining the team, used for the AI Perception to see the player as an Enemy
	FGenericTeamId TeamId;

	// Used to check whether or not the movement inputs should be taken into account
	bool IsMovementEnabled;

	// Pause UMG
	UPROPERTY(EditAnywhere, Category = Pause)
		TSubclassOf<class UUserWidget> UMGPauseWidget;

	// Stores the Pause widget
	UPROPERTY()
		UUserWidget* PauseWidget;

	// The Maze
	UPROPERTY(EditAnywhere, Category = Maze)
		class AMaze* Maze;

	// Last recorded location for the footstep sounds
	FVector PreviousLocation;

	// Current distance walked
	float CurrentDistance;

	// Audio manager
	UPROPERTY(EditAnywhere, Category = Audio)
		class AAudioManager* AudioManager;
};