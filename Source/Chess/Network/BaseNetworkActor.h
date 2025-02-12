// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IWebSocket.h"
#include "BaseNetworkActor.generated.h"

#define CALLBACK(fun) (CallbackType)(fun)

UCLASS()
class CHESS_API ABaseNetworkActor : public AActor
{
	GENERATED_BODY()

public:
	//Reference to previous definition
	using CallbackType = void(*)(ABaseNetworkActor*, const FJsonObject&);
	
	// Sets default values for this actor's properties
	ABaseNetworkActor();

	//Start connection client to server
	void Connect(const FString& Endpoint);

	//Send messages from client to server
	void SendMessage(const FString& Message) const;

	//virtual void OnSocketConnected();

	//Observe event from server to client
	virtual void ObserveEvent(const int32 EventType, const std::function<void(ABaseNetworkActor*, const FJsonObject&)>& iCallback);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Called to close connection with WebSocket
	virtual void EndPlay(const EEndPlayReason::Type Type) override;

	////Function called when message is received
	void OnMessageReceived(const TSharedPtr<FJsonObject>& Message);

public:

	//Function to set ParticipantID of main player
	void SetParticipantId(const FString& ParticipantId);

	//Function to get ParticipantID of main player
	UFUNCTION(BlueprintCallable)
	FString& GetParticipantId();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	//Map of the events from server and to server
	TMap<int32, std::function<void(ABaseNetworkActor*, FJsonObject)>> mEventMap;

	//WebSocket field
	TSharedPtr<IWebSocket> MSocket;

	//ParticipantID of main player 
	FString MParticipantId;
	
};
