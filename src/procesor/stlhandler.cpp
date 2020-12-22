#include "stlhandler.h"
#include <QtMath>
#include <QDebug>
#include <QPainterPath>
#include <QElapsedTimer>
#include <QFile>

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

	QPainterPath cutRegion;
	QRect region(1, 1, 249, 249);
	switch (settingsHandler->GetProgramSettings().cutRectType) {
		case TSettingsHandler::TCutRectType::RoundedSquare:
			cutRegion.addRoundedRect(region, 25, 25);
			break;
		case TSettingsHandler::TCutRectType::Square:
		default:
			cutRegion.addRect(region);
			break;
	}

	QPoint path[4] = {{0, 0}, {0, 1}, {1, 1}, {1, 0}};

	// calculate hightMap
	float calculatedHightMap[251][251];
	for (int i = 0; i < 251; i++)
		for (int j = 0; j < 251; j++) {
			calculatedHightMap[j][i] = cutRegion.contains(QPointF(j, i)) ? calculateZ(highMap[i * 250 + j], hight, coef) : base;
		}

	// generation of image surface
	for (int i = 0; i < 250; i++)
		for (int j = 0; j < 250; j++) {
			QPoint point(j, i);
			TPoint p[4];

			for (int k = 0; k < 4; k ++) {
				QPoint vertex = point + path[k];
				p[k] = QPointF(vertex) / 2.5f;
				p[k].y = 100.0f - p[k].y;
				p[k].z = calculatedHightMap[vertex.x()][vertex.y()];
			}

			model.addFace(p[2], p[0], p[1]);
			model.addFace(p[0], p[2], p[3]);
		}

	//generate a border
	QFile borderFile("://borders/1.bin");
	borderFile.open(QFile::ReadOnly);
	QByteArray border = borderFile.readAll();
	borderFile.close();

	if (border.size() % 12)
		qCritical() << borderFile.fileName() << "is invalid!";

	char *data = border.data();
	for (int i = 0; i < border.size() / 36; i++) {
		TPoint p[3];
		for (int j = 0; j < 3; j++)
			for (int k = 0; k < 3; k++) {
				unsigned coord = *(reinterpret_cast<unsigned *>(data + i * 36 + j * 12 + k * 4));
				if (coord == 0xFFFFFFFF)
					coord = *(reinterpret_cast<unsigned *>(&base));
				p[j][k] = *(reinterpret_cast<float *>(&coord));
			}
		model.addFace(p, 3);
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

