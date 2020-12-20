#include "stlhandler.h"
#include <QtMath>
#include <QDebug>

TStlHandler::TStlHandler() {
	QObject(nullptr);
}

TStlHandler *TStlHandler::getInstance() {
	static TStlHandler instance;

	return &instance;
}

const QByteArray &TStlHandler::getStl(QString name, uchar *highMap) {
	model.setName(name);
	generateModel(model, highMap);
	generateStlFileData(model, fileData);

	return fileData;
}

void TStlHandler::generateModel(T3DModel &model, uchar *highMap) {
	float base = settingsHandler->GetPrinterSettings().baseThickness;
	float hight = settingsHandler->GetPrinterSettings().fullThickness;
	float range = hight - base;

	if (range <= 0.01f) {
		qCritical() << "Range can't be less 0.1. Range is: Full thickness - Base thickness = " << range;

		return;
	}

	float coef = range / 8;

	model.clear();

	// generation of image surface
	for (int i = 0; i < 250 - 1; i++)
		for (int j = 0; j < 250 - 1; j++) {
			TPoint p[3];

			p[0].x = j;		p[0].y = i;		p[0].z = calculateZ(highMap[i * 250 + j], hight, coef);
			p[1].x = j;		p[1].y = i + 1;	p[1].z = calculateZ(highMap[(i + 1) * 250 + j], hight, coef);
			p[2].x = j + 1;	p[2].y = i + 1;	p[2].z = calculateZ(highMap[(i + 1) * 250 + j + 1], hight, coef);

			for (int pi = 0; pi < 3; pi++)
				for (int pj = 0; pj < 2; pj++)
					p[pi][pj] *= 0.4f;

			model.addFace(p[2], p[1], p[0]);

			p[1].x = j + 1;	p[1].y = i;		p[1].z = calculateZ(highMap[i * 250 + j + 1], hight, coef);

			for (int pj = 0; pj < 2; pj++)
				p[1][pj] *= 0.4f;

			model.addFace(p[0], p[1], p[2]);
		}
}

double TStlHandler::calculateZ(int value, float hight, float coef) {
	value += 1;
	float val = log2(value);
	float ret = hight - val * coef;

	return ret;
}

void TStlHandler::generateStlFileData(const T3DModel &model, QByteArray &data) {
	data.push_back(QString("solid " + model.getName() + "\n\n").toUtf8());

	for (auto &face : model.getFaces()) {
		data.push_back(QString("facet normal 0 0 0\n").toUtf8());
		data.push_back(QString("outer loop\n").toUtf8());

		for (auto &point : face.point) {
			QString coord;

			for (int i = 0; i < 3; i++)
				coord += QString::number(point[i]) + " ";

			data.push_back(QString("vertex " + coord + "\n").toUtf8());
		}

		data.push_back(QString("endloop\n").toUtf8());
		data.push_back(QString("endfacet\n\n").toUtf8());
	}

	data.push_back(QString("endsolid " + model.getName() + "\n\n").toUtf8());
}

