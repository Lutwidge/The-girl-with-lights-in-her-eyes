// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MazeDirections.h"
class AMazeCellEdge;
#include "MazeCell.generated.h"

UCLASS(Blueprintable, ClassGroup = Maze)
class TGWLIHE_API AMazeCell : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMazeCell();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// Sets Coordinates to a new value
	void SetCoordinates(FIntVector NewCoordinates);

	// Gets the coordinates value
	FIntVector GetCoordinates();

	// Gets the edge
	AMazeCellEdge* GetEdge(EMazeDirection Direction);

	// Get the passage edges array, used for the AI path
	TArray<AMazeCellEdge*>  GetPassageEdges();

	// Sets the edge
	void SetEdge(EMazeDirection Direction, AMazeCellEdge* Edge);

	// Determines whether or not the cell has all its edges already initialized
	bool IsFullyInitialized();

	// Gives an unbiased random uninitialized direction
	EMazeDirection RandomUninitializedDirection();

private:
	UPROPERTY()
		FIntVector Coordinates;

	//UPROPERTY()
	//AMazeCellEdge* Edges[UMazeDirections::Count] = { nullptr, nullptr, nullptr, nullptr };

	UPROPERTY()
		TArray<AMazeCellEdge*> Edges;

	UPROPERTY()
		int32 InitializedEdgeCount;

};