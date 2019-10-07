// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MonsterKill.generated.h"

/**
*
*/
UCLASS()
class TGWLIHE_API UMonsterKill : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UMonsterKill();

	// Our customized Behavior Tree Task
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	// Maze pointer, used here to reset the character location to the beginning
	class AMaze* Maze;
};