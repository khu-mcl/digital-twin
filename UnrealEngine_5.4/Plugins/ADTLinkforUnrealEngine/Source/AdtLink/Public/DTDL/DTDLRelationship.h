//////////////////////////////////////////////////////
// Copyright (C) WSP. 2021. All rights reserved.
//////////////////////////////////////////////////////

#pragma once

#include "DTDLProperty.h"

#include "DTDLRelationship.generated.h"

/*
*	DTDL Property struct.
*	https://github.com/Azure/opendigitaltwins-DTDL/blob/master/DTDL/v2/DTDLv2.md#property
*
*	_ indicates required.
*
*/
USTRUCT(BlueprintType)
struct FDTDLRelationship {
	GENERATED_USTRUCT_BODY()

public:

	/*
	*	Required
	*	The "programming" name of the property.
	*	Must match this regular expression ^[a-zA-Z](?:[a-zA-Z0-9_]*[a-zA-Z0-9])?$
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		FString _Name;

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
	*   A localizable name for display
   */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		TMap<FString, FString> DisplayName;

	/*
	*	Required - Value -1 indicates the default value infinite
	*	The maximum multiplicity for the target of the relationship. The default value is infinite (there may be an unlimited number of relationship instances for this relationship).
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT", meta = (MakeStructureDefaultValue = "-1"))
		int MaxMultiplicity;

	/*
	*	Required
	*	The minimum multiplicity for the target of the relationship. The default value is 0 (this relationship is permitted to have no instances). In DTDL v2, minMultiplicity must always be 0.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT", meta = (MakeStructureDefaultValue = "0"))
		int MinMultiplicity;

	/*
	*	Required
	*	An interface ID. The default value (when target is not specified) is that the target may be any interface.
   */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		FString Target;

	/*
	*	Required
	*	A boolean value that indicates whether the property is writable by an external source, such as an application, or not.
   */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT", meta = (MakeStructureDefaultValue = "true"))
		bool Writable;

	/*
	*	Not implemented
	*	Optional
	*	A set of Properties that define relationship-specific state
	*
	*	TMap<FString, FDTDLProperty> Property;
	*/

	/*
	*	Ignore
	*	Full JSON string of the relationship, generated on upload or download from ADT.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		FString JSON;
};