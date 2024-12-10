#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "LabInteractInputKeyFactory.generated.h"

UCLASS()
class LABINTERACTIONEDITOR_API ULabInteractInputKeyFactory : public UFactory
{
	GENERATED_BODY()

public:
	ULabInteractInputKeyFactory();
	
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool CanCreateNew() const override;
	
};
