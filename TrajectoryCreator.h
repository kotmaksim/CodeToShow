#pragma once

#include <list>
#include <memory>
#include <curve3d.h>
#include "Passes.h"
#include "TouchStrategies.h"
#include "LinkStrategies.h"

namespace cad 
{
	
/**
 * @brief Полная траектория одного тейпа.
*/
struct TapeTrajectory {
	SPtr<MbCurve3D> Landing; ///< Вход в рабочую траектрию.
	SPtr<MbCurve3D> Runway; ///< Рабочая траектория.
	SPtr<MbCurve3D> Takeoff; ///< Выход из рабочей траектории.
};

/**
 * @brief Полная траектория одного слоя.
*/
struct PlyTrajectory {
	std::vector<TapeTrajectory> Tapes; ///< Набор тейповых траекторий.
	std::vector<SPtr<MbCurve3D>> Links; ///< Набор соединительных траекторий между тейпами.
};

/**
 * @brief Полная траектория выкладки.
*/
struct Trajectory {
	SPtr<MbCurve3D> Enter; ///< Траектория входа головы в рабочую зону.
	std::vector<PlyTrajectory> Plies; ///< Набор послойных траекторий.
	std::vector<SPtr<MbCurve3D>> Links; ///< Набор соединительных траекторий между тейпами.
	SPtr<MbCurve3D> Exit; ///< Траектория выхода головы из рабочей зоны.
};

class MbSurface;

/**
 * @brief Построитель производственных траекторий выкладочной головы.
*/
class TrajectoryCreator {
public:
	/**
	 * @brief Конструктор.
	 * @param plyPasses Список проходов слоев.
	 * @param surface Поверхность выкладки.
	*/
	TrajectoryCreator(const std::list<PlyPass>& plyPasses, const MbSurface& surface);

	/**
	 * @brief Задает стратегию построения входной контактной траектории.
	 * @param strategy Стратегия.
	*/
	void setLandingStrategy(std::unique_ptr<ITouchStrategy> strategy);

	/**
	 * @brief Задает стратегию построения выходной контактной траектории.
	 * @param strategy Стратегия.
	*/
	void setTakeoffStrategy(std::unique_ptr<ITouchStrategy> strategy);

	/**
	 * @brief Задает стратегию построения соединительной траектории.
	 * @param strategy Стратегия.
	*/
	void setLinkStrategy(std::unique_ptr<ILinkStrategy> strategy);

	/**
	 * @brief Создает полную траекторию выкладки.
	 * @param start Позиция головы до начала выкладки.
	 * @param finish Позиция головы после окончания выкладки.
	 * @return Полная траектория выкладки.
	*/
	Trajectory createFull(MbCartPoint3D start, MbCartPoint3D finish) const;

	/**
	 * @brief Создает траекторию выкладки слоя.
	 * @param pass Проход слоя.
	 * @return Полная траектория одного слоя.
	*/
	PlyTrajectory createForPly(const PlyPass& pass) const;

	/**
	 * @brief Создает траекторию выкладки одного тейпа.
	 * @param pass Проход тейпа.
	 * @return Полная траектория одного тейпа.
	*/
	TapeTrajectory createForTape(const TapePass& pass) const;

	/**
	 * @brief Создает рабочую таректорию выкладки от касания ролика поверхности до отрыва.
	 * @param tape Тейп.
	 * @param isBackward Условное направление движение головы вдоль тейпа. 
	 * @return Траектория выкладки.
	*/
	SPtr<MbCurve3D> createRunway(const Tape& tape, bool isBackward) const;

	/**
	 * @brief Создает траекторию выхода от точки отрыва ролика от поверхности выкладки.
	 * @param surfacePoint Точка отрыва ролика от поверхности выкладки.
	 * @return Выходная траектория.
	*/
	SPtr<MbCurve3D> createLanding(MbCartPoint3D surfacePoint) const;	

	/**
	 * @brief Создает траекторию входа к точке касания ролика с поверхностью выкладки.
	 * @param surfacePoint Точка касания ролика с поверхностью выкладки.
	 * @return Входная траектория.
	*/
	SPtr<MbCurve3D> createTakeoff(MbCartPoint3D surfacePoint) const;

	/**
	 * @brief Создает соединительную траектрию между тейпами.
	 * @param sourcePoint Последняя точка выходной траектории пред. тейпа.
	 * @param targetPoint Первая точка входной траектории след. тейпа.
	 * @return 
	*/
	SPtr<MbCurve3D> createLink(MbCartPoint3D sourcePoint, MbCartPoint3D targetPoint) const;

private:
	const std::list<PlyPass>& _plyPasses; ///< Набор проходов слоев.
	const MbSurface& _surface; ///< Поверхность выкладки.
	std::unique_ptr<ITouchStrategy> _landingStrategy; ///< Стратегия построения входной траектории.
	std::unique_ptr<ITouchStrategy> _takeoffStrategy; ///< Стратегия построения выходная траектории.
	std::unique_ptr<ILinkStrategy> _linkStrategy; ///< Стратегия построения соединительной траектории.
};

}

