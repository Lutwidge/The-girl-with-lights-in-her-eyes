// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MonsterResetLocation.generated.h"

/**
 * 
 */
UCLASS()
class TGWLIHE_API UMonsterResetLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	// Our customized Behavior Tree Task
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	
};
