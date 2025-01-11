//Fill out your copyright notice in the Description page of Project Settings.


#include "AlphaGameTaskExecutor.h"
#include "AlphaInGameTaskBase.h"
#include "Engine/DataTable.h"
#include "Project_Alpha/Tables/tabledata/tabledata_enum.h"
#include "Project_Alpha/Tables/tabledata/tabledata_resource.h"
#include "Project_Alpha/UI/UIViews/HUD/WNDUIHUD_InGameMain.h"
#include "Project_Alpha/UI/Widgets/InGameTask/WNDUIWidget_InGameTaskViewer.h"
#include "Project_Alpha/Manager/WNDUIViewManager.h"



#pragma region SortMethod_Summon
bool FSortMethod_InGameTaskPriority::SortInner(TSharedPtr<FInGameTaskBase> A, TSharedPtr<FInGameTaskBase> B) const
{
	if (A->Priority == B->Priority)
	{
		/**등록시간 비교*/
		return A->RegistrationTime > B->RegistrationTime;
	}

	return A->Priority > B->Priority;
}
#pragma endregion

void UAlphaGameTaskExecutor::BeginDestroy()
{
	Super::BeginDestroy();
}

void UAlphaGameTaskExecutor::Init()
{
	if (!FCoreUObjectDelegates::PreLoadMap.IsBoundToObject(this))
	{
		FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UAlphaGameTaskExecutor::OnPreLoadMap);
	}

	if (!FCoreUObjectDelegates::PostLoadMapWithWorld.IsBoundToObject(this))
	{
		FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UAlphaGameTaskExecutor::OnPostLoadMap);
	}

	OnRegisterFunctionMap();
}

void UAlphaGameTaskExecutor::OnRegisterFunctionMap()
{
	FunctionMap.Reset();
	const UEnum* ptrEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EInGameTaskType"), true);
	if (nullptr == ptrEnum)
		return;

	int32 count = ptrEnum->NumEnums();
	for (int32 i = 0; i < count; i++)
	{
		int64 value = ptrEnum->GetValueByIndex(i);
		FString enum_to_name = ptrEnum->GetNameStringByValue(value);
		FString func_name = TEXT("OnCreateTask_") + enum_to_name;
		if (UFunction* func = GetClass()->FindFunctionByName(FName(*func_name)))
		{
			FunctionMap.Emplace(static_cast<EInGameTaskType>(value), func);
		}
	}
}

void UAlphaGameTaskExecutor::UnInstall()
{
	FCoreUObjectDelegates::PreLoadMap.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
}

void UAlphaGameTaskExecutor::OnPreLoadMap(const FString& string)
{
	IsLoading = true;
}

void UAlphaGameTaskExecutor::OnPostLoadMap(UWorld* world)
{
	IsLoading = false;
}

void UAlphaGameTaskExecutor::OnExecute()
{
	if (SequentialTaskList.IsEmpty())
		return;

	if (false == OccupiedTask.IsValid())
	{
		if (false == SequentialTaskList.IsValidIndex(Step))
			return;

		OccupiedTask = SequentialTaskList[Step];
	}

	// 작업 완료됨.
	if (ETaskExecutorResult::Complete == OccupiedTask->DoAction())
	{
		Step++;
		OccupiedTask = nullptr;
	}

	UWNDUIHUD_InGameMain* uiHud = UWNDUIViewManager::Get()->GetActiveView<UWNDUIHUD_InGameMain>();
	if (IsValid(uiHud))
	{
		UWNDUIWidget_InGameTaskViewer* viewer = uiHud->InGameTaskViewer;
		if (IsValid(viewer))
		{
			viewer->OnRefreshUI();
		}
	}
}

/**기존 세팅된 태스크를 모두 해제한다.*/
void UAlphaGameTaskExecutor::ReleaseAll()
{
	Step = 0;

	if (OccupiedTask.IsValid())
	{
		OccupiedTask = nullptr;
	}

	for (auto iter : SequentialTaskList)
	{
		if (false == iter.IsValid())
			continue;

		iter->Clear(); //정리한다.
	}

	SequentialTaskList.Reset();
}

/**새로운 월드가 로딩되었다. 레벨의 태스크 목록을 로딩한다*/
void UAlphaGameTaskExecutor::OnLoadedWorld(FString inWorldName)
{
	ReleaseAll();

	if (inWorldName.IsEmpty())
		return;

	FString strFileName = FString::Printf(TEXT("res_task%s"), *inWorldName);
	FString strFilePath = FString::Printf(TEXT("DataTable'/Game/GameData/resourcetable/%s.%s'"), *strFileName, *strFileName);
	UDataTable* dtInGameTask = LoadObject<UDataTable>(this, *strFilePath);

	if (nullptr == dtInGameTask)
		return;

	FRES_LEVELTASK* rowData = dtInGameTask->FindRow<FRES_LEVELTASK>(FName(TEXT("TEST")), TEXT(""));
	if (nullptr == rowData)
		return;

	SequentialTaskList.Reserve(rowData->TaskList.Num());
	for (int32 i = 0; i < rowData->TaskList.Num(); ++i)
	{
		//FInGameTaskInfo info = FInGameTaskInfo(iter);
		TSharedPtr<FInGameTaskInfo> taskData = MakeShared<FInGameTaskInfo>(rowData->TaskList[i]);
		
		if (false == taskData.IsValid())
			continue;

		if (false == CreateTask(taskData.ToSharedRef()))
			continue;
	}

	UWNDUIHUD_InGameMain* uiHud = UWNDUIViewManager::Get()->GetActiveView<UWNDUIHUD_InGameMain>();
	if (IsValid(uiHud))
	{
		UWNDUIWidget_InGameTaskViewer* viewer = uiHud->InGameTaskViewer;
		if (IsValid(viewer))
		{
			viewer->OnRefreshUI();
		}
	}
}

