#include "imageview.h"
#include <QApplication>
#include <QMouseEvent>
#include <QPainterPath>
#include <QPainter>
#include <QBrush>
#include <QRect>
#include <QColor>
#include <QDebug>

TImageView::TImageView(QWidget *parent) : QWidget(parent) {
	setMouseTracking(true);

	highliteColor = QApplication::palette().color(QPalette::Highlight);
	highliteColorAlpha = highliteColor;
	highliteColorAlpha.setAlpha(100);

	connect(imageHandler, &TImageHandler::isOpened, this, &TImageView::imageOpen);
	connect(imageHandler, &TImageHandler::isClosed, this, &TImageView::imageClose);
	connect(imageHandler, &TImageHandler::isUpdated, this, static_cast<void (TImageView::*)()>(&TImageView::repaint));
}

void TImageView::imageOpen() {
	if (!imageHandler->imageIsOpened())
		return;

	calculateImageFrame();
	repaint();
}

void TImageView::imageClose() {
	repaint();
}

void TImageView::paintEvent(QPaintEvent *) {
	QPainter painter(this);

	QRect frame(0, 0, width()-1, height()-1);

	painter.setPen(Qt::black);
	painter.setBrush(QBrush(Qt::gray));
	painter.drawRect(frame);

	if (imageHandler->imageIsOpened()) {
		const QImage &img = imageHandler->getImage();

		painter.drawImage(imgFrame, img);

		QPainterPath frameRect;
		frameRect.addRect(imgFrame);

		QPainterPath cutRect;
		qreal radius;
		switch (settingsHandler->GetProgramSettings().cutRectType) {
			case TSettingsHandler::RoundedSquare:
				radius = cutRectFrame.size().width() / 10;
				cutRect.addRoundedRect(cutRectFrame, radius, radius);
				break;
			case TSettingsHandler::Square:
			default:
				cutRect.addRect(cutRectFrame);
				break;
		}

		painter.setPen(highliteColor);
		painter.setBrush(QBrush(highliteColorAlpha));
		painter.drawPath(frameRect.subtracted(cutRect));

		painter.setPen(Qt::black);
		painter.setBrush(QBrush(QColor(0, 0, 0, 0)));
		painter.drawRect(imgFrame);
	} else {
		painter.setRenderHint(QPainter::Antialiasing);

		painter.drawLine(0, 0, frame.width(), frame.height());
		painter.drawLine(0, frame.height(), frame.width(), 0);

		QFontMetrics fm(painter.font());
		QSize lblSize(fm.horizontalAdvance(strNoImage)+1, fm.height());

		QRect lblFrame;
		lblFrame.setTopLeft(frame.center() - QPoint(lblSize.width(), lblSize.height()) / 2);
		lblFrame.setSize(lblSize);

		QRect rectFrame;
		rectFrame.setTopLeft(lblFrame.topLeft() - QPoint(5, 5));
		rectFrame.setSize(lblFrame.size() + QSize(10, 10));

		painter.setBrush(QBrush(QColor(200, 100, 100)));
		painter.drawRoundedRect(rectFrame, 5, 5);
		painter.setBrush(Qt::white);
		painter.setPen(Qt::white);
		painter.drawText(lblFrame, strNoImage);
	}

	painter.end();
}

void TImageView::resizeEvent(QResizeEvent *) {
	calculateImageFrame();
}

