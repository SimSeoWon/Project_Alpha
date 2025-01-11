// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Project_Alpha/Tables/tabledata/tabledata_resource.h"

#define REGIST_PEDIGREE(inClass) \
	{ \
		Name = #inClass; \
		Pedigree.Add(#inClass); \
	}

#define TRYCAST_TASK(inClass, inTask, outResult) \
	{ \
		if(inTask.IsValid()) \
		{\
			if(inTask->Pedigree.Contains(#inClass)) \
			{	\
				outResult = StaticCastSharedPtr<inClass>(inTask); \
			}	\
		}\
	}

enum class EInGameSpawnType : uint8;

enum class ETaskExecutorResult : uint8
{
	NONE,
	Complete,
	Fail,
	Progress,
	Max,
};

/**�ΰ��� ������ �����ϴ� �½�ũ ����*/
class FInGameTaskBase
{
public:
	FInGameTaskBase();
	virtual ~FInGameTaskBase() {}
	virtual void Clear() {}
	virtual void SetData(FInGameTaskInfo inTaskInfo)  {}
	virtual ETaskExecutorResult DoAction() = 0;

public:
	int32 Priority = 0;
	uint64 RegistrationTime = 0;

	FString Name;
	TArray<FString> Pedigree;
};

class FInGameMultiTaskBase : public FInGameTaskBase
{
public:
	FInGameMultiTaskBase();
	~FInGameMultiTaskBase() {}
	virtual void Clear() override;
	virtual void SetData(FInGameTaskInfo inTaskInfo) override;
	virtual ETaskExecutorResult DoAction() override;

	void AddSubTask(TSharedRef<FInGameTaskBase> inSubTask);
	int32 GetStep() { return Step; }
	int32 GetTaskCount() { return SubTaskList.Num(); }
	TSharedPtr<FInGameTaskBase> GetTaskWrapper(int32 inIndex)
	{
		if (false == SubTaskList.IsValidIndex(inIndex))
			return nullptr;

		return SubTaskList[inIndex];
	}

protected:
	TArray<TSharedPtr<FInGameTaskBase>> SubTaskList;
	TSharedPtr<FInGameTaskBase> OccupiedTask;

	TSharedPtr<FInGameTaskInfo> TaskInfo;
	int32 Step = 0; //���൵
	EInGameTaskType TaskType = EInGameTaskType::NONE; // �½�ũ Ÿ��.
	FString Title = TEXT(""); // Ÿ��Ʋ
	FString Desc = TEXT(""); // ����
};

class FInGameSubTask_Spawn : public FInGameTaskBase
{
public:
	FInGameSubTask_Spawn();
	~FInGameSubTask_Spawn() {}

	virtual void Clear() {}
	virtual void SetData(FInGameTaskInfo inTaskInfo) override;
	virtual ETaskExecutorResult DoAction() override;

public:
	uint64 ActorSN = 0; // �ø��� �ѹ�..
	int32 ActorID = 0; // ���� ���̵�
	int32 TableID = 0; // ���̺� ���̵�.
	// ���̺� ���� -> ����, ��� �⹰, �÷��̾� ��..
	EInGameSpawnType SpawnType;
	int32 TileIndex = -1;		// �۵��� Ÿ���� �ε���
};

class FInGameSubTask_MarchOn : public FInGameTaskBase
{
public:
	FInGameSubTask_MarchOn();
	~FInGameSubTask_MarchOn() {}

	virtual void Clear() {}
	virtual void SetData(FInGameTaskInfo inTaskInfo) override;
	virtual ETaskExecutorResult DoAction() override;

public:
	int32 ActorID = 0; // ���� ���̵�
	int32 TileIndex = -1;		// �������� Ÿ�� �ε���
};

class FInGameSubTask_SetCamera : public FInGameTaskBase
{
public:
	FInGameSubTask_SetCamera();
	~FInGameSubTask_SetCamera() {}

	virtual void Clear() {}
	virtual void SetData(FInGameTaskInfo inTaskInfo);
	virtual ETaskExecutorResult DoAction();

public:
	int32 TileIndex = -1;		// �۵��� Ÿ���� �ε���
};


class FInGameSubTask_PlayDialog : public FInGameTaskBase
{
public:
	FInGameSubTask_PlayDialog();
	~FInGameSubTask_PlayDialog() {}

	virtual void Clear() {}
	virtual void SetData(FInGameTaskInfo inTaskInfo);
	virtual ETaskExecutorResult DoAction();

public:
	int32 TileIndex = -1;		// �۵��� Ÿ���� �ε���
	int32 DialogID = 0;
};

