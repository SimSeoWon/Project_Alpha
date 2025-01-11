// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Alpha_HexagonTileDataBase.generated.h"

class UAlpha_HexagonTileItem;
enum class EHexagonTileType : uint8;
enum class EHexagonTileHeight : uint8;
/**
 * 
 */
UCLASS()
class PROJECT_ALPHA_API UAlpha_HexagonTileDataBase : public UObject
{
	GENERATED_BODY()
public:
	void Init(FVector2D inCoordinates, float inSize, float inWidth, float inHeight);

	FVector2D GetAxialCoordinates(); // 2축 좌표계로 변경한다.

	void GetAroundDoordinatesList(TArray<FVector2D>& outCoordinatesList);

	FVector2D GetTopPostion();
	bool CollisionCheck(const FVector2D inPos); // 점과 다면체 충돌 검사.

	bool GetIsSelect() { return IsSelect; }
	void SetIsSelect(bool isSelect) { IsSelect = isSelect; }

	float GetSize() { return Size; }

	void Refresh();

	void SetTileObject(UAlpha_HexagonTileItem* inTileObjectPtr);

	void SetTileType(EHexagonTileType inTileType);
	void SetHeightType(EHexagonTileHeight inHeightType);

	void SetTileType(FName inTypeName);
	void SetHeightType(FName inTypeName);

private:
	bool CollisionCheck(FVector2D posA, FVector2D posB, FVector2D posC, const FVector2D inPos);

	FVector2D CubeToAxial(FVector cube); // 3축 좌표계를 2축 좌표계로 변경한다.
	FVector AxialToCube(FVector2D hex); // 2축 좌표계를 3축 좌표계로 변경한다.

	FVector2D OddToAxial(FVector2D offset); // 배열 형태를 2축 좌표계로 변경.

	FVector2D AddOffset(FVector2D a, FVector2D b);
	//FVector2D AxialToOddr(FVector2D hex); // 2축 좌표계를 배열형태로 변경... 음수 처리가 안된다...

public:
	EHexagonTileType TileType;
	EHexagonTileHeight HeightType;
	int32 Index = 0;		// 인덱스
	FVector2D Coordinates; //  좌표
	FVector2D Center;  // 센터 위치
	float Size = 0.0f; // 사이즈
	float Height = 0.0f; //  2 * size;
	float Width = 0.0f; // sqrt(3) * size;
	int32 SubLevelID = 0;
	TArray<FVector2D> CornerList; // 꼭지점 6개

	TArray<TArray<FVector2D>> AppendCoordinates;

	bool IsSelect = false;

	TWeakObjectPtr<UAlpha_HexagonTileItem> PtrTileObject;
};
