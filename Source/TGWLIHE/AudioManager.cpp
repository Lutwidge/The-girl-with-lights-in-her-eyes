// Fill out your copyright notice in the Description page of Project Settings.

#include "AudioManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Maze.h"
#include "EndTriggerVolume.h"
#include "Runtime/Engine/Classes/Components/AudioComponent.h"

AAudioManager::AAudioManager()
{
	// Create the audio component
	AudioPlayer = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioPlayer"));

	// Create the other audio component
	MusicBoxPlayer = CreateDefaultSubobject<UAudioComponent>(TEXT("MusicBoxPlayer"));
}

// Called when the game starts or when spawned
void AAudioManager::BeginPlay()
{
	Super::BeginPlay();

	// Subscribe to the Maze event related to the presence of Death, to launch the audio timer
	Maze->OnDeathPresent().AddUFunction(this, FName("PlayDeathSoundWithTimer"));

	// Subscribe to the Maze event related to the arrival of Death, to remove the timer and play the arrival sound cue
	Maze->OnDeathArrival().AddUFunction(this, FName("PlayDeathArrivalSound"));

	// Subscribe to the Maze event related to the Fade In of the level, so that we fade in a new ambient sound at the same time
	Maze->OnTransitionFinished().AddUFunction(this, FName("FadeInAmbient"));

	// Subscribe to the EndTrigger event related to the Fade Out of the level, so that we fade out the sounds at the same time
	EndTrigger->OnFadeOutLaunched().AddUFunction(this, FName("FadeOutAmbient"));

	// Same, but this time to remove the Death timer (so that it does not continue after the end of the level!)
	EndTrigger->OnFadeOutLaunched().AddUFunction(this, FName("ClearDeathTimer"));

	// Initialize the other variables
	IsLeftFootstep = false;
	AudioIndex = 0;
}

void AAudioManager::PlayDeathSound()
{
	if (DeathTimerCue)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), DeathTimerCue, 1.0f, 1.0f, 0.0f);
	}
}

void AAudioManager::PlayDeathSoundWithTimer(int DeathArrivalTime)
{
	// Create the timer for the death sound
	GetWorld()->GetTimerManager().SetTimer(DeathTimerHandle, this, &AAudioManager::PlayDeathSound, DeathArrivalTime / 5.0f, true);
}

void AAudioManager::ClearDeathTimer()
{
	// Remove the Death sound timer
	GetWorld()->GetTimerManager().ClearTimer(DeathTimerHandle);
}

void AAudioManager::PlayDeathArrivalSound()
{
	// Remove the Death sound timer
	ClearDeathTimer();

	// Play the last sound
	PlayDeathSound();

	// Play the sound
	if (DeathArrivalCue)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), DeathArrivalCue, 1.0f, 1.0f, 0.0f);
	}
}

void AAudioManager::PlayFootstep()
{
	if (IsLeftFootstep)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), FootstepCues[0], 0.05f, 1.0f, 0.0f);
		IsLeftFootstep = false;
	}
	else
	{
		UGameplayStatics::PlaySound2D(GetWorld(), FootstepCues[1], 0.05f, 1.0f, 0.0f);
		IsLeftFootstep = true;
	}
}

void AAudioManager::PlayDeathKillSound()
{
	// Play the sound
	if (DeathKillCue)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), DeathKillCue, 1.0f, 1.0f, 0.0f);
	}
}

void AAudioManager::PlayGirlScream()
{
	// Play the sound
	if (GirlScreamCue)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), GirlScreamCue, 1.0f, 1.0f, 0.0f);
	}
}

void AAudioManager::PlayMusicBox()
{
	// Play the sound
	if (MusicBoxCue)
	{
		MusicBoxPlayer->SetSound(MusicBoxCue);
		MusicBoxPlayer->Play(0.0f);
	}
}

void AAudioManager::PlayEndMusic()
{
	// Play the sound
	if (EndCue)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), EndCue, 1.0f, 1.0f, 0.0f);
	}
}

void AAudioManager::FadeInAmbient()
{
	if (AmbientCues[AudioIndex])
	{
		AudioPlayer->SetSound(AmbientCues[AudioIndex]);
		AudioPlayer->FadeIn(2.0f, 1.0f, 0.0f);
		AudioIndex += 1;
	}
}

void AAudioManager::FadeOutAmbient()
{
	AudioPlayer->FadeOut(2.0f, 0.0f);
}

void AAudioManager::DecrementAudioIndex()
{
	AudioIndex -= 1;
}

void AAudioManager::ResetAudioIndex()
{
	AudioIndex = 0;
}