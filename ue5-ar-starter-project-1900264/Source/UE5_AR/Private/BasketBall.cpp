
#include "BasketBall.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "CustomGameMode.h"


ABasketBall::ABasketBall()
{
	PrimaryActorTick.bCanEverTick = true;

    // Set up a sphere for collision representation
    BallCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    BallCollisionComponent->InitSphereRadius(5.0f);
    BallCollisionComponent->BodyInstance.SetCollisionProfileName("Projectile");
    BallCollisionComponent->OnComponentHit.AddDynamic(this, &ABasketBall::OnHit); // Set up a notification for collision

    // Set collision sphere as root component
    RootComponent = BallCollisionComponent;

    // Set up basketball mesh
    BasketballMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ball Mesh"));
    BasketballMesh->SetupAttachment(RootComponent);

    // Use a ProjectileMovementComponent to update the basketball's movement
    BasketballMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
    BasketballMovement->bShouldBounce = true;
    BasketballMovement->bAutoActivate = false;

    // Set the lifespan of the spawned basketball
    InitialLifeSpan = 3.0f;

}


//////////////       BEGIN PLAY       //////////////
void ABasketBall::BeginPlay()
{
    Super::BeginPlay();
}


//////////////       TICK       //////////////
void ABasketBall::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Destroy the actor if it is under the plane used for game
    if (this->GetActorLocation().Z < -300)
    {
        this->Destroy();
    }
}


//////////////       ON HIT       //////////////
void ABasketBall::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor && OtherActor != this && OtherComponent)
    {
        if (Hit.GetComponent()->ComponentHasTag("Basket"))
        {
            // Check if the ball is going down
            if (BallCollisionComponent->GetComponentVelocity().Z < 0)
            {
                // Disable collision
                BallCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
                // Increase score by + 1
                auto Temp = GetWorld()->GetAuthGameMode();
                auto GM = Cast<ACustomGameMode>(Temp);
                GM->SetScore(GM->GetScore() + 1);
                // Reproduce scoring sound
                ReproduceSound(BasketSound, Hit.Location);
            }
        }

        else if (Hit.GetComponent()->ComponentHasTag("Backboard"))
        {
            // Reproduce ball hitting hoop backboard sound
            ReproduceSound(BackboardHitSound, Hit.Location);
        }

        else
        {
            // Reproduce ball hitting hoop rim sound
            ReproduceSound(RimHitSound, Hit.Location);
        }
    }
}


//////////////       REPRODUCE SOUND       //////////////      
void ABasketBall::ReproduceSound(USoundBase* Sound, FVector Location)
{
    if (Sound != nullptr)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, Location);
    }
}


//////////////       SET BASKETBALL VELOCITY       //////////////    
void ABasketBall::SetBasketballVelocity(FVector Velocity)
{
    BasketballMovement->SetVelocityInLocalSpace(Velocity);
    BasketballMovement->Activate();
}






