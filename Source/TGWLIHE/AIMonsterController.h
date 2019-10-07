// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIMonsterController.generated.h"

UCLASS()
class TGWLIHE_API AAIMonsterController : public AAIController
{
	GENERATED_BODY()

public:
	AAIMonsterController();

	// Accessor for the Blackboard
	FORCEINLINE UBlackboardComponent* GetBlackboard() const { return BlackboardComponent; }

	// Resets knowledge of the target, so that when the player is touched and teleported, the Monster does not continue to follow
	void ResetPerception();

	// Resets the location of the monster
	void ResetLocation();

private:
	// Classic Possess method
	virtual void Possess(APawn* Pawn) override;

	// Determines if the AI Monster Blackboard "Attack" value should be set (=true) and if so, change it
	UFUNCTION()
		void OnPlayerSensed(const TArray<AActor*>& SensedActors);

	// Conducts all the needed actions when the player closes the eyes
	UFUNCTION()
		void EyesAreClosed();

	// Conducts all the needed actions when the player opens the eyes
	UFUNCTION()
		void EyesAreOpened();

public:
	// Name of the "HomeLocation" in the Blackboard
	UPROPERTY(EditAnywhere)
		FName HomeLocationKey;

	// Name of the "TargetLocation" in the Blackboard
	UPROPERTY(EditAnywhere)
		FName TargetLocationKey;

	// Name of the "Attack" in the Blackboard
	UPROPERTY(EditAnywhere)
		FName AttackKey;

	// Name of the "TargetToFollow" in the Blackboard
	UPROPERTY(EditAnywhere)
		FName TargetToFollowKey;

	// Half height of the Monsters, used for Spawning at the right place (avoid collisions on Spawn)
	UPROPERTY(EditAnywhere)
		float MonsterHalfHeight;

private:
	// Below are the needed pointers
	UPROPERTY()
		class UBehaviorTreeComponent* BehaviorTreeComponent;

	UPROPERTY()
		class UBlackboardComponent* BlackboardComponent;

	UPROPERTY()
		class AMaze* Maze;

	UPROPERTY()
		class AAmazeingCharacter* MainCharacter;

	UPROPERTY()
		class UAIPerceptionComponent* AIPerceptionComponent;

	UPROPERTY()
		class UAISenseConfig_Sight* SightConfig;

	// Are the eyes of the main character opened ?
	UPROPERTY()
		bool AreEyesOpened;

	// Used to store the Home Location, so that the Monster can teleport to it
	UPROPERTY()
		FVector HomeLocation;
};