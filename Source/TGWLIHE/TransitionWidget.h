// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TransitionWidget.generated.h"

UCLASS()
class TGWLIHE_API UTransitionWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// Fade in animation, defined in the UMG and assigned through the UMG blueprint "Begin Play" event
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UWidgetAnimation* FadeInAnimation;

	// Fade out animation, defined in the UMG and assigned through the UMG blueprint "Begin Play" event
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UWidgetAnimation* FadeOutAnimation;

	// Monster Kill Fade in animation, defined in the UMG and assigned through the UMG blueprint "Begin Play" event
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UWidgetAnimation* MonsterKillFadeInAnimation;

	// Monster Kill Fade out animation, defined in the UMG and assigned through the UMG blueprint "Begin Play" event
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UWidgetAnimation* MonsterKillFadeOutAnimation;

	// Last Fade out animation, defined in the UMG and assigned through the UMG blueprint "Begin Play" event
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UWidgetAnimation* LastFadeOutAnimation;

	// Restart animation, defined in the UMG and assigned through the UMG blueprint "Begin Play" event
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UWidgetAnimation* RestartAnimation;

	// Text block, used to change the text
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Text)
		class UTextBlock* TextBlock;

	// Table containing the text, editable in the UMG directly
	UPROPERTY(EditAnywhere, Category = "Transition Text", meta = (MultiLine = true))
		FText Text[10];

public:
	// Play the Fade In Animation
	void PlayFadeInAnimation();

	// Play the Fade Out Animation
	void PlayFadeOutAnimation();

	// Play the Last Fade Out Animation
	void PlayLastFadeOutAnimation();

	// Play the Monster Kill Fade In Animation
	void PlayMonsterKillFadeInAnimation();

	// Play the Monster Kill Fade Out Animation
	void PlayMonsterKillFadeOutAnimation();

	// Play the Restart Animation
	void PlayRestartAnimation();

	// Set the text of the Text block
	void SetText(int32 Index);
};