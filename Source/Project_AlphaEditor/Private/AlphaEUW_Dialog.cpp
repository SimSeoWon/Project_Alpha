// Fill out your copyright notice in the Description page of Project Settings.


#include "AlphaEUW_Dialog.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Engine/DataTable.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SavePackage.h"
#include "Editor.h"
#include "EditorModeManager.h"

#include "Project_Alpha/Tables/tabledata/tabledata_resource.h"
#include "Project_Alpha/Tables/tabledata/tabledata.h"
#include "Project_Alpha/UI/Components/Alpha_FlowView.h"
#include "Project_Alpha/UI/Components/Alpha_FlowNodeBase.h"

void UAlphaEUW_Dialog::Run()
{
	if (IsValid(EditorView))
	{
		EditorView->Run();
	}
}

void UAlphaEUW_Dialog::NativeConstruct()
{
	Super::NativeConstruct();


}

void UAlphaEUW_Dialog::NativeDestruct()
{
	Super::NativeDestruct();
}

void UAlphaEditorView_Dialog::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_SaveFile->OnClicked.AddDynamic(this, &UAlphaEditorView_Dialog::OnClicked_SaveFile);


	Btn_LoadFile->OnClicked.AddDynamic(this, &UAlphaEditorView_Dialog::OnClicked_LoadFile);
}

void UAlphaEditorView_Dialog::NativeDestruct()
{
	Super::NativeDestruct();
}


void UAlphaEditorView_Dialog::Run()
{
	FilePath = TEXT("");
	FileName = TEXT("");

	UWorld* world = GetWorld();
	if (IsValid(world))
	{
		FString strWorldName;
		world->GetName(strWorldName);
		if (false == strWorldName.IsEmpty())
		{
			FileName = FString::Printf(TEXT("res_dialoggroup%s"), *strWorldName);
			FilePath = FString::Printf(TEXT("DataTable'/Game/GameData/resourcetable/%s.%s'"), *FileName, *FileName);
		}
	}

	EditTxt_Path->SetText(FText::FromString(FilePath));

	OnClicked_LoadFile();
	//FlowView->
}


void UAlphaEditorView_Dialog::OnClicked_LoadFile()
{
	//FString FileName = TEXT("");
	//FString FilePath = TEXT("");

	if (IsValid(EditTxt_Path))
		FilePath = EditTxt_Path->GetText().ToString();

	

	UDataTable* dtDialog = LoadObject<UDataTable>(this, *FilePath);
	if (dtDialog)
	{
		TArray<FRES_DIALOG_GROUP_LIST*> tableRows;
		dtDialog->GetAllRows(TEXT("mapTableRows == nullptr"), tableRows);

		DialogList.Reset();
		DialogList.Reserve(tableRows.Num());

		for (int32 i = 0; i < tableRows.Num(); i++)
		{
			if (nullptr == tableRows[i])
				continue;

			TSharedPtr<FRES_DIALOG_GROUP_LIST> sharedPtr = MakeShared<FRES_DIALOG_GROUP_LIST>(*tableRows[i]);
			if (false == sharedPtr.IsValid())
				continue;

			DialogList.Add(sharedPtr);
		}
	}

	FlowView->DeserializedData(DialogList);

}

void UAlphaEditorView_Dialog::OnClicked_SaveFile()
{
	FString strFilePath = TEXT("");
	FString strFileName = TEXT("");

	UWorld* world = GetWorld();
	if (IsValid(world))
	{
		FString strWorldName;
		world->GetName(strWorldName);
		if (false == strWorldName.IsEmpty())
		{
			strFileName = FString::Printf(TEXT("res_dialoggroup%s"), *strWorldName);
			strFilePath = FString::Printf(TEXT("DataTable'/Game/GameData/resourcetable/%s.%s'"), *strFileName, *strFileName);
		}
	}

	UDataTable* dtHexagonTileMap = LoadObject<UDataTable>(this, *strFilePath);
	if (dtHexagonTileMap)
	{
		/**직접 제거해야할까??*/
	}

	CreateFile(strFileName);
}

void UAlphaEditorView_Dialog::CreateFile(FString inFileName) 
{
	FString packagePath = FString::Printf(TEXT("/Game/GameData/resourcetable/%s"), *inFileName);
	FString assetPath = FPaths::ProjectContentDir() + TEXT("GameData/resourcetable/");
	UPackage* package = CreatePackage(*packagePath);
	if (false == IsValid(package))
		return;

	package->FullyLoad();
	UDataTable* dataTable = NewObject<UDataTable>(package, UDataTable::StaticClass(), *inFileName, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone);
	if (false == IsValid(dataTable))
		return;

	SetData(dataTable);
	FAssetRegistryModule::AssetCreated(dataTable);
	package->MarkPackageDirty();

	FString filePath = FString::Printf(TEXT("%s%s%s"), *assetPath, *inFileName, *FPackageName::GetAssetPackageExtension());
	bool bSuccess = UPackage::SavePackage(package, dataTable, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *filePath);

	if (bSuccess)
	{
		// 컨텐츠 브라우저에 에셋 표시
		TArray<UObject*> ObjectsToSync;
		ObjectsToSync.Add(dataTable);
		GEditor->SyncBrowserToObjects(ObjectsToSync);
	}
}

void UAlphaEditorView_Dialog::SetData(UDataTable* inDataTable)
{
	if (false == IsValid(inDataTable))
		return;

	inDataTable->RowStruct = FRES_DIALOG_GROUP_LIST::StaticStruct();

	for (auto iter : FlowView->EntryDataList)
	{
		if (false == IsValid(iter))
			continue;

		TSharedPtr<FRES_DIALOG_GROUP_LIST> rowData;
		rowData = MakeShared< FRES_DIALOG_GROUP_LIST>();
		if (false == rowData.IsValid())
			continue;

		iter->SerializedData(rowData);
		FName rowName = FName(FString::FormatAsNumber(rowData->ID));
		inDataTable->AddRow(rowName, *rowData.Get());
	}

}