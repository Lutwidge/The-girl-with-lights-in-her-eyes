// Fill out your copyright notice in the Description page of Project Settings.

#include "Maze.h"
#include "MazeCell.h"
#include "ConstructorHelpers.h"
#include "MazePassage.h"
#include "MazeWall.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "AmazeingCharacter.h"
#include "EndTriggerVolume.h"
#include "MazeCellEdge.h"
#include "AICharacter.h"
#include "AmazeingGameMode.h"
#include "Runtime/Engine/Classes/Components/ArrowComponent.h"

// Sets default values
AMaze::AMaze()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Grabs the classes of the blueprints
	static ConstructorHelpers::FObjectFinder<UClass> CellClassFinder(TEXT("Class'/Game/Blueprints/Maze/BP_MazeCell.BP_MazeCell_C'"));
	if (CellClassFinder.Object)
	{
		CellBlueprint = CellClassFinder.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: Blueprint MazeCell not found"));
	}

	static ConstructorHelpers::FObjectFinder<UClass> PassageClassFinder(TEXT("Class'/Game/Blueprints/Maze/BP_MazePassage.BP_MazePassage_C'"));
	if (PassageClassFinder.Object)
	{
		PassageBlueprint = PassageClassFinder.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: Blueprint MazePassage not found"));
	}

	static ConstructorHelpers::FObjectFinder<UClass> WallClassFinder(TEXT("Class'/Game/Blueprints/Maze/BP_MazeWall.BP_MazeWall_C'"));
	if (WallClassFinder.Object)
	{
		WallBlueprint = WallClassFinder.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: Blueprint MazeWall not found"));
	}

	static ConstructorHelpers::FObjectFinder<UClass> LastLevelClassFinder(TEXT("Class'/Game/Blueprints/Maze/BP_LastLevel.BP_LastLevel_C'"));
	if (LastLevelClassFinder.Object)
	{
		LastLevelBlueprint = LastLevelClassFinder.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: Blueprint LastLevel not found"));
	}

	static ConstructorHelpers::FObjectFinder<UClass> AIMonsterClassFinder(TEXT("Class'/Game/Blueprints/AI/BP_AICharacter.BP_AICharacter_C'"));
	if (AIMonsterClassFinder.Object)
	{
		AIMonsterBlueprint = AIMonsterClassFinder.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: Blueprint AIMonster not found"));
	}

	static ConstructorHelpers::FObjectFinder<UClass> AIDeathClassFinder(TEXT("Class'/Game/Blueprints/AI/BP_AIDeath.BP_AIDeath_C'"));
	if (AIDeathClassFinder.Object)
	{
		AIDeathBlueprint = AIDeathClassFinder.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: Blueprint AIDeath not found"));
	}

	IsEventNeeded = false;
	IsCountdownFinished = false;
	IsGenerationFinished = false;
	Countdown = 0.0f;
}

// Called when the game starts or when spawned
void AMaze::BeginPlay()
{
	Super::BeginPlay();

	// Subscribe to the end trigger fade out event, so that the maze is destroyed when we reach this trigger
	SubscribeDestroyMaze();

	if (GetWorld())
	{
		// Teleport the player once the Monster Kill Fade Out animation is finished
		GameMode = (AAmazeingGameMode*)GetWorld()->GetAuthGameMode();
		GameMode->OnMonsterKillFadeOutFinished().AddUFunction(this, FName("ResetCharacterLocation"));
	}
}

void AMaze::Tick(float DeltaTime)
{
	// Call the base class
	Super::Tick(DeltaTime);

	// Global logic: if we launch a transition, we need to know when to launch the "fadein" of the next scene --> This class will broadcast an event
	// This ensures that the event is broadcasted 1) Once the generation of the maze is finished & 2) After a given duration, to enable actually reading the transition text
	if (IsEventNeeded)
	{
		if (!IsCountdownFinished)
		{
			Countdown += DeltaTime;

			if (Countdown >= TransitionDuration)
			{
				IsCountdownFinished = true;
			}
		}
		else
		{
			if (IsGenerationFinished)
			{
				// Broadcast event
				TransitionFinishedEvent.Broadcast();

				// Launch the countdown at this point: event for sound + here
				if (IsDeathActivated)
				{
					// Broadcast the event notifying that this maze has Death
					DeathPresentEvent.Broadcast(DeathArrivalTime);
					DeathCountdown = 0.0f;
				}

				// Reinitialize the parameters
				IsEventNeeded = false;
				IsCountdownFinished = false;
				IsGenerationFinished = false;
				Countdown = 0.0f;
			}
		}
	}

	// Countdown of the "Death" character
	if (IsDeathActivated)
	{
		DeathCountdown += DeltaTime;

		if (DeathCountdown >= DeathArrivalTime)
		{
			if (GetWorld())
			{
				FActorSpawnParameters Params;
				AAICharacter* AIMonster = GetWorld()->SpawnActor<AAICharacter>(AIDeathBlueprint, StartLocation + FVector(0.0f, 0.0f, 200.0f), FRotator(0.0f), Params);
				AIMonster->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
			}

			// Broadcast that Death has arrived
			DeathArrivalEvent.Broadcast();

			// Reinitialize the parameters
			IsDeathActivated = false;
			DeathCountdown = 0.0f;
		}
	}
}

