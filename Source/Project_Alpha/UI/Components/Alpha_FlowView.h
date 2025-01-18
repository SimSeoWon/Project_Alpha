// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Alpha_FlowView.generated.h"

/**
 * 
 */

class UImage;
class UPanelWidget;
class UAlpha_FlowNodeBase;
class USlateBrushAsset;
class UAlpha_FlowNodeDataBase;

UCLASS()
class PROJECT_ALPHA_API UAlpha_FlowView : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual void SynchronizeProperties() override;

	void ClearFlowView();

	void OnPaint_Internal(FPaintContext& inContext) const;
	void DrowTree(FPaintContext& inContext, UAlpha_FlowNodeDataBase* inEntryData) const;

	UAlpha_FlowNodeBase* MakeTreeNode(UAlpha_FlowNodeDataBase* inEntryData, FVector2D vecStartPos);

public:
	void SetListItems(const TArray<UAlpha_FlowNodeDataBase*>& inListItems);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<UAlpha_FlowNodeBase> NodeAsset;

	TArray<TWeakObjectPtr<UAlpha_FlowNodeBase>> FlowNodeList;

	UPROPERTY(Transient)
	TArray<UAlpha_FlowNodeDataBase*> EntryDataList;

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

	
#endif

protected:
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



	UPROPERTY(EditAnywhere)
	USlateBrushAsset* BrushAsset;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* RootPanel = nullptr;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* TilePanel = nullptr;
	
};
