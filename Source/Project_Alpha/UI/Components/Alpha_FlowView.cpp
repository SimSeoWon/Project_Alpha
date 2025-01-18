// Fill out your copyright notice in the Description page of Project Settings.


#include "Alpha_FlowView.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/PanelWidget.h"
#include "Components/CanvasPanelSlot.h"


#include "Alpha_FlowNodeBase.h"
#include "Project_Alpha/Tables/tabledata/tabledata_resource.h"

void UAlpha_FlowView::NativeConstruct()
{
	Super::NativeConstruct();
}

void UAlpha_FlowView::NativeDestruct()
{
	Super::NativeDestruct();
}

void UAlpha_FlowView::SynchronizeProperties()
{
	Super::SynchronizeProperties();


#if WITH_EDITORONLY_DATA
	// UI 디자인 시점 지원
	if (false == IsDesignTime())
		return;

	if (false == Edit_Preview)
		return;

	if (0 >= Edit_Count)
		return;

	TArray<UAlpha_FlowNodeDataBase*> arrListItems;

	for (int32 i = 0; i < Edit_Count; i++)
	{
		UAlpha_FlowNodeDataBase* info = NewObject<UAlpha_FlowNodeDataBase>();
		if (false == IsValid(info))
			continue;

		arrListItems.Add(info);
	}

	for (auto iter : arrListItems)
	{
		if (false == IsValid(iter))
			continue;

		UAlpha_FlowNodeDataBase* parent = iter;
		for (int32 i = 1; i < Edit_TreeHeight; i++)
		{
			UAlpha_FlowNodeDataBase* child = NewObject<UAlpha_FlowNodeDataBase>();
			if (false == IsValid(child))
				continue;

			parent->Attach(child);


			for (int32 j = 1; j < Edit_TreeWidth; j++) 
			{
				UAlpha_FlowNodeDataBase* siblings = NewObject<UAlpha_FlowNodeDataBase>();
				if (false == IsValid(siblings))
					continue;

				parent->Attach(siblings);
			}
			parent = child;
		}
	}

	SetListItems(arrListItems);

#endif
}

void UAlpha_FlowView::DeserializedData(const TArray<TSharedPtr<FRES_DIALOG_GROUP_LIST>>& inDialogList) 
{
	TArray<UAlpha_FlowNodeDataBase*> arrListItems;
	for (TSharedPtr<FRES_DIALOG_GROUP_LIST> iter : inDialogList)
	{
		if (false == iter.IsValid())
			continue;

		UAlpha_FlowRootNodeData* info = NewObject<UAlpha_FlowRootNodeData>();
		if (false == IsValid(info))
			continue;

		info->DeserializedData(iter);
		arrListItems.Add(info);
	}

	SetListItems(arrListItems);

	///Todo 테스트 코드
	/*

	for (int32 i = 0; i < Edit_Count; i++)
	{
		UAlpha_FlowRootNodeData* info = NewObject<UAlpha_FlowRootNodeData>();
		if (false == IsValid(info))
			continue;

		info->ID = i;

		arrListItems.Add(info);
	}

	for (auto iter : arrListItems)
	{
		if (false == IsValid(iter))
			continue;

		UAlpha_FlowNodeDataBase* parent = iter;
		for (int32 i = 1; i < Edit_TreeHeight; i++)
		{
			UAlpha_FlowNodeDataBase* child = NewObject<UAlpha_FlowNodeDataBase>();
			if (false == IsValid(child))
				continue;


			child->ID = parent->ID * 100 + i * 10;

			parent->Attach(child);


			for (int32 j = 1; j < Edit_TreeWidth; j++)
			{
				UAlpha_FlowNodeDataBase* siblings = NewObject<UAlpha_FlowNodeDataBase>();
				if (false == IsValid(siblings))
					continue;

				siblings->ID = parent->ID * 100 + i * 10 + j;
				parent->Attach(siblings);
			}
			parent = child;
		}
	}*/

	
}

void UAlpha_FlowView::SetListItems(const TArray<UAlpha_FlowNodeDataBase*>& inListItems)
{
	ClearFlowView();
	EntryDataList.Append(inListItems);

	int32 nodeCount = EntryDataList.Num();

	FVector2D vecStartPos = FVector2D::Zero();
	// 모든 트리의 루트들 일단 우측으로 세팅할까?
	for (int32 i = 0; i < nodeCount; i++)
	{
		UAlpha_FlowNodeDataBase* entryData = EntryDataList[i];
		if (false == IsValid(entryData))
			continue;

		UAlpha_FlowNodeBase* widget = MakeTreeNode(entryData, vecStartPos);
		if (false == IsValid(widget))
			continue;

		vecStartPos.X = vecStartPos.X + entryData->GetTotalWidth() + VecPadding.X;
	}
}

