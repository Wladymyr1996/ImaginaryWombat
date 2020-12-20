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
	data.clear();

	// Generate name to first 80 ASCII bytes
	QByteArray header;
	// Header can't contain word "solid" in 0 index
	if (data.indexOf("solid") != 0) {
		header.push_back(model.getName().toUtf8());
		if (header.length() > 80)
			header.remove(80, header.length() - 80);
	}
	if (header.length() < 80)
		header.push_back(QByteArray(80-header.length(), 0));

	// Insert header to data
	data.push_back(header);

	// Number of the triangle faces
	uint32_t count = model.getFaces().count();
	data.append(reinterpret_cast<char *>(&count), sizeof(count));

	// Check data sizes
	if (sizeof(TPoint::x) != 4) {
		qCritical() << "sizeOf(TPoint::*) ≠ 4! Bad day bad day bad day!";
		return;
	}

	// Pushing all faces
	for (auto &face : model.getFaces()) {
		// Add 3 coords 4 bytes to 0 (Empty normal) [12 bytes]
		data.append(3 * 4, 0);

		// All coords of all points
		for (auto point : face.point) {
			for (int j = 0; j < 3; j++) {
				data.append(reinterpret_cast<char *>(&(point[j])), sizeof(point[j]));
			}
		}

		// Attribute byte count 0 [2 bytes]
		data.append(2, 0);
	}


	/* Generating ASCII format of STL (OLD)
	 *
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
	*/
}

