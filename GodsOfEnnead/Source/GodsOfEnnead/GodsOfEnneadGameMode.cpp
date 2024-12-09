// Fill out your copyright notice in the Description page of Project Settings.

#include "GodsOfEnneadGameMode.h"

#include "GadsOfEnneadCharacter.h"
#include "GodsOfEnneadPlayerController.h"

AGodsOfEnneadGameMode::AGodsOfEnneadGameMode()
{
	PlayerControllerClass = AGodsOfEnneadPlayerController::StaticClass();
	DefaultPawnClass = AGadsOfEnneadCharacter::StaticClass();
}
