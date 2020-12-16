#include "imagehandler.h"
#include <QDebug>

TImageHandler *TImageHandler::GetInstance() {
	static TImageHandler instance;

	return &instance;
}

const QImage &TImageHandler::getImage() {
	return tmpImage;
}

QRect TImageHandler::getCutRect() {
	return cutRect;
}

void TImageHandler::setCutRect(const QRect &rect) {
	cutRect = rect;
}

void TImageHandler::setHistagramLevels(int blackLvl, int whiteLvl) {
	blackLevel = blackLvl;
	whiteLevel = whiteLvl;

	updateImage();
}

uchar *TImageHandler::getHistogramMapPtr() {
	return histogramMap;
}

void TImageHandler::getHighmap(uchar *map, int widthMap, int heightMap) {
	if (!imageIsOpened())
		return;

	QImage img = tmpImage.copy(cutRect).scaled(widthMap, heightMap, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	int depth = 4;
	for (int i = 0; i < heightMap; i++) {
		uchar *line = img.scanLine(i);

		for (int j = 0; j < widthMap; j++) {
			QRgb *pixel = reinterpret_cast<QRgb *>(line + j * depth);
			map[i * widthMap + j] = qRed(*pixel);
		}
	}
}

int TImageHandler::openImage(QString filename) {
	IMG_ERROR ret = Ok;

	if (origImage.load(filename)) {
		if (origImage.width() < 512 || origImage.height() < 512)
			ret = SizeError;

		if (origImage.width() / origImage.height() > 3 || origImage.height() / origImage.width() > 3)
			ret = RatioError;

		if (ret == Ok) {
			opened = true;
			cutRect.setTopLeft(QPoint(0, 0));
			cutRect.setSize(origImage.size());

			if (cutRect.width() > cutRect.height())
				cutRect.setWidth(cutRect.height());
			else
				cutRect.setHeight(cutRect.width());

			QPoint p = origImage.rect().center() - cutRect.center();
			cutRect.translate(p);
			origImage.convertTo(QImage::Format_ARGB32);

			convertToGray();
			calculateHistogramMap();
			tmpImage = origImage;

			emit isOpened();
			return ret;
		}
	} else
		ret = ParseError;

	if (opened) {
		closeImage();
	}

	return ret;
}

bool TImageHandler::imageIsOpened() {
	return opened;
}

void TImageHandler::closeImage() {
	tmpImage = QImage();
	opened = false;

	emit isClosed();
}

TImageHandler::TImageHandler() {

}

void TImageHandler::convertToGray() {
	int depth = 4;
	for (int i = 0; i < origImage.height(); i++) {
		uchar *line = origImage.scanLine(i);

		for (int j = 0; j < origImage.width(); j++) {
			QRgb *pixel = reinterpret_cast<QRgb *>(line + j * depth);
			int gray = qGray(*pixel);
			*pixel = QColor(gray, gray, gray).rgba();
		}
	}
}

void TImageHandler::calculateHistogramMap() {
	if (!imageIsOpened()) return;

	qint64 map[256] = {0};

	int depth = 4;
	for (int i = 0; i < origImage.height(); i++) {
		uchar *line = origImage.scanLine(i);

		for (int j = 0; j < origImage.width(); j++) {
			QRgb *pixel = reinterpret_cast<QRgb *>(line + j * depth);
			map[qRed(*pixel)]++;
		}
	}

	qint64 max = 0;
	for (int i = 0; i < 256; i++) {
		if (map[i] > max)
			max = map[i];
	}

	for (int i = 0; i < 256; i++) {
		histogramMap[i] = (map[i] * 255) / max;
		qDebug() << "histogramMap[" << i << "] = " << histogramMap[i] << "; map[" << i << "] = " << map[i] << "; max = " << max;
	}
}

void TImageHandler::updateImage() {
	if (!imageIsOpened())
		return;

	int depth = 4;
	int range = whiteLevel - blackLevel;

	qDebug() << "white: " << whiteLevel << "; black: " << blackLevel;

	for (int i = 0; i < origImage.height(); i++) {
		uchar *origLine = origImage.scanLine(i);
		uchar *tmpLine = tmpImage.scanLine(i);

		for (int j = 0; j < origImage.width(); j++) {
			QRgb *origPixel = reinterpret_cast<QRgb *>(origLine + j * depth);
			int gray = qRed((*origPixel));
			int newGray = (gray - blackLevel) * 255 / range;
			if (newGray < 0)
				newGray = 0;
			if (newGray > 255)
				newGray = 255;

			QRgb *tmpPixel = reinterpret_cast<QRgb *>(tmpLine + j * depth);
			*tmpPixel = QColor(newGray, newGray, newGray).rgba();
		}
	}

	emit isUpdated();
}
