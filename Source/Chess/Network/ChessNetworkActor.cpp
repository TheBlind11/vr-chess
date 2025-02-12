// Fill out your copyright notice in the Description page of Project Settings.

#include "ChessNetworkActor.h"
#include "Utility/Utility.h"

//Namespace of the events: communication of events with server is defined by numbers
namespace Events
{
	static constexpr int32 GConnected_Event = 0;
	static constexpr int32 GPlayer_Seated_Event = 1;
	static constexpr int32 GPlayerWantToPlay_Event = 3;
	static constexpr int32 GPlayerMove_Event = 4;
	static constexpr int32 GPlayerOfferDraw_Event = 5;
	static constexpr int32 GPlayerAcceptDraw_Event = 6;
	static constexpr int32 GPlayer_Turn_Event = 7;
	static constexpr int32 GCountdown_Game_Event = 10;
	static constexpr int32 GPlayer_Has_Moved_Event = 11;
	static constexpr int32 GPlayer_Has_Offered_Draw_Event = 12;
	static constexpr int32 GPlayerClaimDraw_Event = 13;
	static constexpr int32 GResign_Event = 15;
	static constexpr int32 GPlayerOfferRematch_Event = 16;
	static constexpr int32 GPlayerHasOfferedRematch_Event = 17;
	static constexpr int32 GPlayerAcceptRematch_Event = 18;
	static constexpr int32 GGame_Finished_Event = 21;
}


// Sets default values
AChessNetworkActor::AChessNetworkActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//We don't need anything called by every frame
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AChessNetworkActor::BeginPlay()
{
	Super::BeginPlay();
}

//Function that initialize ChessNetworkActor
void AChessNetworkActor::Initialize()
{
	//Every event is observed and mapped in ChessNetworkActor
	ObserveEvent(Events::GConnected_Event, CALLBACK(&AChessNetworkActor::OnConnectedEvent));
	ObserveEvent(Events::GPlayer_Seated_Event, CALLBACK(&AChessNetworkActor::OnPlayerSeatedEvent));
	ObserveEvent(Events::GCountdown_Game_Event, CALLBACK(&AChessNetworkActor::OnCountdownGameEvent));
	ObserveEvent(Events::GPlayer_Turn_Event, CALLBACK(&AChessNetworkActor::OnPlayerTurnEvent));
	ObserveEvent(Events::GPlayer_Has_Moved_Event, CALLBACK(&AChessNetworkActor::OnPlayerHasMovedEvent));
	ObserveEvent(Events::GGame_Finished_Event, CALLBACK(&AChessNetworkActor::OnGameFinishedEvent));
	ObserveEvent(Events::GPlayer_Has_Offered_Draw_Event, CALLBACK(&AChessNetworkActor::OnPlayerHasOfferedDrawEvent));
	ObserveEvent(Events::GPlayerHasOfferedRematch_Event, CALLBACK(&AChessNetworkActor::OnPlayerHasOfferedRematchEvent));
}

//This function is called when ConnectedEvent is received from server (look at sequence diagram to understand it better)
void AChessNetworkActor::OnConnectedEvent(AChessNetworkActor* iCaller, const FJsonObject& iJsonObject)
{
	//const int Balance = iJsonObject.GetIntegerField("balance");
	iCaller->MParticipantId = iJsonObject.GetStringField("participantId");  //Get ParticipantID of the player that connected to server (WebSocket) and set it in the field of ChessNetworkActor
	const TSharedPtr<FJsonObject>& StateOfGame = iJsonObject.GetObjectField("stateOfGame");  //State of the game is useful to know if other players are seated in game
	//const int CurrentTimestamp = iJsonObject.GetNumberField("currentTimestamp");

	if(iCaller && iCaller->OnConnectedEventDelegate.IsBound())
	{
		UE_LOG(LogTemp, Display, TEXT("CONNECTED EVENT RECEIVED FROM CHESS SERVER!"));
	 	iCaller->OnConnectedEventDelegate.Execute(iCaller->MParticipantId, StateOfGame);  //Call the function delegated to handle this event
	}

	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Magenta, "CONNECTED EVENT RECEIVED FROM CHESS SERVER!!!");
	UE_LOG(LogTemp, Display, TEXT("AChessNetworkActor::OnConnectedEvent participantiId = %s"), *iCaller->MParticipantId);
}

