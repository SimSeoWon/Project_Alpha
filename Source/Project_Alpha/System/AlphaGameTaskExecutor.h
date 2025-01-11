// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AlphaGameTaskExecutor.generated.h"

class FInGameTaskBase;
struct FInGameTaskInfo;

enum class EInGameTaskType : uint8;
/**
 *
 */

#pragma region Sort Method
struct FSortMethod_InGameTaskPriority
{
	bool operator() (TSharedPtr<FInGameTaskBase> A, TSharedPtr<FInGameTaskBase> B) const
	{
		return SortInner(A, B);
	}

	bool SortInner(TSharedPtr<FInGameTaskBase> A, TSharedPtr<FInGameTaskBase> B) const;
};
#pragma endregion

UCLASS()
class PROJECT_ALPHA_API UAlphaGameTaskExecutor : public UObject
{
	GENERATED_BODY()

public:
	virtual void BeginDestroy() override;
	void Init();
	void UnInstall();

	void OnExecute(); // 테스크용 기능..
	void ReleaseAll();
	void OnLoadedWorld(FString inWorldName);
	void OnRegisterFunctionMap();
	int32 GetStep() { return Step; }
	int32 GetTaskCount() { return SequentialTaskList.Num(); }
	TSharedPtr<FInGameTaskBase> GetTaskWrapper(int32 inIndex)
	{
		if (false == SequentialTaskList.IsValidIndex(inIndex))
			return 0;

		return SequentialTaskList[inIndex];
	}
	/**
	- 테이블 로드
	- 태스크 추가, 제거
	- 다음 스탭으로..
	- 인터럽트/ 상태 복구
	*/

	FSortMethod_InGameTaskPriority& GetSortMethod() { return SortMethod; }

	//	UFUNCTION()
//		void OnExecuteCraft();
#pragma region Create Task Function
	bool CreateTask(TSharedRef<FInGameTaskInfo> inTaskInfo);

	UFUNCTION()
	bool OnCreateTask_SPAWN(FInGameTaskInfo inTaskInfo);

	UFUNCTION()
	bool OnCreateTask_SPAWN_MONSTER(FInGameTaskInfo inTaskInfo);

	UFUNCTION()
	bool OnCreateTask_MARCH_ON(FInGameTaskInfo inTaskInfo);
#pragma endregion

private:
	void OnPreLoadMap(const FString& string);
	void OnPostLoadMap(UWorld* world);

protected:
	FSortMethod_InGameTaskPriority SortMethod;
	bool IsLoading = false;
	TArray<TSharedPtr<FInGameTaskBase>> SequentialTaskList;
	TMap<EInGameTaskType, UFunction*> FunctionMap;
	int32 Step = 0;
	TSharedPtr<FInGameTaskBase> OccupiedTask;
};
