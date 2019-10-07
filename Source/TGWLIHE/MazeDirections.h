// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MazeCell2DArray.h"
#include "MazeDirections.generated.h"

/**
 * Class with general functions to manipulate the Maze Cell Directions
 */
UCLASS()
class TGWLIHE_API UMazeDirections : public UObject
{
	GENERATED_BODY()

public:
	// Returns a random direction
	static EMazeDirection GetRandomMazeDirection();

	// Returns the vector corresponding to the direction
	static FIntVector ToIntVector(EMazeDirection Direction);

	// Returns the vector corresponding to the opposite direction
	static EMazeDirection GetOppositeDirection(EMazeDirection Direction);

	// Returns the rotation corresponding to the direction
	static FRotator GetRotation(EMazeDirection Direction);
	
public:
	// Number of direction
	static const int Count = 4;

private:
	// Vector of the direction
	static FIntVector DirectionVectors[Count];

	// Vector of the opposite direction
	static EMazeDirection OppositeDirections[Count];

	// Rotation of the direction
	static FRotator DirectionRotations[Count];
};
