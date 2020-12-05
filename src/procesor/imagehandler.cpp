#include "imagehandler.h"

TImageHandler *TImageHandler::GetInstance() {
	static TImageHandler instance;

	return &instance;
}

const QImage &TImageHandler::getImage() {
	return image;
}

bool TImageHandler::openImage(QString filename) {
	if (image.load(filename)) {
		opened = true;

		emit isOpened();
		return true;
	}

	if (opened) {
		closeImage();
	}

	return false;
}

bool TImageHandler::imageIsOpened() {
	return opened;
}

void TImageHandler::closeImage() {
	image = QImage();
	opened = false;

	emit isClosed();
}

TImageHandler::TImageHandler() {

}
