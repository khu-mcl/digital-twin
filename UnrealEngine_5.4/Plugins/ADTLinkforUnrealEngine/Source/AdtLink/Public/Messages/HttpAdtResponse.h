//////////////////////////////////////////////////////
// Copyright (C) WSP. 2021. All rights reserved.
//////////////////////////////////////////////////////

#pragma once

#include "HttpAdtResponse.generated.h"

//Struct for standard ADT http response 
USTRUCT(BlueprintType)
struct FHttpAdtResponse
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTTP")
		FString Message;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTTP")
		FString Status;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTTP")
		int Code;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTTP")
		FString RequestUrl;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTTP")
		bool Success;
};