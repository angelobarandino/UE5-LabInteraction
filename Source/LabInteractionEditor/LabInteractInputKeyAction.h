#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

class LABINTERACTIONEDITOR_API ULabInteractInputKeyAction : public FAssetTypeActions_Base
{
public:
	ULabInteractInputKeyAction(EAssetTypeCategories::Type Category);
	
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor) override;
	virtual uint32 GetCategories() override;

private:
	EAssetTypeCategories::Type AssetCategory;
};