UAlpha_FlowNodeBase* UAlpha_FlowView::MakeTreeNode(UAlpha_FlowNodeDataBase* inEntryData, FVector2D vecStartPos)
{
	if (false == IsValid(inEntryData))
		return nullptr;

	FMargin margin{ 0, 0, 0, 0 };
	FVector2D vecPos(VecPadding.X + vecStartPos.X, VecPadding.Y + vecStartPos.Y);
	inEntryData->Initialized_Paint(vecPos, Width, Height, VecPadding);
	UAlpha_FlowNodeBase* widget = CreateWidget<UAlpha_FlowNodeBase>(GetWorld(), NodeAsset.LoadSynchronous());
	if (false == IsValid(widget))
		return nullptr;

	TilePanel->AddChild(widget);
	UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(widget->Slot);
	if (IsValid(slot))
		slot->SetOffsets(margin);

	widget->NativeOnListItemObjectSet(inEntryData);
	widget->SetRenderTranslation(inEntryData->VecPostion);
	FlowNodeList.Emplace(widget);


	if (0 >= inEntryData->ChildrenList.Num())
		return widget;

	FVector2D vecChildrenStartPos{ vecStartPos.X, (vecStartPos.Y + Height + VecPadding.Y) };
	for (auto iter : inEntryData->ChildrenList)
	{
		MakeTreeNode(iter, vecChildrenStartPos);
		vecChildrenStartPos.X = vecChildrenStartPos.X + iter->GetTotalWidth() + VecPadding.X;
	}

	return widget;
}

void UAlpha_FlowView::ClearFlowView()
{
	for (int32 i = 0; i < FlowNodeList.Num(); i++)
	{
		if (false == FlowNodeList[i].IsValid())
			continue;

		TilePanel->RemoveChild(FlowNodeList[i].Get());
	}

	FlowNodeList.Reset();
	EntryDataList.Reset();
}

int32 UAlpha_FlowView::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	//Super::NativePaint();
	if (bHasScriptImplementedPaint) // 블루프린트에서 구현된 경우
	{
		FPaintContext Context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
		OnPaint_Internal(Context);
		OnPaint(Context);

		//return FMath::Max(LayerId, Context.MaxLayer);
		return LayerId;
	}

	return LayerId;
}

void UAlpha_FlowView::OnPaint_Internal(FPaintContext& inContext) const
{

	for (auto iter : EntryDataList)
	{
		if (false == IsValid(iter))
			continue;

		DrowTree(inContext, iter);
	}
}

void UAlpha_FlowView::DrowTree(FPaintContext& inContext, UAlpha_FlowNodeDataBase* inEntryData) const
{

	if (false == IsValid(inEntryData))
		return;

	if(inEntryData->ChildrenList.IsEmpty())
		return;

	FVector2D vecParentBC = inEntryData->GetBottomCenter();
	FVector2D vecFirstChildTC = inEntryData->ChildrenList[0]->GetTopCenter();
	FVector2D vecYoungestChildTC = inEntryData->ChildrenList.Last()->GetTopCenter();
	float halfPadding = VecPadding.Y / 2;
	
	FVector2D vecCenterLine1(vecFirstChildTC.X,  vecParentBC.Y + halfPadding);
	FVector2D vecCenterLine2(vecYoungestChildTC.X, vecParentBC.Y + halfPadding);

	UWidgetBlueprintLibrary::DrawLine(inContext, vecParentBC, vecFirstChildTC, LineColor, true, LineThickness);
	UWidgetBlueprintLibrary::DrawLine(inContext, vecCenterLine1, vecCenterLine2, LineColor, true, LineThickness);

	for (auto iter : inEntryData->ChildrenList)
	{
		if (false == IsValid(iter))
			continue;

		FVector2D vecTopCenter = iter->GetTopCenter();
		FVector2D vecFrom(vecTopCenter.X, vecTopCenter.Y - halfPadding);
		
		UWidgetBlueprintLibrary::DrawLine(inContext, vecFrom, vecTopCenter, LineColor, true, LineThickness);
		DrowTree(inContext, iter);
	}

	//UWidgetBlueprintLibrary::DrawBox(inContext, FVector2D(0.0f, 0.0f), FVector2D(200.0f, 200.0f), BrushAsset, LineColor);

	// 자식들과 연결된 선 그리기.
	// 자식에게도 그리라고 재귀로 돌리기
	
}