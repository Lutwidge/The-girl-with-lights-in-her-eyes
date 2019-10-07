// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include "AIDeathController.generated.h"

UCLASS()
class TGWLIHE_API AAIDeathController : public AAIController
{
	GENERATED_BODY()

public:
	AAIDeathController();

	// Accessor for the Blackboard
	FORCEINLINE UBlackboardComponent* GetBlackboard() const { return BlackboardComponent; }

private:
	// Classic Possess method
	virtual void Possess(APawn* Pawn) override;

public:
	// Name of the "TargetToFollow" in the Blackboard
	UPROPERTY(EditAnywhere)
		FName TargetToFollowKey;

	// Half height of the Death Character, used for Spawning at the right place (avoid collisions on Spawn)
	UPROPERTY(EditAnywhere)
		float DeathHalfHeight;

private:
	// Below are the needed pointers
	UPROPERTY()
		class UBehaviorTreeComponent* BehaviorTreeComponent;

	UPROPERTY()
		class UBlackboardComponent* BlackboardComponent;

	UPROPERTY()
		class AAmazeingCharacter* MainCharacter;
};