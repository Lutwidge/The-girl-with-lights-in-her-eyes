// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "AmazeingGameMode.h"
#include "AmazeingHUD.h"
#include "AmazeingCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "TransitionWidget.h"
#include "Maze.h"
#include "EngineUtils.h"
#include "Runtime/UMG/Public/Animation/UMGSequencePlayer.h"
#include "EndTriggerVolume.h"
#include "AudioManager.h"

// TO DO: Decouple logic and UI functionalities -> Create another class or use the Transition Widget
AAmazeingGameMode::AAmazeingGameMode()
	: Super()
{
	// Set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/Character/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// Use our custom HUD class
	HUDClass = AAmazeingHUD::StaticClass();

	// Initialize the state index
	StateIndex = 0;

	// Grab the classes of the UMG
	static ConstructorHelpers::FObjectFinder<UClass> UMGClassFinder(TEXT("Class'/Game/Blueprints/UI/Transitions.Transitions_C'"));
	if (UMGClassFinder.Object)
	{
		UMGTransitionWidget = UMGClassFinder.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: Blueprint UMG not found"));
	}

	// Grab the Maze & Main Character Instances
	if (GetWorld())
	{
		for (TActorIterator<AMaze> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			Maze = *ActorItr;
			//UE_LOG(LogTemp, Warning, TEXT("Got the maze"));
		}

		// Grab the Main Character Instance
		for (TActorIterator<AAmazeingCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			MainCharacter = *ActorItr;
			//UE_LOG(LogTemp, Warning, TEXT("Got the character"));
		}

		// Grab the AudioManager instance
		for (TActorIterator<AAudioManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			AudioManager = *ActorItr;
		}
	}
}

void AAmazeingGameMode::BeginPlay()
{
	Super::BeginPlay();

	// First, get the Transition Widget and make it visible
	if (UMGTransitionWidget) // Check if the Asset is assigned in the blueprint.
	{
		// Create the widget and store it.
		TransitionWidget = CreateWidget<UTransitionWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), UMGTransitionWidget);

		if (TransitionWidget)
		{
			// Show the UMG at the start of the game with the right text
			TransitionWidget->AddToViewport(); // The constructor is run here, so we set here the variables of the widget
			TransitionWidget->SetText(StateIndex); // Here, the text block should be available, so we set the text
			TransitionWidget->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("TransitionWidget in AmazeingGameMode undefined"));
		}
	}

	// Play the Music box
	AudioManager->PlayMusicBox();

	// Then, generate the first Maze
	UWorld* const World = GetWorld();
	if (World)
	{
		/// Subscribe to the necessary events
		// Launch the fade in actions once the generation of the maze is finished (maze generated && minimal time elapsed)
		Maze->OnTransitionFinished().AddUFunction(this, FName("LaunchFadeIn"));
		// Launch the fade out actions once the end trigger is touched
		FadeOutHandle = Maze->EndTriggerVolume->OnFadeOutLaunched().AddUFunction(this, FName("LaunchFadeOut"));
		// Launch the Monster Kill Fade Out animation when the player is touched by a monster
		Maze->OnMonsterKill().AddUFunction(this, FName("LaunchMonsterKillFadeOut"));
		// Launch the Monster Kill Fade In animation when the player location has been reset
		Maze->OnLocationReset().AddUFunction(this, FName("LaunchMonsterKillFadeIn"));

		// Generate the first Maze
		GenerateMaze();
	}
}

void AAmazeingGameMode::LaunchFadeIn()
{
	// Launch the fade in animation
	TransitionWidget->PlayFadeInAnimation();

	// Call "FadeInActions" at the end of the animation
	TransitionWidget->ActiveSequencePlayers.Last()->OnSequenceFinishedPlaying().RemoveAll(this);
	TransitionWidget->ActiveSequencePlayers.Last()->OnSequenceFinishedPlaying().AddUFunction(this, FName("FadeInActions"));
}

void AAmazeingGameMode::FadeInActions()
{
	// Hide the UMG
	TransitionWidget->SetVisibility(ESlateVisibility::Hidden);

	// Broadcast the Fade In Finished Event, so that other classes can also profit from the "OnSequenceFinishedPlaying()" event
	FadeInFinishedEvent.Broadcast();
}

void AAmazeingGameMode::LaunchFadeOut(bool IsDeathKill)
{
	if (IsDeathKill)
	{
		AudioManager->PlayGirlScream();
		AudioManager->PlayDeathKillSound();
	}
	else
	{
		// PLay the music box
		AudioManager->PlayMusicBox();
	}

	// Change the text to appear
	TransitionWidget->SetText(StateIndex);

	// Launch the fadeout animation
	TransitionWidget->SetVisibility(ESlateVisibility::Visible);
	TransitionWidget->PlayFadeOutAnimation();

	// Call "FadeOutActions" at the end of the animation
	TransitionWidget->ActiveSequencePlayers.Last()->OnSequenceFinishedPlaying().RemoveAll(this);
	TransitionWidget->ActiveSequencePlayers.Last()->OnSequenceFinishedPlaying().AddUFunction(this, FName("FadeOutActions"));
}

void AAmazeingGameMode::FadeOutActions()
{
	// Recreates a maze, the previous one has been destroyed following the event sent by the EndTriggerVolume
	GenerateMaze();

	// Broadcast the Fade In Finished Event, so that other classes can also profit from the "OnSequenceFinishedPlaying()" event
	FadeOutFinishedEvent.Broadcast();
}

