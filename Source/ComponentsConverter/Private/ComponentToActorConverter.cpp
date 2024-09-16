#include "ComponentToActorConverter.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"

AComponentToActorConverter::AComponentToActorConverter()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AComponentToActorConverter::ConvertComponentsToActors(AActor* SourceActor, bool DestroyActor)
{
    if (!SourceActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid source actor provided."));
        return;
    }

    USceneComponent* SourceRootComponent = SourceActor->GetRootComponent();
    if (!SourceRootComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("Source actor has no root component."));
        return;
    }

    ProcessComponent(SourceRootComponent, nullptr);

    if(DestroyActor) SourceActor->Destroy();
    
}

void AComponentToActorConverter::ProcessComponent(USceneComponent* Component, AActor* ParentActor)
{
    if (!Component)
        return;

    FVector Location = Component->GetComponentLocation();
    FRotator Rotation = Component->GetComponentRotation();

    AActor* NewActor = CreateActorFromComponent(Component, Location, Rotation);

    TransferComponentProperties(Component, NewActor);

    if (ParentActor)
    {
        NewActor->AttachToActor(ParentActor, FAttachmentTransformRules::KeepWorldTransform);
        NewActor->SetActorRelativeTransform(Component->GetRelativeTransform());
    }

    

    TArray<USceneComponent*> ChildComponents;
    Component->GetChildrenComponents(false, ChildComponents);

    for (USceneComponent* ChildComponent : ChildComponents)
    {
        ProcessComponent(ChildComponent, NewActor);
    }

}

AActor* AComponentToActorConverter::CreateActorFromComponent(USceneComponent* Component, FVector Location, FRotator Rotation)
{
    UWorld* World = GetWorld();
    if (!World)
        return nullptr;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AActor* NewActor = World->SpawnActor<AActor>(AActor::StaticClass(), Location, Rotation, SpawnParams);
    NewActor->SetActorLabel(Component->GetName());

    return NewActor;
}

void AComponentToActorConverter::TransferComponentProperties(USceneComponent* SourceComponent, AActor* TargetActor)
{
    if (UStaticMeshComponent* SourceMeshComponent = Cast<UStaticMeshComponent>(SourceComponent))
    {
        UStaticMeshComponent* NewMeshComponent = NewObject<UStaticMeshComponent>(TargetActor, UStaticMeshComponent::StaticClass(), SourceComponent->GetFName());

        TargetActor->AddInstanceComponent(NewMeshComponent);
        NewMeshComponent->AttachToComponent(TargetActor->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
        TargetActor->SetRootComponent(NewMeshComponent);

        NewMeshComponent->SetWorldTransform(SourceComponent->GetComponentTransform());
        NewMeshComponent->RegisterComponent();

        TransferStaticMeshProperties(SourceMeshComponent, NewMeshComponent);
    }
    else
    {
        USceneComponent* NewSceneComponent = NewObject<USceneComponent>(TargetActor, USceneComponent::StaticClass(), SourceComponent->GetFName());

        TargetActor->AddInstanceComponent(NewSceneComponent);
        NewSceneComponent->AttachToComponent(TargetActor->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
        TargetActor->SetRootComponent(NewSceneComponent);

        NewSceneComponent->SetWorldTransform(SourceComponent->GetComponentTransform());
        NewSceneComponent->RegisterComponent();
    }
}

void AComponentToActorConverter::TransferStaticMeshProperties(UStaticMeshComponent* SourceMeshComponent, UStaticMeshComponent* TargetMeshComponent)
{
    TargetMeshComponent->SetStaticMesh(SourceMeshComponent->GetStaticMesh());
    TargetMeshComponent->SetMaterial(0, SourceMeshComponent->GetMaterial(0));
    TargetMeshComponent->SetCollisionProfileName(SourceMeshComponent->GetCollisionProfileName());
    TargetMeshComponent->SetGenerateOverlapEvents(SourceMeshComponent->GetGenerateOverlapEvents());
    TargetMeshComponent->SetCanEverAffectNavigation(SourceMeshComponent->CanEverAffectNavigation());
}