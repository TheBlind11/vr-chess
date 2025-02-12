// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseNetworkActor.h"
#include "ChessNetworkActor.generated.h"

//Here i'm defining how many params (and which kind of params) do we need for every event handler
DECLARE_DELEGATE_TwoParams(FOnConnectedEvent, const FString&, const TSharedPtr<FJsonObject>&);
DECLARE_DELEGATE_ThreeParams(FOnPlayerSeatedEvent, const FString&, const FString&, const int);
DECLARE_DELEGATE_OneParam(FOnCountdownGameEvent, const int);
DECLARE_DELEGATE_FourParams(FOnPlayerTurnEvent, const FString&, const FString&, const bool, const TArray<TSharedPtr<FJsonValue>>&);
DECLARE_DELEGATE_NineParams(FOnPlayerHasMovedEvent, const FString&, const FString&, const int, const int, const int, const int, const FString&, const TArray<TSharedPtr<FJsonValue>>&, const FString&);
DECLARE_DELEGATE_NineParams(FOnGameFinishedEvent, const FString&, const FString&, const FString&, const FString&, const bool, const int, const int, const FString&, const int);
DECLARE_DELEGATE(FOnPlayerHasOfferedDrawEvent);
DECLARE_DELEGATE(FOnPlayerHasOfferedRematchEvent);

UCLASS()
class CHESS_API AChessNetworkActor final: public ABaseNetworkActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AChessNetworkActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Initialization of ChessNetworkActor that is unique
	void Initialize();

	//Delegates (or event handler)
	FOnConnectedEvent OnConnectedEventDelegate;
	FOnPlayerSeatedEvent OnPlayerSeatedEventDelegate;
	FOnCountdownGameEvent OnCountdownGameEventDelegate;
	FOnPlayerTurnEvent OnPlayerTurnEventDelegate;
	FOnPlayerHasMovedEvent OnPlayerHasMovedEventDelegate;
	FOnGameFinishedEvent OnGameFinishedEventDelegate;
	FOnPlayerHasOfferedDrawEvent OnPlayerHasOfferedDrawEventDelegate;
	FOnPlayerHasOfferedRematchEvent OnPlayerHasOfferedRematchEventDelegate;

	//From server (on every event we need to get fields of the json object sent by server)
	static void OnConnectedEvent(AChessNetworkActor* iCaller, const FJsonObject& iJsonObject);
	static void OnPlayerSeatedEvent(AChessNetworkActor* iCaller, const FJsonObject& iJsonObject);
	static void OnCountdownGameEvent(AChessNetworkActor* iCaller, const FJsonObject& iJsonObject);
	static void OnPlayerTurnEvent(AChessNetworkActor* iCaller, const FJsonObject& iJsonObject);
	static void OnPlayerHasMovedEvent(AChessNetworkActor* iCaller, const FJsonObject& iJsonObject);
	static void OnGameFinishedEvent(AChessNetworkActor* iCaller, const FJsonObject& iJsonObject);
	static void OnPlayerHasOfferedDrawEvent(AChessNetworkActor* iCaller, const FJsonObject& iJsonObject);
	static void OnPlayerHasOfferedRematchEvent(AChessNetworkActor* iCaller, const FJsonObject& iJsonObject);

	//To server (these are functions that are called in order to send particular messages to server)
	UFUNCTION(BlueprintCallable, Category="network")
	void SendPlayerWantToPlay(UPARAM(ref) FString& Nickname);

	UFUNCTION(BlueprintCallable, Category="network")
	void SendMove(UPARAM(ref) FString& TurnId, int Source, int Endpoint, UPARAM(ref) FString PromoteTo);

	UFUNCTION(BlueprintCallable, Category="network")
	void OfferDraw();

	UFUNCTION(BlueprintCallable, Category="network")
	void AcceptDraw();

	UFUNCTION(BlueprintCallable, Category="network")
	void ClaimDraw(UPARAM(ref) FString& Reason);

	UFUNCTION(BlueprintCallable, Category="network")
	void Resign();

	UFUNCTION(BlueprintCallable, Category="network")
	void OfferRematch();

	UFUNCTION(BlueprintCallable, Category="network")
	void AcceptRematch();
	
};