// Generates a Maze, returns two random locations for the start and finish
void AMaze::Generate(int32 SizeX, int32 SizeY, int32 NumberOfMonsters, int32 MonsterPathLength, int32 DeathTimer)
{
	// For the "fadein" event broadcast
	IsEventNeeded = true;

	// We update all the class values with the new ones
	Size.X = SizeX;
	Size.Y = SizeY;
	MonsterNumber = NumberOfMonsters;
	AIPathLength = MonsterPathLength;

	// First, we initialize the size of the Cells array
	Cells.AddZeroed(Size.X);
	for (int i = 0; i < Size.X; i++)
	{
		Cells[i].AddZeroed(Size.Y);
	}
	// We give every element the "nullptr" value
	for (int i = 0; i < Size.X; i++)
	{
		for (int j = 0; j < Size.Y; j++)
		{
			Cells[i].Array2ndDimension[j] = nullptr;
		}
	}

	// Then, we do the same for the IsCellUsed array
	IsCellUsed.AddZeroed(Size.X);
	for (int i = 0; i < Size.X; i++)
	{
		IsCellUsed[i].AddZeroed(Size.Y);
	}
	// We give every element the "false" value
	for (int i = 0; i < Size.X; i++)
	{
		for (int j = 0; j < Size.Y; j++)
		{
			IsCellUsed[i].Array2ndDimension[j] = false;
		}
	}

	// Then, we create the maze
	DoFirstGenerationStep(ActiveCells);

	while (ActiveCells.Num() > 0)
	{
		DoNextGenerationStep(ActiveCells);
	}

	// Then, we define the random coordinates for the initial placement of the FPC & Goal - And remove them from possible placements for the AI
	// Starting point, which goes to the player
	int32 TempY = FMath::RandRange(0, Size.Y - 1);
	StartLocation = Cells[0].Array2ndDimension[TempY]->GetActorLocation();
	IsCellUsed[0].Array2ndDimension[TempY] = true;
	UE_LOG(LogTemp, Warning, TEXT("Start is %s"), *StartLocation.ToString());
	FirstPersonCharacter->InitializeLocation(StartLocation);

	// Finish point, which goes to the end trigger
	TempY = FMath::RandRange(0, Size.Y - 1);
	FVector EndLocation = Cells[Size.X - 1].Array2ndDimension[TempY]->GetActorLocation();
	IsCellUsed[Size.X - 1].Array2ndDimension[TempY] = true;
	UE_LOG(LogTemp, Warning, TEXT("End is %s"), *EndLocation.ToString());
	EndTriggerVolume->SetActorLocation(EndLocation + FVector(0.0f, 0.0f, 100));

	// Finally, we spawn the appropriate number of monsters
	UWorld* const World = GetWorld();
	for (int i = 0; i < MonsterNumber; i++)
	{
		if (World)
		{
			FActorSpawnParameters Params;
			Params.Name = FName(*FString("Monster number " + FString::FromInt(i)));
			AAICharacter* AIMonster = World->SpawnActor<AAICharacter>(AIMonsterBlueprint, EndLocation + FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f), Params);
			AIMonster->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
		}
	}

	// Signals that the maze generation is finished
	IsGenerationFinished = true;

	// Enable the death timer if needed, and set it. If DeathTimer has the default value, do not enable "Death"
	if (DeathTimer == 0)
	{
		IsDeathActivated = false;
	}
	else
	{
		IsDeathActivated = true;
		DeathArrivalTime = DeathTimer;
	}
}

// Creates a Cell with a Plane at location (X,Y)
AMazeCell* AMaze::CreateCell(FIntVector Coordinates)
{
	// First, spawn an instance of the cell blueprint
	UWorld* const World = GetWorld();
	if (World)
	{
		FActorSpawnParameters Params;
		Params.Name = FName(*FString("Cell x=" + FString::FromInt(Coordinates.X) + " y=" + FString::FromInt(Coordinates.Y)));
		AMazeCell* NewCell = World->SpawnActor<AMazeCell>(CellBlueprint, Params);
		NewCell->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
		NewCell->SetActorRelativeLocation(FVector(500 * (Coordinates.X - Size.X * 0.5f + 0.5f), 500 * (Coordinates.Y - Size.Y * 0.5f + 0.5f), 0.0f));
		// Then, we keep track of the cell created in the Cells 2D array
		NewCell->SetCoordinates(Coordinates);
		Cells[Coordinates.X].Array2ndDimension[Coordinates.Y] = NewCell;
		return NewCell;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Bug: Passage else dans AMaze::CreateCell"));
		return nullptr;
	}
}

