#include "3dmodel.h"

T3DModel::T3DModel(QObject *parent) : QObject(parent) {

}

const QVector<TFace> &T3DModel::getFaces() const {
	return faces;
}

void T3DModel::addFace(TPoint p1, TPoint p2, TPoint p3) {
	TFace face;
	face.point.push_back(p1);
	face.point.push_back(p2);
	face.point.push_back(p3);

	addFace(face);
}

void T3DModel::addFace(TFace face) {
	faces.push_back(face);
}

void T3DModel::setName(const QString name) {
	this->name = name;
}

QString T3DModel::getName() const {
	return name;
}

void T3DModel::clear() {
	faces.clear();
}
