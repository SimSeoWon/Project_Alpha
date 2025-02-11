// Fill out your copyright notice in the Description page of Project Settings.


#include "Alpha_PathFinderManager.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

#include "Project_Alpha/AlphaGameInstance.h"
#include "Project_Alpha/Base/AlphaLevelScriptActor.h"
#include "Project_Alpha/Tables/tabledata/tabledata_resource.h"
#include "Project_Alpha/Tables/tabledata/tabledata_enum.h"

UAlpha_PathFinderManager* UAlpha_PathFinderManager::UAlpha_PathFinderManager::Get()
{
	UAlphaGameInstance* instance = UAlphaGameInstance::Get();
	if (false == IsValid(instance))
		return nullptr;

	UWorld* world = instance->GetWorld();
	if (false == IsValid(world))
		return nullptr;

	return world->GetSubsystem<UAlpha_PathFinderManager>();
}

void UAlpha_PathFinderManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogCore, Warning, TEXT("UAlpha_PathFinderManager::Initialize()"));

	int32 count = 100;
	FString strFileName = TEXT("");
	FString strFilePath = TEXT("");

	UAlphaGameInstance* instance = UAlphaGameInstance::Get();
	if (false == IsValid(instance))
		return;

	UWorld* world = instance->GetWorld();
	if (false == IsValid(world))
		return;

	FString strWorldName;
	world->GetName(strWorldName);
	if (false == strWorldName.IsEmpty())
	{
		strFileName = FString::Printf(TEXT("res_%s"), *strWorldName);
		strFilePath = FString::Printf(TEXT("DataTable'/Game/GameData/resourcetable/%s.%s'"), *strFileName, *strFileName);
	}

	UDataTable* dtHexagonTileMap = LoadObject<UDataTable>(this, *strFilePath);
	if (dtHexagonTileMap)
	{
		FRES_HEXAGONTILEMAP* rowData = dtHexagonTileMap->FindRow<FRES_HEXAGONTILEMAP>(FName(TEXT("TEST")), TEXT(""));
		if (rowData)
		{
			Column = rowData->Column;
			Offset = rowData->Offset;
			TileSize = rowData->TileSize;

			VecMaximum = rowData->Maximum;
			VecMinimum = rowData->Minimum;

			TileHeight = 2 * TileSize;
			TileHalfHeight = TileSize;
			TileQuarterHeight = TileHalfHeight / 2;

			TileWidth = FMath::Sqrt(3.0f) * TileSize;
			TileHalfWidth = TileWidth / 2;

			SetHexagonTile(rowData);
		}
	}
}

void UAlpha_PathFinderManager::SetHexagonTile(FRES_HEXAGONTILEMAP* inHexagonTileMap)
{
	if (nullptr == inHexagonTileMap)
		return;

	int32 count = inHexagonTileMap->TileList.Num();

	HexTileList.Reset();
	HexTileList.Reserve(count);

	int32 x = 0;
	int32 y = 0;

	for (int32 i = 0; i < count; i++)
	{
		x = i % Column;
		y = i / Column;

		TSharedPtr<FHexagonTIleWrapper> tileInfo = MakeShared<FHexagonTIleWrapper>();
		if (false == tileInfo.IsValid())
			continue;

		tileInfo->SetTableData(FVector2D(x, y), &inHexagonTileMap->TileList[i], TileSize, TileWidth, TileHeight);
		HexTileList.Add(tileInfo);
	}
}

void UAlpha_PathFinderManager::Deinitialize()
{
	UE_LOG(LogCore, Warning, TEXT("UAlpha_PathFinderManager::Deinitialize()"));
	Clear();
	Super::Deinitialize();
}

void UAlpha_PathFinderManager::Clear()
{
	for (auto iter : HexTileList)
	{
		if (false == iter.IsValid())
			continue;

		iter->Clear();
	}
}

