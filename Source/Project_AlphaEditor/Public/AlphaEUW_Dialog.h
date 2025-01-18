// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "AlphaEUW_Dialog.generated.h"

/**
 *
 */
class UAlphaEditorView_Dialog;
class UAlpha_FlowView;
class UButton;
class UEditableTextBox;
struct FRES_DIALOG_LIST;
struct FRES_DIALOG_GROUP_LIST;
class UDataTable;

UCLASS()
class UAlphaEUW_Dialog : public UEditorUtilityWidget
{
	GENERATED_BODY()
public:
	void Run();


protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	UAlphaEditorView_Dialog* EditorView = nullptr;

};

UCLASS()
class UAlphaEditorView_Dialog : public UEditorUtilityWidget
{
	GENERATED_BODY()
public:
	void Run();

	UFUNCTION()
	void OnClicked_LoadFile();

	UFUNCTION()
	void OnClicked_SaveFile();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void SetData(UDataTable* inDataTable);
	void CreateFile(FString inFileName);
#if WITH_EDITORONLY_DATA
private:
	UPROPERTY(EditAnywhere)
	bool Edit_Preview = false;

	UPROPERTY(EditAnywhere)
	int32 Edit_Count = 3;

	UPROPERTY(EditAnywhere)
	int32 Edit_TreeHeight = 2;

	UPROPERTY(EditAnywhere)
	int32 Edit_TreeWidth = 2;


	UPROPERTY(EditAnywhere)
	FVector2D VecPadding;

	UPROPERTY(EditAnywhere)
	FVector2D NodeSize;

	UPROPERTY(EditAnywhere)
	FLinearColor LineColor = FLinearColor::Red;

	UPROPERTY(EditAnywhere)
	float LineThickness = 1.0f;

	UPROPERTY(EditAnywhere)
	float Width = 100.0f;

	UPROPERTY(EditAnywhere)
	float Height = 100.0f;
#endif

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_SaveFile = nullptr;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_LoadFile = nullptr;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* EditTxt_Path = nullptr;

	UPROPERTY(meta = (BindWidget))
	UAlpha_FlowView* FlowView = nullptr;

	FString FilePath;
	FString FileName;

	TArray<TSharedPtr<FRES_DIALOG_GROUP_LIST>> DialogList;
};