#pragma region Create Task Function
bool UAlphaGameTaskExecutor::CreateTask(TSharedRef<FInGameTaskInfo> inTaskInfo)
{
	TSharedPtr<FInGameTaskInfo> ptrTaskInfo = inTaskInfo;
	if (false == ptrTaskInfo.IsValid())
		return false;

	if (false == FunctionMap.Contains(ptrTaskInfo->TaskType))
		return false;

	UFunction* func = FunctionMap[ptrTaskInfo->TaskType];
	if (false == IsValid(func))
		return false;

	// Setup parameters to the Func as a struct
	struct
	{
		FInGameTaskInfo taskInfo;
	} FuncParams;

	FuncParams.taskInfo = FInGameTaskInfo(*ptrTaskInfo.Get());

	this->ProcessEvent(func, &FuncParams);
	return true;
}

bool UAlphaGameTaskExecutor::OnCreateTask_SPAWN(FInGameTaskInfo inTaskInfo)
{
	TSharedPtr<FInGameMultiTaskBase> mainTask = MakeShared<FInGameMultiTaskBase>();
	if (false == mainTask.IsValid())
		return false;

	mainTask->SetData(inTaskInfo);

	// 액터 스폰 태스크
	TSharedPtr<FInGameSubTask_Spawn> subTask_Spown = MakeShared<FInGameSubTask_Spawn>();
	if (subTask_Spown.IsValid())
	{
		subTask_Spown->SetData(inTaskInfo);
		mainTask->AddSubTask(subTask_Spown.ToSharedRef());
	}

	// 카메라 세팅 태스크
	TSharedPtr<FInGameSubTask_SetCamera> subTask_SetCamera = MakeShared<FInGameSubTask_SetCamera>();
	if (subTask_SetCamera.IsValid())
	{
		subTask_SetCamera->SetData(inTaskInfo);
		mainTask->AddSubTask(subTask_SetCamera.ToSharedRef());
	}

	SequentialTaskList.Add(mainTask);
	return true;
}

bool UAlphaGameTaskExecutor::OnCreateTask_MARCH_ON(FInGameTaskInfo inTaskInfo)
{
	TSharedPtr<FInGameMultiTaskBase> mainTask = MakeShared<FInGameMultiTaskBase>();
	if (false == mainTask.IsValid())
		return false;

	mainTask->SetData(inTaskInfo);
	// 스폰된 액터를 찾아서 이동을 명령하자.
	TSharedPtr<FInGameSubTask_MarchOn> subTask_MarchOn = MakeShared<FInGameSubTask_MarchOn>();
	if (subTask_MarchOn.IsValid())
	{
		subTask_MarchOn->SetData(inTaskInfo);
		mainTask->AddSubTask(subTask_MarchOn.ToSharedRef());
	}

	// 카메라 세팅 태스크... 해당 액터를 팔로우하면 좋을꺼같은데... 나중에!
	TSharedPtr<FInGameSubTask_SetCamera> subTask_SetCamera = MakeShared<FInGameSubTask_SetCamera>();
	if (subTask_SetCamera.IsValid())
	{
		subTask_SetCamera->SetData(inTaskInfo);
		mainTask->AddSubTask(subTask_SetCamera.ToSharedRef());
	}

	// 다이얼로그 테스트용 태스크... 
	TSharedPtr<FInGameSubTask_PlayDialog> subTask_PlayDialog = MakeShared<FInGameSubTask_PlayDialog>();
	if (subTask_PlayDialog.IsValid())
	{
		subTask_PlayDialog->SetData(inTaskInfo);
		mainTask->AddSubTask(subTask_PlayDialog.ToSharedRef());
	}

	SequentialTaskList.Add(mainTask);

	return false;

}

bool UAlphaGameTaskExecutor::OnCreateTask_SPAWN_MONSTER(FInGameTaskInfo inTaskInfo) 
{
	TSharedPtr<FInGameMultiTaskBase> mainTask = MakeShared<FInGameMultiTaskBase>();
	if (false == mainTask.IsValid())
		return false;

	mainTask->SetData(inTaskInfo);
	// 액터 스폰 태스크
	TSharedPtr<FInGameSubTask_Spawn> subTask_Spown = MakeShared<FInGameSubTask_Spawn>();
	if (subTask_Spown.IsValid())
	{
		subTask_Spown->SetData(inTaskInfo);
		mainTask->AddSubTask(subTask_Spown.ToSharedRef());
	}

	// 카메라 세팅 태스크
	TSharedPtr<FInGameSubTask_SetCamera> subTask_SetCamera = MakeShared<FInGameSubTask_SetCamera>();
	if (subTask_SetCamera.IsValid())
	{
		subTask_SetCamera->SetData(inTaskInfo);
		mainTask->AddSubTask(subTask_SetCamera.ToSharedRef());
	}

	SequentialTaskList.Add(mainTask);


	return false;
}
#pragma endregion
