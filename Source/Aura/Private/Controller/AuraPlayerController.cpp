// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/AuraEnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraMappingContext);
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(Subsystem);
	Subsystem->AddMappingContext(AuraMappingContext,0);
	// Set Mouse Cursor
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
	
	
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&AAuraPlayerController::Move);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator InputRotator = GetControlRotation();
	const FRotator YawRotation(0,InputRotator.Yaw,0);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	if (APawn* ControlledPawn = Cast<APawn>(GetPawn()))
	{
		ControlledPawn->AddMovementInput(ForwardDirection,InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection,InputAxisVector.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility,false,CursorHit);
	if (!CursorHit.bBlockingHit)return;

	LastActor = ThisActor;
	ThisActor = Cast<IAuraEnemyInterface>(CursorHit.GetActor());
	/*
	 * If ThisActor == nullptr and LastActor == nullptr return
	 * If ThisActor != nullptr and LastActor == nullptr  -> Highlight ThisActor
	 * If ThisActor == nullptr and LastActor != nullptr  -> UnHighlight LastActor
	 * If ThisActor != nullptr and LastActor != nullptr and ThisActor != LastActor -> UnHighlight LastActor and Highlight ThisActor
	 * If ThisActor != nullptr and LastActor != nullptr and ThisActor == LastActor -> return
	 */
	if (ThisActor==nullptr && LastActor==nullptr)
	{
		return;
	}
	else if (ThisActor!=nullptr && LastActor==nullptr)
	{
		ThisActor->HighLightActor();
	}
	else if (ThisActor==nullptr && LastActor!=nullptr)
	{
		LastActor->UnHighLightActor();
	}
	else if (ThisActor!=nullptr && LastActor!=nullptr && ThisActor!=LastActor)
	{
		LastActor->UnHighLightActor();
		ThisActor->HighLightActor();
	}
	else if (ThisActor!=nullptr && LastActor!=nullptr && ThisActor==LastActor)
	{
		return;
	}
}

