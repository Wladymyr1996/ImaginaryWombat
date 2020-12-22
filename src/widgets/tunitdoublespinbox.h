#pragma once
#include <QDoubleSpinBox>

class TUnitDoubleSpinBox : public QDoubleSpinBox
{
	Q_OBJECT

	public:
		TUnitDoubleSpinBox(double coef = 1.0f, QWidget *parent = nullptr);
		void setCoefficient(double coefficient);
		void setCoefValue(double value);
		double coefValue();

	private:
		double coef = 1.0f;
};

