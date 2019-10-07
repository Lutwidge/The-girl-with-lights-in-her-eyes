// Fill out your copyright notice in the Description page of Project Settings.

#include "AIMonsterController.h"
#include "AICharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EngineUtils.h"
#include "Maze.h"
#include "AmazeingCharacter.h"
#include "Runtime/AIModule/Classes/Perception/AIPerceptionComponent.h"
#include "Runtime/AIModule/Classes/Perception/AISenseConfig_Sight.h"
#include "Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h"

AAIMonsterController::AAIMonsterController()
{
	// Initialize BehaviorTreeComponent, BlackboardComponent, and AIPerceptionComponent
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception Component"));

	// Configure the Sight Sense of AI Perception
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	SightConfig->SightRadius = 10000.0f;
	SightConfig->LoseSightRadius = 15000.0f;
	SightConfig->PeripheralVisionAngleDegrees = 360.0f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

	// IMPORTANT : Set the perception component as this one, as the base AIController class has already one !
	// Add a listener to the Perception event, the function needs to be a UFUNCTION !
	AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AAIMonsterController::OnPlayerSensed);
	SetPerceptionComponent(*AIPerceptionComponent);

	// Necessary to check if there is a world as we are in the constructor, if not bug due to hot reload ! Works fine in game
	if (GetWorld())
	{
		for (TActorIterator<AMaze> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			Maze = *ActorItr;
			//UE_LOG(LogTemp, Warning, TEXT("Got the maze"));
		}

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

	// Initially set the Main Character eyes as closed
	AreEyesOpened = false;
}

void AAIMonsterController::Possess(APawn* Pawn)
{
	Super::Possess(Pawn);

	// Get the possessed character and check if it's one of the monsters
	AAICharacter* AIMonster = Cast<AAICharacter>(Pawn);

	if (AIMonster)
	{
		// If so, we initialize the blackboard (if it is valid) for the corresponding behavior tree
		if (AIMonster->BehaviorTree->BlackboardAsset)
		{
			BlackboardComponent->InitializeBlackboard(*(AIMonster->BehaviorTree->BlackboardAsset));

			// We then initialize the values
			// TO DO : voir si selon axe Z change quoi que ce soit ?
			TArray<FVector> MonsterLocations = Maze->CreateAIPath();
			HomeLocation = MonsterLocations[0] + FVector(0.0f, 0.0f, MonsterHalfHeight);
			BlackboardComponent->SetValueAsVector(HomeLocationKey, HomeLocation);
			BlackboardComponent->SetValueAsVector(TargetLocationKey, MonsterLocations[1] + FVector(0.0f, 0.0f, MonsterHalfHeight));
			BlackboardComponent->SetValueAsBool(AttackKey, false);

			AIMonster->SetActorRelativeLocation(MonsterLocations[0] + FVector(0.0f, 0.0f, MonsterHalfHeight));
			//UE_LOG(LogTemp, Warning, TEXT("Monster position is %s"), *AIMonster->GetActorLocation().ToString());
		}

		// Subscribe to the correct AAmazeingCharacter events
		MainCharacter->OnEyesClosed().AddUFunction(this, FName("EyesAreClosed"));
		MainCharacter->OnEyesOpened().AddUFunction(this, FName("EyesAreOpened"));
		// Subscribe to the transition finish event, so that the AI can properly respond to the presence of the player after the text is done being shown
		Maze->OnTransitionFinished().AddUFunction(this, FName("EyesAreOpened"));

		// Start the behavior tree
		BehaviorTreeComponent->StartTree(*AIMonster->BehaviorTree);

		// Initially, the player cannot be detected (while the text appears)
		EyesAreClosed();
	}
}

void AAIMonsterController::OnPlayerSensed(const TArray<AActor*>& SensedActors)
{
	// If the eyes are opened
	if (AreEyesOpened)
	{
		// Check if the AI sees the player (Event called on both entering and exiting AI field of view, here we only want the entering)
		for (int i = 0; i < SensedActors.Num(); i++)
		{
			const FActorPerceptionInfo* Info = AIPerceptionComponent->GetActorInfo(*SensedActors[i]);
			if (Info && Info->LastSensedStimuli.Num() > 0)
			{
				// Here, we successfully sensed the player
				if (Info->LastSensedStimuli[0].WasSuccessfullySensed())
				{
					// If so, launch the "attack" and change the AI Character speed
					BlackboardComponent->SetValueAsObject(TargetToFollowKey, MainCharacter);
					BlackboardComponent->SetValueAsBool(AttackKey, true);

					// Change the walking speed
					AAICharacter* AIMonster = Cast<AAICharacter>(GetPawn());
					if (AIMonster)
					{
						UCharacterMovementComponent* AIMovement = Cast<UCharacterMovementComponent>(AIMonster->GetCharacterMovement());
						if (AIMovement)
						{
							AIMovement->MaxWalkSpeed = 650.0f;
						}
					}
				}
				// Here, we lost sight of him : stop following the player
				else
				{
					// Changing the Blackboard value removes the track behavior of the behavior tree (as we have abort set)
					BlackboardComponent->SetValueAsBool(AttackKey, false);

					// Reset the walking speed
					AAICharacter* AIMonster = Cast<AAICharacter>(GetPawn());
					if (AIMonster)
					{
						UCharacterMovementComponent* AIMovement = Cast<UCharacterMovementComponent>(AIMonster->GetCharacterMovement());
						if (AIMovement)
						{
							AIMovement->MaxWalkSpeed = 50.0f;
						}
					}
				}
			}
		}
	}
}

void AAIMonsterController::EyesAreClosed()
{
	AreEyesOpened = false;

	// Changing the Blackboard value removes the track behavior of the behavior tree (as we have abort set)
	BlackboardComponent->SetValueAsBool(AttackKey, false);

	// Reset the walking speed
	AAICharacter* AIMonster = Cast<AAICharacter>(GetPawn());
	if (AIMonster)
	{
		UCharacterMovementComponent* AIMovement = Cast<UCharacterMovementComponent>(AIMonster->GetCharacterMovement());
		if (AIMovement)
		{
			AIMovement->MaxWalkSpeed = 50.0f;
		}
	}
}
void AAIMonsterController::EyesAreOpened()
{
	AreEyesOpened = true;

	// Reset the perception so that the OnPlayerSensed function will be called again at this point
	AIPerceptionComponent->ForgetAll();
}

void AAIMonsterController::ResetPerception()
{
	// Changing the Blackboard value removes the track behavior of the behavior tree (as we have abort set)
	BlackboardComponent->SetValueAsBool(AttackKey, false);

	// Reset the walking speed
	AAICharacter* AIMonster = Cast<AAICharacter>(GetPawn());
	if (AIMonster)
	{
		UCharacterMovementComponent* AIMovement = Cast<UCharacterMovementComponent>(AIMonster->GetCharacterMovement());
		if (AIMovement)
		{
			AIMovement->MaxWalkSpeed = 50.0f;
		}
	}

	// Reset the perception so that the OnPlayerSensed function will be called again at this point
	AIPerceptionComponent->ForgetAll();
}

void AAIMonsterController::ResetLocation()
{
	// Reset the walking speed
	AAICharacter* AIMonster = Cast<AAICharacter>(GetPawn());
	if (AIMonster)
	{
		AIMonster->SetActorLocation(HomeLocation);
	}
}