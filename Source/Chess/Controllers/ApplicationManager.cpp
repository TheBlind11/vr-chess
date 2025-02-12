// Fill out your copyright notice in the Description page of Project Settings.


#include "ApplicationManager.h"

#include "SceneController.h"
#include "Network/ChessNetworkActor.h"
#include "Network/Conf.h"
#include "Utility/Logger.h"


// Sets default values
AApplicationManager::AApplicationManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//We don't need anything called by every frame
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AApplicationManager::BeginPlay()
{
	Super::BeginPlay();

	//Log important to check every step
	UE_LOG(LogTemp, Display, TEXT("ApplicationManager::BeginPlay"));
	FLogger::Log(TEXT("ApplicationManager::BeginPlay"));

	UWorld* World = GetWorld();  //Get the world of the scene

	if(World)  //Check if World is null or not
	{
		//Spawn ChessNetworkActor and SceneController in the world
		MChess = static_cast<AChessNetworkActor*>(World->SpawnActor(AChessNetworkActor::StaticClass()));
		SceneController = static_cast<ASceneController*>(World->SpawnActor(ASceneController::StaticClass()));
		MChess->Tags.Add("NetworkActor");  //This tag is useful to get the object in the world
		SceneController->Tags.Add("SceneController");

		if(MChess)  //Check if MChess is null or not
		{
			UE_LOG(LogTemp, Display, TEXT("ApplicationManager::BeginPlay Initializing chess server"));
			FLogger::Log(TEXT("ApplicationManager::BeginPlay Initializing chess server"));
			MChess->Initialize(); //Initialization
			UE_LOG(LogTemp, Display, TEXT("ApplicationManager::BeginPlay connecting to %s"), *Conf::CHESS_HOST);
			FLogger::Log(FString::Printf(TEXT("ApplicationManager::BeginPlay connecting to %s"), *Conf::CHESS_HOST));
			MChess->Connect(TEXT("" + Conf::CHESS_HOST));  //Start connection
		}

		if(SceneController)  //Check if SceneController is not null
			SceneController->Init();  //Initialization

		//Binding of all Delegates (every event will be handled by a function)
		MChess->OnConnectedEventDelegate.BindUObject(SceneController, &ASceneController::ChessConnected);
		MChess->OnPlayerSeatedEventDelegate.BindUObject(SceneController, &ASceneController::PlayerSeated);
		MChess->OnCountdownGameEventDelegate.BindUObject(SceneController, &ASceneController::ShowCountdown);
		MChess->OnPlayerTurnEventDelegate.BindUObject(SceneController, &ASceneController::PlayerTurn);
		MChess->OnPlayerHasMovedEventDelegate.BindUObject(SceneController, &ASceneController::PlayerHasMoved);
		MChess->OnGameFinishedEventDelegate.BindUObject(SceneController, &ASceneController::ShowGameFinishedWidget);
		MChess->OnPlayerHasOfferedDrawEventDelegate.BindUObject(SceneController, &ASceneController::ShowCanAcceptDrawUI);
		MChess->OnPlayerHasOfferedRematchEventDelegate.BindUObject(SceneController, &ASceneController::ShowCanAcceptRematchUI);
	}
}

// Called every frame
void AApplicationManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

