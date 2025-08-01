// SPDX-FileCopyrightText: 2023 Erin Catto
// SPDX-License-Identifier: MIT
#pragma once

#include "array.h"
#include "solver.h"

#include "box2d/types.h"

typedef struct b2DebugDraw b2DebugDraw;
typedef struct b2StepContext b2StepContext;
typedef struct b2World b2World;

/// A joint edge is used to connect bodies and joints together
/// in a joint graph where each body is a node and each joint
/// is an edge. A joint edge belongs to a doubly linked list
/// maintained in each attached body. Each joint has two joint
/// nodes, one for each attached body.
typedef struct b2JointEdge
{
	int bodyId;
	int prevKey;
	int nextKey;
} b2JointEdge;

// Map from b2JointId to b2Joint in the solver sets
typedef struct b2Joint
{
	void* userData;

	// index of simulation set stored in b2World
	// B2_NULL_INDEX when slot is free
	int setIndex;

	// index into the constraint graph color array, may be B2_NULL_INDEX for sleeping/disabled joints
	// B2_NULL_INDEX when slot is free
	int colorIndex;

	// joint index within set or graph color
	// B2_NULL_INDEX when slot is free
	int localIndex;

	b2JointEdge edges[2];

	int jointId;
	int islandId;
	int islandPrev;
	int islandNext;

	float drawScale;

	b2JointType type;

	// This is monotonically advanced when a body is allocated in this slot
	// Used to check for invalid b2JointId
	uint16_t generation;

	bool isMarked;
	bool collideConnected;

} b2Joint;

typedef struct b2DistanceJoint
{
	float length;
	float hertz;
	float dampingRatio;
	float lowerSpringForce;
	float upperSpringForce;
	float minLength;
	float maxLength;

	float maxMotorForce;
	float motorSpeed;

	float impulse;
	float lowerImpulse;
	float upperImpulse;
	float motorImpulse;

	int indexA;
	int indexB;
	b2Vec2 anchorA;
	b2Vec2 anchorB;
	b2Vec2 deltaCenter;
	b2Softness distanceSoftness;
	float axialMass;

	bool enableSpring;
	bool enableLimit;
	bool enableMotor;
} b2DistanceJoint;

typedef struct b2MotorJoint
{
	b2Vec2 linearVelocity;
	float maxVelocityForce;
	float angularVelocity;
	float maxVelocityTorque;
	float linearHertz;
	float linearDampingRatio;
	float maxSpringForce;
	float angularHertz;
	float angularDampingRatio;
	float maxSpringTorque;

	b2Vec2 linearVelocityImpulse;
	float angularVelocityImpulse;
	b2Vec2 linearSpringImpulse;
	float angularSpringImpulse;

	b2Softness linearSpring;
	b2Softness angularSpring;

	int indexA;
	int indexB;
	b2Transform frameA;
	b2Transform frameB;
	b2Vec2 deltaCenter;
	b2Mat22 linearMass;
	float angularMass;
} b2MotorJoint;

typedef struct b2MouseJoint
{
	float hertz;
	float dampingRatio;
	float maxForce;

	b2Vec2 linearImpulse;
	float angularImpulse;

	b2Softness linearSoftness;
	b2Softness angularSoftness;
	int indexA;
	int indexB;
	b2Transform frameA;
	b2Transform frameB;
	b2Vec2 deltaCenter;
	b2Mat22 linearMass;
	float angularMass;
} b2MouseJoint;

typedef struct b2PrismaticJoint
{
	b2Vec2 impulse;
	float springImpulse;
	float motorImpulse;
	float lowerImpulse;
	float upperImpulse;
	float hertz;
	float dampingRatio;
	float targetTranslation;
	float maxMotorForce;
	float motorSpeed;
	float lowerTranslation;
	float upperTranslation;

	int indexA;
	int indexB;
	b2Transform frameA;
	b2Transform frameB;
	b2Vec2 deltaCenter;
	float axialMass;
	b2Softness springSoftness;

	bool enableSpring;
	bool enableLimit;
	bool enableMotor;
} b2PrismaticJoint;

typedef struct b2RevoluteJoint
{
	b2Vec2 linearImpulse;
	float springImpulse;
	float motorImpulse;
	float lowerImpulse;
	float upperImpulse;
	float hertz;
	float dampingRatio;
	float targetAngle;
	float maxMotorTorque;
	float motorSpeed;
	float lowerAngle;
	float upperAngle;

	int indexA;
	int indexB;
	b2Transform frameA;
	b2Transform frameB;
	b2Vec2 deltaCenter;
	float axialMass;
	b2Softness springSoftness;

	bool enableSpring;
	bool enableMotor;
	bool enableLimit;
} b2RevoluteJoint;

typedef struct b2WeldJoint
{
	float linearHertz;
	float linearDampingRatio;
	float angularHertz;
	float angularDampingRatio;

	b2Softness linearSpring;
	b2Softness angularSpring;
	b2Vec2 linearImpulse;
	float angularImpulse;

	int indexA;
	int indexB;
	b2Transform frameA;
	b2Transform frameB;
	b2Vec2 deltaCenter;
	float axialMass;
} b2WeldJoint;

typedef struct b2WheelJoint
{
	float perpImpulse;
	float motorImpulse;
	float springImpulse;
	float lowerImpulse;
	float upperImpulse;
	float maxMotorTorque;
	float motorSpeed;
	float lowerTranslation;
	float upperTranslation;
	float hertz;
	float dampingRatio;

	int indexA;
	int indexB;
	b2Transform frameA;
	b2Transform frameB;
	b2Vec2 deltaCenter;
	float perpMass;
	float motorMass;
	float axialMass;
	b2Softness springSoftness;

	bool enableSpring;
	bool enableMotor;
	bool enableLimit;
} b2WheelJoint;

