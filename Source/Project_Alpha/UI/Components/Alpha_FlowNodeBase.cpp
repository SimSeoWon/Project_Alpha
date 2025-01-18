// Fill out your copyright notice in the Description page of Project Settings.


#include "Alpha_FlowNodeBase.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanelSlot.h"
#include "Project_Alpha/Tables/tabledata/tabledata_resource.h"

void UAlpha_FlowNodeBase::NativeOnListItemObjectSet(UObject* InObject)
{
	if (false == IsValid(InObject))
		return;

	EntryData = Cast<UAlpha_FlowNodeDataBase>(InObject);
	EntryData->SetTileObject(this);
	Refresh();
}

void UAlpha_FlowNodeBase::Refresh()
{
	if (false == IsValid(EntryData))
		return;


	if (false == IsValid(Img_BG->Slot))
		return;


	UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(Img_BG->Slot);
	if (false == IsValid(slot))
		return;

	slot->SetSize(FVector2D(EntryData->Width, EntryData->Height));
}

void UAlpha_FlowNodeBase::SetSelect(bool isSelect)
{
	//PlayAnimationForward(isSelect ? Ani_Select : Ani_Default);
}

void UAlpha_FlowNodeDataBase::Init()
{

}

void UAlpha_FlowNodeDataBase::Initialized_Paint(FVector2D pos, float inWidth, float inHeight, FVector2D inVecPadding)
{
	VecPostion = pos;
	//Parent = inParent;
	Width = inWidth;
	Height = inHeight;
	VecPadding = inVecPadding;

	TopCenter = BottomCenter = VecPostion;
	TopCenter.X += Width / 2;
	BottomCenter.X += Width / 2;
	BottomCenter.Y += Height;
}

void UAlpha_FlowNodeDataBase::SetTileObject(UAlpha_FlowNodeBase* inTileObjectPtr)
{

}

float UAlpha_FlowNodeDataBase::GetTotalWidth()
{

	if (0 >= ChildrenList.Num())
		return Width + VecPadding.X;

	float width = 0;

	for (int32 i = 0; i < ChildrenList.Num(); i++)
	{
		UAlpha_FlowNodeDataBase* child = ChildrenList[i];
		if (false == IsValid(child))
			continue;

		width += child->GetTotalWidth();
	}

	return width;
}

float UAlpha_FlowNodeDataBase::GetTotalHeight()
{
	return Height;
}

void UAlpha_FlowNodeDataBase::Attach(UAlpha_FlowNodeDataBase* inChild)
{
	if (false == IsValid(inChild))
		return;

	if (ChildrenList.Contains(inChild))
		return;

	ChildrenList.Add(inChild);
	inChild->Parent = this;
}

FVector2D UAlpha_FlowNodeDataBase::GetBottomCenter()
{
	return BottomCenter;
}

FVector2D UAlpha_FlowNodeDataBase::GetTopCenter()
{
	return TopCenter;
}

int32 UAlpha_FlowNodeDataBase::GetParentID()
{
	if (false == Parent.IsValid())
		return 0;

	return Parent->ID;
}

void UAlpha_FlowNodeDataBase::SerializedData(TSharedPtr<FRES_DIALOG_GROUP_LIST> outData)
{
	if (false == outData.IsValid())
		return;

	FDialogInfo dialogData;
	dialogData.ID = ID;
	dialogData.ParentID = GetParentID();
	//dialogData.ScriptList
	//dialogData.SelectList

	outData->SerializedData.Add(dialogData);

	// �ڽ� ������ �����.

	// ���� ��尡 �ִ°�?
	if (ChildrenList.IsEmpty())
		return;

	for (auto iter : ChildrenList)
	{
		if (false == IsValid(iter))
			continue;

		iter->SerializedData(outData);
	}
}

void UAlpha_FlowNodeDataBase::SetData(FDialogInfo& inDialogData)
{
	ID = inDialogData.ID;
	//.ParentID; // �θ� ����� ���̵�
	//inDialogData.ScriptList; // �����
	//inDialogData.SelectList; // ������

}


void UAlpha_FlowRootNodeData::SerializedData(TSharedPtr<FRES_DIALOG_GROUP_LIST> outData)
{
	if (false == outData.IsValid())
		return;

	// �� ���, ���극�� ���̵� �� �߰�

	outData->SubLevelID = SubLevelID;
	outData->ID = ID;

	Super::SerializedData(outData);
}

bool UAlpha_FlowRootNodeData::DeserializedData(TSharedPtr<FRES_DIALOG_GROUP_LIST> inTableData)
{
	if (false == inTableData.IsValid())
		return false;

	TableData = inTableData;

	SubLevelID = inTableData->SubLevelID;
	ID = inTableData->ID;
	//inTableData->CharacterList; // �����ι�

	TArray<UAlpha_FlowNodeDataBase*> parentStack;
	parentStack.Reserve(5);

	for (FDialogInfo& iter : inTableData->SerializedData)
	{
		// ����!
		if (iter.ID == ID)
		{
			SetData(iter);
			parentStack.Emplace(this);
			continue;
		}
		else 
		{
			while (true)
			{
				// ��ϵ� ��尡 ���� ������...?!!!
				if (parentStack.IsEmpty())
					break;

				// ���ÿ� ���� �������� ��ϵ� ���� ���Ѵ�.
				if (iter.ParentID != parentStack.Last()->ID)
				{
					parentStack.Pop(); // ������ ��ϵ� ��� ����
					continue;
				}


				UAlpha_FlowNodeDataBase* child = NewObject<UAlpha_FlowNodeDataBase>();
				if (false == IsValid(child))
					break;

				child->SetData(iter);
				parentStack.Last()->Attach(child);
				parentStack.Emplace(child);
				break;
			}
		}
	}

	return false;
}