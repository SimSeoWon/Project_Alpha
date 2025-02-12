// Fill out your copyright notice in the Description page of Project Settings.


#include "AlphaActor_Marker.h"
#include "Components/WidgetComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SceneComponent.h"
#include "NiagaraComponent.h"
#include "Project_Alpha/UI/Widgets/AlphaUIWidget_CommandList.h"

// Sets default values
AAlphaActor_Marker::AAlphaActor_Marker()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	Widget->SetupAttachment(Root);
	Widget->SetHiddenInGame(true);

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	Spline->SetupAttachment(Root);
	Spline->SetHiddenInGame(true);

	Niagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	Niagara->SetupAttachment(Spline);
	Niagara->SetHiddenInGame(true);
	RootComponent = Root;
}

// Called when the game starts or when spawned
void AAlphaActor_Marker::BeginPlay()
{
	Super::BeginPlay();

	UUserWidget* userWidget = Widget->GetWidget();
	if (false == IsValid(userWidget))
		return;

	UAlphaUIWidget_CommandList* commandList = Cast<UAlphaUIWidget_CommandList>(userWidget);
	if (false == IsValid(commandList))
		return;

	commandList->GetOnClicked_MoveCommand().Unbind();
	commandList->GetOnClicked_MoveCommand().BindUObject(this, &AAlphaActor_Marker::OnMoveCommand);

}

void AAlphaActor_Marker::OnMoveCommand(UAlphaUIWidget_CommandList* inUIWidget/**= nullptr*/)
{
	if (false == IsValid(inUIWidget))
		return;

	Widget->SetHiddenInGame(true);
	Spline->ClearSplinePoints();
	Spline->SetHiddenInGame(true);
	Niagara->SetHiddenInGame(true);
	Niagara->Deactivate();
}

// Called every frame
void AAlphaActor_Marker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAlphaActor_Marker::SetLocation(FVector inLocation)
{
	SetActorLocation(inLocation);
	Widget->SetHiddenInGame(true);
	Spline->ClearSplinePoints();
	Spline->SetHiddenInGame(true);
	Niagara->SetHiddenInGame(true);
	Niagara->Deactivate();
}

void AAlphaActor_Marker::SetSplinePoints(int32 inEntityID, TArray<int32> inArrPath, TArray<FVector> newPoints)
{
	Spline->SetSplinePoints(newPoints, ESplineCoordinateSpace::World);
	Spline->SetHiddenInGame(false);
	Widget->SetHiddenInGame(false);
	Niagara->SetHiddenInGame(false);
	Niagara->Activate();

	UUserWidget* userWidget = Widget->GetWidget();
	if (false == IsValid(userWidget))
		return;

	UAlphaUIWidget_CommandList* commandList = Cast<UAlphaUIWidget_CommandList>(userWidget);
	if (false == IsValid(commandList))
		return;

	EntityID = inEntityID;
	ArrPath.Reset();
	ArrPath.Reserve(inArrPath.Num());
	ArrPath.Append(inArrPath);

	commandList->SetCommandData(inEntityID, inArrPath);

}