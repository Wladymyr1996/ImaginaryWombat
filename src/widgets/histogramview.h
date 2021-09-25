#pragma once

#include <QWidget>
#include <QPolygon>
#include "procesor/imagehandler.h"

class THistogramView : public QWidget {
	Q_OBJECT

	public:
		explicit THistogramView(QWidget *parent = nullptr);

	protected:
		void paintEvent(QPaintEvent *);
		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *);
		void mouseMoveEvent(QMouseEvent *event);


	private slots:
		void imageOpened();
		void imageClosed();
        void histogramChanged(int blackLevel, int whiteLevel);

	private:
		void calculatePointersPos();
		void calculatelevels();

		TImageHandler *imageHandler = TImageHandler::GetInstance();

		uchar *histogramMap;
		int blackLevel = 0;
		int whiteLevel = 255;
		int blackLevelX;
		int whiteLevelX;

		int histogramWidth;

		QPolygon pointer;

		bool movingWhite = false;
		bool movingBlack = false;
		int differenceX;
};

