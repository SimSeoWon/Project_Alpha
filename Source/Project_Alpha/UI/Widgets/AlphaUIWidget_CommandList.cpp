// Fill out your copyright notice in the Description page of Project Settings.


#include "AlphaUIWidget_CommandList.h"
#include "Components/Button.h"
#include "Project_Alpha/Manager/EntityManager.h"
#include "Project_Alpha/Character/AlphaCharacter.h"

void UAlphaUIWidget_CommandList::Construct()
{
	Super::Construct();
	UI_BIND_BTN_CLICK_EVENT(Btn_MoveCommand, &UAlphaUIWidget_CommandList::OnClickBtn_MoveCommand);
}

void UAlphaUIWidget_CommandList::Destruct() 
{
	Super::Destruct();
}

void UAlphaUIWidget_CommandList::OnClickBtn_MoveCommand()
{
	if (OnClicked_MoveCommand.IsBound())
		OnClicked_MoveCommand.Execute(this);

	UEntityManager* entityManager = UEntityManager::Get();
	if (false == IsValid(entityManager))
		return;

	Entity* entity = entityManager->FindEntity(EntityId(EntityID));
	if (nullptr == entity)
		return;

	AAlphaCharacter* character = entity->GetActor<AAlphaCharacter>();
	if(false == IsValid(character))
		return;

	if(false == character->GetIsSelect())
		return;

	character->AddMoveAction(ArrPath);
	character->SetIsSelect(false);
}

void UAlphaUIWidget_CommandList::SetCommandData(int32 inEntityID, TArray<int32> inArrPath) 
{
	EntityID = inEntityID;
	ArrPath.Reset(inArrPath.Num());
	ArrPath.Append(inArrPath);
}