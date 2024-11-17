#include "GodsOfEnneadPlayerController.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void AGodsOfEnneadPlayerController::BeginPlay()
{
    Super::BeginPlay();
    
    FInputModeUIOnly InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

    if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        PlayerController->bShowMouseCursor = true;
        PlayerController->SetInputMode(InputMode);
    }

    bEnableClickEvents = false;
    bEnableMouseOverEvents = false;
    SetIgnoreLookInput(true);  // Ignore look input
    SetIgnoreMoveInput(true);  // Ignore move input

    if (APawn* ControlledPawn = GetPawn())
    {
        ControlledPawn->DisableInput(this);
    }

    const APlayerStart* PlayerStart = nullptr;
    for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
    {
        PlayerStart = *It;
        break;
    }

    if (PlayerStart)
    {
        const FVector InitialLocation = PlayerStart->GetActorLocation();
        GetPawn()->SetActorLocation(InitialLocation);
    }

    EndLocation = FVector(7076.87f, 6673.92f, 2516.89f);
    EndRotation = FRotator(-68.99f, -0.60f, 0.00f);

    MoveDuration = 3.0f;

    const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
    if (CurrentLevelName == "Game")    
    {
        MovePlayerToTarget();
    }
}

void AGodsOfEnneadPlayerController::MovePlayerToTarget()
{
    if (!GetPawn() || MoveDuration <= 0.0f)
    {
        return;
    }

    StartLocation = GetPawn()->GetActorLocation();
    StartRotation = GetPawn()->GetActorRotation();
    ElapsedTime = 0.0f;
    bIsMoving = true;

    SetTickableWhenPaused(true);
    SetActorTickEnabled(true);
    UE_LOG(LogTemp, Warning, TEXT("StartRotation: %s, EndRotation: %s"), *StartRotation.ToString(), *EndRotation.ToString());
}

void AGodsOfEnneadPlayerController::UpdateMovement(float DeltaTime)
{
    if (!bIsMoving || !GetPawn())
    {
        return;
    }

    ElapsedTime += DeltaTime;
    const float Alpha = FMath::Clamp(ElapsedTime / MoveDuration, 0.0f, 1.0f);

    const FVector NewLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);

    const FQuat StartQuat = FQuat(StartRotation);
    const FQuat EndQuat = FQuat(EndRotation);
    const FQuat NewQuat = FQuat::Slerp(StartQuat, EndQuat, Alpha);

    if (USceneComponent* PawnRootComponent = GetPawn()->GetRootComponent())
    {
        PawnRootComponent->SetRelativeLocation(NewLocation);
        PawnRootComponent->SetRelativeRotation(NewQuat);
    }

    if (APlayerController* PlayerController = Cast<APlayerController>(this))
    {
        FRotator CameraRotation = NewQuat.Rotator();
        PlayerController->SetControlRotation(CameraRotation);
    }

    if (Alpha >= 1.0f)
    {
        bIsMoving = false;
        SetActorTickEnabled(false);
    }
}




void AGodsOfEnneadPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateMovement(DeltaTime);
}
