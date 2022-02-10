#pragma once

#include <curve3d.h>

/**
 * @brief Типы соединительных траекторий
*/
enum class LinkStrategyTypes {
	Offset, ///< По линии эквидистантной поверхности.
	Straight ///< По прямой линии.
};

/**
 * @brief Интерфейсный класс для стратегий построения соединительных траекторий.
*/
class ILinkStrategy {
public:
	/**
	 * @brief Создает соединительную траекторию.
	 * @param sourcePoint Начальная точка траектории.
	 * @param targetPoint Конечная точка траектории.
	 * @return 
	*/
	virtual SPtr<MbCurve3D> createTrajectory(MbCartPoint3D sourcePoint, MbCartPoint3D targetPoint) const = 0;

	/**
	 * @brief Деструктор.
	*/
	virtual ~ILinkStrategy() = default;
};

/**
 * @brief Стратегия построения соединительной кривой по эквидистантной поверхности.
*/
class OffsetLink : public ILinkStrategy {
public:
	/**
	 * @brief Конструктор.
	 * @param surface Базовая поверхность.
	 * @param safeHeight Безопастная высота.
	*/
	OffsetLink(const MbSurface& surface, double safeHeight);

	/**
	 * @brief Строит соединительную траекторию.
	 * @param sourcePoint Начальная точка траектории.
	 * @param targetPoint Конечная точка траектории.
	 * @return Соединительная траектория.
	*/
	SPtr<MbCurve3D> createTrajectory(MbCartPoint3D sourcePoint, MbCartPoint3D targetPoint) const override;

private:
	const MbSurface& _surface; ///< Поверхность выкладки.
	const double _height; ///< Безопасная высота.
	SPtr<MbSurface> _offset; ///< Эквидистантная поверхность.
	
	/**
	 * @brief Создает СК секущей плоскости.
	 * @param sourcePoint Точка поверхности, в которой по нормали строится секущая плоскость.
	 * @param cutDirection Направление секущей плоскости.
	 * @return СК секущей плоскости.
	*/
	MbPlacement3D buildPlaneCutter(MbCartPoint3D sourcePoint, MbVector3D cutDirection) const;

	/**
	 * @brief Строит усеченную кривую по декартовым точкам (проецируются на исходную кривую).
	 * @param curve Исходная кривая.
	 * @param fromPoint Исходная точка.
	 * @param toPoint Конечная точка.
	 * @return Усеченная кривая.
	*/
	SPtr<MbCurve3D> buildTrimmed(const MbCurve3D& curve, MbCartPoint3D fromPoint, MbCartPoint3D toPoint) const;
};