//This function is called when PlayerSeatedEvent is received from server (look at sequence diagram to understand it better)
void AChessNetworkActor::OnPlayerSeatedEvent(AChessNetworkActor* iCaller, const FJsonObject& iJsonObject)
{
	const FString& PlayerId = iJsonObject.GetStringField("participantId"); //Get ParticipantID of the player that has just seated in game
	const FString& Nick = iJsonObject.GetStringField("nickname");  //Get Nickname of the player 
	const int SeatNumber = iJsonObject.GetIntegerField("seatNumber");  //Get SeatNumber of the player 

	if (iCaller && iCaller->OnPlayerSeatedEventDelegate.IsBound())
	{
		iCaller->OnPlayerSeatedEventDelegate.Execute(PlayerId, Nick, SeatNumber);  //Call the function delegated to handle this event
	}
}

//This function is called when CountdownGameEvent is received from server (look at sequence diagram to understand it better)
void AChessNetworkActor::OnCountdownGameEvent(AChessNetworkActor* iCaller, const FJsonObject& iJsonObject)
{
	int64 CountdownTimestamp = iJsonObject.GetNumberField("countDownTimestamp");
	FDateTime DateTime = FDateTime::UtcNow();
	int64 CurrentTimestamp = DateTime.ToUnixTimestamp() * 1000;
	int Seconds = round((CountdownTimestamp - CurrentTimestamp) / 1000);
	//Message on screen: "<seconds> TO GO"

	if (iCaller && iCaller->OnCountdownGameEventDelegate.IsBound())
	{
		iCaller->OnCountdownGameEventDelegate.Execute(Seconds);  //Call the function delegated to handle this event
	}
}

//Function called when PlayerTurnEvent is received from server (look at sequence diagram to understand it better), this message is sent to both players and it defines first turn of the game
void AChessNetworkActor::OnPlayerTurnEvent(AChessNetworkActor* iCaller, const FJsonObject& iJsonObject)
{
	const FString& ParticipantId = iJsonObject.GetStringField("participantId");  //Get ParticipantID of the player that has the turn

	int64 ActionDuration = iJsonObject.GetNumberField("actionTimestamp");
	FDateTime DateTime = FDateTime::UtcNow();
	int64 CurrentTimestamp = DateTime.ToUnixTimestamp() * 1000;
	const int ActionSeconds = round((ActionDuration - CurrentTimestamp) / 1000);

	const int SeatNumber = iJsonObject.GetIntegerField("seatNumber");  //Get SeatNumber of the player that has the turn
	
	const FString& TurnId = iJsonObject.GetStringField("turnId");  //Get TurnID of the turn
	const bool IsWhite = iJsonObject.GetBoolField("isWhite");  //Get IsWhite field (important to understand who is the first player that can make a move: the white one)
	TArray<TSharedPtr<FJsonValue>> AvailableMoves;
	if(IsWhite)  //The player who has received this message is going to start the game with his/her move
	{
		AvailableMoves = iJsonObject.GetArrayField("availableMoves");  //Get AvailableMoves that player can do in turn
	}

	if(iCaller && iCaller->OnPlayerTurnEventDelegate.IsBound())
	{
		iCaller->OnPlayerTurnEventDelegate.Execute(ParticipantId, TurnId, IsWhite, AvailableMoves);  //Call the function delegated to handle this event
	}
}
//This function is called when PlayerHasMovedEvent is received from server (look at sequence diagram to understand it better)
void AChessNetworkActor::OnPlayerHasMovedEvent(AChessNetworkActor* iCaller, const FJsonObject& iJsonObject)
{
	const FString& ParticipantId = iJsonObject.GetStringField("participantId");  //Get ParticipantID of the player who has just made last move
	const FString& TurnId = iJsonObject.GetStringField("turnId");  //Get TurnID of the turn

	int64 ActionDuration = iJsonObject.GetNumberField("actionTimestamp");
	FDateTime DateTime = FDateTime::UtcNow();
	int64 CurrentTimestamp = DateTime.ToUnixTimestamp() * 1000;

	const int ActionSeconds = round((ActionDuration - CurrentTimestamp) / 1000);
	const int OpponentSource = iJsonObject.GetIntegerField("opponentSource");  //Get OpponentSource of last move
	const int OpponentEndpoint = iJsonObject.GetIntegerField("opponentEndpoint");  //Get OpponentEndpoint of last move
	const int SeatNumber = iJsonObject.GetIntegerField("seatNumber");  //Get SeatNumber of the player that has the turn now
	const FString& PromoteTo = iJsonObject.GetStringField("promoteTo");  //Get PromoteTo field to understand if the opponent has promoted a pawn to another piece
	
	const FString& Reason = iJsonObject.GetStringField("canClaimDraw");  //Get CanClaimDraw field in order to understand if the player can claim draw or not due to opponent last move
	
	TArray<TSharedPtr<FJsonValue>> AvailableMoves;
	AvailableMoves = iJsonObject.GetArrayField("availableMoves");  //Get AvailableMoves that player can now do in this turn

	if(iCaller && iCaller->OnPlayerHasMovedEventDelegate.IsBound())
	{
		iCaller->OnPlayerHasMovedEventDelegate.Execute(ParticipantId, TurnId, ActionSeconds, OpponentSource, OpponentEndpoint, SeatNumber, PromoteTo, AvailableMoves, Reason);  //Call the function delegated to handle this event
	}
}

