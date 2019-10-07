// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AmazeingGameMode.generated.h"

// Declaration of event signature
DECLARE_EVENT(AAmazeingGameMode, FFade)

UCLASS(minimalapi)
class AAmazeingGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAmazeingGameMode();

	// Used to launch the FadeIn animation at the end of the transitions
	UFUNCTION()
		void LaunchFadeIn();

	// Used to launch the transition at the end of a "level" by updating the text and launching the animation
	UFUNCTION()
		void LaunchFadeOut(bool IsDeathKill);

	// Used to launch the transition at the end of the last level
	UFUNCTION()
		void LaunchLastFadeOut(bool IsDeathKill);

	// Holds all the actions to be conducted at the end of the FadeIn
	UFUNCTION()
		void FadeInActions();

	// Holds all the actions to be conducted at the end of the FadeOut
	UFUNCTION()
		void FadeOutActions();

	// Holds all the actions to be conducted at the end of the LastFadeOut
	UFUNCTION()
		void LastFadeOutActions();

	// Used to launch the fade in animation after the player is touched by a monster
	UFUNCTION()
		void LaunchMonsterKillFadeIn();

	// Used to launch the fade out animation when the player is touched by a monster
	UFUNCTION()
		void LaunchMonsterKillFadeOut();

	// Holds all the actions to be conducted at the end of the MonsterKillFadeIn
	UFUNCTION()
		void MonsterKillFadeInActions();

	// Holds all the actions to be conducted at the end of the MonsterKillFadeOut
	UFUNCTION()
		void MonsterKillFadeOutActions();

	// Accessors for events
	FFade& OnFadeInFinished() { return FadeInFinishedEvent; }
	FFade& OnFadeOutFinished() { return FadeOutFinishedEvent; }
	FFade& OnMonsterKillFadeInFinished() { return MonsterKillFadeInFinishedEvent; }
	FFade& OnMonsterKillFadeOutFinished() { return MonsterKillFadeOutFinishedEvent; }

	// Used for treating Death
	void DecrementIndex();

protected:
	virtual void BeginPlay() override;

	void GenerateMaze();

private:
	// Reference the UMG asset
	UPROPERTY()
		TSubclassOf<class UTransitionWidget> UMGTransitionWidget;

	// Variable to hold the Transition Widget after creating it
	UPROPERTY()
		UTransitionWidget* TransitionWidget;

	// Current Maze reference
	UPROPERTY()
		class AMaze* Maze;

	// Main Character reference
	UPROPERTY()
		class AAmazeingCharacter* MainCharacter;

	// AudioManager reference
	UPROPERTY()
		class AAudioManager* AudioManager;

	// Indicates the current Index of the game state
	int StateIndex;

	// Event for Fade In
	FFade FadeInFinishedEvent;

	// Event for Fade out
	FFade FadeOutFinishedEvent;

	// Event for Monster Kill Fade In
	FFade MonsterKillFadeInFinishedEvent;

	// Event for Monster Kill Fade out
	FFade MonsterKillFadeOutFinishedEvent;

	// Delegate used for removing a function from an event
	FDelegateHandle FadeOutHandle;
};