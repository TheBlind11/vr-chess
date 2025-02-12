// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Network/ChessNetworkActor.h"
#include "SceneController.h"
#include "GameFramework/Actor.h"
#include "ApplicationManager.generated.h"

UCLASS()
class CHESS_API AApplicationManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AApplicationManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	//ChessNetworkActor field
	UPROPERTY()
	class AChessNetworkActor* MChess = nullptr;

	//SceneController field
	UPROPERTY()
	class ASceneController* SceneController = nullptr;
	
};
