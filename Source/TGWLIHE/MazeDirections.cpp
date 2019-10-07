// Fill out your copyright notice in the Description page of Project Settings.

#include "MazeDirections.h"

FIntVector UMazeDirections::DirectionVectors[] =
{
	FIntVector(0, 1, 0),
	FIntVector(-1, 0, 0),
	FIntVector(0, -1, 0),
	FIntVector(1, 0, 0),
};

EMazeDirection UMazeDirections::OppositeDirections[] =
{
	EMazeDirection::South,
	EMazeDirection::West,
	EMazeDirection::North,
	EMazeDirection::East
};

FRotator UMazeDirections::DirectionRotations[] =
{
	FRotator(0.0f, 0.0f, 0.0f),
	FRotator(0.0f, 90.0f, 0.0f),
	FRotator(0.0f, 180.0f, 0.0f),
	FRotator(0.0f, -90.0f, 0.0f)
};

EMazeDirection UMazeDirections::GetRandomMazeDirection()
{
	uint8 temp = FMath::RandRange(0, uint8(Count - 1));
	return (EMazeDirection)temp;
}

FIntVector UMazeDirections::ToIntVector(EMazeDirection Direction)
{
	return DirectionVectors[(uint8)Direction];
}

EMazeDirection UMazeDirections::GetOppositeDirection(EMazeDirection	Direction)
{
	return OppositeDirections[(uint8)Direction];
}

FRotator UMazeDirections::GetRotation(EMazeDirection Direction)
{
	return DirectionRotations[(uint8)Direction];
}


