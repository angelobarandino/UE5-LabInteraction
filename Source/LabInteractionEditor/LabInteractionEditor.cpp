#include "LabInteractionEditor.h"
#include "IAssetTools.h"
#include "LabInteractInputKeyAction.h"

#define LOCTEXT_NAMESPACE "FLabInteractionEditorModule"

void FLabInteractionEditorModule::StartupModule()
{
	IAssetTools& AssetToolsModule = IAssetTools::Get();
	EAssetTypeCategories::Type AssetType = AssetToolsModule.RegisterAdvancedAssetCategory(
		FName(TEXT("LabInteraction")),
		FText::FromString("Interaction Plugin"));
	
	TSharedPtr<ULabInteractInputKeyAction> InteractionKeyAction = MakeShareable(new ULabInteractInputKeyAction(AssetType));
	AssetToolsModule.RegisterAssetTypeActions(InteractionKeyAction.ToSharedRef());
}

void FLabInteractionEditorModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FLabInteractionEditorModule, LabInteractionEditor)