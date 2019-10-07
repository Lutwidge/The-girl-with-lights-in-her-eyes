// Fill out your copyright notice in the Description page of Project Settings.

#include "DeathKill.h"
#include "EndTriggerVolume.h"
#include "EngineUtils.h"
#include "AmazeingGameMode.h"

UDeathKill::UDeathKill()
{
	// Grab the Maze Instance
	if (GetWorld())
	{
		for (TActorIterator<AEndTriggerVolume> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			EndTriggerVolume = *ActorItr;
		}
	}
}

EBTNodeResult::Type UDeathKill::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Find the GameMode
	// TO DO: Change this for an event. Static ? Otherwise, how to find the instance with the event ? -> Architecture change needed
	if (GetWorld())
	{
		AAmazeingGameMode* GameMode = (AAmazeingGameMode*)GetWorld()->GetAuthGameMode();
		GameMode->DecrementIndex();
	}

	// Then, behave like the end trigger volume was touched, instead it restarts the same level
	EndTriggerVolume->DeathBroadcast();

	return EBTNodeResult::Succeeded;
}


