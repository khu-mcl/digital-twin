//////////////////////////////////////////////////////
// Copyright (C) WSP. 2021. All rights reserved.
//////////////////////////////////////////////////////

#pragma once

#include "HttpBaseRequest.generated.h"

//Struct for a standard http request
USTRUCT(BlueprintType)
struct FHttpBaseRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTTP")
		FString Url;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTTP")
		FString Method;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTTP")
		TMap<FString, FString> Headers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTTP")
		FString Payload;
};