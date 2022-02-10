#include "SurfacePointBasisBuilder.h"
#include <cur_line_segment3d.h>
#include "SimpleSparParameters.h"
#include "TestingGeometry.h"
#include "LinkStrategies.h"

using namespace cad;

OffsetLink::OffsetLink(const MbSurface& surface, double safeHeight) 
	: _surface(surface) 
	, _height(safeHeight) 	
{
	const bool SAME = false;
	_offset = surface.Offset(-_height, SAME);
}

SPtr<MbCurve3D> OffsetLink::createTrajectory(MbCartPoint3D sourcePoint, MbCartPoint3D targetPoint) const {
	SurfaceSectionBuilder sectionBuilder(*_offset);
	auto cutter = buildPlaneCutter(sourcePoint, targetPoint - sourcePoint);
	SPtr sectionCurve(sectionBuilder.cutWith(cutter));	
	return buildTrimmed(*sectionCurve, sourcePoint, targetPoint);
}

MbPlacement3D OffsetLink::buildPlaneCutter(MbCartPoint3D sourcePoint, MbVector3D cutDirection) const {
	MbVector3D xDirection(cutDirection);
	xDirection.z = 0;
	MbVector3D yDirection(MbPlacement3D::global.GetAxisZ());
	const bool TAKE_CURVE_EXTENSION = false;
	double u, v;
	_offset->NearPointProjection(sourcePoint, u, v, TAKE_CURVE_EXTENSION);
	MbCartPoint3D origin;
	_offset->PointOn(u, v, origin);
	return MbPlacement3D(xDirection, yDirection, origin);
}

SPtr<MbCurve3D> OffsetLink::buildTrimmed(const MbCurve3D& curve, MbCartPoint3D fromPoint, MbCartPoint3D toPoint) const {
	const bool TAKE_CURVE_EXTENSION = false;
	double trimFrom;
	curve.NearPointProjection(fromPoint, trimFrom, TAKE_CURVE_EXTENSION);
	double trimTo;
	curve.NearPointProjection(toPoint, trimTo, TAKE_CURVE_EXTENSION);
	const int NEW_CURVE_DIRECTION = 1;
	return SPtr(curve.Trimmed(trimFrom, trimTo, NEW_CURVE_DIRECTION));
}
