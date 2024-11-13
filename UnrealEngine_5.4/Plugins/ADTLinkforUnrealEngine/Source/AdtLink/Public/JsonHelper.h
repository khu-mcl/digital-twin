//////////////////////////////////////////////////////
// Copyright (C) WSP. 2021. All rights reserved.
//////////////////////////////////////////////////////

#pragma once

#include "CoreMinimal.h"
#include "Serialization/JsonSerializer.h"

#include "DTDL/DTDLInterface.h"
#include "DTDL/DTDLRelationship.h"
#include "DTDL/DTDLSchemasEnum.h"

#include "LiveData/ADTTwinData.h"
#include "LiveData/ADTTwinRelationship.h"

#include "JSONHelper.generated.h"

/**
 *
 */
UCLASS()
class ADTLINK_API UJSONHelper : public UObject
{
	GENERATED_BODY()

public:


	//Exposed functions to blueprint for handling JSON

	/*
	*	Get a string value from a JSON string
	*	@param JSON - The JSON string to get value from
	*	@param Key - Key to search for
	*	@param Value -  Value returned
	*	@param Success - True if valid value was found
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get String from JSON", Keywords = "JSON get string"), Category = "ADT JSON Helper")
		static void GetStringValueFromJSON(FString JSON, FString Key, FString& Value, bool& Success);

	/*
	*	Get a numeric float value from a JSON string
	*	@param JSON - The JSON string to get value from
	*	@param Key - Key to search for
	*	@param Value -  Value returned
	*	@param Success - True if valid value was found
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Float from JSON", Keywords = "JSON get float"), Category = "ADT JSON Helper")
		static void GetFloatValueFromJSON(FString JSON, FString Key, float& Value, bool& Success);

	/*
	*	Get a bool value from a JSON string
	*	@param JSON - The JSON string to get value from
	*	@param Key - Key to search for
	*	@param Value -  Value returned
	*	@param Success - True if valid value was found
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Bool from JSON", Keywords = "JSON get bool"), Category = "ADT JSON Helper")
		static void GetBoolValueFromJSON(FString JSON, FString Key, bool& Value, bool& Success);

	/*
	*	Get an int value from a JSON string
	*	@param JSON - The JSON string to get value from
	*	@param Key - Key to search for
	*	@param Value -  Value returned
	*	@param Success - True if valid value was found
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get int from JSON", Keywords = "JSON get int"), Category = "ADT JSON Helper")
		static void GetIntValueFromJSON(FString JSON, FString Key, int& Value, bool& Success);

	/*
	*	Get a JSON field from a JSON string
	*	@param JSON - The JSON string to get value from
	*	@param Key - Key to search for
	*	@param Value -  Value returned
	*	@param Success - True if valid value was found
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Field from JSON", Keywords = "JSON get field"), Category = "ADT JSON Helper")
		static void GeTJsonFieldFromJSON(FString JSON, FString Key, FString& Value, bool& Success);

	/*
	*	Get a Array String field from a JSON string
	*	@param JSON - The JSON string to get value from
	*	@param Key - Key to search for
	*	@param Value -  Value returned
	*	@param Success - True if valid value was found
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Array from JSON", Keywords = "JSON get array"), Category = "ADT JSON Helper")
		static void GetArrayFieldFromJSON(FString JSON, FString Key, TArray<FString>& Values, bool& Success);

private:

	//Simple blueprint helper functions

	/*
	*	Deserialize JSON string and check if key exist
	*	@param JSON - The JSON string to get value from
	*	@param Key - Key to search for
	*	@param Success - True if valid value was found
	*	@return Array of JSON Objects found in JSON string
	*/
	static TSharedPtr<FJsonObject> GeTJsonSetup(FString JSON, FString Key, bool& Success);
};
