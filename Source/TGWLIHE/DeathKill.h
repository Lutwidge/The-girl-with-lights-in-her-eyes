// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "DeathKill.generated.h"

/**
*
*/
UCLASS()
class TGWLIHE_API UDeathKill : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UDeathKill();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	// EndTriggerVolume reference
	class AEndTriggerVolume* EndTriggerVolume;

};