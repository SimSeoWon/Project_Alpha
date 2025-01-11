// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Blueprint/UserWidget.h"
#include "Alpha_HexagonTileView.generated.h"

class UImage;
class UPanelWidget;
class UAlpha_HexagonTileDataBase;
class UAlpha_HexagonTileItem;
class UAlpha_HexagonTouchButton;

DECLARE_DELEGATE_OneParam(FSelectHexagonTileDelegate, const FVector2D)
/**
 * 
 */
UCLASS()
class PROJECT_ALPHA_API UAlpha_HexagonTileView : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void SetTileListItems(float inTileSize, int32 inColumn, int32 inCount);

	void SetTileListItems(const TArray<UAlpha_HexagonTileDataBase*>& InListItems);

	UAlpha_HexagonTileDataBase* GetTileListItem(int32 inIndex);

	bool NearestTileCollisionCheck(FVector2D inPostion, FVector2D inAxialCoordinates);

	UFUNCTION(BlueprintCallable)
	void ClearHexagonTileList();

	UFUNCTION()
	void OnTouchStart(const FVector2D inPostion);
	void SetSelectTile(int32 inIndex);

	UFUNCTION()
	void OnClicked();

protected:
	virtual void SynchronizeProperties() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<UAlpha_HexagonTileItem> HexagonTileAsset;

	UPROPERTY(meta = (BindWidget))
	UAlpha_HexagonTouchButton* TouchButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* RootPanel = nullptr;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* TilePanel = nullptr;

#pragma region standards info
	UPROPERTY(EditAnywhere)
	float TileSize = 0; // 타일 사이즈

	float TileHeight = 0.0f;
	float TileWidth = 0.0f;
	float TileHalfHeight = 0.0f;
	float TileQuarterHeight = 0.0f;
	float TileHalfWidth = 0.0f;
	float TotalWidth = 0.0f;
	float TotalHeight = 0.0f;

#pragma endregion
	UPROPERTY(EditAnywhere)
	int32 Column = 1; // 열

	UPROPERTY(EditAnywhere)
	bool UseMultipleSelect = false;

	UPROPERTY(Transient)
	TArray<UAlpha_HexagonTileDataBase*> TileDataList;

	UPROPERTY(Transient)
	UAlpha_HexagonTileDataBase* SelectTileData = nullptr;

	TArray<TWeakObjectPtr<UAlpha_HexagonTileItem>> HexagonTileList;

	FSelectHexagonTileDelegate OnSelectHexagonTile;

#if WITH_EDITORONLY_DATA
private:
	UPROPERTY(EditAnywhere)
	bool Edit_Preview = false;

	UPROPERTY(EditAnywhere)
	int32 Edit_Count = 10;
#endif
	
};
