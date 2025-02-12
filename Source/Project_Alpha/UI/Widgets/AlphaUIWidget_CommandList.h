// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WNDUIWidgetBase.h"
#include "AlphaUIWidget_CommandList.generated.h"

class UButton;
/**
 *
 */
UCLASS()
class PROJECT_ALPHA_API UAlphaUIWidget_CommandList : public UWNDUIWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void Construct() override;
	virtual void Destruct() override;

	UFUNCTION()
	void OnClickBtn_MoveCommand();

public:
	void SetCommandData(int32 inEntityID, TArray<int32> inArrPath);

	DECLARE_DELEGATE_OneParam(FOnClickedDelegate, UAlphaUIWidget_CommandList*);
	FOnClickedDelegate& GetOnClicked_MoveCommand() { return OnClicked_MoveCommand; }

protected:

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_MoveCommand = nullptr;

	FOnClickedDelegate OnClicked_MoveCommand;
	int32 EntityID;
	TArray<int32> ArrPath;
};
