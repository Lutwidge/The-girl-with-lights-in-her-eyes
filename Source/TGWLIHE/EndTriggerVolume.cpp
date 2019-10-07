// Fill out your copyright notice in the Description page of Project Settings.

#include "EndTriggerVolume.h"
#include "AmazeingCharacter.h"
#include "AmazeingGameMode.h"

AEndTriggerVolume::AEndTriggerVolume()
{
	// We register the event
	OnActorBeginOverlap.AddDynamic(this, &AEndTriggerVolume::OnOverlapBegin);
}

void AEndTriggerVolume::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
	if (Cast<AAmazeingCharacter>(OtherActor))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, TEXT("You found the exit !"));
		LaunchFadeOutEvent.Broadcast(false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EndTriggerVolume collided with something else than the Main Character"));
	}
}

void AEndTriggerVolume::DrawTrigger()
{
	//DrawDebugBox(GetWorld(), GetActorLocation(), GetActorScale() * 100, FColor::Cyan, true, -1, 0, 5);
}

void AEndTriggerVolume::DeathBroadcast()
{
	LaunchFadeOutEvent.Broadcast(true);
}