/// The base joint class. Joints are used to constraint two bodies together in
/// various fashions. Some joints also feature limits and motors.
typedef struct b2JointSim
{
	int jointId;

	int bodyIdA;
	int bodyIdB;

	b2JointType type;

	b2Transform localFrameA;
	b2Transform localFrameB;

	float invMassA, invMassB;
	float invIA, invIB;

	float constraintHertz;
	float constraintDampingRatio;

	b2Softness constraintSoftness;

	float forceThreshold;
	float torqueThreshold;

	union
	{
		b2DistanceJoint distanceJoint;
		b2MotorJoint motorJoint;
		b2MouseJoint mouseJoint;
		b2RevoluteJoint revoluteJoint;
		b2PrismaticJoint prismaticJoint;
		b2WeldJoint weldJoint;
		b2WheelJoint wheelJoint;
	};
} b2JointSim;

void b2DestroyJointInternal( b2World* world, b2Joint* joint, bool wakeBodies );

b2Joint* b2GetJointFullId( b2World* world, b2JointId jointId );
b2JointSim* b2GetJointSim( b2World* world, b2Joint* joint );
b2JointSim* b2GetJointSimCheckType( b2JointId jointId, b2JointType type );

void b2PrepareJoint( b2JointSim* joint, b2StepContext* context );
void b2WarmStartJoint( b2JointSim* joint, b2StepContext* context );
void b2SolveJoint( b2JointSim* joint, b2StepContext* context, bool useBias );

void b2PrepareOverflowJoints( b2StepContext* context );
void b2WarmStartOverflowJoints( b2StepContext* context );
void b2SolveOverflowJoints( b2StepContext* context, bool useBias );

void b2GetJointReaction( b2JointSim* sim, float invTimeStep, float* force, float* torque );

void b2DrawJoint( b2DebugDraw* draw, b2World* world, b2Joint* joint );

b2Vec2 b2GetDistanceJointForce( b2World* world, b2JointSim* base );
b2Vec2 b2GetMotorJointForce( b2World* world, b2JointSim* base );
b2Vec2 b2GetMouseJointForce( b2World* world, b2JointSim* base );
b2Vec2 b2GetPrismaticJointForce( b2World* world, b2JointSim* base );
b2Vec2 b2GetRevoluteJointForce( b2World* world, b2JointSim* base );
b2Vec2 b2GetWeldJointForce( b2World* world, b2JointSim* base );
b2Vec2 b2GetWheelJointForce( b2World* world, b2JointSim* base );

float b2GetMotorJointTorque( b2World* world, b2JointSim* base );
float b2GetMouseJointTorque( b2World* world, b2JointSim* base );
float b2GetPrismaticJointTorque( b2World* world, b2JointSim* base );
float b2GetRevoluteJointTorque( b2World* world, b2JointSim* base );
float b2GetWeldJointTorque( b2World* world, b2JointSim* base );
float b2GetWheelJointTorque( b2World* world, b2JointSim* base );

void b2PrepareDistanceJoint( b2JointSim* base, b2StepContext* context );
void b2PrepareMotorJoint( b2JointSim* base, b2StepContext* context );
void b2PrepareMouseJoint( b2JointSim* base, b2StepContext* context );
void b2PreparePrismaticJoint( b2JointSim* base, b2StepContext* context );
void b2PrepareRevoluteJoint( b2JointSim* base, b2StepContext* context );
void b2PrepareWeldJoint( b2JointSim* base, b2StepContext* context );
void b2PrepareWheelJoint( b2JointSim* base, b2StepContext* context );

void b2WarmStartDistanceJoint( b2JointSim* base, b2StepContext* context );
void b2WarmStartMotorJoint( b2JointSim* base, b2StepContext* context );
void b2WarmStartMouseJoint( b2JointSim* base, b2StepContext* context );
void b2WarmStartPrismaticJoint( b2JointSim* base, b2StepContext* context );
void b2WarmStartRevoluteJoint( b2JointSim* base, b2StepContext* context );
void b2WarmStartWeldJoint( b2JointSim* base, b2StepContext* context );
void b2WarmStartWheelJoint( b2JointSim* base, b2StepContext* context );

void b2SolveDistanceJoint( b2JointSim* base, b2StepContext* context, bool useBias );
void b2SolveMotorJoint( b2JointSim* base, b2StepContext* context );
void b2SolveMouseJoint( b2JointSim* base, b2StepContext* context );
void b2SolvePrismaticJoint( b2JointSim* base, b2StepContext* context, bool useBias );
void b2SolveRevoluteJoint( b2JointSim* base, b2StepContext* context, bool useBias );
void b2SolveWeldJoint( b2JointSim* base, b2StepContext* context, bool useBias );
void b2SolveWheelJoint( b2JointSim* base, b2StepContext* context, bool useBias );

void b2DrawDistanceJoint( b2DebugDraw* draw, b2JointSim* base, b2Transform transformA, b2Transform transformB );
void b2DrawPrismaticJoint( b2DebugDraw* draw, b2JointSim* base, b2Transform transformA, b2Transform transformB, float drawSize );
void b2DrawRevoluteJoint( b2DebugDraw* draw, b2JointSim* base, b2Transform transformA, b2Transform transformB, float drawSize );
void b2DrawWeldJoint( b2DebugDraw* draw, b2JointSim* base, b2Transform transformA, b2Transform transformB, float drawSize );
void b2DrawWheelJoint( b2DebugDraw* draw, b2JointSim* base, b2Transform transformA, b2Transform transformB );

// Define inline functions for arrays
B2_ARRAY_INLINE( b2Joint, b2Joint )
B2_ARRAY_INLINE( b2JointSim, b2JointSim )