TSharedPtr<FHexagonTIleWrapper> UAlpha_PathFinderManager::CollisionCheck(FVector inLocation) 
{
	if (VecMinimum.X > inLocation.X || VecMinimum.Y > inLocation.Y ||
		VecMaximum.X < inLocation.X || VecMaximum.Y < inLocation.Y)
		return nullptr;

	int32 selectRow = 0;
	int32 selectColumn = 0;
	int32 index = 0;
	float threeQuartersHeight = TileHeight - TileQuarterHeight;
	FVector2D vec2dPos = FVector2D(inLocation.Y, (-1 * inLocation.X));

	if (vec2dPos.Y > TileQuarterHeight)
		selectRow = (vec2dPos.Y - TileQuarterHeight) / threeQuartersHeight;

	if ((selectRow & 0x0001))
	{
		selectColumn = vec2dPos.X - TileHalfWidth;
		if (selectColumn <= 0)
		{
			selectColumn = 0;
		}
		else
		{
			selectColumn = selectColumn / TileWidth;
		}
	}
	else
	{
		selectColumn = vec2dPos.X / TileWidth;
	}

	if (selectColumn >= Column)
		selectColumn = 1 - Column;

	index = selectRow * Column + selectColumn;
	if (false == HexTileList.IsValidIndex(index))
		return nullptr;

	if (HexTileList[index]->CollisionCheck(inLocation))
		return HexTileList[index];

	TArray<FVector2D> neighborsCoordinates;
	HexTileList[index]->GetAroundDoordinatesList(neighborsCoordinates);

	for (int32 i = 0; i < neighborsCoordinates.Num(); i++)
	{
		TSharedPtr<FHexagonTIleWrapper> nearestTile = GetNearestTile(neighborsCoordinates[i]);
		if (false == nearestTile.IsValid())
			continue;

		if (false == nearestTile->CollisionCheck(inLocation))
			continue;

		return nearestTile;
	}

	return nullptr;

}

TSharedPtr<FHexagonTIleWrapper> UAlpha_PathFinderManager::GetTileInfo(int32 index)
{
	if (false == HexTileList.IsValidIndex(index))
		return nullptr;

	TSharedPtr<FHexagonTIleWrapper> tileInfo = HexTileList[index];
	if (false == tileInfo.IsValid())
		return nullptr;

	return tileInfo;
}

TSharedPtr<FHexagonTIleWrapper> UAlpha_PathFinderManager::GetNearestTile(FVector2D inNeighbors) 
{
	if (inNeighbors.X >= Column)
		return nullptr;

	if (inNeighbors.X < 0 || inNeighbors.Y < 0)
		return nullptr;

	int32 index = inNeighbors.Y * Column + inNeighbors.X;
	if (HexTileList.IsValidIndex(index) == false)
		return nullptr;

	return HexTileList[index];
}

double UAlpha_PathFinderManager::Heuristic(TSharedPtr<FHexagonTIleWrapper> a, TSharedPtr<FHexagonTIleWrapper> b) 
{
	FVector centerA = a->GetCenter();
	FVector centerB = b->GetCenter();
	return FMath::Abs(centerA.X - centerB.X) + FMath::Abs(centerA.Y - centerB.Y);
}

