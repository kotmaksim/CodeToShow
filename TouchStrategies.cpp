#include "TouchStrategies.h"
#include "SurfacePointBasisBuilder.h"
#include <surface.h>
#include <cur_line_segment3d.h>

NormalTouch::NormalTouch(const MbSurface& surface, double safeHeight) 
	: _surface(surface)
  , _height(safeHeight) {
}

SPtr<MbCurve3D> NormalTouch::createTrajectory(MbCartPoint3D surfacePoint, TouchDirections direction) const {
	const bool TAKE_CURVE_EXTENSION = false;
	double u, v;
	_surface.NearPointProjection(surfacePoint, u, v, TAKE_CURVE_EXTENSION);
	SurfacePointBasisBuilder basisBuilder(_surface);
	MbVector3D normal = _height * basisBuilder.buildNormal(u, v);
	SPtr trajectory(new MbLineSegment3D(surfacePoint, normal));
	if(direction == TouchDirections::In)
		trajectory->Inverse();
	return trajectory;
}
