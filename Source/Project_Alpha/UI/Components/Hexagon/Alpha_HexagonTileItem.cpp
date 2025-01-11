// Fill out your copyright notice in the Description page of Project Settings.


#include "Alpha_HexagonTileItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanelSlot.h"


#include "Alpha_HexagonTileDataBase.h"

void UAlpha_HexagonTileItem::NativeOnListItemObjectSet(UObject* InObject)
{
	if (false == IsValid(InObject))
		return;

	TileData = Cast<UAlpha_HexagonTileDataBase>(InObject);
	TileData->SetTileObject(this);
	Refresh();
}

void UAlpha_HexagonTileItem::Refresh()
{
	if (false == IsValid(TileData))
		return;

	float size = TileData->GetSize();
	if (IsValid(Img_Hexagon)) 
	{
		UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(Img_Hexagon->Slot);
		if (IsValid(slot)) 
		{
			slot->SetSize(FVector2D(size * 2, size * 2));
		}
	}
	if (IsValid(Txt_Index)) 
	{
		Txt_Index->SetText(FText::FromString(FString::Printf(TEXT("%d"), TileData->Index)));
	}
	
	bool isSelect = TileData->GetIsSelect();
	SetSelect(isSelect);
}

void UAlpha_HexagonTileItem::SetSelect(bool isSelect) 
{
	PlayAnimationForward(isSelect ? Ani_Select : Ani_Default);
}