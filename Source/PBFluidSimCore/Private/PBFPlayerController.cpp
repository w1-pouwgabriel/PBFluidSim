#include "PBFPlayerController.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"

APBFPlayerController::APBFPlayerController()
{
    bIsFocused = false;
}

void APBFPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    check(InputComponent);

    // Bind axes (values from -1 to 1)
    InputComponent->BindAxis("MoveForward", this, &APBFPlayerController::MoveForward);
    InputComponent->BindAxis("MoveRight", this, &APBFPlayerController::MoveRight);

    InputComponent->BindAction("LeftClick", IE_Pressed, this, &APBFPlayerController::HandleLeftClick);

    // Focus/unfocus
    InputComponent->BindAction("FocusViewport", IE_Pressed, this, &APBFPlayerController::FocusViewport);
    InputComponent->BindAction("UnfocusViewport", IE_Pressed, this, &APBFPlayerController::UnfocusViewport);

    // Mouse look (axes from mouse movement)
    InputComponent->BindAxis("Turn", this, &APBFPlayerController::Turn);
    InputComponent->BindAxis("LookUp", this, &APBFPlayerController::LookUp);

    // Bind actions (pressed/released)
    InputComponent->BindAction("Jump", IE_Pressed, this, &APBFPlayerController::JumpAction);
}

void APBFPlayerController::MoveForward(float Value)
{
    if (APawn* ControlledPawn = GetPawn())
    {
        ControlledPawn->AddMovementInput(ControlledPawn->GetActorForwardVector(), Value);
    }
}

void APBFPlayerController::MoveRight(float Value)
{
    if (APawn* ControlledPawn = GetPawn())
    {
        ControlledPawn->AddMovementInput(ControlledPawn->GetActorRightVector(), Value);
    }
}

void APBFPlayerController::FocusViewport()
{
    bShowMouseCursor = false;
    SetInputMode(FInputModeGameOnly());
    bIsFocused = true;

    UE_LOG(LogTemp, Log, TEXT("Viewport focused"));
}

void APBFPlayerController::UnfocusViewport()
{
    bShowMouseCursor = true;
    SetInputMode(FInputModeGameAndUI());
    bIsFocused = false;

    UE_LOG(LogTemp, Log, TEXT("Viewport unfocused"));
}

void APBFPlayerController::HandleLeftClick()
{
    if (!bIsFocused)
    {
        // First click → focus viewport
        SetInputMode(FInputModeGameOnly());

        UE_LOG(LogTemp, Log, TEXT("Viewport focused"));
    }
    else
    {
        // Already focused → fire ray
        FireRay();
    }
}

void APBFPlayerController::Turn(float Value)
{
    if (bIsFocused && Value != 0.0f)
    {
        AddYawInput(Value);
    }
}

void APBFPlayerController::LookUp(float Value)
{
    if (bIsFocused && Value != 0.0f)
    {
        AddPitchInput(Value);
    }
}

void APBFPlayerController::JumpAction()
{
    if (ACharacter* ControlledChar = Cast<ACharacter>(GetPawn()))
    {
        ControlledChar->Jump();
    }
}

void APBFPlayerController::FireRay()
{
    if (!bIsFocused) return;

    FVector Location;
    FRotator Rotation;
    GetPlayerViewPoint(Location, Rotation);

    FVector End = Location + (Rotation.Vector() * 10000.0f);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetPawn());

    if (GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECC_Visibility, Params))
    {
        UE_LOG(LogTemp, Log, TEXT("Hit: %s"), *Hit.GetActor()->GetName());
        DrawDebugLine(GetWorld(), Location, Hit.Location, FColor::Green, false, 1.f, 0, 1.f);
    }
    else
    {
        DrawDebugLine(GetWorld(), Location, End, FColor::Red, false, 1.f, 0, 1.f);
    }
}