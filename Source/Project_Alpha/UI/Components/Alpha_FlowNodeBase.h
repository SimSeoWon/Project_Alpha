// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Blueprint/IUserObjectListEntry.h>
#include "Alpha_FlowNodeBase.generated.h"

class UImage;
class UTextBlock;
class UAlpha_FlowNodeDataBase;
struct FRES_DIALOG_GROUP_LIST;
struct FDialogInfo;
/**
 *
 */
UCLASS()
class PROJECT_ALPHA_API UAlpha_FlowNodeBase : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:
	virtual void NativeOnListItemObjectSet(UObject* InObject) override;
	virtual void SetSelect(bool isSelect);
	virtual void Refresh();

private:
	UPROPERTY(meta = (BindWidget))
	UImage* Img_BG = nullptr;

	UAlpha_FlowNodeDataBase* EntryData = nullptr;
};

UCLASS()
class PROJECT_ALPHA_API UAlpha_FlowNodeDataBase : public UObject
{
	GENERATED_BODY()
public:
	void Init();
	void Initialized_Paint(FVector2D pos, float inWidth, float inHeight, FVector2D inVecPadding);

	void SetTileObject(UAlpha_FlowNodeBase* inTileObjectPtr);

	void Attach(UAlpha_FlowNodeDataBase* inChild);

	FVector2D GetBottomCenter();
	FVector2D GetTopCenter();
	
	virtual void SerializedData(TSharedPtr<FRES_DIALOG_GROUP_LIST> outData);
	void SetData(FDialogInfo& inDialogData);

	int32 GetParentID();

public:
	TWeakObjectPtr<UAlpha_FlowNodeDataBase> Parent;
	TArray<UAlpha_FlowNodeDataBase*> ChildrenList;
	TWeakObjectPtr<UAlpha_FlowNodeBase> PtrTileObject;

	float GetTotalWidth();
	float GetTotalHeight();

	FVector2D VecPostion;
	FVector2D VecPadding;
	float Width = 100.0f;
	float Height = 100.0f;
	
	FVector2D BottomCenter;
	FVector2D TopCenter;

	int32 ID = 0;
};



UCLASS()
class PROJECT_ALPHA_API UAlpha_FlowRootNodeData : public UAlpha_FlowNodeDataBase
{
	GENERATED_BODY()
public:
	virtual void SerializedData(TSharedPtr<FRES_DIALOG_GROUP_LIST> outData) override;
	bool DeserializedData(TSharedPtr<FRES_DIALOG_GROUP_LIST> inTableData);


public:
	int32 SubLevelID = 0;


	TWeakPtr<FRES_DIALOG_GROUP_LIST> TableData;
	// 자신의 아이디,
	// 캐릭터 목록
	// 서브레벨 아이디

};