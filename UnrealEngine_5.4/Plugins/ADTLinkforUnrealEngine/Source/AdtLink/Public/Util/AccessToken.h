//////////////////////////////////////////////////////
// Copyright (C) WSP. 2021. All rights reserved.
//////////////////////////////////////////////////////

#pragma once

#include "AccessToken.generated.h"

//Struct for Authentication towards ADT
USTRUCT(BlueprintType)
struct FAccessToken
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT Access Token Azure Digital Twin")
		FString Token;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT Access Token Azure Digital Twin")
		FDateTime ExpiresOn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT Access Token Azure Digital Twin")
		FString TokenType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT Access Token Azure Digital Twin")
		FString TenantID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT Access Token Azure Digital Twin")
		FString ClientID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT Access Token Azure Digital Twin")
		FString ClientSecret;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT Access Token Azure Digital Twin")
		bool Inisiated = false;
};