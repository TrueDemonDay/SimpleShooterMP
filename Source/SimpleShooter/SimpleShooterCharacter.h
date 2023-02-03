// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SimpleShooterCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;
class ASimpleShooterPlayerController;
class ASimpleShooterGameMode;

UCLASS(config=Game)
class ASimpleShooterCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* FP_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

public:
	ASimpleShooterCharacter();

protected:
	virtual void BeginPlay();

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector GunShootDistance;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class ASimpleShooterProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	//AnimMitages for MP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimationAim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimationHip;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint8 bUsingMotionControllers : 1;

protected:
	
	/** Fires a projectile. */
	void OnFire();

	/** Resets HMD orientation and position in VR. */
	//void OnResetVR();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	//Sprint functions
	void StartSprint();
	void EndSprint();

	//AimFunctions
	void StartAim();
	void EndAim();

	UFUNCTION(Server, Reliable)
	void SetMaxWalkSpeed(float NewSpeed);

	UFUNCTION(Server, Reliable)
	void ChangeAimStatus(bool NewAim);

	UFUNCTION(Server, Reliable)
	void PlayMotageServer(UAnimMontage* Montage);

	UFUNCTION(NetMulticast, Reliable)
	void PlayMotageMulticast(UAnimMontage* Montage);

	UFUNCTION(Server, Reliable)
	void ShootLineTrace();

	UFUNCTION(Server, Reliable)
	void Respawn();

	UFUNCTION(Server, Reliable)
	void UpdateRotator();

	UFUNCTION(NetMulticast, Reliable)
	void PlayerDead();

	UFUNCTION()
	void TakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser);

	UFUNCTION()
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(Server, Reliable)
	void DestroySelf();

	UFUNCTION(NetMulticast, Reliable)
	void UpdateHPWidget();

	UFUNCTION(NetMulticast, Reliable)
	void UpdateHPMulticast(float NewHealth);


	//Overrite all include damage (use it if need rewrite damage system)
	/*UFUNCTION()
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser) override;*/

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UPROPERTY()
	ASimpleShooterPlayerController* SimpleShooterPlayerControllerRef;

	UPROPERTY()
	ASimpleShooterGameMode* SimpleShooterGameModeRef;

	//SetSpeedProperty
	UPROPERTY(EditDefaultsOnly, Category = Movement)
	float MaxWalkSpeed = 400.f;
	UPROPERTY(EditDefaultsOnly, Category = Movement)
	float MaxSprintSpeed = 700.f;
	UPROPERTY(EditDefaultsOnly, Category = Movement)
	float MaxAimSpeed = 200.f;

	//DamageVar (set default)
	UPROPERTY(EditDefaultsOnly, Category = Shooting)
	float fDamage = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = Health, Replicated)
	float Health = 100.f;

	//Varribles for Rotation Animation
	UPROPERTY(Replicated, BlueprintReadOnly)
	FRotator ControllerRotation;
	FTimerHandle RotationUpdateTimer;

	//Var for Aim animation
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool Aiming = false;

	bool bIsDead = false;
	bool bTakeHeadShot = false;
};