//This function is called when GameFinishedEvent is received from server (look at sequence diagram to understand it better)
void AChessNetworkActor::OnGameFinishedEvent(AChessNetworkActor* iCaller, const FJsonObject& iJsonObject)
{
	const FString& WinnerId = iJsonObject.GetStringField("winner");  //Get the ParticipantID of the player who has won the game
	const FString& LoserId = iJsonObject.GetStringField("loser");  //Get the ParticipantID of the player who has lost the game
	FString LastTurnPlayerId;
	int NftId;
	iJsonObject.TryGetStringField("participantId", LastTurnPlayerId);  //Get ParticipantID of the player who has done last move (this is got only when game finishes with a draw)
	iJsonObject.TryGetNumberField("nftId", NftId);
	const FString& Reason = iJsonObject.GetStringField("reason");  //Get Reason that has caused the end of the game
	const bool IsDraw = iJsonObject.GetBoolField("isDraw");  //Get IsDraw field to understand if game ended with a draw
	const int Source = iJsonObject.GetIntegerField("source");  //Get Source of last move done
	const int Endpoint = iJsonObject.GetIntegerField("endpoint");  //Get Endpoint of last move done
	FString PromoteTo;
	iJsonObject.TryGetStringField("promoteTo", PromoteTo);  //Get PromoteTo field in order to understand if game finished with a promotion or not

	if(iCaller && iCaller->OnGameFinishedEventDelegate.IsBound())
	{
		iCaller->OnGameFinishedEventDelegate.Execute(LastTurnPlayerId, WinnerId, LoserId, Reason, IsDraw, Source, Endpoint, PromoteTo, NftId);  //Call the function delegated to handle this event
	}
}

//This function is called when PlayerHasOfferedDraw  Event is received from server (look at sequence diagram to understand it better)
void AChessNetworkActor::OnPlayerHasOfferedDrawEvent(AChessNetworkActor* iCaller, const FJsonObject& iJsonObject)
{
	const FString& ParticipantId = iJsonObject.GetStringField("participantId");  //Get ParticipantID of the player who has offered a draw

	if(iCaller && iCaller->OnPlayerHasOfferedDrawEventDelegate.IsBound())
	{
		iCaller->OnPlayerHasOfferedDrawEventDelegate.Execute();  //Call the function delegated to handle this event
	}
}

void AChessNetworkActor::OnPlayerHasOfferedRematchEvent(AChessNetworkActor* iCaller, const FJsonObject& iJsonObject)
{
	const FString& ParticipantId = iJsonObject.GetStringField("participantId");  //Get ParticipantID of the player who has offered a rematch

	if(iCaller && iCaller->OnPlayerHasOfferedRematchEventDelegate.IsBound())
	{
		iCaller->OnPlayerHasOfferedRematchEventDelegate.Execute();  //Call the function delegated to handle this event
	}
}

//This function is called when Player clicks on "Play Button" in ChessUIWidget (look at blueprint graph)
void AChessNetworkActor::SendPlayerWantToPlay(FString& Nickname)
{
	//Create Json Object to send to server
	FJsonObject o;
	o.SetStringField("participantId",this->MParticipantId);  //Set ParticipantID of the main player who is sending this message
	o.SetNumberField("event", Events::GPlayerWantToPlay_Event);  //Set the event 
	o.SetStringField("nickname", Nickname);  //Set Nickname of the player who wants to play
	o.SetNumberField("balance", 20000);  //Set the balance of the player that wants to play
	SendMessage(Utility::JsonToString(o));  //Send the message to server
}