float UAlpha_PathFinderManager::GetAngle(int32 inSource, int32 inDestination, bool isDegreee)
{
	// 목적지가 동일한 위치라면?
	if (inSource == inDestination)
		return 0.0f;

	// 타일이 존재하는가?
	if (false == HexTileList.IsValidIndex(inSource))
		return 0.0f;

	//타일이 존재하는가?
	if (false == HexTileList.IsValidIndex(inDestination))
		return 0.0f;

	TSharedPtr<FHexagonTIleWrapper> sourceTile = HexTileList[inSource];
	if (false == sourceTile.IsValid())
		return 0.0f;
	//UE_LOG(LogCore, Warning, TEXT("AAlphaLevelScriptActor::GetAngle() - sourceTile(%f, %f, %f)"), sourceTile->Center.X, sourceTile->Center.Y, sourceTile->Center.Z);

	TSharedPtr<FHexagonTIleWrapper> destinationTile = HexTileList[inDestination];
	if (false == destinationTile.IsValid())
		return 0.0f;
	//UE_LOG(LogCore, Warning, TEXT("AAlphaLevelScriptActor::GetAngle() - destinationTile(%f, %f, %f)"), destinationTile->Center.X, destinationTile->Center.Y, destinationTile->Center.Z);


	// 시작점에서 목적지를 향한 벡터... (주의 : 타일에 높낮이가 다르다!)
	FVector vecToDestination = destinationTile->GetCenter() - sourceTile->GetCenter();
	//UE_LOG(LogCore, Warning, TEXT("AAlphaLevelScriptActor::GetAngle() - vecToDestination(%f, %f, %f)"), vecToDestination.X, vecToDestination.Y, vecToDestination.Z);

	vecToDestination.Normalize(); // 길이를 1로 수정한다.
	//UE_LOG(LogCore, Warning, TEXT("AAlphaLevelScriptActor::GetAngle() - vecToDestination(%f, %f, %f)"), vecToDestination.X, vecToDestination.Y, vecToDestination.Z);

	// 외적을 구해서 왼편 오른편 구분하기
	FVector vecForword = FVector(0.0f, 1.0f, 0.0f);
	//UE_LOG(LogCore, Warning, TEXT("AAlphaLevelScriptActor::GetAngle() - vecForword(%f, %f, %f)"), vecForword.X, vecForword.Y, vecForword.Z);

	FVector vceCross = FVector::CrossProduct(vecToDestination, vecForword);
	//UE_LOG(LogCore, Warning, TEXT("AAlphaLevelScriptActor::GetAngle() - vceCross(%f, %f, %f)"), vceCross.X, vceCross.Y, vceCross.Z);

	float dotValue = FVector::DotProduct(vecForword, vecToDestination); // 2차원 평면으로 생각했을때...
	//UE_LOG(LogCore, Warning, TEXT("AAlphaLevelScriptActor::GetAngle() - dotValue(%f)"), dotValue);

	float radian = FMath::Acos(dotValue); // 내적에 Acos // 0 ~ 180 사의의 값이 나온다.
	//UE_LOG(LogCore, Warning, TEXT("AAlphaLevelScriptActor::GetAngle() - radian(%f)"), radian);
	if (false == isDegreee)
		return radian;

	float degreee = FMath::RadiansToDegrees(radian);
	if (0 < vceCross.Z) degreee = 360.0f - degreee;

	//UE_LOG(LogCore, Warning, TEXT("AAlphaLevelScriptActor::GetAngle() - degreee(%f)"), degreee);
	return degreee;
}

bool UAlpha_PathFinderManager::FindPath(TArray<int32>& OutPaths, int32 inSource, int32 inDestination)
{
	bool result = false;
	UE_LOG(LogCore, Warning, TEXT("AAlphaLevelScriptActor::FindPath() - Search Start inSource(%d), inDestination(%d)"), inSource, inDestination);
	//목적지까지 경로를 탐색해야한다...!
	OutPaths.Reset();

	// 목적지가 동일한 위치라면?
	if (inSource == inDestination)
		return result;

	// 타일이 존재하는가?
	if (false == HexTileList.IsValidIndex(inSource))
		return result;

	TSharedPtr<FHexagonTIleWrapper> sourceTile = HexTileList[inSource];
	if (false == sourceTile.IsValid())
		return result;

	//타일이 존재하는가?
	if (false == HexTileList.IsValidIndex(inDestination))
		return result;

	TSharedPtr<FHexagonTIleWrapper> destinationTile = HexTileList[inDestination];
	if (false == destinationTile.IsValid())
		return result;

	if (sourceTile->GetHeightType() != destinationTile->GetHeightType())
	{
		///Todo 사다리와 같은 기믹을 개발하면 개선하자. 지금은 일단 스킵처리
		return result;
	}

	TQueue<TSharedPtr<FHexagonTIleWrapper>> frontier; // 우선순위 큐
	frontier.Empty();// 초기화하고...

	TMap<int32, int32> mapPrevTile; // 이전 타일 정보
	TMap<int32, int32> mapCost; // 이동 코스트 정보

	if (!mapPrevTile.Contains(inSource)) mapPrevTile.Add(inSource, 0);
	mapPrevTile[inSource] = inSource;

	if (!mapCost.Contains(inSource)) mapCost.Add(inSource, 0);
	mapCost[inSource] = 0;

	frontier.Enqueue(sourceTile); // 시작 타일 넣고
	while (false == frontier.IsEmpty())
	{
		TSharedPtr<FHexagonTIleWrapper> currTile;
		frontier.Dequeue(currTile);

		if (false == currTile.IsValid())
			break;

		int32 currIndex = currTile->GetIndex();

		// 목적지에 도착함.
		if (destinationTile == currTile)
		{
			//UE_LOG(LogCore, Warning, TEXT("AAlphaLevelScriptActor::FindPath() - Goal!"));
			result = true;
			OutPaths.Add(currIndex);
			while (true)
			{
				int32 val = OutPaths.Last();
				if (false == mapPrevTile.Contains(val))
					break;

				if (val == inSource)
					break;

				int32 prevIndex = mapPrevTile[val];
				OutPaths.Add(prevIndex);
			}
			break;
		}

		// 1. 각도 계산.
		float degreee = GetAngle(currIndex, inDestination);
		int32 startIdx = degreee / 60.0f;

		//인접한 이웃 타일을 모두 가져오자.
		TArray<FVector2D> neighborsCoordinates;
		currTile->GetAroundDoordinatesList(neighborsCoordinates);
		int32 neighborsCount = neighborsCoordinates.Num();

		for (int32 i = 0; i < neighborsCount; i++)
		{
			int32 index = i + startIdx;
			index = index % neighborsCount;

			if (false == neighborsCoordinates.IsValidIndex(index))
				continue;

			// 인접한 이웃 타일
			TSharedPtr<FHexagonTIleWrapper> nearestTile = GetNearestTile(neighborsCoordinates[index]);
			if (false == nearestTile.IsValid())
				continue;

			///Todo: 지금은 일단 시작 위치와 다른 높이 타입은 무시하기.
			if (sourceTile->GetHeightType() != nearestTile->GetHeightType())
				continue;

			int32 nearestIndex = nearestTile->GetIndex();
			int32 newCost = mapCost[currIndex] + 1;
			if (!mapCost.Contains(nearestIndex) || newCost < mapCost[nearestIndex])
			{
				if (!mapCost.Contains(nearestIndex)) mapCost.Add(nearestIndex, 0);
				mapCost[nearestIndex] = newCost;

				if (!mapPrevTile.Contains(nearestIndex)) mapPrevTile.Add(nearestIndex, 0);
				mapPrevTile[nearestIndex] = currIndex;

				frontier.Enqueue(nearestTile);
			}
		}
	}

	return result;
}

