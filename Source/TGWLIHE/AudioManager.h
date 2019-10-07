// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AudioManager.generated.h"

UCLASS()
class TGWLIHE_API AAudioManager : public AActor
{
	GENERATED_BODY()

public:
	AAudioManager();

	// Plays the Death timer cue one time
	UFUNCTION()
		void PlayDeathSound();

	// Plays the Death timer cue on a timer
	UFUNCTION()
		void PlayDeathSoundWithTimer(int DeathArrivalTime);

	// Clears the Death timer
	UFUNCTION()
		void ClearDeathTimer();

	// Plays the Death arrival cue
	UFUNCTION()
		void PlayDeathArrivalSound();

	// Plays the Death Kill cue
	UFUNCTION()
		void PlayDeathKillSound();

	// Plays the Girl Scream cue
	UFUNCTION()
		void PlayGirlScream();

	// Plays the Music box cue
	UFUNCTION()
		void PlayMusicBox();

	// Plays the End music cue
	UFUNCTION()
		void PlayEndMusic();

	// Plays a footstep sound randomly
	void PlayFootstep();

	// Fade out the sound cue played by the Audio Component
	UFUNCTION()
	void FadeOutAmbient();

	// Fade in a new sound cue played by the Audio Component
	UFUNCTION()
	void FadeInAmbient();

	// Decrement the ambient sounds table index - used for Death Kill
	void DecrementAudioIndex();

	// Resets the ambient sounds table index - used for restarting the game
	void ResetAudioIndex();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// Cue of the Death timer
	UPROPERTY(EditAnywhere)
		class USoundBase* DeathTimerCue;

	// Cue of the Death arrival
	UPROPERTY(EditAnywhere)
		class USoundBase* DeathArrivalCue;

	// Cue of the Death Kill
	UPROPERTY(EditAnywhere)
		class USoundBase* DeathKillCue;

	// Cue of the Girl scream
	UPROPERTY(EditAnywhere)
		class USoundBase* GirlScreamCue;

	// Cue of the Music box
	UPROPERTY(EditAnywhere)
		class USoundBase* MusicBoxCue;

	// Cue of the End music
	UPROPERTY(EditAnywhere)
		class USoundBase* EndCue;

	// Maze instance
	UPROPERTY(EditAnywhere)
		class AMaze* Maze;

	// Footstep cues
	UPROPERTY(EditAnywhere)
		class USoundBase* FootstepCues[2];

	// Handle for the timer of Death sounds
	FTimerHandle DeathTimerHandle;

	// Previous index of the footstep cue played
	bool IsLeftFootstep;

	// Audio component that we will create in code
	UPROPERTY()
	UAudioComponent* AudioPlayer;

	// Other Audio component that we will create in code
	UPROPERTY()
	UAudioComponent* MusicBoxPlayer;

	// End trigger, used to subscribe to the event
	UPROPERTY(EditAnywhere)
		class AEndTriggerVolume* EndTrigger;

	// Ambient sound cues
	UPROPERTY(EditAnywhere)
		class USoundBase* AmbientCues[8];

	// Ambient sound table index
		uint8 AudioIndex;
};