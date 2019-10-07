// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MazeCell2DArray.h"
#include "MazeCell.h"
#include "MazeCellEdge.generated.h"

UCLASS(abstract)
class TGWLIHE_API AMazeCellEdge : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMazeCellEdge();

	// Initialization of the Edge
	void Initialize(AMazeCell* Cell, AMazeCell* OtherCell, EMazeDirection Direction, ECellEdgeType Type);

	// Get the direction of the Edge, used for defining the AI path in AMaze
	EMazeDirection GetDirection() const { return Direction; }

	// Get the type of the Edge, used for defining the AI path in AMaze
	ECellEdgeType GetType() const { return Type; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// First cell corresponding to this edge
	UPROPERTY()
		AMazeCell* Cell;

	// Second cell corresponding to this edge
	UPROPERTY()
		AMazeCell* OtherCell;

	// Direction of this edge wrt the first cell
	UPROPERTY()
		EMazeDirection Direction;

	// Type of the edge (Passage, Wall)
	UPROPERTY()
		ECellEdgeType Type;
};