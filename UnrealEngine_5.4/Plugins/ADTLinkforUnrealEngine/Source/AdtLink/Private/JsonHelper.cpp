//////////////////////////////////////////////////////
// Copyright (C) WSP. 2021. All rights reserved.
//////////////////////////////////////////////////////

#include "JSONHelper.h"


//Simple blueprint helper functions

void UJSONHelper::GetStringValueFromJSON(FString JSON, FString Key, FString& Value, bool& Success) {

	TSharedPtr<FJsonObject> Object = GeTJsonSetup(JSON, Key, Success);
	if (Success) {
		Success = Object->TryGetStringField(Key, Value);
	}
}

void UJSONHelper::GetFloatValueFromJSON(FString JSON, FString Key, float& Value, bool& Success) {

	TSharedPtr<FJsonObject> Object = GeTJsonSetup(JSON, Key, Success);
	if (Success) {
		double DoubleValue;
		Success = Object->TryGetNumberField(Key, DoubleValue);
		Value = DoubleValue;
	}
}

void UJSONHelper::GetBoolValueFromJSON(FString JSON, FString Key, bool& Value, bool& Success) {

	TSharedPtr<FJsonObject> Object = GeTJsonSetup(JSON, Key, Success);
	if (Success) {
		Success = Object->TryGetBoolField(Key, Value);
	}
}

void UJSONHelper::GetIntValueFromJSON(FString JSON, FString Key, int& Value, bool& Success) {

	TSharedPtr<FJsonObject> Object = GeTJsonSetup(JSON, Key, Success);
	if (Success) {
		Success = Object->TryGetNumberField(Key, Value);
	}
}

void UJSONHelper::GeTJsonFieldFromJSON(FString JSON, FString Key, FString& Value, bool& Success) {

	TSharedPtr<FJsonObject> Object = GeTJsonSetup(JSON, Key, Success);
	if (Success) {
		TSharedPtr<FJsonValue> Field = Object->TryGetField(Key);
		if (Field->IsNull()) {
			Success = false;
		}
		else {
			//Add full JSON model to interface
			FString JSONOutput;
			TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&JSONOutput);
			FJsonSerializer::Serialize(Field->AsObject().ToSharedRef(), Writer);

			Value = JSONOutput;
		}
	}
}

void UJSONHelper::GetArrayFieldFromJSON(FString JSON, FString Key, TArray<FString>& Values, bool& Success) {

	TSharedPtr<FJsonObject> Object = GeTJsonSetup(JSON, Key, Success);
	if (Success) {
		const TArray<TSharedPtr<FJsonValue>>* FieldArray;
		Success = Object->TryGetArrayField(Key, FieldArray);
		if (Success) {
			for (auto& JSONValue : *FieldArray) {
				if (JSONValue.IsValid()) {
					FString JSONOutput;
					TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&JSONOutput);
					FJsonSerializer::Serialize(JSONValue->AsObject().ToSharedRef(), Writer);

					Values.Add(JSONOutput);
				}
			}
		}
	}
}

TSharedPtr<FJsonObject> UJSONHelper::GeTJsonSetup(FString JSON, FString Key, bool& Success) {

	//Deserialize string to JSON object
	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(JSON);
	TSharedPtr<FJsonObject> Object;
	Success = true;

	//Return if not successful
	if (!FJsonSerializer::Deserialize(Reader, Object)) {
		Success = false;
		UE_LOG(LogClass, Warning, TEXT("Could not deserialize JSON data : %s"), *JSON);
	}
	else if (!Object->HasField(Key)) {
		Success = false;
		UE_LOG(LogClass, Warning, TEXT("Could not find key %s in JSON in data : %s"), *Key, *JSON);
	}

	return Object;
}