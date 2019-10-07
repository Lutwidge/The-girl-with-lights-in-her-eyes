#pragma once

#include "CoreMinimal.h"
#include "Bool2DArray.generated.h"

USTRUCT()
struct TGWLIHE_API FBool2DArray
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<bool> Array2ndDimension;

	bool operator[] (int32 i)
	{
		return Array2ndDimension[i];
	}

	/**
	* Allows us to use Insert in the following fashion for the 2nd dimension: Array[i].Insert(Cell, index)
	*/
	void Insert(bool Bool, int32 i)
	{
		Array2ndDimension.Insert(Bool, i);
	}

	/**
	* Allows us to use Init in the following fashion for the 2nd dimension: Array[i].Init(Cell, index)
	*/
	void Init(bool Bool, int32 i)
	{
		Array2ndDimension.Init(Bool, i);
	}

	/**
	* Allows us to use AddZeroed in the following fashion for the 2nd dimension: Array[i].AddZeroed(index)
	*/
	void AddZeroed(int32 i)
	{
		Array2ndDimension.AddZeroed(i);
	}
};