void TImageView::mouseMoveEvent(QMouseEvent *event) {
	if (!imageHandler->imageIsOpened()) {
		setCursor(Qt::ArrowCursor);
		return;
	}

	if (!isCutRectMoving && !isCutRectResizing){
		if (QRect(cutRectFrame.bottomRight() - QPoint(10, 10), QSize(20, 20)).contains(event->pos())) {
			setCursor(Qt::SizeFDiagCursor);
		} else if (cutRectFrame.contains(event->pos())){
			setCursor(Qt::OpenHandCursor);
		} else {
			setCursor(Qt::ArrowCursor);
		}
	} else {
		QPoint difference = (pBeginPoint - event->pos());

		if (isCutRectMoving) {
			QRect newCutRectFrame = QRect(oldCutRectFrame.topLeft() - difference, oldCutRectFrame.size());
			if (newCutRectFrame.x() < imgFrame.x())
				newCutRectFrame.translate(imgFrame.x()-newCutRectFrame.x(), 0);
			if (newCutRectFrame.y() < imgFrame.y())
				newCutRectFrame.translate(0, imgFrame.y()-newCutRectFrame.y());
			if (newCutRectFrame.right() > imgFrame.right())
				newCutRectFrame.translate(imgFrame.right()-newCutRectFrame.right(), 0);
			if (newCutRectFrame.bottom() > imgFrame.bottom())
				newCutRectFrame.translate(0, imgFrame.bottom()-newCutRectFrame.bottom());

			setNewCutRectFrame(newCutRectFrame);
			setCursor(Qt::ClosedHandCursor);
		} else if (isCutRectResizing) {
			int value;

			if (difference.x() > difference.y())
				value = oldCutRectFrame.width() - difference.x();
			else
				value = oldCutRectFrame.width() - difference.y();

			if (value < imgFrame.width() / 5)
				value = imgFrame.width() / 5;
			if (value < imgFrame.height() / 5)
				value = imgFrame.height() / 5;
			if (oldCutRectFrame.x() + value > imgFrame.right())
				value = imgFrame.right() - oldCutRectFrame.x();
			if (oldCutRectFrame.y() + value > imgFrame.bottom())
				value = imgFrame.bottom() - oldCutRectFrame.y();

			QRect newCutRectFrame = QRect(oldCutRectFrame.topLeft(), QSize(value, value));
			setNewCutRectFrame(newCutRectFrame);
			setCursor(Qt::SizeFDiagCursor);
		}
	}
}

void TImageView::mousePressEvent(QMouseEvent *event) {
	if (!imageHandler->imageIsOpened())
		return;

	if (QRect(cutRectFrame.bottomRight() - QPoint(10, 10), QSize(20, 20)).contains(event->pos())) {
		isCutRectResizing = true;
		pBeginPoint = event->pos();
		oldCutRectFrame = cutRectFrame;
	} else if (cutRectFrame.contains(event->pos())) {
		isCutRectMoving = true;
		pBeginPoint = event->pos();
		oldCutRectFrame = cutRectFrame;
	}

	mouseMoveEvent(event);
}

void TImageView::mouseReleaseEvent(QMouseEvent *event) {
	if (!imageHandler->imageIsOpened())
		return;

	if (isCutRectMoving) {

		isCutRectMoving = false;
	} else if (isCutRectResizing) {

		isCutRectResizing = false;
	}

	mouseMoveEvent(event);
}

void TImageView::calculateImageFrame() {
	if (!imageHandler->imageIsOpened())
		return;

	QSize frameSize(width()-1, height()-1);
	QSize imgSize = imageHandler->getImage().size();

	float frameRatio = (float)frameSize.width() / (float)frameSize.height();
	float imgRatio = (float)imgSize.width() / (float)imgSize.height();

	if (frameRatio > imgRatio) {
		scale = (float)frameSize.height() / (float)imgSize.height();
		imgSize *= scale;

		imgFrame.setX((frameSize.width() - imgSize.width()) / 2);
		imgFrame.setY(0);
	} else {
		scale = (float)frameSize.width() / (float)imgSize.width();
		imgSize *= scale;

		imgFrame.setX(0);
		imgFrame.setY((frameSize.height() - imgSize.height()) / 2);
	}

	imgFrame.setSize(imgSize);
	calculateCutRectFrame();
}

void TImageView::calculateCutRectFrame() {
	QRect cutRect = imageHandler->getCutRect();
	cutRectFrame.setTopLeft(cutRect.topLeft() * scale);
	cutRectFrame.setSize(cutRect.size() * scale);

	cutRectFrame.translate(imgFrame.topLeft());
}

void TImageView::setNewCutRectFrame(QRect &rect) {
	QRect cutRect;
	rect.translate(-imgFrame.topLeft());
	cutRect.setX(rect.x() / scale);
	cutRect.setY(rect.y() / scale);
	cutRect.setSize(rect.size() / scale);
	imageHandler->setCutRect(cutRect);

	calculateCutRectFrame();
	repaint();
}