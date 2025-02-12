// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MotionControllerComponent.h"
#include "WidgetInteractionComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "Pieces/Piece.h"
#include "ChessPlayer.generated.h"

UCLASS()
class CHESS_API AChessPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AChessPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	FVector GetCameraPanDirection();

	UFUNCTION(BlueprintImplementableEvent)
	UCameraComponent* GetCamera();

	UFUNCTION(BlueprintImplementableEvent)
	void SetLocationAndRotation(FVector NewLocation, FRotator NewRotation);

	//Function to set ParticipantID of main player
	void SetParticipantId(const FString& PlayerId);

	//Function to get ParticipantID of main player
	UFUNCTION(BlueprintCallable)
	FString& GetParticipantId();

	//Function to get main player color
	UFUNCTION(BlueprintCallable)
	bool GetIsWhite();

	//Function to set main player color
	void SetIsWhite(bool White);

	//Function to know if it's main player turn or not
	bool GetIsTurn();

	//Function to set main player turn
	void SetIsTurn(bool Turn);

	void SetNickname(const FString& Nick);

	UFUNCTION(BlueprintImplementableEvent)
	UMotionControllerComponent* GetRightMotionController();

	UFUNCTION(BlueprintImplementableEvent)
	UWidgetInteractionComponent* GetRightWidgetInteractionComponent();

	void DoLineTraceFromController(FHitResult& OutHitResult);

	UPrimitiveComponent* LastHoveredComponent = nullptr;
	
	//void SetSeatNumber(int32 INT32);
	
	//void SetBalance(int32 INT32);

	//Functions to get player information

	//int GetSeatNumber();

	FString GetNickname();

	//int GetBalance();
	
	//void SetId(const FString& String);

protected:

	//UPROPERTY()
	//	class USpringArmComponent* CameraSpringArm;

private:

	//Field of ParticipantID of main player
	FString ParticipantId;

	FString Nickname;

	//Boolean field of main player color
	bool IsWhite;

	//Boolean field to check main player turn
	bool IsTurn;
	
	APlayerController* PC;

};