FIntVector AMaze::RandomCoordinates()
{
	return FIntVector(FMath::RandRange(0, Size.X - 1), FMath::RandRange(0, Size.Y - 1), 0);
}

bool AMaze::ContainsCoordinates(FIntVector Coordinate)
{
	return Coordinate.X >= 0 && Coordinate.X < Size.X && Coordinate.Y >= 0 && Coordinate.Y < Size.Y;
}

AMazeCell* AMaze::GetCell(FIntVector Coordinates)
{
	return Cells[Coordinates.X][Coordinates.Y];
}

void AMaze::DoFirstGenerationStep(TArray<AMazeCell*>& ActiveCells)
{
	ActiveCells.Add(CreateCell(RandomCoordinates()));
}

void AMaze::DoNextGenerationStep(TArray<AMazeCell*>& ActiveCells)
{
	int32 CurrentIndex = ActiveCells.Num() - 1;
	CurrentCell = ActiveCells[CurrentIndex];

	if (CurrentCell->IsFullyInitialized())
	{
		ActiveCells.RemoveAt(CurrentIndex);
		return;
	}

	EMazeDirection Direction = CurrentCell->RandomUninitializedDirection();
	FIntVector Coordinates = CurrentCell->GetCoordinates() + UMazeDirections::ToIntVector(Direction);

	if (ContainsCoordinates(Coordinates))
	{
		Neighbor = GetCell(Coordinates);
		if (Neighbor == nullptr)
		{
			Neighbor = CreateCell(Coordinates);
			CreatePassage(CurrentCell, Neighbor, Direction);
			ActiveCells.Add(Neighbor);
		}
		else
		{
			CreateWall(CurrentCell, Neighbor, Direction);
		}
	}
	else
	{
		CreateWall(CurrentCell, nullptr, Direction);
	}
}

void AMaze::CreatePassage(AMazeCell* Cell, AMazeCell* OtherCell, EMazeDirection Direction)
{
	UWorld * const World = GetWorld();
	if (World)
	{
		AMazePassage* Passage = World->SpawnActor<AMazePassage>(PassageBlueprint);
		Passage->Initialize(Cell, OtherCell, Direction, ECellEdgeType::Passage);
		Passage->Initialize(OtherCell, Cell, UMazeDirections::GetOppositeDirection(Direction), ECellEdgeType::Passage);
	}
}

void AMaze::CreateWall(AMazeCell* Cell, AMazeCell* OtherCell, EMazeDirection Direction)
{
	UWorld * const World = GetWorld();
	if (World)
	{
		AMazeWall* Wall = World->SpawnActor<AMazeWall>(WallBlueprint);
		Wall->Initialize(Cell, OtherCell, Direction, ECellEdgeType::Wall);
		if (OtherCell != nullptr)
		{
			Wall->Initialize(OtherCell, Cell, UMazeDirections::GetOppositeDirection(Direction), ECellEdgeType::Wall);
		}
	}
}

