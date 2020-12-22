#include "tunitdoublespinbox.h"

#include <QDebug>

TUnitDoubleSpinBox::TUnitDoubleSpinBox(double coefficient, QWidget *parent) : QDoubleSpinBox(parent) {
	setCoefficient(coefficient);
}

void TUnitDoubleSpinBox::setCoefficient(double coefficient) {
	setValue(value() * coefficient / coef);
	coef = coefficient;
}

void TUnitDoubleSpinBox::setCoefValue(double value) {
	setValue(value * coef);
}

double TUnitDoubleSpinBox::coefValue() {
	return value() / coef;
}
