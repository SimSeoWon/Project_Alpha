// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include <Widgets/Input/SButton.h>
#include <Templates/SharedPointer.h>
#include "Alpha_HexagonTouchButton.generated.h"

class SAlpha_HexagonTouchButton : public SButton
{
public:
	SAlpha_HexagonTouchButton() {}

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;

	virtual void Press() override;
	virtual void Release() override;

	void ReleaseButtonAndCapture(FReply& InReply);

	FVector2D GetTouchStartPosition() { return TouchStartPosition; }
	FVector2D GetTouchEndPosition() { return TouchEndPosition; }
	FVector2D GetTouchMovePosition() { return TouchMovePosition; }

protected:
	FVector2D DragDist = FVector2D::ZeroVector;

	// 터치 다운 발생 시 마우스 위치.
	FVector2D TouchStartPosition = FVector2D::ZeroVector;
	// 터치 무브 발생 시 마우스 위치.
	FVector2D TouchMovePosition = FVector2D::ZeroVector;
	// 터치 업 발생 시 마우스 위치.
	FVector2D TouchEndPosition = FVector2D::ZeroVector;
};

UCLASS()
class UAlpha_HexagonTouchButton : public UButton
{
	GENERATED_BODY()

public:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	FVector2D GetTouchStartPosition();
	FVector2D GetTouchMovePosition();
	FVector2D GetTouchEndPosition();
};