/*
    This file is part of Zarlok.

    Copyright (C) 2012  Rafał Lalik <rafal.lalik@zhp.net.pl>

    Zarlok is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Zarlok is distributed in the hope that it will be useful,
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
#include <QContextMenuEvent>
#include <QTimeLine>
#include <QDate>
#include <qcoreevent.h>

class EventFilter;

class AbstractDimmingWidget : public QWidget {
Q_OBJECT
public:
	explicit AbstractDimmingWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
	virtual ~AbstractDimmingWidget();

	enum PTextPosition { TOPLEFT, TOPCENTER, TOPRIGHT, MIDLEFT, MIDCENTER, MIDRIGHT, BOTLEFT, BOTCENTER, BOTRIGHT, CURRENT };
	enum PState { UNDEFINDE, IN, OUT, FINISHED };
	enum PCallbacksActions { CALL_UPDATE, CALL_SETFRAME, CALL_STOPFRAME };

	void setOverlayWidget(QWidget * widget);
	QWidget * overlayWidget();

	void setOverlayAnimated(bool animated);
	bool overlayAnimated();

	void setOverlayStyled(bool use_style);
	bool overlayStyled();

	void setOverlayDefaultOpacity(int opacity);
	int overlayDefaultOpacity();

	void setShadow(int offset = 3, int radius = 9, QColor color = Qt::black);

	void enableOverlay();
	void disableOverlay();

	bool isEnabled();

	void setAnimationMove(PTextPosition start, PTextPosition pause, PTextPosition stop);
	void setAnimationCurve(QEasingCurve curve);
	void setDuration(int duration = 2000, int animate_duration = 1000);

	void setEventTransparent(bool transparent = true);

	virtual void resizeEvent(QResizeEvent* );

	virtual void showWidget();
	virtual void hideWidget();

public slots:
	void setOverlayOpacity(int opacity);

protected slots:
	virtual void setVisible(bool visible);
	virtual void setHidden(bool visible);
	virtual void show();
	virtual void hide();
	void moveFrame(int frame);

private slots:
	void animate();
	void finalize();
	void parentResizeEvent();
	void eventCaptured(QEvent * evt);

signals:
	void widgetIn();
	void widgetOut();
private:
	void disableParent();

private:
	void prepareOverlay();

protected:
	int duration;

private:
	QWidget * overlay_widget;
	QWidget * parent_widget;
	bool custom_overlay;

	bool overlay_enabled;
	bool overlay_animated;
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

	bool disable_parent_in_shown;

	QTimeLine * sceneX;
	QTimeLine * sceneY;
	QTimeLine * sceneF;

	QTimeLine * delayed;

	QGraphicsDropShadowEffect * effect;
	EventFilter * evfilter;

	QString overlay_widget_stylesheet;
};

#endif // ABSTRACTDIMMINGWIDGET_H
