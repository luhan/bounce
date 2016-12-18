/*
* Copyright (c) 2016-2016 Irlan Robson http://www.irlan.net
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef B3_COLLISION_H
#define B3_COLLISION_H

#include <bounce\common\geometry.h>
#include <bounce\collision\shapes\aabb3.h>
#include <bounce\collision\shapes\capsule.h>

// Input for a ray cast query.
struct b3RayCastInput
{
	b3Vec3 p1; // first point on segment
	b3Vec3 p2; // second point on segment
	float32 maxFraction; // maximum intersection
};

// Output of ray cast query.
struct b3RayCastOutput
{
	float32 fraction; // time of intersection
	b3Vec3 normal; // surface normal of intersection
};

// Find the closest point for a point P to a normalized plane.
b3Vec3 b3ClosestPointOnPlane(const b3Vec3& P, const b3Plane& plane);

// Find the closest point for a point P to a segment AB.
b3Vec3 b3ClosestPointOnSegment(const b3Vec3& P,
	const b3Vec3& A, const b3Vec3& B);

// Find the closest point for a point P to a triangle ABC.
b3Vec3 b3ClosestPointOnTriangle(const b3Vec3& P,
	const b3Vec3& A, const b3Vec3& B, const b3Vec3& C);

// Find the closest points of two lines.
void b3ClosestPointsOnLines(b3Vec3* C1, b3Vec3* C2,
	const b3Vec3& P1, const b3Vec3& E1,
	const b3Vec3& P2, const b3Vec3& E2);

// Find the closest points of two normalized lines.
void b3ClosestPointsOnNormalizedLines(b3Vec3* C1, b3Vec3* C2,
	const b3Vec3& P1, const b3Vec3& N1,
	const b3Vec3& P2, const b3Vec3& N2);

// Find the closest points of two segments P1-Q1 to a segment P2-Q2.
void b3ClosestPointsOnSegments(b3Vec3* C1, b3Vec3* C2,
	const b3Vec3& P1, const b3Vec3& Q1,
	const b3Vec3& P2, const b3Vec3& Q2);

#endif