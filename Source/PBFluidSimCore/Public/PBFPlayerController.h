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

    /** Example: Jump */
    void JumpAction();
    
    bool bIsFocused;
};
