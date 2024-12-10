#include "LabInteractInputKeyAction.h"

#include "LabInteraction/Core/LabInteractInputKey.h"

ULabInteractInputKeyAction::ULabInteractInputKeyAction(EAssetTypeCategories::Type Category)
{
	AssetCategory = Category;
}

FText ULabInteractInputKeyAction::GetName() const
{
	return FText::FromString("Interact Input Key");
}

FColor ULabInteractInputKeyAction::GetTypeColor() const
{
	return FColor(255, 111, 0);
}

UClass* ULabInteractInputKeyAction::GetSupportedClass() const
{
	return ULabInteractInputKey::StaticClass();
}

void ULabInteractInputKeyAction::OpenAssetEditor(const TArray<UObject*>& InObjects,
	TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	FAssetTypeActions_Base::OpenAssetEditor(InObjects, EditWithinLevelEditor);
}

uint32 ULabInteractInputKeyAction::GetCategories()
{
	return AssetCategory;
}