//This function is called when player makes a move (look at ChessController::MakeMOve())
void AChessNetworkActor::SendMove(FString& TurnId, int Source, int Endpoint, FString PromoteTo)
{
	//Create Json Object to send to server
	FJsonObject o;
	o.SetStringField("participantId",this->MParticipantId);  //Set ParticipantID of the main player who is sending this message
	o.SetStringField("turnId", TurnId);
	o.SetNumberField("event", Events::GPlayerMove_Event);  //Set the event 
	o.SetNumberField("source", Source);
	o.SetNumberField("endpoint", Endpoint);
	if(PromoteTo != "")
		o.SetStringField("promoteTo", PromoteTo);
	SendMessage(Utility::JsonToString(o));  //Send the message to server
}

//This function is called when player wants to offer a draw to the opponent, so when player clicks on "Offer Draw Button" in ChessUIWidget (look at blueprint graph)
void AChessNetworkActor::OfferDraw()
{
	//Create Json Object to send to server
	FJsonObject o;
	o.SetStringField("participantId",this->MParticipantId);  //Set ParticipantID of the main player who is sending this message
	o.SetNumberField("event", Events::GPlayerOfferDraw_Event);  //Set the event 
	SendMessage(Utility::JsonToString(o));  //Send the message to server
}

//This function is called when player wants to accept draw offered by the opponent, so when player clicks on "Yes Button" in ChessUIWidget (look at blueprint graph)
void AChessNetworkActor::AcceptDraw()
{
	//Create Json Object to send to server
	FJsonObject o;
	o.SetStringField("participantId",this->MParticipantId);  //Set ParticipantID of the main player who is sending this message
	o.SetNumberField("event", Events::GPlayerAcceptDraw_Event);  //Set the event 
	SendMessage(Utility::JsonToString(o));  //Send the message to server
}

//This function is called when player wants to claim draw, so when player clicks on "Claim Draw Button" in ChessUIWidget (look at blueprint graph)
void AChessNetworkActor::ClaimDraw(UPARAM(ref) FString& Reason)
{
	//Create Json Object to send to server
	FJsonObject o;
	o.SetStringField("participantId",this->MParticipantId);  //Set ParticipantID of the main player who is sending this message
	o.SetNumberField("event", Events::GPlayerClaimDraw_Event);  //Set the event 
	o.SetStringField("reason", Reason);  //Set the Reason which will cause a draw
	SendMessage(Utility::JsonToString(o));  //Send the message to server
}

//This function is called when player wants to resign, so when player click on "Resign Button" in ChessUIWidget (look at blueprint graph)
void AChessNetworkActor::Resign()
{
	//Create Json Object to send to server
	FJsonObject o;
	o.SetStringField("participantId",this->MParticipantId);  //Set ParticipantID of the main player who is sending this message
	o.SetNumberField("event", Events::GResign_Event);  //Set the event 
	SendMessage(Utility::JsonToString(o));  //Send the message to server
}

//This function is useful to offer rematch to the opponent after a GameFinishedEvent, it is called when "Rematch Button" on GameFinishedUIWidget is pressed
void AChessNetworkActor::OfferRematch()
{
	FJsonObject o;
	o.SetStringField("participantId",this->MParticipantId);  //Set ParticipantID of the main player who is sending this message
	o.SetNumberField("event", Events::GPlayerOfferRematch_Event);  //Set the event 
	SendMessage(Utility::JsonToString(o));  //Send the message to server
}

//This function is useful to accept rematch offered by th opponent after a GameFinishedEvent, it is called when "Yes button" on GameFinishedUIWidget is pressed
void AChessNetworkActor::AcceptRematch()
{
	FJsonObject o;
	o.SetStringField("participantId",this->MParticipantId);  //Set ParticipantID of the main player who is sending this message
	o.SetNumberField("event", Events::GPlayerAcceptRematch_Event);  //Set the event 
	SendMessage(Utility::JsonToString(o));  //Send the message to server
}

// Called every frame
void AChessNetworkActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

