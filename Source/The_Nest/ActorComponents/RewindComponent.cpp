
#include "RewindComponent.h"

#include "Components/CapsuleComponent.h"
#include "The_Nest/Player/Character/ThirdPersonCharacter.h"


URewindComponent::URewindComponent()
{

	PrimaryComponentTick.bCanEverTick = false;
    MaxFrames = 20;
    UpdateRewindFrequency = 0.032f;

}

void URewindComponent::SaveFrame(const TMap<FName, UCapsuleComponent*>& Colliders)
{
    FFramePackage Frame;
    Frame.Time = GetWorld()->GetTimeSeconds();

    for (const auto& ColliderPair : Colliders)
    {
        const UCapsuleComponent* Capsule = ColliderPair.Value;

        FFrameCollider FrameCollider;
        FrameCollider.Location = Capsule->GetComponentLocation();
        FrameCollider.Rotation = Capsule->GetComponentRotation();
        FrameCollider.HalfHeight = Capsule->GetScaledCapsuleHalfHeight();
        FrameCollider.Radius = Capsule->GetScaledCapsuleRadius();

        FColliderPair NewPair;
        NewPair.BoneName = ColliderPair.Key;
        NewPair.Collider = FrameCollider;

        Frame.Colliders.Add(NewPair);
    }

    if (const AActor* OwnerActor = GetOwner())
    {
        // Ensure the actor transform is valid
        if (OwnerActor->GetActorTransform().IsValid())
        {
            Frame.ForwardVector = OwnerActor->GetActorForwardVector().GetSafeNormal();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Invalid actor transform at time %f"), Frame.Time);
            Frame.ForwardVector = FVector::ZeroVector;
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("SaveFrame: OwnerActor is null at time %f"), Frame.Time);
        Frame.ForwardVector = FVector::ZeroVector;
    }
    
    History.AddHead(Frame);

    // Limit the number of stored frames
    while (History.Num() > MaxFrames)
    {
        History.RemoveNode(History.GetTail());
    }
}

FFramePackage URewindComponent::GetRewindFrame(float Time)
{
    if (!History.GetHead())
        return {};

    auto Node = History.GetHead();
    FFramePackage RewindFrame;

    while (Node && Node->GetNextNode())
    {
        const FFramePackage& CurrentFrame = Node->GetValue();
        const FFramePackage& NextFrame = Node->GetNextNode()->GetValue();

        if (CurrentFrame.Time >= Time && NextFrame.Time <= Time)
        {
            const float Alpha = FMath::Clamp((Time - NextFrame.Time) / (CurrentFrame.Time - NextFrame.Time), 0.0f, 1.0f);

            RewindFrame.Time = Time;

            for (const auto& Pair : CurrentFrame.Colliders)
            {
                const FName& BoneName = Pair.BoneName;
                const FFrameCollider& OlderCollider = Pair.Collider;
                const FFrameCollider* NewerCollider = nullptr;
                for (const auto& NextPair : NextFrame.Colliders)
                {
                    if (NextPair.BoneName == BoneName)
                    {
                        NewerCollider = &NextPair.Collider;
                        break;
                    }
                }

                FFrameCollider InterpolatedCollider;
                InterpolatedCollider.Location = FMath::VInterpTo(NewerCollider->Location, OlderCollider.Location, 1.0f, Alpha);
                InterpolatedCollider.Rotation = FMath::RInterpTo(NewerCollider->Rotation, OlderCollider.Rotation, 1.0f, Alpha);
                InterpolatedCollider.HalfHeight = FMath::Lerp(NewerCollider->HalfHeight, OlderCollider.HalfHeight, Alpha);
                InterpolatedCollider.Radius = FMath::Lerp(NewerCollider->Radius, OlderCollider.Radius, Alpha);

                FColliderPair InterpolatedPair;
                InterpolatedPair.BoneName = BoneName;
                InterpolatedPair.Collider = InterpolatedCollider;

                RewindFrame.Colliders.Add(InterpolatedPair);
            }
            // Interpolate ForwardVector using SLERP
            FVector OlderForwardVector = CurrentFrame.ForwardVector;
            FVector NewerForwardVector = NextFrame.ForwardVector;

            // Convert to quaternions for smooth interpolation
            FQuat OlderQuat = FQuat::FindBetweenNormals(FVector::ForwardVector, OlderForwardVector);
            FQuat NewerQuat = FQuat::FindBetweenNormals(FVector::ForwardVector, NewerForwardVector);

            // Perform quaternion interpolation
            FQuat InterpolatedQuat = FQuat::Slerp(NewerQuat, OlderQuat, Alpha);

            // Convert back to a forward vector
            RewindFrame.ForwardVector = InterpolatedQuat.RotateVector(FVector::ForwardVector);

            break;
        }

        Node = Node->GetNextNode();
    }

    return RewindFrame;
}

void URewindComponent::ShowHistory() const
{
    for (auto Node = History.GetHead(); Node; Node = Node->GetNextNode())
    {
        const FFramePackage& Frame = Node->GetValue();

        for (const auto& ColliderPair : Frame.Colliders)
        {
            const FFrameCollider& Collider = ColliderPair.Collider;
            DrawDebugCapsule(
                GetWorld(),
                Collider.Location,
                Collider.HalfHeight,
                Collider.Radius,
                Collider.Rotation.Quaternion(),
                FColor::Red,
                false,
                0.1f
            );
        }
    }
}


// Called when the game starts
void URewindComponent::BeginPlay()
{
	Super::BeginPlay();

    AActor* OwnerActor = GetOwner();
    if (!OwnerActor)
        return;

    if(AThirdPersonCharacter* OwnerPawn = Cast<AThirdPersonCharacter>(OwnerActor))
    {
        ColliderMap = OwnerPawn->GetBones();
    }
    
    eventTest.BindDynamic(this, &URewindComponent::CustomTickFunction);

    GetWorld()->GetTimerManager().SetTimer(UpdateRewindTimerHandle, eventTest, UpdateRewindFrequency, true);
	
}

void URewindComponent::CustomTickFunction()
{
    if (ColliderMap.Num() > 0)
    {
        SaveFrame(ColliderMap);
    }
    
    //ShowHistory();
}

void URewindComponent::SaveFrameCall()
{
    SaveFrame(ColliderMap);
}


