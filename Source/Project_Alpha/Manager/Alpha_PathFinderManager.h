// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Alpha_PathFinderManager.generated.h"

struct FHexagonTile;
struct FRES_HEXAGONTILEMAP;
class FHexagonTIleWrapper;
enum class EHexagonTileHeight :uint8;


/**
 *
 */
UCLASS()
class UAlpha_PathFinderManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static UAlpha_PathFinderManager* Get();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override; // Begin USubsystem 
	virtual void Deinitialize() override; // End USubsystem

	TSharedPtr<FHexagonTIleWrapper> CollisionCheck(FVector inLocation);
	TSharedPtr<FHexagonTIleWrapper> GetNearestTile(FVector2D inNeighbors);
	TSharedPtr<FHexagonTIleWrapper> GetTileInfo(int32 index);

	double Heuristic(TSharedPtr<FHexagonTIleWrapper> a, TSharedPtr<FHexagonTIleWrapper> b);
	bool FindPath(TArray<int32>& OutPaths, int32 inSource, int32 inDestination);
	float GetAngle(int32 inSource, int32 inDestination, bool isDegreee = true);
	
	void SetHexagonTile(FRES_HEXAGONTILEMAP* inHexagonTileMap);
	void Clear();
	
protected:
#pragma region Tile Map Data
	TSharedPtr<FRES_HEXAGONTILEMAP> TableData = nullptr; // 해당 레벨의 테이블 데이터
	int32 Column = 1; // 열
	FVector Offset = FVector(0.0f, 0.0f, 0.0f);
	float TileSize = 0; // 타일 사이즈
	float TileHeight = 0.0f;
	float TileWidth = 0.0f;
	float TileHalfHeight = 0.0f;
	float TileHalfWidth = 0.0f;
	float TileQuarterHeight = 0.0f;
	FVector VecMinimum; // 다각형 충돌 검출을 줄이기 위해서 사용함..
	FVector VecMaximum; // 다각형 충돌 검출을 줄이기 위해서 사용함..
	TArray<TSharedPtr<FHexagonTIleWrapper>> HexTileList;
	TSharedPtr<FHexagonTIleWrapper> SelectTileData = nullptr;
#pragma endregion
};

class FHexagonTIleWrapper
{
public:
	FHexagonTIleWrapper();
	void Clear();
	void SetTableData(FVector2D inCoordinates, FHexagonTile* inData, float inSize, float inWidth, float inHeight);
	void GetAroundDoordinatesList(TArray<FVector2D>& outCoordinatesList);
	FVector GetTopPostion();
	FVector2D GetAxialCoordinates(); //2축 좌표계로 변경한다.
	bool CollisionCheck(const FVector inPos); // 점과 다면체 충돌 검사.

	int32 GetIndex();
	FVector GetCenter();
	EHexagonTileHeight GetHeightType();
	bool GetCornerList(TArray<FVector>& outCornerList);

protected:
	bool CollisionCheck(FVector posA, FVector posB, FVector posC, const FVector inPos);
	FVector2D CubeToAxial(FVector cube); // 3축 좌표계를 2축 좌표계로 변경한다.
	FVector AxialToCube(FVector2D axuak); // 2축 좌표계를 3축 좌표계로 변경한다.
	FVector2D OddToAxial(FVector2D offset); //배열 형태를 2축 좌표계로 변경한다.
	FVector2D AddOffset(FVector2D a, FVector2D b);

protected:
	FVector2D Coordinates; // 그리드 좌표
	float Size = 0.0f; // 사이즈
	float Height = 0.0f; // 2 * Size
	float Width = 0.0f; // sqrt(3) * size
	TSharedPtr<FHexagonTile> HexagonTile = nullptr;
	TArray<TArray<FVector2D>> AppendCoordinates;
};