#pragma once

#include "CoreMinimal.h"
class AMazeCell;
#include "MazeCell2DArray.generated.h"

USTRUCT()
struct TGWLIHE_API FMazeCell2DArray
{
	GENERATED_BODY()

public:
	UPROPERTY()
		TArray<AMazeCell*> Array2ndDimension;

	AMazeCell* operator[] (int32 i)
	{
		return Array2ndDimension[i];
	}

	/**
	* Allows us to use Insert in the following fashion for the 2nd dimension: Array[i].Insert(Cell, index)
	*/
	void Insert(AMazeCell* Cell, int32 i)
	{
		Array2ndDimension.Insert(Cell, i);
	}

	/**
	* Allows us to use Init in the following fashion for the 2nd dimension: Array[i].Init(Cell, index)
	*/
	void Init(AMazeCell* Cell, int32 i)
	{
		Array2ndDimension.Init(Cell, i);
	}

	/**
	* Allows us to use AddZeroed in the following fashion for the 2nd dimension: Array[i].AddZeroed(index)
	*/
	void AddZeroed(int32 i)
	{
		Array2ndDimension.AddZeroed(i);
	}
};

UENUM()
enum class EMazeDirection : uint8
{
	North,
	East,
	South,
	West
};

UENUM()
enum class ECellEdgeType : uint8
{
	Passage,
	Wall
};