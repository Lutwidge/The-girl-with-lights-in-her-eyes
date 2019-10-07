// Fill out your copyright notice in the Description page of Project Settings.

#include "MazeCell.h"
#include "Maze.h"
#include "EngineUtils.h"
#include "MazeCellEdge.h"


// Sets default values
AMazeCell::AMazeCell()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AMazeCell::BeginPlay()
{
	Super::BeginPlay();

	// First, we initialize the size of the array
	Edges.AddZeroed(UMazeDirections::Count);
	// Then, we give every element the "nullptr" value
	for (int i = 0; i < UMazeDirections::Count; i++)
	{
		Edges[i] = nullptr;
	}

}

// Called every frame
void AMazeCell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMazeCell::SetCoordinates(FIntVector NewCoordinates)
{
	Coordinates = NewCoordinates;
}

FIntVector AMazeCell::GetCoordinates()
{
	return Coordinates;
}

AMazeCellEdge* AMazeCell::GetEdge(EMazeDirection Direction)
{
	return this->Edges[(uint8)Direction];
}

void AMazeCell::SetEdge(EMazeDirection Direction, AMazeCellEdge* Edge)
{
	this->Edges[(uint8)Direction] = Edge;
	this->InitializedEdgeCount += 1;
}

bool AMazeCell::IsFullyInitialized()
{
	return this->InitializedEdgeCount >= UMazeDirections::Count;
}

EMazeDirection AMazeCell::RandomUninitializedDirection()
{
	// We determine a random number of skips to do
	int32 Skips = FMath::RandRange(0, UMazeDirections::Count - InitializedEdgeCount - 1);

	// Then, we loop through the array of the edges. When we find an uninitialized edge, we take the associated direction when there's finally no skip left.
	for (uint8 i = 0; i < UMazeDirections::Count; i++)
	{
		if (!Edges[i])
		{
			if (Skips == 0)
			{
				return (EMazeDirection)i;
			}
			Skips -= 1;
		}
	}

	// We should not attain this return in a nominal case
	// Exception to implement here, instead of a default return
	UE_LOG(LogTemp, Warning, TEXT("Bug: Direction aleatoire definie par defaut (Nord) valeur IEC: %d"), this->InitializedEdgeCount);
	return EMazeDirection::North;
}

TArray<AMazeCellEdge*> AMazeCell::GetPassageEdges()
{
	TArray<AMazeCellEdge*> PassageEdges;
	for (int i = 0; i < Edges.Num(); i++)
	{
		if (Edges[i]->GetType() == ECellEdgeType::Passage)
		{
			PassageEdges.Add(Edges[i]);
		}
	}

	return PassageEdges;
}