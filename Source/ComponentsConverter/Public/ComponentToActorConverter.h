#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ComponentToActorConverter.generated.h"

UCLASS()
class COMPONENTSCONVERTER_API  AComponentToActorConverter : public AActor
{
    GENERATED_BODY()

public:
    AComponentToActorConverter();

    UFUNCTION(BlueprintCallable, Category = "Component Conversion")
    void ConvertComponentsToActors(AActor* SourceActor, bool DestroyActor);

private:
    void ProcessComponent(USceneComponent* Component, AActor* ParentActor);
    AActor* CreateActorFromComponent(USceneComponent* Component, FVector Location, FRotator Rotation);
    void TransferComponentProperties(USceneComponent* SourceComponent, AActor* TargetActor);
    void TransferStaticMeshProperties(UStaticMeshComponent* SourceMeshComponent, UStaticMeshComponent* TargetMeshComponent);
};