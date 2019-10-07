// Fill out your copyright notice in the Description page of Project Settings.

#include "MonsterResetLocation.h"
#include "AIMonsterController.h"

EBTNodeResult::Type UMonsterResetLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Reset the perception
	AAIMonsterController* AICon = Cast<AAIMonsterController>(OwnerComp.GetAIOwner());
	if (AICon)
	{
		AICon->ResetLocation();
	}

	return EBTNodeResult::Succeeded;
}


