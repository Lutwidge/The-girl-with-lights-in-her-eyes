// Fill out your copyright notice in the Description page of Project Settings.

#include "MonsterKill.h"
#include "AIMonsterController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Maze.h"
#include "EngineUtils.h"

UMonsterKill::UMonsterKill()
{
	// Grab the Maze Instance
	if (GetWorld())
	{
		for (TActorIterator<AMaze> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			Maze = *ActorItr;
			// UE_LOG(LogTemp, Warning, TEXT("ON PASSE BIEN PAR BT TASK NODE"));
		}
	}
}

EBTNodeResult::Type UMonsterKill::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Respawn the character
	Maze->RespawnCharacter();

	return EBTNodeResult::Succeeded;
}