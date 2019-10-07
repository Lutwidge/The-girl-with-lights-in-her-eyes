// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
class AMazeCell;
#include "MazeCell2DArray.h"
class AMazePassage;
class AMazeWall;
class AEndTriggerVolume;
class AAmazeingCharacter;
class AAICharacter;
#include "Bool2DArray.h"
#include "Maze.generated.h"

// Declaration of event signature with no return and no param
DECLARE_EVENT(AAmaze, FAction)

// Declaration of event signature for the presence of Death in the maze level
DECLARE_EVENT_OneParam(AAmaze, FDeathPresent, int32)

UCLASS(Blueprintable, ClassGroup = Maze)
class TGWLIHE_API AMaze : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMaze();

	// Generates a Maze, sets the player at a start point, the end at an end point, and the AI characters
	void Generate(int32 SizeX, int32 SizeY, int32 NumberOfMonsters, int32 MonsterPathLength, int32 DeathTimer = 0);

	// Generates the last level
	void GenerateLastLevel();

	// Returns random coordinates
	FIntVector RandomCoordinates();

	// Verifies whether or not the coordinates are inside the maze
	bool ContainsCoordinates(FIntVector Coordinate);

	// Gets the cell at given coordinates, returns nullptr if none has been found
	AMazeCell* GetCell(FIntVector Coordinates);

	// Initializes the maze generation by adding a cell to the active cells list
	void DoFirstGenerationStep(TArray<AMazeCell*>& ActiveCells);

	// Generates the next maze cell following a backtrack algorithm
	void DoNextGenerationStep(TArray<AMazeCell*>& ActiveCells);

	// Generates a passage
	void CreatePassage(AMazeCell* Cell, AMazeCell* OtherCell, EMazeDirection Direction);

	// Generates a wall
	void CreateWall(AMazeCell* Cell, AMazeCell* OtherCell, EMazeDirection Direction);

	// Gives the beginning and end of the patrol path for the AI Monster
	TArray<FVector> CreateAIPath();

	// Accessor to the event to warn that the maze is generated && minimal waiting time passed is elapsed
	FAction& OnTransitionFinished() { return TransitionFinishedEvent; }

	// Destroy all children and AI
	UFUNCTION()
		void DestroyMaze(bool IsDeathKill);

	// Respawn the character after being touched by a Monster, called by the Monster Behavior Tree
	UFUNCTION()
		void RespawnCharacter();

	// Accessor to the event to warn that this maze has Death
	FDeathPresent& OnDeathPresent() { return DeathPresentEvent; }

	// Accessor to the event to warn that Death has arrived
	FAction& OnDeathArrival() { return DeathArrivalEvent; }

	// Accessor to the event to warn that the player has been "killed"/touched by a monster
	FAction& OnMonsterKill() { return MonsterKillEvent; }

	// Event to warn that the player has been teleported
	FAction& OnLocationReset() { return LocationResetEvent; }

	// Method to remove the destroy maze ufunction from the end trigger event - used for the last level
	void UnsubscribeDestroyMaze();

	// Method to add it - used for the restart of the game
	void SubscribeDestroyMaze();

	// Reset the End trigger scale - used for the restart of the game
	void ResetEndTriggerScale();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame, used here to generate events for the transitions
	virtual void Tick(float DeltaSeconds) override;

private:
	// Creates a Cell with a Plane at location (X,Y), and returns a pointer to it
	AMazeCell * CreateCell(FIntVector Coordinates);

	// Returns a random Cell which has a passage edge with the Cell in parameter and is not yet used for the AI
	AMazeCell* RandomUsableNeighborCell(AMazeCell* Cell);

	// Resets the player character position to the start of the maze
	UFUNCTION()
	void ResetCharacterLocation();

public:
	// Size Vector, Z-axis is not used
	UPROPERTY()
		FIntVector Size;

	// Sizedd Vector, Z-axis is not used
	UPROPERTY()
		int32 MonsterNumber;

	// int to store the number of cells the AI Monsters should include in their patrol
	UPROPERTY()
		int32 AIPathLength;

	// Blueprint pointer to the MazeCell Blueprint
	UPROPERTY(EditAnywhere)
		TSubclassOf<class AMazeCell> CellBlueprint;

	// Blueprint pointer to the MazeWall Blueprint
	UPROPERTY(EditAnywhere)
		TSubclassOf<class AMazeCellEdge> WallBlueprint;

	// Blueprint pointer to the MazePassage Blueprint
	UPROPERTY(EditAnywhere)
		TSubclassOf<class AMazeCellEdge> PassageBlueprint;

	// Blueprint pointer to the Last Level Blueprint
	UPROPERTY(EditAnywhere)
		TSubclassOf<class AMazeCell> LastLevelBlueprint;

	// Blueprint pointer to the AIMonster Blueprint
	UPROPERTY(EditAnywhere)
		TSubclassOf<class AAICharacter> AIMonsterBlueprint;

	// Blueprint pointer to the AIDeath Blueprint
	UPROPERTY(EditAnywhere)
		TSubclassOf<class AAICharacter> AIDeathBlueprint;

	// Pointer to the FPC
	UPROPERTY(EditAnywhere)
		AAmazeingCharacter* FirstPersonCharacter;

	// Pointer to the Goal Trigger
	UPROPERTY(EditAnywhere)
		AEndTriggerVolume* EndTriggerVolume;

	// 2D array using a USTRUCT to store the cells used
	UPROPERTY()
		TArray<FMazeCell2DArray> Cells;

	// Used for broadcasting the "fadein" event at the right time : minimal duration during which the text shall appear
	UPROPERTY(EditAnywhere)
		int32 TransitionDuration;

private:
	// Used to track the cell path during the backtrack algorithm (Defined here to be properly garbaged collected)
	UPROPERTY()
		TArray<AMazeCell*> ActiveCells;

	// Used to track which cells shall not be used for the AI Monster paths
	UPROPERTY()
		TArray<FBool2DArray> IsCellUsed;

	// Current Cell of the maze generation
	UPROPERTY()
		AMazeCell* CurrentCell;

	// Current Neighbor of the maze generation
	UPROPERTY()
		AMazeCell* Neighbor;

	// Used for broadcasting the "fadein" event at the right time
	bool IsEventNeeded;

	// Used for broadcasting the "fadein" event at the right time : tracks the countdown
	bool IsCountdownFinished;

	// Used for broadcasting the "fadein" event at the right time : tracks the maze generation
	bool IsGenerationFinished;

	// Used for broadcasting the "fadein" event at the right time : countdown
	float Countdown;

	// Used for the Death activation
	bool IsDeathActivated;

	// Used for Death spawn
	float DeathCountdown;

	// Used for the Death activation
	int32 DeathArrivalTime;

	// Event to warn that the maze is generated && minimal waiting time passed is elapsed
	FAction TransitionFinishedEvent;

	// Starting location of the character in the maze, stored for respawn/spawning of Death
	FVector StartLocation;

	// Event to warn that this maze has Death
	FDeathPresent DeathPresentEvent;

	// Event to warn that Death has arrived
	FAction DeathArrivalEvent;

	// Event to warn that the player has been "killed"/touched by a monster
	FAction MonsterKillEvent;

	// Event to warn that the player has been teleported
	FAction LocationResetEvent;

	// GameMode
	class AAmazeingGameMode* GameMode;

	// Delegate used for removing a function from an event
	FDelegateHandle DestroyMazeHandle;
};