TArray<FVector> AMaze::CreateAIPath()
{
	TArray<FVector> AIPath;
	// First, we select a random cell that is not used : this will be the "home" location of the AI
	int RandomX;
	int RandomY;
	do
	{
		RandomX = FMath::RandRange(0, Size.X - 1);
		RandomY = FMath::RandRange(0, Size.Y - 1);
	} while (IsCellUsed[RandomX][RandomY] == true);
	IsCellUsed[RandomX].Array2ndDimension[RandomY] = true; // The cell is now used

														   // We store the "home" location in the first element of the array
	AIPath.Add(Cells[RandomX][RandomY]->GetActorLocation());

	// Then, we determine a path of AIPathLength length
	TArray<AMazeCell*> CellsInAIPath;
	CellsInAIPath.Add(Cells[RandomX][RandomY]);

	while (CellsInAIPath.Num() < AIPathLength)
	{
		// Get a random available cell, use it if any is available, otherwise keep the path as it is
		AMazeCell* NextCell = RandomUsableNeighborCell(CellsInAIPath.Last(0));
		if (NextCell != nullptr)
		{
			CellsInAIPath.Add(NextCell);
		}
		else
		{
			break;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Number of cells of path=%d"), CellsInAIPath.Num());
	// We store the last "target" location in the second element of the array
	AIPath.Add(CellsInAIPath.Last(0)->GetActorLocation());

	return AIPath;
}

AMazeCell* AMaze::RandomUsableNeighborCell(AMazeCell* Cell)
{
	TArray<AMazeCellEdge*> ValidPassageEdges;
	FIntVector SelectedCoordinates;
	// We get the passage edges of the cell
	TArray<AMazeCellEdge*> PassageEdges = Cell->GetPassageEdges();

	// Then, we keep only the valid ones (not used yet)
	for (uint8 i = 0; i < PassageEdges.Num(); i++)
	{
		SelectedCoordinates = Cell->GetCoordinates() + UMazeDirections::ToIntVector(PassageEdges[i]->GetDirection());
		// Clamp so that we are never out of the domain
		SelectedCoordinates.X = FMath::Clamp<int>(SelectedCoordinates.X, 0, Size.X - 1);
		SelectedCoordinates.Y = FMath::Clamp<int>(SelectedCoordinates.Y, 0, Size.Y - 1);
		if (!IsCellUsed[SelectedCoordinates.X][SelectedCoordinates.Y])
		{
			ValidPassageEdges.Add(PassageEdges[i]);
		}
	}

	if (ValidPassageEdges.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("AI Monster path not completely defined"));
		return nullptr;
	}
	else
	{
		// Then, we return a randomly selected one
		int RandomIndex = FMath::RandRange(0, ValidPassageEdges.Num() - 1);
		SelectedCoordinates = Cell->GetCoordinates() + UMazeDirections::ToIntVector(ValidPassageEdges[RandomIndex]->GetDirection());
		IsCellUsed[SelectedCoordinates.X].Array2ndDimension[SelectedCoordinates.Y] = true;
		return Cells[SelectedCoordinates.X][SelectedCoordinates.Y];
	}

	// TO DO: Ajouter test pour éviter culs de sacs ? Et si tout mène à cul de sac, arrêter ?
}

void AMaze::DestroyMaze(bool IsDeathKill)
{
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);

	TArray<AActor*> ChildAttachedActors;
	for (AActor* Actor : AttachedActors)
	{
		// For the AI Characters, the controller should be destroyed at the same time
		Actor->GetAttachedActors(ChildAttachedActors);

		for (AActor* ChildActor : ChildAttachedActors)
		{
			// For the AI Characters, the controller should be destroyed at the same time
			ChildActor->Destroy();
		}

		Actor->Destroy();
	}

	// TO DO: Forcer le passage du GC ici ?
}

void AMaze::RespawnCharacter()
{
	// Global organization:
	// here -> UI manager for launching fade out / disable player movement -> end of animation launches ResetCharacterLocation -> In turn, this launches the fade in of the UI manager -> end of animation allows the player to move once again
	MonsterKillEvent.Broadcast();
}

void AMaze::ResetCharacterLocation()
{
	FirstPersonCharacter->SetActorLocation(StartLocation + FVector(0.0f, 0.0f, 100.0f));
	LocationResetEvent.Broadcast();
}

void AMaze::GenerateLastLevel()
{
	// For the "fadein" event broadcast
	IsEventNeeded = true;

	// First, spawn an instance of the last level blueprint
	UWorld* const World = GetWorld();
	AMazeCell* LastLevel = nullptr;
	if (World)
	{
		FActorSpawnParameters Params;
		LastLevel = World->SpawnActor<AMazeCell>(LastLevelBlueprint, Params);
		LastLevel->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
		LastLevel->SetActorRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	}

	// Signals that the level generation is finished
	IsGenerationFinished = true;

	// Then, we define the initial coordinates for the initial placement of the FPC & Goal
	FirstPersonCharacter->InitializeLocation(FVector(0.0f, 0.0f, 0.0f));
	FirstPersonCharacter->Rotate(FRotator(0.0, -90.0, 0.0));

	// Finish point, which goes to the end trigger
	// Find the arrow component of the last level and set the goal location to its location
	if (LastLevel)
	{
		TArray<UArrowComponent*> Comps;
		LastLevel->GetComponents(Comps);
		if (Comps.Num() > 0)
		{
			EndTriggerVolume->SetActorLocation(Comps[0]->GetComponentLocation() + FVector(0.0f, 0.0f, 100));
		}
	}

	// Show the last goal
	EndTriggerVolume->SetActorScale3D(FVector(100.0f, 1.0f, 10.0f));

	IsDeathActivated = false;
}

void AMaze::SubscribeDestroyMaze()
{
	DestroyMazeHandle = EndTriggerVolume->OnFadeOutLaunched().AddUFunction(this, FName("DestroyMaze"));
}

void AMaze::UnsubscribeDestroyMaze()
{
	EndTriggerVolume->OnFadeOutLaunched().Remove(DestroyMazeHandle);
}

void AMaze::ResetEndTriggerScale()
{
	EndTriggerVolume->SetActorScale3D(FVector(1.7f, 1.0f, 1.0f));
}



