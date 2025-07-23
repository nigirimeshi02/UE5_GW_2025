// Copyright Epic Games, Inc. All Rights Reserved.


#include "UE5_GW_2025PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "UE5_GW_2025CameraManager.h"

AUE5_GW_2025PlayerController::AUE5_GW_2025PlayerController()
{
	// set the player camera manager class
	PlayerCameraManagerClass = AUE5_GW_2025CameraManager::StaticClass();
}

void AUE5_GW_2025PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
		{
			Subsystem->AddMappingContext(CurrentContext, 0);
		}
	}
}
