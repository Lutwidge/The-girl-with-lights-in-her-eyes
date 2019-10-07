// Fill out your copyright notice in the Description page of Project Settings.

#include "MazeCellEdge.h"


// Sets default values
AMazeCellEdge::AMazeCellEdge()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AMazeCellEdge::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AMazeCellEdge::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMazeCellEdge::Initialize(AMazeCell* Cell, AMazeCell* OtherCell, EMazeDirection Direction, ECellEdgeType Type)
{
	// Initializes the Edge variables
	this->Cell = Cell;
	this->OtherCell = OtherCell;
	this->Direction = Direction;
	this->Type = Type;

	// Add it to the Cell
	Cell->SetEdge(Direction, this);

	// Attach and place it in the world
	this->AttachToActor((AActor*)Cell, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
	this->SetActorRelativeLocation(FVector(0, 0, 0));
	this->SetActorRelativeRotation(UMazeDirections::GetRotation(Direction));
}