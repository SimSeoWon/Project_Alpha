//Fill out your copyright notice in the Description page of Project Settings.


#include "AlphaInGameTaskBase.h"

#include "Engine/DataTable.h"
#include "Project_Alpha/Tables/tabledata/tabledata_enum.h"
#include "Project_Alpha/AlphaGameInstance.h"
#include "Project_Alpha/Base/Entity.h"
#include "Project_Alpha/Base/AlphaLevelScriptActor.h"
#include "Project_Alpha/Manager/EntityManager.h"
#include "Project_Alpha/Manager/AlphaManager_Dialog.h"
#include "Project_Alpha/Character/AlphaCharacter.h"


FInGameTaskBase::FInGameTaskBase()
{
	REGIST_PEDIGREE(FInGameTaskBase);

}

FInGameMultiTaskBase::FInGameMultiTaskBase()
	: FInGameTaskBase()
{
	REGIST_PEDIGREE(FInGameMultiTaskBase);
}

void FInGameMultiTaskBase::Clear()
{
	FInGameTaskBase::Clear();

	Step = 0;

	if (OccupiedTask.IsValid())
	{
		OccupiedTask = nullptr;
	}

	for (auto iter : SubTaskList)
	{
		if (false == iter.IsValid())
			continue;

		iter->Clear(); //정리한다.
	}
}

void FInGameMultiTaskBase::SetData(FInGameTaskInfo inTaskInfo)
{
	FInGameTaskBase::SetData(inTaskInfo);
	TaskInfo = MakeShared<FInGameTaskInfo>(inTaskInfo);
	// 태스크 스탭, 타이틀, 설명, 조건등의 데이터...
	//Step = TaskInfo->Step;
	TaskType = TaskInfo->TaskType;
	Title = TaskInfo->Title;
	Desc = TaskInfo->Desc;
}

void FInGameMultiTaskBase::AddSubTask(TSharedRef<FInGameTaskBase> inSubTask)
{
	TSharedPtr<FInGameTaskBase> subTask = inSubTask;
	if (false == subTask.IsValid())
		return;

	SubTaskList.Add(inSubTask);
}

ETaskExecutorResult FInGameMultiTaskBase::DoAction()
{
	/**등록된 서브 태스크가 없음*/
	if (SubTaskList.IsEmpty())
		return ETaskExecutorResult::Complete;

	if (false == OccupiedTask.IsValid())
	{
		/**남은 서브 태스크가 없음*/
		if (false == SubTaskList.IsValidIndex(Step))
			return  ETaskExecutorResult::Complete;

		OccupiedTask = SubTaskList[Step];
	}

	ETaskExecutorResult result = OccupiedTask->DoAction();
	if (ETaskExecutorResult::Complete == result)
	{
		++Step;
		OccupiedTask = nullptr;
		result = ETaskExecutorResult::Progress;
	}

	return result;
}

FInGameSubTask_MarchOn::FInGameSubTask_MarchOn()
	:FInGameTaskBase()
{
	REGIST_PEDIGREE(FInGameSubTask_MarchOn);
}

void FInGameSubTask_MarchOn::SetData(FInGameTaskInfo inTaskInfo)
{
	FInGameTaskBase::SetData(inTaskInfo);

	ActorID = 0; // 액터 아이디
	TileIndex = -1;		// 목적지의 타일 인덱스

	TSharedPtr<FInGameTaskInfo> taskInfo = MakeShared<FInGameTaskInfo>(inTaskInfo);
	if (false == taskInfo.IsValid())
		return;

	TileIndex = inTaskInfo.TileIndex;
	if (inTaskInfo.TaskType == EInGameTaskType::MARCH_ON)
	{
		TSharedPtr<FInGameTask_MarchOn> marchOnTask = StaticCastSharedPtr<FInGameTask_MarchOn>(taskInfo);
		if (false == marchOnTask.IsValid())
			return;

		ActorID = marchOnTask->GetActorID();
	}
}

ETaskExecutorResult FInGameSubTask_MarchOn::DoAction()
{
	UEntityManager* manager = UEntityManager::Get();
	if (false == IsValid(manager))
		return ETaskExecutorResult::Fail;

	EntityId entityId(ActorID);
	Entity* entity = manager->FindEntity(entityId);
	if (nullptr == entity)
		return ETaskExecutorResult::Fail;

	//entity 에게 이동을 명령한다.
	//자신의 턴에 Character가 entity에 등록된 정보를 가져가자.
	entity->SetDestination(TileIndex);
	return  ETaskExecutorResult::Complete;
}


