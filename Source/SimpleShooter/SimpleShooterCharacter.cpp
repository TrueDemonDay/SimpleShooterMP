// Copyright Epic Games, Inc. All Rights Reserved.

#include "SimpleShooterCharacter.h"
#include "SimpleShooterProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h" //Need for replication
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "SimpleShooterPS.h"
#include "SimpleShooterPlayerController.h"
#include "SimpleShooterGameMode.h"
#include "GameFramework/GameModeBase.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ASimpleShooterCharacter

ASimpleShooterCharacter::ASimpleShooterCharacter()
{
	//Add Dynamic events
	OnTakePointDamage.AddDynamic(this, &ASimpleShooterCharacter::TakePointDamage);
	OnTakeAnyDamage.AddDynamic(this, &ASimpleShooterCharacter::TakeAnyDamage);

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// otherwise won't be visible in the multiplayer
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);
	GunShootDistance = FVector(10000.0f, 0.0f, 0.0f);

	SetReplicates(true);
}


//In the implementation of the actor class, you need to implement the GetLifetimeReplicatedProps function: 
void ASimpleShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASimpleShooterCharacter, Aiming);
	DOREPLIFETIME(ASimpleShooterCharacter, ControllerRotation);
	DOREPLIFETIME(ASimpleShooterCharacter, Health);
}

void ASimpleShooterCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	Mesh1P->SetHiddenInGame(false, true);

	EndAim();

	GetWorldTimerManager().SetTimer(RotationUpdateTimer, this, &ASimpleShooterCharacter::UpdateRotator, 0.022f, true);
	SimpleShooterGameModeRef = (ASimpleShooterGameMode*)GetWorld()->GetAuthGameMode();

	SimpleShooterPlayerControllerRef = Cast<ASimpleShooterPlayerController>(GetController());
	//if (SimpleShooterPlayerControllerRef)
	//	SimpleShooterPlayerControllerRef->AddState();
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASimpleShooterCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASimpleShooterCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASimpleShooterCharacter::EndSprint);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASimpleShooterCharacter::OnFire);

	// Bind Aim event
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ASimpleShooterCharacter::StartAim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ASimpleShooterCharacter::EndAim);

	// Bind ShowScore event
	PlayerInputComponent->BindAction("ShowScore", IE_Pressed, this, &ASimpleShooterCharacter::ShowScore);
	PlayerInputComponent->BindAction("ShowScore", IE_Released, this, &ASimpleShooterCharacter::HideScore);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	//PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ASimpleShooterCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ASimpleShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASimpleShooterCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASimpleShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASimpleShooterCharacter::LookUpAtRate);
}

void ASimpleShooterCharacter::OnFire()
{
	// try and fire a projectile
	/*if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			/*if (bUsingMotionControllers)
			{
				const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
				const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
				World->SpawnActor<ASimpleShooterProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
			}
			else*/
			/*{
				const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// spawn the projectile at the muzzle
				World->SpawnActor<ASimpleShooterProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			}
		}
	}*/
	if (bIsDead) return;

	ShootLineTrace();
	// try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}

	//try and play animation for MP mesh
	if (GetMesh())
	{
		if (FireAnimationAim && Aiming && GetMesh()->GetAnimInstance())
		{
			PlayMotageServer(FireAnimationAim);
		}
		else
		if (FireAnimationHip && !Aiming && GetMesh()->GetAnimInstance())
		{
			PlayMotageServer(FireAnimationHip);
		}
	}
	
}

/*void ASimpleShooterCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}*/

void ASimpleShooterCharacter::TakePointDamage(AActor * DamagedActor, float Damage, AController * InstigatedBy, FVector HitLocation, UPrimitiveComponent * FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType * DamageType, AActor * DamageCauser)
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("PointDamage!"));
	if (BoneName == "head" && !bIsDead)
	{
		bTakeHeadShot = true;
		TakeAnyDamage(this, Damage*2, DamageType, InstigatedBy, DamageCauser); //Make triple damage to head shot (First take Any damage, and now deal plus double)
	}
}

void ASimpleShooterCharacter::TakeAnyDamage(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	if (bIsDead) return;

	Health -= Damage; //Set health on server (By replication)
	UpdateHPMulticast(Health); //Update health on clients
	if (Health <= 0)
	{
		if (!bIsDead)
		{
			PlayerDead();
			if (DamageCauser) //Try points to causer is valid
			{
				ASimpleShooterCharacter* Causer = Cast<ASimpleShooterCharacter>(DamageCauser);
				if (Causer) //Try take causer character
				{
					ASimpleShooterPS* ShooterPlayerState = Cast<ASimpleShooterPS>(Causer->GetPlayerState());
					if (ShooterPlayerState) //Try take state
					{
						if(bTakeHeadShot)
							ShooterPlayerState->PlayerHeadShots += 1; //Add HeadShot point
						ShooterPlayerState->PlayerScore += 1; //Add Kills
						//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Add kill score to Killer!"));
					}
				}
			}

			ASimpleShooterPS* SelfPlayerState = Cast<ASimpleShooterPS>(GetPlayerState());
			if (SelfPlayerState)
			{
				SelfPlayerState->PlayerDeaths += 1;
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Add death to self!"));
			}
		}
	}
	bTakeHeadShot = false;
	UpdateHPWidget();
}

void ASimpleShooterCharacter::UpdateHPMulticast_Implementation(float NewHealth)
{
	Health = NewHealth;
}

void ASimpleShooterCharacter::UpdateHPWidget_Implementation()
{
	if (SimpleShooterPlayerControllerRef)
	{
		SimpleShooterPlayerControllerRef->SetHidenHPWidget(!bIsDead);
		SimpleShooterPlayerControllerRef->SetHPWidget(Health);
	}
}

void ASimpleShooterCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void ASimpleShooterCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void ASimpleShooterCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void ASimpleShooterCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ASimpleShooterCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ASimpleShooterCharacter::StartSprint() 
{
	if (Aiming)
		EndAim();
	SetMaxWalkSpeed(MaxSprintSpeed);
	GetCharacterMovement()->MaxWalkSpeed = MaxSprintSpeed;
}

void ASimpleShooterCharacter::EndSprint()
{
	if (Aiming) return;
	SetMaxWalkSpeed(MaxWalkSpeed);
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void ASimpleShooterCharacter::StartAim()
{
	ChangeAimStatus(true);
	Aiming = true;
	FirstPersonCameraComponent->FieldOfView = 55;
	SetMaxWalkSpeed(MaxAimSpeed);
	GetCharacterMovement()->MaxWalkSpeed = MaxAimSpeed;
}

void ASimpleShooterCharacter::EndAim()
{
	ChangeAimStatus(false);
	Aiming = false;
	FirstPersonCameraComponent->FieldOfView = 90;
	SetMaxWalkSpeed(MaxWalkSpeed);
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void ASimpleShooterCharacter::ShowScore()
{
	if (SimpleShooterPlayerControllerRef)
		SimpleShooterPlayerControllerRef->SetHidenScoreWidget(false);
}

void ASimpleShooterCharacter::HideScore()
{
	if (SimpleShooterPlayerControllerRef)
		SimpleShooterPlayerControllerRef->SetHidenScoreWidget(true);
}

/*void ASimpleShooterCharacter::SetMaxWalkSpeed(float NewSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}*/

//Set MaxSpeed OnServer
void ASimpleShooterCharacter::SetMaxWalkSpeed_Implementation(float NewSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}

//SetAimStatusOnServer
void ASimpleShooterCharacter::ChangeAimStatus_Implementation(bool NewAim)
{
	Aiming = NewAim;
}

//CallMulticastPlayMonatge On server
void ASimpleShooterCharacter::PlayMotageServer_Implementation(UAnimMontage* Montage)
{
	PlayMotageMulticast(Montage);
}

//Start multycast play montage
void ASimpleShooterCharacter::PlayMotageMulticast_Implementation(UAnimMontage* Montage)
{
	GetMesh()->GetAnimInstance()->Montage_Play(Montage, 1.f);
}

void ASimpleShooterCharacter::ShootLineTrace_Implementation()
{
	const FRotator LineTraceRotation = GetControlRotation();
	// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
	const FVector LineTraceSpawnLocation = ((FirstPersonCameraComponent) ? FirstPersonCameraComponent->GetComponentLocation() : GetActorLocation()) + LineTraceRotation.RotateVector(FVector(5.f, 0.f,0.f));
	const FVector EndTraceSpawnLocation = ((FirstPersonCameraComponent) ? FirstPersonCameraComponent->GetComponentLocation() : GetActorLocation()) + LineTraceRotation.RotateVector(FVector(100000.f, 0.f, 0.f));
	FHitResult OutHit; 
	FCollisionQueryParams Params = FCollisionQueryParams::DefaultQueryParam;
	Params.AddIgnoredActor(this);
	if (GetWorld()->LineTraceSingleByChannel(OutHit, LineTraceSpawnLocation, EndTraceSpawnLocation, ECollisionChannel::ECC_Visibility, Params))
	{
		if (OutHit.GetActor())
		{
			UGameplayStatics::ApplyPointDamage(OutHit.GetActor(), fDamage, LineTraceSpawnLocation, OutHit, nullptr, this, nullptr);
		}
	}
	//Draw line for Debug
	/*DrawDebugLine(
		GetWorld(),
		LineTraceSpawnLocation,
		EndTraceSpawnLocation,
		FColor(255, 0, 0),
		false, 1, 0,
		1
	);*/
}

void ASimpleShooterCharacter::PlayerDead_Implementation()
{
	//On ragdoll and show for player
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetOwnerNoSee(false);
	//Hide hands and weapon for player
	Mesh1P->SetHiddenInGame(true, true);
	//Off collision for pawns
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	bIsDead = true;

	//StartTimer for respawn
	FTimerHandle TimerHandele;
	GetWorldTimerManager().SetTimer(TimerHandele, this, &ASimpleShooterCharacter::Respawn, 5, false);

	GetWorldTimerManager().PauseTimer(RotationUpdateTimer);
}

void ASimpleShooterCharacter::Respawn_Implementation()
{	
	AActor *NewBody;
	NewBody = GetWorld()->SpawnActor(ASimpleShooterCharacter::GetClass());
	FTransform NewTransform = SimpleShooterGameModeRef->GetNewRespawLocation();
	NewBody->SetActorTransform(NewTransform);

	if (NewBody && GetController() != nullptr)
	{
		ASimpleShooterCharacter* NewCharacter = Cast<ASimpleShooterCharacter>(NewBody);
		GetController()->Possess(NewCharacter);
		NewCharacter->SimpleShooterPlayerControllerRef = SimpleShooterPlayerControllerRef;
		NewCharacter->UpdateHPWidget();
	}

	FTimerHandle TimerHandele;
	GetWorldTimerManager().SetTimer(TimerHandele, this, &ASimpleShooterCharacter::DestroySelf, 15, false);
}

void ASimpleShooterCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASimpleShooterCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool ASimpleShooterCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ASimpleShooterCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &ASimpleShooterCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ASimpleShooterCharacter::TouchUpdate);
		return true;
	}
	
	return false;
}

void ASimpleShooterCharacter::UpdateRotator_Implementation()
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Update rotation!"));
	ControllerRotation = GetControlRotation();
}

void ASimpleShooterCharacter::DestroySelf_Implementation()
{
	Destroy();
}
