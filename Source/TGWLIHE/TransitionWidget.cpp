// Fill out your copyright notice in the Description page of Project Settings.

#include "TransitionWidget.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"

void UTransitionWidget::PlayFadeInAnimation()
{
	PlayAnimation(FadeInAnimation);
}

void UTransitionWidget::PlayFadeOutAnimation()
{
	PlayAnimation(FadeOutAnimation);
}

void UTransitionWidget::PlayLastFadeOutAnimation()
{
	PlayAnimation(LastFadeOutAnimation);
}

void UTransitionWidget::PlayMonsterKillFadeInAnimation()
{
	PlayAnimation(MonsterKillFadeInAnimation);
}

void UTransitionWidget::PlayMonsterKillFadeOutAnimation()
{
	PlayAnimation(MonsterKillFadeOutAnimation);
}

void UTransitionWidget::PlayRestartAnimation()
{
	PlayAnimation(RestartAnimation);
}


void UTransitionWidget::SetText(int32 Index)
{
	TextBlock->SetText(Text[Index]);
}