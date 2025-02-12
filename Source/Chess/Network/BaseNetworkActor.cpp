// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseNetworkActor.h"

#include "WebSocketsModule.h"
#include "Modules/ModuleManager.h"
#include "Utility/Logger.h"


// Sets default values
ABaseNetworkActor::ABaseNetworkActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//We don't need anything called by every frame
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ABaseNetworkActor::BeginPlay()
{
	Super::BeginPlay();
	
}

//Called when the game ends 
void ABaseNetworkActor::EndPlay(const EEndPlayReason::Type Type)
{
	UE_LOG(LogTemp, Display, TEXT("ABaseNetworkActor::EndPlay"));  //Log to check every step

	if (MSocket.IsValid())  //Check if WebSocket is not null
	{
		MSocket->Close();  //Close the connection
	}

	Super::EndPlay(Type);
}

//Function useful to connect to WebSocket
void ABaseNetworkActor::Connect(const FString& Endpoint)
{
	UE_LOG(LogTemp, Display, TEXT("ABaseNetworkActor::Connect to %s"), *Endpoint);  //Log to check every step
	FLogger::Log(FString::Printf(TEXT("ABaseNetworkActor::Connect to %s"), *Endpoint));
	
	const FString& WebsocketEndpoint = Endpoint;  //Set up the end point of the connection
	const FString& Protocol = TEXT("json");  //Set up the protocol of the connection
	FWebSocketsModule* WebSocketModule = nullptr;

	if(!FModuleManager::Get().IsModuleLoaded("WebSockets"))
	{
		FModuleManager::Get().LoadModule("WebSockets");
		WebSocketModule = &FWebSocketsModule::Get();
	}

	MSocket = WebSocketModule->CreateWebSocket(WebsocketEndpoint, Protocol);  //Create WebSocket

	//When connected this is called
	MSocket->OnConnected().AddLambda(
		[=]() -> void
		{
			UE_LOG(LogTemp, Display, TEXT("ABaseNetworkActor::BeginPlay::Connected"));  //Log to check every step
			FLogger::Log(TEXT("ABaseNetworkActor::BeginPlay::Connected"));
			//OnSocketConnected();
		}
	);

	//Called when a web socket text message has been received
	MSocket->OnMessage().AddLambda(
		[=](const FString& iMessage) -> void
		{
			UE_LOG(LogTemp, Display, TEXT("ABaseNetworkActor::BeginPlay::OnMessage message = %s"), *iMessage);  //Log to check every step
			FLogger::Log(FString::Printf(TEXT("ABaseNetworkActor::BeginPlay::OnMessage message = %s"), *iMessage));
			const TSharedRef<TJsonReader<>> JSONReader = TJsonReaderFactory<>::Create(iMessage);  //Reader of the Json received

			if (TSharedPtr<FJsonObject> JSONObject; FJsonSerializer::Deserialize(JSONReader, JSONObject))  //Json message is deserialized and checked if it's not null
			{
				OnMessageReceived(JSONObject);  //Handler of the message received
			}
		}
	);

	//This is called when connection is closed
	MSocket->OnClosed().AddLambda( 
		[=](int32 iStatusCode, const FString& iReason, bool iWasClean) -> void
		{
			UE_LOG(LogTemp, Display, TEXT("ABaseNetworkActor::BeginPlay::OnClosed message = %d %s %d"),  //Log to check every step
				   iStatusCode, *iReason, iWasClean);
			FLogger::Log(FString::Printf(TEXT("ABaseNetworkActor::BeginPlay::OnClosed message = %d %s %d"),  //Log to check every step
				   iStatusCode, *iReason, iWasClean));
		}
	);

	//This is called when there is an error 
	MSocket->OnConnectionError().AddLambda(
		[=](const FString& iError) -> void
		{
			UE_LOG(LogTemp, Display, TEXT("ABaseNetworkActor::BeginPlay::OnConnectionError error = %s"), *iError);  //Log to check every step
			FLogger::Log(FString::Printf(TEXT("ABaseNetworkActor::BeginPlay::OnConnectionError error = %s"), *iError));
			GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Magenta, "ERROR CONNECTING TO SERVER!");
		}
	);

	//Connect to WebSocket
	MSocket->Connect();
}

//Send message from client to server through WebSocket
void ABaseNetworkActor::SendMessage(const FString& Message) const
{
	//Check if client and server are connected
	if (MSocket->IsConnected())
	{
		MSocket->Send(Message);  //Send the message through WebSocket
	}
	
}

//Message received, check the map event to handle it
void ABaseNetworkActor::OnMessageReceived(const TSharedPtr<FJsonObject>& Message)
{
	if (Message.IsValid())  //Check if the message is not null
	{
		const FJsonObject& JSONObject = *(Message.Get());
		if (const int32 EventType = JSONObject.GetIntegerField("event"); mEventMap.Contains(EventType))  //Check if the key of the event exists in map or not
		{
			mEventMap[EventType](this, JSONObject);  //Get the event from map of the events with the key
		}
	}
}

//Function that observes events sent by server
void ABaseNetworkActor::ObserveEvent(const int32 EventType,
									 const std::function<void(ABaseNetworkActor*, const FJsonObject&)>& iCallback)
{
	//Add event and its callback in the map of events
	mEventMap.Add(EventType, iCallback);
}

//Function to set ParticipantID of main player
void ABaseNetworkActor::SetParticipantId(const FString& ParticipantId)
{
	this->MParticipantId = ParticipantId;
}

//Function to get ParticipantID of main player
FString& ABaseNetworkActor::GetParticipantId()
{
	return this->MParticipantId;
}

// Called every frame
void ABaseNetworkActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

