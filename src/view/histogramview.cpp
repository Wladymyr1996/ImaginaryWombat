#include "histogramview.h"
#include <QMouseEvent>
#include <QPainter>
#include <QApplication>
#include <QDebug>

THistogramView::THistogramView(QWidget *parent) : QWidget(parent) {
	setFixedHeight(110);
	setMouseTracking(true);

	histogramMap = imageHandler->getHistogramMapPtr();
	connect(imageHandler, &TImageHandler::isOpened, this, &THistogramView::imageOpened);
	connect(imageHandler, &TImageHandler::isClosed, this, &THistogramView::imageClosed);

	pointer.push_back(QPoint(0, 0));
	pointer.push_back(QPoint(3, 6));
	pointer.push_back(QPoint(-3, 6));
}

void THistogramView::paintEvent(QPaintEvent *) {
	const QColor &clBg = palette().color(QPalette::Base);
	const QColor &clColumn = palette().color(QPalette::Active, QPalette::Text);
	const QColor &clColumnInactive = palette().color(QPalette::Disabled, QPalette::Text);

	QPainter painter(this);
	painter.setPen(palette().color(QPalette::ColorRole::Midlight));
	painter.setBrush(QBrush(clBg));

	painter.drawRect(3, 0, width() - 9, 100);


	if (imageHandler->imageIsOpened()) {
		int currentIndex = 0;
		for (int i = 0; i < histogramWidth; i++) {
			int max = 0;

			for (; currentIndex < i * 255 / histogramWidth; currentIndex++)
				if (max < histogramMap[currentIndex])
					max = histogramMap[currentIndex];

			int x = i + 3;
			if (x < blackLevelX || x > whiteLevelX)
				painter.setPen(clColumnInactive);
			else
				painter.setPen(clColumn);

			painter.drawLine(x, 100, x, 100 - (max * 100) / 256);
		}

		painter.setPen(palette().color(QPalette::ColorRole::Midlight));
		painter.setBrush(QBrush(Qt::black));
		painter.drawPolygon(pointer.translated(blackLevelX, 102));
		painter.setBrush(QBrush(Qt::white));
		painter.drawPolygon(pointer.translated(whiteLevelX, 102));
	} else {

	}

	if (!isEnabled()) {
		QColor color = QApplication::palette().color(QPalette::Inactive, QPalette::Window);
		color.setAlpha(200);

		painter.setBrush(QBrush(color));
		painter.drawRect(-1, -1, width()+1, height() + 1);
	}

	painter.end();
}

void THistogramView::mousePressEvent(QMouseEvent *event) {
	int x = event->pos().x();

	if (x > blackLevelX - 3 && x < blackLevelX + 3) {
		differenceX = x - blackLevelX;
		movingBlack = true;
	}

	if (x > whiteLevelX - 3 && x < whiteLevelX + 3) {
		differenceX = x - whiteLevelX;
		movingWhite = true;
	}
}

void THistogramView::mouseReleaseEvent(QMouseEvent *) {
	if (movingBlack || movingWhite) {
		calculatelevels();
		imageHandler->setHistagramLevels(blackLevel, whiteLevel);

		movingBlack = false;
		movingWhite = false;
	}
}

void THistogramView::mouseMoveEvent(QMouseEvent *event) {
	if (!imageHandler->imageIsOpened()) {
		setCursor(Qt::ArrowCursor);
		return;
	}

	if (movingWhite) {
		whiteLevelX = event->pos().x() - differenceX;
		if (whiteLevelX < blackLevelX + 6)
			whiteLevelX = blackLevelX + 6;

		if (whiteLevelX > histogramWidth + 3)
			whiteLevelX = histogramWidth + 3;

		setCursor(Qt::SizeHorCursor);
		repaint();
	} else if (movingBlack) {
		blackLevelX = event->pos().x() - differenceX;
		if (blackLevelX > whiteLevelX - 6)
			blackLevelX = whiteLevelX - 6;

		if (blackLevelX < 3)
			blackLevelX = 3;

		setCursor(Qt::SizeHorCursor);
		repaint();
	} else {
		int x = event->pos().x();

		if ((x > blackLevelX - 3 && x < blackLevelX + 3) ||
			(x > whiteLevelX - 3 && x < whiteLevelX + 3))

			setCursor(Qt::SizeHorCursor);
		else
			setCursor(Qt::ArrowCursor);
	}
}

void THistogramView::imageOpened() {
	histogramWidth = width() - 9; // minus left/right borders;
	blackLevel = 0;
	whiteLevel = 255;

	calculatePointersPos();
	repaint();
}

void THistogramView::imageClosed() {
	repaint();
}

void THistogramView::calculatePointersPos() {
	blackLevelX = (blackLevel * histogramWidth / 255) + 3;
	whiteLevelX = (whiteLevel * histogramWidth / 255) + 3;
}

void THistogramView::calculatelevels() {
	blackLevel = (blackLevelX - 3) * 255 / histogramWidth;
	whiteLevel = (whiteLevelX - 3) * 255 / histogramWidth;
}
