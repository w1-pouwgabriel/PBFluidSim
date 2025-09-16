#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PBFPlayerController.generated.h"

UCLASS()
class PBFLUIDSIMCORE_API APBFPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    APBFPlayerController();

protected:
    virtual void SetupInputComponent() override;

    /** Move forward/backward */
    void MoveForward(float Value);

    /** Move right/left */
    void MoveRight(float Value);

    /** Toggle focus/capture state */
    void FocusViewport();
    void UnfocusViewport();
    void HandleLeftClick();

    /** Look functions (mouse input) */
    void LookUp(float Value);
    void Turn(float Value);

    /** Example: Jump */
    void JumpAction();

    /** Fire ray forward */
    void FireRay();

    bool bIsFocused;
};