#pragma once

#include <QWidget>
#include "procesor/imagehandler.h"
#include "procesor/settingshandler.h"

class TImageView : public QWidget {
	Q_OBJECT
	public:
		explicit TImageView(QWidget *parent = nullptr);

	public slots:
		void imageOpen();
		void imageClose();

	protected:
		void paintEvent(QPaintEvent *);
		void resizeEvent(QResizeEvent *);
		void mouseMoveEvent(QMouseEvent *event);
		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);

	private:
		void calculateImageFrame();
		void calculateCutRectFrame();

		void setNewCutRectFrame(QRect &rect);

		TImageHandler *imageHandler = TImageHandler::GetInstance();
		TSettingsHandler *settingsHandler = TSettingsHandler::GetInstance();

		QString strNoImage = "Image not opened";
		QRect imgFrame;
		QRect cutRectFrame;
		double scale;

		bool isCutRectMoving = false;
		bool isCutRectResizing = false;
		QPoint pBeginPoint;
		QRect oldCutRectFrame;

		QColor highliteColor, highliteColorAlpha;
};

