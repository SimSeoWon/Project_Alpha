// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AlphaActor_Marker.generated.h"

class USceneComponent;
class UWidgetComponent;
class USplineComponent;
class UNiagaraComponent;
class UAlphaUIWidget_CommandList;

UCLASS()
class PROJECT_ALPHA_API AAlphaActor_Marker : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAlphaActor_Marker();

	void SetLocation(FVector inLocation);
	void SetSplinePoints(int32 inEntityID, TArray<int32> inArrPath, TArray<FVector> newPoints);
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	void OnMoveCommand(UAlphaUIWidget_CommandList* inUIWidget = nullptr);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* Widget = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USplineComponent* Spline = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* Niagara = nullptr;


	int32 EntityID = 0;
	TArray<int32> ArrPath;

};
