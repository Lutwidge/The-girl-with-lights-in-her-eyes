// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MonsterResetPerception.generated.h"

/**
 * 
 */
UCLASS()
class TGWLIHE_API UMonsterResetPerception : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	// Our customized Behavior Tree Task
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
