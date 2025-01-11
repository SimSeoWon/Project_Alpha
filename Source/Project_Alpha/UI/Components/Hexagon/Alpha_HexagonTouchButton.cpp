// Fill out your copyright notice in the Description page of Project Settings.
#include "Alpha_HexagonTouchButton.h"
#include <Components/ButtonSlot.h>

#include "../../../System/WNDLog.h"

FReply SAlpha_HexagonTouchButton::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	//TouchStartPosition = MouseEvent.GetScreenSpacePosition();
	TouchStartPosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
	SButton::OnMouseButtonDown(MyGeometry, MouseEvent);
	FReply Reply = FReply::Unhandled();
	return Reply;
}

FReply SAlpha_HexagonTouchButton::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	TouchEndPosition = MouseEvent.GetScreenSpacePosition();
	return SButton::OnMouseButtonUp(MyGeometry, MouseEvent);
}

FReply SAlpha_HexagonTouchButton::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FReply Reply = FReply::Unhandled();
	return Reply;
	// -----------------------------------------------------------------------------------------------------------------------------------------------
}

void SAlpha_HexagonTouchButton::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	SButton::OnMouseLeave(MouseEvent);
	const bool bWasHovered = IsHovered();

	// Call parent implementation
	SWidget::OnMouseLeave(MouseEvent);
	if (bWasHovered)
	{
	}

	Invalidate(EInvalidateWidget::Layout);
}

void SAlpha_HexagonTouchButton::Press()
{
	SButton::Press();
}

void SAlpha_HexagonTouchButton::Release()
{
	SButton::Release();
}

void SAlpha_HexagonTouchButton::ReleaseButtonAndCapture(FReply& InReply)
{
	// 마우스 캡쳐 Release
	if (InReply.GetMouseCaptor().IsValid() == false && HasMouseCapture())
	{
		InReply.ReleaseMouseCapture();
	}

	// 버튼 Release
	Release();
}

TSharedRef<SWidget> UAlpha_HexagonTouchButton::RebuildWidget()
{
	TSharedPtr<SAlpha_HexagonTouchButton> NewWNDTouchButton = SNew(SAlpha_HexagonTouchButton)
		.OnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, SlateHandleClicked))
		.OnPressed(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandlePressed))
		.OnReleased(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandleReleased))
		.OnHovered_UObject(this, &ThisClass::SlateHandleHovered)
		.OnUnhovered_UObject(this, &ThisClass::SlateHandleUnhovered)
		.ButtonStyle(&WidgetStyle)
		.ClickMethod(ClickMethod)
		.TouchMethod(TouchMethod)
		.PressMethod(PressMethod)
		.IsFocusable(IsFocusable)
		;

	MyButton = NewWNDTouchButton;

	if (GetChildrenCount() > 0)
	{
		Cast<UButtonSlot>(GetContentSlot())->BuildSlot(MyButton.ToSharedRef());
	}

	return MyButton.ToSharedRef();
}

FVector2D UAlpha_HexagonTouchButton::GetTouchStartPosition()
{
	SAlpha_HexagonTouchButton* myTouchButton = static_cast<SAlpha_HexagonTouchButton*>(MyButton.Get());
	if (myTouchButton == nullptr)
		return FVector2D::ZeroVector;

	return myTouchButton->GetTouchStartPosition();
}

FVector2D UAlpha_HexagonTouchButton::GetTouchMovePosition()
{
	SAlpha_HexagonTouchButton* myTouchButton = static_cast<SAlpha_HexagonTouchButton*>(MyButton.Get());
	if (myTouchButton == nullptr)
		return FVector2D::ZeroVector;

	return myTouchButton->GetTouchMovePosition();
}

FVector2D UAlpha_HexagonTouchButton::GetTouchEndPosition()
{
	SAlpha_HexagonTouchButton* myTouchButton = static_cast<SAlpha_HexagonTouchButton*>(MyButton.Get());
	if (myTouchButton == nullptr)
		return FVector2D::ZeroVector;

	return myTouchButton->GetTouchEndPosition();
}