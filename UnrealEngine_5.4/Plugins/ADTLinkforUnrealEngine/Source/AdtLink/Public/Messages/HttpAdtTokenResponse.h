//////////////////////////////////////////////////////
// Copyright (C) WSP. 2021. All rights reserved.
//////////////////////////////////////////////////////

#pragma once
#include "../Util/AccessToken.h"

#include "HttpAdtTokenResponse.generated.h"

//ADT response with added token
USTRUCT(BlueprintType)
struct FHttpTokenResponse : public FHttpAdtResponse
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Token")
		FAccessToken AccessToken;
};