void AAmazeingGameMode::GenerateMaze()
{
	// Depending on the state index, generate a different maze
	switch (StateIndex)
	{
	case 0:
		Maze->Generate(4, 4, 0, 0);
		StateIndex += 1;
		break;

	case 1:
		Maze->Generate(7, 7, 0, 0);
		StateIndex += 1;
		break;

	case 2:
		Maze->Generate(20, 1, 1, 10);
		StateIndex += 1;
		break;

	case 3:
		Maze->Generate(8, 8, 2, 15);
		StateIndex += 1;
		break;

	case 4:
		Maze->Generate(10, 10, 5, 15, 100);
		StateIndex += 1;
		break;

	case 5:
		Maze->Generate(15, 15, 8, 20, 300);
		StateIndex += 1;
		break;

	case 6:
		Maze->GenerateLastLevel();
		// Remove the previous Fade Out callback and replace it by the one for the end game
		Maze->EndTriggerVolume->OnFadeOutLaunched().Remove(FadeOutHandle);
		FadeOutHandle = Maze->EndTriggerVolume->OnFadeOutLaunched().AddUFunction(this, FName("LaunchLastFadeOut"));
		// Warn Maze that it should not Destroy the Maze juste yet - remove the callback for now
		Maze->UnsubscribeDestroyMaze();
		StateIndex += 1;
		break;

	default:
		Maze->Generate(FMath::RandRange(10, 30), FMath::RandRange(10, 30), FMath::RandRange(10, 30), FMath::RandRange(10, 30), FMath::RandRange(60, 240));
		StateIndex += 1;
	}
}

void AAmazeingGameMode::DecrementIndex()
{
	// We will reload the same state next time the maze is generated
	StateIndex -= 1;

	// Also warns the audio manager that the ambient sound should be the same as before
	AudioManager->DecrementAudioIndex();
}

void AAmazeingGameMode::LaunchMonsterKillFadeOut()
{
	// Show the UMG
	TransitionWidget->SetVisibility(ESlateVisibility::Visible);

	// Launch the fade in animation
	TransitionWidget->PlayMonsterKillFadeOutAnimation();

	// Call "MonsterKillFadeOutActions" at the end of the animation
	TransitionWidget->ActiveSequencePlayers.Last()->OnSequenceFinishedPlaying().RemoveAll(this);
	TransitionWidget->ActiveSequencePlayers.Last()->OnSequenceFinishedPlaying().AddUFunction(this, FName("MonsterKillFadeOutActions"));
}

void AAmazeingGameMode::MonsterKillFadeOutActions()
{
	// Broadcast the Monster Kill Fade Out Finished Event, so that other classes can also profit from the "OnSequenceFinishedPlaying()" event
	MonsterKillFadeOutFinishedEvent.Broadcast();
}

void AAmazeingGameMode::LaunchMonsterKillFadeIn()
{
	// Launch the fade in animation
	TransitionWidget->PlayMonsterKillFadeInAnimation();

	// Call "MonsterKillFadeOutActions" at the end of the animation
	TransitionWidget->ActiveSequencePlayers.Last()->OnSequenceFinishedPlaying().RemoveAll(this);
	TransitionWidget->ActiveSequencePlayers.Last()->OnSequenceFinishedPlaying().AddUFunction(this, FName("MonsterKillFadeInActions"));
}

void AAmazeingGameMode::MonsterKillFadeInActions()
{
	// Hide the UMG
	TransitionWidget->SetVisibility(ESlateVisibility::Hidden);

	// Broadcast the Monster Kill Fade In Finished Event, so that other classes can also profit from the "OnSequenceFinishedPlaying()" event
	MonsterKillFadeInFinishedEvent.Broadcast();
}

void AAmazeingGameMode::LaunchLastFadeOut(bool IsDeathKill)
{
		// TO DO: Lancer sons de Nico !
		// + Lancer son au début du jeu aussi
		// + Lancer son dans le LastFadeOutActions pour le reset du jeu aussi
	AudioManager->PlayEndMusic();

	// Change the text to appear
	TransitionWidget->SetText(StateIndex);

	// Launch the fadeout animation
	TransitionWidget->SetVisibility(ESlateVisibility::Visible);
	TransitionWidget->PlayLastFadeOutAnimation();

	// Call "LastFadeOutActions" at the end of the animation
	TransitionWidget->ActiveSequencePlayers.Last()->OnSequenceFinishedPlaying().RemoveAll(this);
	TransitionWidget->ActiveSequencePlayers.Last()->OnSequenceFinishedPlaying().AddUFunction(this, FName("LastFadeOutActions"));
}

void AAmazeingGameMode::LastFadeOutActions()
{
	/// Restart the game
	// Remove the previous Last Fade Out callback and replace it by the one for the game
	Maze->EndTriggerVolume->OnFadeOutLaunched().Remove(FadeOutHandle);
	FadeOutHandle = Maze->EndTriggerVolume->OnFadeOutLaunched().AddUFunction(this, FName("LaunchFadeOut"));
	// Subscribe again to the Destroy Maze so that it is used in the game
	Maze->SubscribeDestroyMaze();

	// Reset the state index
	StateIndex = 0;
	AudioManager->ResetAudioIndex();

	// Destroy the current maze
	Maze->DestroyMaze(false);

	// Reset the end trigger scale
	Maze->ResetEndTriggerScale();

	// Launch the UMG animation
	TransitionWidget->SetText(StateIndex);
	TransitionWidget->PlayRestartAnimation();

	// Generate the first maze
	GenerateMaze();
}