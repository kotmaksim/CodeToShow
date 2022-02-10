#pragma once

#include <curve3d.h>

/**
 * @brief Типы контактных траекторий.
*/
enum class TouchStrategyTypes {
	Normal, ///< По нормали.
	Arc, ///< По дуге.
	Ramp ///< По наклонной.
};

/**
 * @brief Направления траектории.
*/
enum class TouchDirections {
	In, ///< К поверхности.
	Out ///< От поверхности.
};

/**
 * @brief Интерфейсный класс стратегий пострения контактных траекторий.
*/
class ITouchStrategy {
public:
	/**
	 * @brief Создает контактную траекторию.
	 * @param surfacePoint Точка контакта на поверхности выкладки.
	 * @param direction Направление траектории.
	 * @return Контактная траектория.
	*/
	virtual SPtr<MbCurve3D> createTrajectory(MbCartPoint3D surfacePoint, TouchDirections direction) const = 0;

	/**
	 * @brief Деструктор.
	*/
	virtual ~ITouchStrategy() = default;
};

/**
 * @brief Стратегия построения контактной кривой по нормали к поверхности выкладки.
*/
class NormalTouch : public ITouchStrategy {
public:
	/**
	 * @brief Конструктор.
	 * @param surface Поверхность выкладки.
	 * @param safeHeight Безопасная высота, мм.
	*/
	NormalTouch(const MbSurface& surface, double safeHeight);

	/**
	 * @brief Создает контактную траекторию.
	 * @param surfacePoint Точка контакта на поверхности выкладки.
	 * @param direction Направление траектории.
	 * @return 
	*/
	SPtr<MbCurve3D> createTrajectory(MbCartPoint3D surfacePoint, TouchDirections direction) const override;

private:
	const MbSurface& _surface; ///< Поверхность выкладки.
	const double _height; ///< Безопасная высота, мм.
};
