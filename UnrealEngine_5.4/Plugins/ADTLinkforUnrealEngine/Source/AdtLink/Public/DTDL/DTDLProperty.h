//////////////////////////////////////////////////////
// Copyright (C) WSP. 2021. All rights reserved.
//////////////////////////////////////////////////////

#pragma once

#include "DTDLSchemasEnum.h"

#include "DTDLProperty.generated.h"

/*
*	DTDL Property struct.
*	https://github.com/Azure/opendigitaltwins-DTDL/blob/master/DTDL/v2/DTDLv2.md#property
*
*	_ indicates required.
*
*/
USTRUCT(BlueprintType)
struct FDTDLProperty
{
	GENERATED_USTRUCT_BODY()

public:

	/*
	*	Ignore: This must be "Interface",
	*	This must at least be "Property". It can also include a semantic type.
	*/
	//	FString _Type;

	/*
	*	Required
	*	The "programming" name of the property.
	*	Must match this regular expression ^[a-zA-Z](?:[a-zA-Z0-9_]*[a-zA-Z0-9])?$
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		FString _Name;

	/*
	*	Required
	*	boolean	A boolean value
	*	date	A full-date as defined in section 5.6 of RFC 3339
	*	dateTime	A date-time as defined in RFC 3339
	*	double	An IEEE 8-byte floating point
	*	duration	A duration in ISO 8601 format (not implemented)
	*	float	An IEEE 4-byte floating point
	*	integer	A signed 4-byte integer
	*	long	A signed 8-byte integer
	*	string	A UTF8 string
	*	time	A full-time as defined in section 5.6 of RFC 3339
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		DTDLSchemas _Schema;

	/*
	*	Optional
	*	The ID of the property. If no @id is provided, the digital twin interface processor will assign one.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		FString Id;

	/*
	*	Optional
	*	A comment for model authors
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		FString Comment;

	/*
	*	Optional
	*	A localizable description for display
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		TMap<FString, FString> Description;

	/*
	*	Optional
	*	A localizable name for display
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		TMap<FString, FString> DisplayName;

	/*
	*  Not implemented
	*  The unit type of the property. A semantic type is required for the unit property to be available.
	*/
	//	FString Unit;

	/*
	*	Required
	*	A boolean value that indicates whether the property is writable by an external source, such as an application, or not.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT", meta = (MakeStructureDefaultValue = "true"))
		bool Writable;

	/*
	*	Ignore.
	*	Full JSON string of the property, generated on upload or download from ADT.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		FString JSON;
};