FHexagonTIleWrapper::FHexagonTIleWrapper()
{
	AppendCoordinates =
	{
		{FVector2D(-1, -1), FVector2D(-1, 0), FVector2D(-1, 1), FVector2D(0, 1), FVector2D(1, 0), FVector2D(0, -1)},

		{FVector2D(0, -1), FVector2D(-1, 0), FVector2D(0, 1), FVector2D(1, 1), FVector2D(1, 0), FVector2D(1, -1)},
	};
}

void FHexagonTIleWrapper::SetTableData(FVector2D inCoordinates, FHexagonTile* inData, float inSize, float inWidth, float inHeight)
{
	if (nullptr == inData)
		return;

	HexagonTile = MakeShared<FHexagonTile>();
	(*HexagonTile.Get()) = *inData; // 대입연산자로 깊은 복사를 처리..

	Coordinates = inCoordinates;
	Size = inSize;
	Width = inWidth;
	Height = inHeight;
	float halfWidth = Width / 2;
	float halfHeight = Size;
	float quarterHeight = halfHeight / 2;

}

int32 FHexagonTIleWrapper::GetIndex()
{
	if (false == HexagonTile.IsValid())
		return 0;

	return HexagonTile->Index;
}

EHexagonTileHeight FHexagonTIleWrapper::GetHeightType()
{
	if (false == HexagonTile.IsValid())
		return EHexagonTileHeight::NONE;

	return HexagonTile->HeightType;
}

FVector FHexagonTIleWrapper::GetCenter()
{
	if (false == HexagonTile.IsValid())
		return FVector::Zero();

	return HexagonTile->VecCenter;
}

bool FHexagonTIleWrapper::GetCornerList(TArray<FVector>& outCornerList)
{
	outCornerList.Reset();
	if (false == HexagonTile.IsValid())
		return false;


	outCornerList.Reset(HexagonTile->CornerList.Num());
	for (auto iter : HexagonTile->CornerList)
	{
		outCornerList.Emplace(iter);
	}

	return true;
}

