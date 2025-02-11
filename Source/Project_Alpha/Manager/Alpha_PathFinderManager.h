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
	TSharedPtr<FRES_HEXAGONTILEMAP> TableData = nullptr; // �ش� ������ ���̺� ������
	int32 Column = 1; // ��
	FVector Offset = FVector(0.0f, 0.0f, 0.0f);
	float TileSize = 0; // Ÿ�� ������
	float TileHeight = 0.0f;
	float TileWidth = 0.0f;
	float TileHalfHeight = 0.0f;
	float TileHalfWidth = 0.0f;
	float TileQuarterHeight = 0.0f;
	FVector VecMinimum; // �ٰ��� �浹 ������ ���̱� ���ؼ� �����..
	FVector VecMaximum; // �ٰ��� �浹 ������ ���̱� ���ؼ� �����..
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
	FVector2D GetAxialCoordinates(); //2�� ��ǥ��� �����Ѵ�.
	bool CollisionCheck(const FVector inPos); // ���� �ٸ�ü �浹 �˻�.

	int32 GetIndex();
	FVector GetCenter();
	EHexagonTileHeight GetHeightType();
	bool GetCornerList(TArray<FVector>& outCornerList);

protected:
	bool CollisionCheck(FVector posA, FVector posB, FVector posC, const FVector inPos);
	FVector2D CubeToAxial(FVector cube); // 3�� ��ǥ�踦 2�� ��ǥ��� �����Ѵ�.
	FVector AxialToCube(FVector2D axuak); // 2�� ��ǥ�踦 3�� ��ǥ��� �����Ѵ�.
	FVector2D OddToAxial(FVector2D offset); //�迭 ���¸� 2�� ��ǥ��� �����Ѵ�.
	FVector2D AddOffset(FVector2D a, FVector2D b);

protected:
	FVector2D Coordinates; // �׸��� ��ǥ
	float Size = 0.0f; // ������
	float Height = 0.0f; // 2 * Size
	float Width = 0.0f; // sqrt(3) * size
	TSharedPtr<FHexagonTile> HexagonTile = nullptr;
	TArray<TArray<FVector2D>> AppendCoordinates;
};