// Fill out your copyright notice in the Description page of Project Settings.

#include "MonsterResetPerception.h"
#include "AIMonsterController.h"

EBTNodeResult::Type UMonsterResetPerception::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Reset the perception
	AAIMonsterController* AICon = Cast<AAIMonsterController>(OwnerComp.GetAIOwner());
	if (AICon)
	{
		AICon->ResetPerception();
	}

	return EBTNodeResult::Succeeded;
}