bool FHexagonTIleWrapper::CollisionCheck(const FVector inPos)
{
	if (false == HexagonTile.IsValid())
		return false;

	TArray<FVector> cornerList;
	if (false == GetCornerList(cornerList))
		return false;

	FVector center = GetCenter();

	if (CollisionCheck(center, cornerList[0], cornerList[1], inPos))
		return true;

	if (CollisionCheck(center, cornerList[1], cornerList[2], inPos))
		return true;

	if (CollisionCheck(center, cornerList[2], cornerList[3], inPos))
		return true;

	if (CollisionCheck(center, cornerList[3], cornerList[4], inPos))
		return true;

	if (CollisionCheck(center, cornerList[4], cornerList[5], inPos))
		return true;

	if (CollisionCheck(center, cornerList[5], cornerList[0], inPos))
		return true;

	return false;
}

bool FHexagonTIleWrapper::CollisionCheck(FVector posA, FVector posB, FVector posC, const FVector inPos)
{
	FVector vecAB = FVector(posB.X, posB.Y, 0) - FVector(posA.X, posA.Y, 0);
	FVector vecAC = FVector(posC.X, posC.Y, 0) - FVector(posA.X, posA.Y, 0);

	FVector vecBC = FVector(posC.X, posC.Y, 0) - FVector(posB.X, posB.Y, 0);
	FVector vecCA = FVector(posA.X, posA.Y, 0) - FVector(posC.X, posC.Y, 0);

	FVector vecAP = FVector(inPos.X, inPos.Y, 0) - FVector(posA.X, posA.Y, 0);
	FVector vecBP = FVector(inPos.X, inPos.Y, 0) - FVector(posB.X, posB.Y, 0);
	FVector vecCP = FVector(inPos.X, inPos.Y, 0) - FVector(posC.X, posC.Y, 0);

	FVector vecNormal = FVector::CrossProduct(vecAB, vecAC);
	vecNormal.Normalize();

	FVector vecCross_AB_AP = FVector::CrossProduct(vecAB, vecAP);
	FVector vecCross_BC_BP = FVector::CrossProduct(vecBC, vecBP);
	FVector vecCross_CA_CP = FVector::CrossProduct(vecCA, vecCP);


	if (FVector::DotProduct(vecNormal, vecCross_AB_AP) <= 0)
		return false;

	if (FVector::DotProduct(vecNormal, vecCross_BC_BP) <= 0)
		return false;

	if (FVector::DotProduct(vecNormal, vecCross_CA_CP) <= 0)
		return false;

	return true;
}

void FHexagonTIleWrapper::GetAroundDoordinatesList(TArray<FVector2D>& outCoordinatesList)
{
	int32 parity = int32(Coordinates.Y) & 1;

	outCoordinatesList =
	{
		AddOffset(Coordinates, AppendCoordinates[parity][0]),
		AddOffset(Coordinates, AppendCoordinates[parity][1]),
		AddOffset(Coordinates, AppendCoordinates[parity][2]),
		AddOffset(Coordinates, AppendCoordinates[parity][3]),
		AddOffset(Coordinates, AppendCoordinates[parity][4]),
		AddOffset(Coordinates, AppendCoordinates[parity][5]),
	};
}

FVector2D FHexagonTIleWrapper::AddOffset(FVector2D a, FVector2D b)
{
	return FVector2D(a.X + b.X, a.Y + b.Y);
}

FVector FHexagonTIleWrapper::GetTopPostion()
{
	if (false == HexagonTile.IsValid())
		return FVector::Zero();

	TArray<FVector> cornerList;
	if (false == GetCornerList(cornerList))
		return FVector::Zero();

	return cornerList[0];
}

FVector2D FHexagonTIleWrapper::GetAxialCoordinates()
{
	return OddToAxial(Coordinates);
}

FVector2D FHexagonTIleWrapper::OddToAxial(FVector2D offset)
{
	int32 col = offset.X;
	int32 row = offset.Y;

	int32 q = row;
	int32 r = col - (row - (row & 1)) / 2;

	return FVector2D(q, r);
}

FVector2D FHexagonTIleWrapper::CubeToAxial(FVector cube)
{
	int32 q = cube.X;
	int32 r = cube.Y;
	return FVector2D(q, r);
}

FVector FHexagonTIleWrapper::AxialToCube(FVector2D hex)
{
	int32 q = hex.X;
	int32 r = hex.Y;
	int32 s = -q - r;

	return FVector(q, r, s);
}

void FHexagonTIleWrapper::Clear()
{

}