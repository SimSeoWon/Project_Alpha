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

		iter->Clear(); //�����Ѵ�.
	}
}

void FInGameMultiTaskBase::SetData(FInGameTaskInfo inTaskInfo)
{
	FInGameTaskBase::SetData(inTaskInfo);
	TaskInfo = MakeShared<FInGameTaskInfo>(inTaskInfo);
	// �½�ũ ����, Ÿ��Ʋ, ����, ���ǵ��� ������...
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
	/**��ϵ� ���� �½�ũ�� ����*/
	if (SubTaskList.IsEmpty())
		return ETaskExecutorResult::Complete;

	if (false == OccupiedTask.IsValid())
	{
		/**���� ���� �½�ũ�� ����*/
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

	ActorID = 0; // ���� ���̵�
	TileIndex = -1;		// �������� Ÿ�� �ε���

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

	//entity ���� �̵��� ����Ѵ�.
	//�ڽ��� �Ͽ� Character�� entity�� ��ϵ� ������ ��������.
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
	// �۵��� Ÿ�� ��ġ ��������
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

	//Todo ���߿� TileIndex�� �������� ȭ�鿡 ���̴� �ֵ鸸 ������Ű��.
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
	//Todo ������ ��ǥ��(TileIndex)�� ī�޶� �����δ�.
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
	
	//Todo �ӽ÷� ��� �����Ű��! ���� ���̾�α� ������� ETaskExecutorResult::Progress; �� �����ؾ��Ѵ�.
	return ETaskExecutorResult::Complete;
}