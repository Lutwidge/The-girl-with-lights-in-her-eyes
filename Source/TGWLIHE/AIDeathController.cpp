// Fill out your copyright notice in the Description page of Project Settings.

#include "AIDeathController.h"
#include "AICharacter.h"
#include "EngineUtils.h"
#include "AmazeingCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

AAIDeathController::AAIDeathController()
{
	// Initialize BehaviorTreeComponent, BlackboardComponent, and AIPerceptionComponent
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

	// Necessary to check if there is a world as we are in the constructor, if not bug due to hot reload ! Works fine in game
	if (GetWorld())
	{
		// Grab the Main Character Instance
		for (TActorIterator<AAmazeingCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			MainCharacter = *ActorItr;
			//UE_LOG(LogTemp, Warning, TEXT("Got the character"));
		}
	}

	// Assign to Team 1, used for the AI Perception to see the player as an Enemy
	SetGenericTeamId(FGenericTeamId(1));
}

void AAIDeathController::Possess(APawn* Pawn)
{
	Super::Possess(Pawn);

	// Get the possessed character and check if it's one of the monsters
	AAICharacter* AIDeath = Cast<AAICharacter>(Pawn);

	if (AIDeath)
	{
		// If so, we initialize the blackboard (if it is valid) for the corresponding behavior tree
		if (AIDeath->BehaviorTree->BlackboardAsset)
		{
			BlackboardComponent->InitializeBlackboard(*(AIDeath->BehaviorTree->BlackboardAsset));

			// We then initialize the values
			BlackboardComponent->SetValueAsObject(TargetToFollowKey, MainCharacter);
		}
		// Start the behavior tree
		BehaviorTreeComponent->StartTree(*AIDeath->BehaviorTree);
	}
}