#include "TrajectoryCreator.h"
#include "SurfacePointBasisBuilder.h"
#include <cur_line_segment3d.h>
#include "SimpleSparParameters.h"
#include "TestingGeometry.h"


TrajectoryCreator::TrajectoryCreator(const std::list<PlyPass>& plyPasses, const MbSurface& surface)
	:_plyPasses(plyPasses)
	, _surface(surface)
{
	const double MIN_SAFE_HEIGHT = 200;
	_landingStrategy = std::make_unique<NormalTouch>(_surface, MIN_SAFE_HEIGHT);
	_takeoffStrategy = std::make_unique<NormalTouch>(_surface, MIN_SAFE_HEIGHT);
	_linkStrategy = std::make_unique<OffsetLink>(_surface, MIN_SAFE_HEIGHT);
}

void TrajectoryCreator::setLandingStrategy(std::unique_ptr<ITouchStrategy> strategy) {
	_landingStrategy = std::move(strategy);
}

void TrajectoryCreator::setTakeoffStrategy(std::unique_ptr<ITouchStrategy> strategy) {
	_takeoffStrategy = std::move(strategy);
}

void TrajectoryCreator::setLinkStrategy(std::unique_ptr<ILinkStrategy> strategy) {
	_linkStrategy = std::move(strategy);
}

Trajectory TrajectoryCreator::createFull(MbCartPoint3D start, MbCartPoint3D finish) const {
	Trajectory trajectory;
	trajectory.Plies.reserve(_plyPasses.size());
	for(const auto& plyPass : _plyPasses) {
		trajectory.Plies.push_back(createForPly(plyPass));
	}

	const ptrdiff_t FIRST = 1;
	const ptrdiff_t LAST = 2;
	trajectory.Links.reserve(trajectory.Plies.size() - 1);
	for(size_t i = 0; !_plyPasses.empty() && i < _plyPasses.size() - 1; ++i) {
		const auto& currentPly = trajectory.Plies[i];
		const auto& nextPly = trajectory.Plies[i + 1];
		auto sourcePoint = currentPly.Tapes.back().Takeoff->GetLimitPoint(LAST);
		auto targetPoint = nextPly.Tapes.front().Landing->GetLimitPoint(FIRST);
		trajectory.Links.push_back(createLink(sourcePoint, targetPoint));
	}

	trajectory.Enter = SPtr(new MbLineSegment3D(start, trajectory.Plies.front().Tapes.front().Landing->GetLimitPoint(FIRST)));
	trajectory.Exit = SPtr(new MbLineSegment3D(trajectory.Plies.back().Tapes.back().Takeoff->GetLimitPoint(LAST), finish));
	return trajectory;
}

PlyTrajectory TrajectoryCreator::createForPly(const PlyPass& pass) const {
	PlyTrajectory trajectory;
	trajectory.Tapes.reserve(pass.TapePasses.size());
	for(const auto& tapePass : pass.TapePasses) {
		trajectory.Tapes.push_back(createForTape(tapePass));
	}

	const ptrdiff_t FIRST = 1;
	const ptrdiff_t LAST = 2;
	trajectory.Links.reserve(trajectory.Tapes.size() - 1);
	for(size_t i = 0; !_plyPasses.empty() && i < _plyPasses.size() - 1; ++i) {
		const auto& currentTape = trajectory.Tapes[i];
		const auto& nextTape = trajectory.Tapes[i + 1];
		auto sourcePoint = currentTape.Takeoff->GetLimitPoint(LAST);
		auto targetPoint = nextTape.Landing->GetLimitPoint(FIRST);
		trajectory.Links.push_back(createLink(sourcePoint, targetPoint));
	}
	return trajectory;
}

TapeTrajectory TrajectoryCreator::createForTape(const TapePass& pass) const {
	TapeTrajectory trajectory;
	trajectory.Landing = createLanding(pass.startPoint());
	trajectory.Runway = createRunway(*pass.Tape, pass.IsBackward);
	trajectory.Takeoff = createTakeoff(pass.endPoint());
	return trajectory;
}

SPtr<MbCurve3D> TrajectoryCreator::createRunway(const Tape& tape, bool isBackward) const {
	const auto& trajectory = isBackward
		? tape.backwardTrajectory().curve()
		: tape.forwardTrajectory().curve();
	const int DIRECTION = 1;
	return SPtr(trajectory.Trimmed(trajectory.GetTMin(), trajectory.GetTMax(), DIRECTION));
}

SPtr<MbCurve3D> TrajectoryCreator::createLanding(MbCartPoint3D surfacePoint) const {
	return _landingStrategy->createTrajectory(surfacePoint, TouchDirections::In);
}

SPtr<MbCurve3D> TrajectoryCreator::createTakeoff(MbCartPoint3D surfacePoint) const {
	return _takeoffStrategy->createTrajectory(surfacePoint, TouchDirections::Our);
}

SPtr<MbCurve3D> TrajectoryCreator::createLink(MbCartPoint3D sourcePoint, MbCartPoint3D targetPoint) const {
	return _linkStrategy->createTrajectory(sourcePoint, targetPoint);
}
