// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Blueprint/IUserObjectListEntry.h>
#include "Alpha_HexagonTileItem.generated.h"

class UImage;
class UTextBlock;
class UAlpha_HexagonTileDataBase;
/**
 * 
 */
UCLASS()
class PROJECT_ALPHA_API UAlpha_HexagonTileItem : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:
	virtual void NativeOnListItemObjectSet(UObject* InObject) override;
	virtual void SetSelect(bool isSelect);
	virtual void Refresh();

public:
	UPROPERTY(meta = (BindWidget))
	UImage* Img_Hexagon = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Index = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Ani_Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Ani_Select;

private:
	UAlpha_HexagonTileDataBase* TileData = nullptr;
};