FInGameSubTask_Spawn::FInGameSubTask_Spawn()
	:FInGameTaskBase()
{
	REGIST_PEDIGREE(FInGameSubTask_Spawn);
}

void FInGameSubTask_Spawn::SetData(FInGameTaskInfo inTaskInfo)
{
	// 작동할 타일 위치 가져오기
	FInGameTaskBase::SetData(inTaskInfo);

	TSharedPtr<FInGameTaskInfo> taskInfo = MakeShared<FInGameTaskInfo>(inTaskInfo);
	if (false == taskInfo.IsValid())
		return;

	TileIndex = inTaskInfo.TileIndex;
	SpawnType = EInGameSpawnType::NONE;
	ActorID = 0;
	TableID = 0;

	if (inTaskInfo.TaskType == EInGameTaskType::SPAWN_MONSTER)
	{
		TSharedPtr<FInGameTask_SpawnMonster> spawnMonsterTask = StaticCastSharedPtr<FInGameTask_SpawnMonster>(taskInfo);
		if (false == spawnMonsterTask.IsValid())
			return;

		SpawnType = EInGameSpawnType::MONSTER;
		ActorID = spawnMonsterTask->GetActorID();
		TableID = spawnMonsterTask->GetMonsterID();
	}
}

ETaskExecutorResult FInGameSubTask_Spawn::DoAction()
{
	UEntityManager* manager = UEntityManager::Get();
	if (false == IsValid(manager))
		return ETaskExecutorResult::Fail;

	TSharedPtr<Entity> entity = manager->CreateEntity(TileIndex, ActorID, TableID);
	if (false == entity.IsValid())
		return ETaskExecutorResult::Fail;

	//Todo 나중에 TileIndex를 기준으로 화면에 보이는 애들만 스폰시키자.
	manager->CreateActor(entity);


	return ETaskExecutorResult::Complete;
}

FInGameSubTask_SetCamera::FInGameSubTask_SetCamera()
	:FInGameTaskBase()
{
	REGIST_PEDIGREE(FInGameSubTask_SetCamera);
}

void FInGameSubTask_SetCamera::SetData(FInGameTaskInfo inTaskInfo)
{
	FInGameTaskBase::SetData(inTaskInfo);

	TileIndex = inTaskInfo.TileIndex;
}

ETaskExecutorResult FInGameSubTask_SetCamera::DoAction()
{
	//Todo 설정된 좌표에(TileIndex)로 카메라를 움직인다.
	UAlphaGameInstance* gameInstance = UAlphaGameInstance::Get();
	if (false == IsValid(gameInstance))
		return ETaskExecutorResult::Fail;

	UWorld* world = gameInstance->GetWorld();
	if (false == IsValid(world))
		return ETaskExecutorResult::Fail;

	AAlphaLevelScriptActor* levelScriptActor = Cast<AAlphaLevelScriptActor>(world->GetLevelScriptActor());
	if (false == IsValid(levelScriptActor))
		return ETaskExecutorResult::Fail;

	if (false == levelScriptActor->SetCameraActor(TileIndex))
		return ETaskExecutorResult::Fail;

	return ETaskExecutorResult::Complete;
}

FInGameSubTask_PlayDialog::FInGameSubTask_PlayDialog()
	:FInGameTaskBase()
{
	REGIST_PEDIGREE(FInGameSubTask_PlayDialog);
}

void FInGameSubTask_PlayDialog::SetData(FInGameTaskInfo inTaskInfo)
{
	FInGameTaskBase::SetData(inTaskInfo);


}

ETaskExecutorResult FInGameSubTask_PlayDialog::DoAction()
{

	UAlphaManager_Dialog* manager  = UAlphaManager_Dialog::Get();
	if (false == IsValid(manager))
		return ETaskExecutorResult::Fail;

	if(false == manager->PlayDialog(1))
		return ETaskExecutorResult::Fail;
	
	//Todo 임시로 즉시 종료시키기! 추후 다이얼로그 종료까지 ETaskExecutorResult::Progress; 를 리턴해야한다.
	return ETaskExecutorResult::Complete;
}