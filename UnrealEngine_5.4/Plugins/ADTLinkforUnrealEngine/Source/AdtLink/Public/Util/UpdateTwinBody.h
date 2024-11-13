//////////////////////////////////////////////////////
// Copyright (C) WSP. 2021. All rights reserved.
//////////////////////////////////////////////////////

#pragma once

#include "UpdateTwinBody.generated.h"

//Enum to decide what update twin operation to be performed
UENUM(BlueprintType)
enum UpdateTwinOperation {
	Replace = 0,
	Add = 1,
	Remove = 2,
};

//Enum to decide what JSON value type a message has
UENUM(BlueprintType)
enum JsonValueType {
	_Bool = 0,
	_String = 1,
	_Numeric = 2,
};

//Sturct for used to create a Update Twin message
USTRUCT(BlueprintType)
struct FUpdateTwinBody
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTTP")
		TEnumAsByte < UpdateTwinOperation > Operation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTTP")
		TEnumAsByte < JsonValueType > ValueType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTTP")
		FString PropertyName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTTP")
		FString StringValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTTP")
		bool BoolValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTTP")
		float NumericValue;
};