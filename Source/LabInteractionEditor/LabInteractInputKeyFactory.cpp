#include "LabInteractInputKeyFactory.h"

#include "LabInteraction/Core/LabInteractInputKey.h"

ULabInteractInputKeyFactory::ULabInteractInputKeyFactory()
{
	SupportedClass = ULabInteractInputKey::StaticClass();
	bCreateNew = true;
	bEditorImport = false;
	bEditAfterNew = true;
}

UObject* ULabInteractInputKeyFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName,
	EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<ULabInteractInputKey>(InParent, InClass, InName, Flags);
}

bool ULabInteractInputKeyFactory::CanCreateNew() const
{
	return true;
}
