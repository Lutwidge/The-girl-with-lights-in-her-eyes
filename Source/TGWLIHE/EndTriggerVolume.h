// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "DrawDebugHelpers.h"
#include "EndTriggerVolume.generated.h"

// Declaration of event signature
DECLARE_EVENT_OneParam(AEndTriggerVolume, FFadeOut, bool)
/**
*
*/
UCLASS()
class TGWLIHE_API AEndTriggerVolume : public ATriggerVolume
{
	GENERATED_BODY()

public:
	// Constructor sets default values for this actor's properties
	AEndTriggerVolume();

	// Overlap begin function
	UFUNCTION()
		void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

	// Draws the volume
	void DrawTrigger();

	// Accessor for the event
	FFadeOut& OnFadeOutLaunched() { return LaunchFadeOutEvent; }

	// Launch the death actions, used by the Behavior tree custom Kill task of Death
	void DeathBroadcast();

private:
	// Event based on the delegate signature
	FFadeOut LaunchFadeOutEvent;
};