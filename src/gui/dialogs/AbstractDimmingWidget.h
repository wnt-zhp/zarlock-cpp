/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  Rafał Lalik <rafal.lalik@ph.tum.de>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef ABSTRACTDIMMINGWIDGET_H
#define ABSTRACTDIMMINGWIDGET_H

#include <QWidget>
#include <QGraphicsDropShadowEffect>
#include <QTimeLine>
#include <QDate>
#include <qcoreevent.h>

class AbstractDimmingWidget : public QWidget {
Q_OBJECT
public:
	explicit AbstractDimmingWidget(QWidget * overlay = NULL, QWidget* parent = 0, Qt::WindowFlags f = 0);
	virtual ~AbstractDimmingWidget();

	enum PTextPosition { TOPLEFT, TOPCENTER, TOPRIGHT, MIDLEFT, MIDCENTER, MIDRIGHT, BOTLEFT, BOTCENTER, BOTRIGHT, CURRENT };
	enum PState { IN, OUT, FINISHED };
	enum PCallbacksActions { CALL_UPDATE, CALL_SETFRAME, CALL_STOPFRAME };

	void setOverlay(bool animated = false, bool use_style = true);
	void setShadow(int offset = 3, int radius = 9, QColor color = Qt::black);
	void disableOverlay();

	void setAnimationMove(PTextPosition start, PTextPosition pause, PTextPosition stop);
	void setAnimationCurve(QEasingCurve curve);
	void setDuration(int duration = 2000, int animate_duration = 1000);
	void setOverlayOpacity(int opacity = 200);
// 	virtual void resizeEvent(QResizeEvent * event);

	virtual void go();
	virtual void og();

protected slots:
	virtual void setVisible(bool visible);
	virtual void setHidden(bool visible);
	virtual void show();
	virtual void hide();

private:
	QTimeLine * animate();
	QTimeLine * animate(QTimeLine * start_after);

	void updateParentResizeEvent();

	void disable_parent();

	void setOverlayClickMethod(/*method*/);

private slots:
	void finalize();

protected slots:
// 	void runCallBacks(PState state = FINISHED);
	void setOverlayStyle(int opacity);
	void moveFrame(int frame);
// 	void setMeHidden();

protected:
	int duration;

private:
	QWidget * overlay;
	QWidget * parent_widget;

	bool init_finished;

	bool overlay_animated;
	bool overlay_enabled;
	bool overlay_styled;
	int overlay_opacity;
	bool overlay_just_resize;
	bool overlay_dont_animate;
	
	PState animate_direction;
	PState last_state;
	QTimeLine::Direction animate_move_direction;
	PTextPosition animate_start;
	PTextPosition animate_pause;
	PTextPosition animate_stop;
	int animate_duration;
	QEasingCurve animation;

	QTimeLine * sceneX;
	QTimeLine * sceneY;
	QTimeLine * sceneF;

	QGraphicsDropShadowEffect * effect;
};

#endif // ABSTRACTDIMMINGWIDGET_H
