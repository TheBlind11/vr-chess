// Fill out your copyright notice in the Description page of Project Settings.

#include "ChessPlayer.h"

#include "Chessboard.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "WidgetInteractionComponent.h"
#include "Camera/CameraComponent.h"
#include "Controllers/WidgetController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Utility/Utility.h"

// Sets default values
AChessPlayer::AChessPlayer()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	//CameraSpringArm->SetupAttachment(RootComponent);
	//CameraSpringArm->SetRelativeLocationAndRotation(FVector(50.0f, -50.0f, 400.0f), FRotator(-60.0f, 0.0f, 0.0f));
	//CameraSpringArm->bDoCollisionTest = false;
	//CameraSpringArm->TargetArmLength = 400.f;
	//CameraSpringArm->bEnableCameraLag = true;
	//CameraSpringArm->CameraLagSpeed = 3.0f;

	//Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("GameCamera"));
	//Camera->SetupAttachment(CameraSpringArm, USpringArmComponent::SocketName);

	//Take control of the default Player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	
}

// Called when the game starts or when spawned
void AChessPlayer::BeginPlay()
{
	Super::BeginPlay();
	PC = Cast<APlayerController>(GetController());
}

// Called every frame
void AChessPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//AddActorWorldOffset(GetCameraPanDirection());
	
	FHitResult HitResult;
	this->DoLineTraceFromController(HitResult);
	//FHitResult HitResult = this->GetRightWidgetInteractionComponent()->GetLastHitResult();
	
	if(HitResult.GetComponent())
	{
		UPrimitiveComponent* NewHoveredComponent = HitResult.GetComponent();
		
		if (NewHoveredComponent != LastHoveredComponent)
		{
			if (LastHoveredComponent)
			{
				if (AChessboard* CB = Cast<AChessboard>(LastHoveredComponent->GetOwner()))
				{
					CB->EndCursorOver(LastHoveredComponent);
				}
			}
			
			LastHoveredComponent = NewHoveredComponent;
			
			if (AChessboard* CB = Cast<AChessboard>(LastHoveredComponent->GetOwner()))
			{
				CB->OnCursorOver(LastHoveredComponent);
			}
		}
	}
	else
	{
		if (LastHoveredComponent)
		{
			if (AChessboard* CB = Cast<AChessboard>(LastHoveredComponent->GetOwner()))
			{
				CB->EndCursorOver(LastHoveredComponent);
			}
			LastHoveredComponent = nullptr;
		}
	}
}

// Called to bind functionality to input
void AChessPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AChessPlayer::DoLineTraceFromController(FHitResult& OutHitResult)
{
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(VRTrace), true, this);
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.AddIgnoredActor(this);
	TraceParams.AddIgnoredActor(Utility::FindByTag(GetWorld(), "WidgetController"));
      
	//UMotionControllerComponent* RightController = this->GetRightMotionController();
	UWidgetInteractionComponent* RightWidgetComponent = this->GetRightWidgetInteractionComponent();
	
	FVector Start = RightWidgetComponent->GetComponentLocation();
	FVector End = Start + (RightWidgetComponent->GetForwardVector() * 5000.f); // lunghezza

	//DrawDebugLine(GetWorld(), Start, Start + RightController->GetForwardVector()*50, FColor::Blue, false, 0.f, 0, 2.f);
	//DrawDebugLine(GetWorld(), Start, Start + RightController->GetRightVector()*50, FColor::Green, false, 0.f, 0, 2.f);
	//DrawDebugLine(GetWorld(), Start, Start + RightController->GetUpVector()*50, FColor::Red, false, 0.f, 0, 2.f);
      
	// Esegui il line trace
	GetWorld()->LineTraceSingleByChannel(
		OutHitResult,
		Start,
		End,
		ECC_Visibility, 
		TraceParams
	);
}

FVector AChessPlayer::GetCameraPanDirection()
{
	// Variable to hold mouse position
	float MousePositionX;
	float MousePositionY;
	float CameraDirectionX = 0;
	float CameraDirectionY = 0;

	PC->GetMousePosition(MousePositionX, MousePositionY);
	if (MousePositionX == 0)
	{
		CameraDirectionY = -1;
	}
	return FVector(CameraDirectionX, CameraDirectionY, 0);
}

//Function to set ParticipantID of main player
void AChessPlayer::SetParticipantId(const FString& PlayerId)
{
	this->ParticipantId = PlayerId;
}

//Function to get ParticipantID of main player
FString& AChessPlayer::GetParticipantId()
{
	return this->ParticipantId;
}

//Function to get main player color
bool AChessPlayer::GetIsWhite()
{
	return this->IsWhite;
}

//Function to know if it's main player turn or not
bool AChessPlayer::GetIsTurn()
{
	return this->IsTurn;
}

//Function to set main player turn
void AChessPlayer::SetIsTurn(bool Turn)
{
	this->IsTurn = Turn;
}

void AChessPlayer::SetNickname(const FString& Nick)
{
	this->Nickname = Nick;
}

//Function to set main player color
void AChessPlayer::SetIsWhite(bool White)
{
	this->IsWhite = White;
}
