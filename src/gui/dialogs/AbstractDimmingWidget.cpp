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

#include "globals.h"

#include <QContextMenuEvent>

#include "AbstractDimmingWidget.h"
#include "EventFilter.h"

AbstractDimmingWidget::AbstractDimmingWidget(QWidget* parent, Qt::WindowFlags f): QWidget(parent, f),
	duration(2000), overlay_widget(NULL), parent_widget(parent),
	overlay_enabled(false), overlay_animated(true), overlay_styled(true),
	overlay_opacity(200), overlay_just_resize(false), overlay_dont_animate(false),
	animate_direction(FINISHED), last_state(UNDEFINDE), animate_move_direction(QTimeLine::Forward),
	animate_start(TOPCENTER), animate_pause(MIDCENTER), animate_stop(BOTCENTER),
	animate_duration(1400),  disable_parent_in_shown(false),
	sceneX(new QTimeLine), sceneY(new QTimeLine), sceneF(new QTimeLine),
	delayed(NULL), effect(NULL),
	evfilter(NULL)
{
	this->hide();

	setEnabled(true);

	setOverlayDefaultOpacity(overlay_opacity);
	setAnimationCurve(animation);
	setDuration(duration, animate_duration);

	// Finalize
	connect(sceneX, SIGNAL(finished()), this, SLOT(finalize()));

	connect(sceneX, SIGNAL(frameChanged(int)), this, SLOT(moveFrame(int)));
	connect(sceneY, SIGNAL(frameChanged(int)), this, SLOT(moveFrame(int)));
	connect(sceneF, SIGNAL(frameChanged(int)), this, SLOT(setOverlayOpacity(int)));

	evfilter = new EventFilter;
	parent_widget->installEventFilter(evfilter);
	evfilter->registerFilter(QEvent::Resize);

	connect(evfilter, SIGNAL(eventFiltered(QEvent*)), this, SLOT(eventCaptured(QEvent*)));
}

AbstractDimmingWidget::~AbstractDimmingWidget() {
	disconnect(evfilter, SIGNAL(eventFiltered(QEvent*)), this, SLOT(eventCaptured(QEvent*)));
	parent_widget->removeEventFilter(evfilter);

	delete sceneX;
	delete sceneY;
	delete sceneF;

	delete effect;
}

// void AbstractDimmingWidget::resizeEvent(QResizeEvent* event) {
// 	// FIXME: what to do with tris resize?
// // 	if (overlay_enabled) {
// // 		overlay->resize(parent_widget->size());
// // 		if (isVisible() or last_direction == IN)
// // 			animate(last_direction, last_move_direction, CURRENT, last_stop, duration, true);
// // 	}
// 	QWidget::resizeEvent(event);
// }

void AbstractDimmingWidget::setAnimationMove(AbstractDimmingWidget::PTextPosition start, AbstractDimmingWidget::PTextPosition pause, AbstractDimmingWidget::PTextPosition stop) {
	animate_start = start;
	animate_pause = pause;
	animate_stop = stop;
}

void AbstractDimmingWidget::setOverlayWidget(QWidget* widget) {
	if (!widget)
		return;

	if (overlay_widget)
		overlay_widget->setStyleSheet(overlay_widget_stylesheet);

	overlay_widget_stylesheet = widget->styleSheet();

	overlay_widget = widget;
	overlay_widget->hide();
	overlay_widget->stackUnder(this);
	overlay_enabled = true;

	setOverlayOpacity(0);

	prepareOverlay();
}

QWidget* AbstractDimmingWidget::overlayWidget() {
	return overlay_widget;
}

void AbstractDimmingWidget::enableOverlay() {
	if (overlay_widget)
		overlay_enabled = true;
	else
		overlay_enabled = false;
}

void AbstractDimmingWidget::disableOverlay() {
	overlay_enabled = false;
}

bool AbstractDimmingWidget::isEnabled() {
	return overlay_enabled;
}

void AbstractDimmingWidget::setOverlayAnimated(bool animated) {
	// Resize the overlay with parent's size
	overlay_animated = animated;
	prepareOverlay();
}

bool AbstractDimmingWidget::overlayAnimated() {
	return overlay_animated;
}

void AbstractDimmingWidget::setOverlayStyled(bool use_style) {
	// Resize the overlay with parent's size
	overlay_styled = use_style;
	prepareOverlay();
}

bool AbstractDimmingWidget::overlayStyled() {
	return overlay_styled;
}

void AbstractDimmingWidget::setOverlayOpacity(int opacity) {
	if (!overlay_widget)
		return;

	overlay_widget->setStyleSheet(QString("QWidget { background-color: rgba(0, 0, 0, %1) }").arg(opacity));
}

void AbstractDimmingWidget::prepareOverlay() {
	if (!overlay_widget)
		return;

	if (overlay_animated)
		sceneF->setUpdateInterval(20);

	if (overlay_styled) {
		if (overlay_animated) {
			// Register animation range for overlay fade-in/out effect
		} else {
			// Set overlay opacity
			setOverlayOpacity(overlay_opacity);
		}
	} else {
		overlay_widget->setAutoFillBackground(true);
	}
}

void AbstractDimmingWidget::animate() {
	if (overlay_widget)
		overlay_widget->resize(parent_widget->size());

	// Stop all running animations
	sceneX->stop();
	sceneY->stop();
	sceneF->stop();

	if (animate_direction == IN)
		animate_move_direction = QTimeLine::Forward;
	else
	if (animate_direction == OUT)
		animate_move_direction = QTimeLine::Backward;

	// Set X coordinate timeline settings
	sceneX->setDirection(animate_move_direction);
	sceneX->setUpdateInterval(20);

	// Set Y coordinate timeline settings
	sceneY->setDirection(animate_move_direction);
	sceneY->setUpdateInterval(20);

	sceneF->setDirection(animate_move_direction);
// 	sceneF->setUpdateInterval(20);

	// Get current sizes
	int p_width = parent_widget->width();
	int p_height = parent_widget->height();
	int _width  = this->width();
	int _height = this->height();
	int curr_x = this->x();
	int curr_y = this->y();

	// Calculate new positions for given points
	int limits[CURRENT+1][2] = {
		{ 0, 0 }, { p_width/2 - _width/2, 0 }, { p_width - _width, 0 },
		{ 0, p_height/2 - _height/2 }, { p_width/2 - _width/2, p_height/2 - _height/2 }, { p_width - _width, p_height/2 - _height/2 },
		{ 0, p_height - _height }, { p_width/2 - _width/2, p_height - _height }, { p_width - _width, p_height - _height },
		{ curr_x, curr_y }
	};

	// Update start and stop positions for given direction
	if (animate_direction == IN) {
		this->show();
		switch (animate_start) {
			case TOPLEFT:
			case MIDLEFT:
			case BOTLEFT:
				limits[animate_start][0] -= _width;
				break;
			case TOPRIGHT:
			case MIDRIGHT:
			case BOTRIGHT:
				limits[animate_start][0] += _width;
				break;
			case TOPCENTER:
				limits[animate_start][1] -= _height;
				break;
			case BOTCENTER:
				limits[animate_start][1] += _height;
				break;
			default:
				break;
		}
		move(limits[animate_start][0], limits[animate_start][1]);

		// Set calculated start and stop positions
		sceneX->setFrameRange(limits[animate_start][0], limits[animate_pause][0]);
		sceneY->setFrameRange(limits[animate_start][1], limits[animate_pause][1]);
	} else
	if (animate_direction == OUT) {
		switch (animate_stop) {
			case TOPLEFT:
			case MIDLEFT:
			case BOTLEFT:
				limits[animate_stop][0] -= _width;
				break;
			case TOPRIGHT:
			case MIDRIGHT:
			case BOTRIGHT:
				limits[animate_stop][0] += _width;
				break;
			case TOPCENTER:
				limits[animate_stop][1] -= _height;
				break;
			case BOTCENTER:
				limits[animate_stop][1] += _height;
				break;
			default:
				break;
		}
		move(limits[animate_pause][0], limits[animate_pause][1]);

		// Set calculated start and stop positions
		sceneX->setFrameRange(limits[animate_stop][0], limits[animate_pause][0]);
		sceneY->setFrameRange(limits[animate_stop][1], limits[animate_pause][1]);
	} else {
// 		move(limits[animate_start][0], limits[animate_start][1]);

		// Set calculated start and stop positions
		sceneX->setFrameRange(curr_x, limits[animate_pause][0]);
		sceneY->setFrameRange(curr_y, limits[animate_pause][1]);
	}

	// Move the widget to calculated start position

	// Run predefined callback functions for given direction
// 	runCallBacks(animate_direction);
	disableParent();

	if (overlay_widget) {
		// Show/hide overlay if overlay enabled
		overlay_widget->setVisible(overlay_enabled);

		if (overlay_dont_animate) {
			overlay_widget->setHidden(animate_direction == OUT);
			setHidden(animate_direction == OUT);
// 			runCallBacks(FINISHED);
		} else {
			// Start the animation!
			if (sceneX->state() == QTimeLine::NotRunning) {

				sceneX->start();
				sceneY->start();
				if (!overlay_just_resize) {
					// The animation will just work for repositioning the widget,
					// so we dont need overlay fade animation
					sceneF->start();
					overlay_just_resize = false;
				}
			}
		}
	}
}

void AbstractDimmingWidget::finalize() {
	FII();
// 	parentResizeEvent();

	last_state = animate_direction;
	animate_direction = FINISHED;

	if (animate_direction == FINISHED) {
// 		parentResizeEvent();
	}

	if (last_state == IN) {
		emit widgetIn();
	}
	else
		if (last_state == OUT) {
		emit widgetOut();
	}

	if (overlay_widget)
		overlay_widget->setHidden(last_state == OUT);

	this->setHidden(last_state == OUT);

	if (delayed) {
		if (delayed->state() != QTimeLine::Running) {
			animate();
			delayed = NULL;
			// 			return;
		}
	}
}

void AbstractDimmingWidget::moveFrame(int frame) {
	if (sender() == sceneX)
		this->move(frame, y());
	if (sender() == sceneY)
		this->move(x(), frame);
}

void AbstractDimmingWidget::disableParent() {
	// FIXME: WTF it is?

	if (disable_parent_in_shown) {
		for (QObjectList::const_iterator it = parent_widget->children().begin(); it != parent_widget->children().end(); ++it)
			if (*it != this and !(*it)->inherits("QLayout"))
				((QWidget *)(*it))->setEnabled(last_state != IN);
// 		parent.c
// 		for item in self.__parent.children():
// 		if not item == self and not item.inherits("QLayout"):
// 			try:
// 			item.setEnabled(not self.__last_direction == IN)
// 			except:
// 			pass
	}
}

void AbstractDimmingWidget::setAnimationCurve(QEasingCurve curve) {
	sceneX->setEasingCurve(curve);
	sceneY->setEasingCurve(curve);
}

void AbstractDimmingWidget::showWidget() {
	adjustSize();
	if (last_state != IN) {
		animate_direction = IN;
		this->show();
		animate();
	}
}

void AbstractDimmingWidget::hideWidget() {
	if (last_state != OUT) {
		animate_direction = OUT;
		animate();
	}
}

void AbstractDimmingWidget::setDuration(int duration, int animate_duration) {
	this->duration = duration;
	// Set X coordinate duration
	sceneX->setDuration(duration);
	// Set X coordinate ruration
	sceneY->setDuration(duration);
	// Update duration for overlay fade effect
	sceneF->setDuration(animate_duration);
}

void AbstractDimmingWidget::setShadow(int offset, int radius, QColor color)  {
	// Enable shadow for mainwidget with given features
	delete effect;
	effect = new QGraphicsDropShadowEffect;
	effect->setBlurRadius(radius);
	effect->setOffset(offset);
	effect->setColor(color);
	setGraphicsEffect(effect);
}

void AbstractDimmingWidget::setOverlayDefaultOpacity(int opacity) {
	overlay_opacity = opacity;
	sceneF->setFrameRange(0, opacity);
}

int AbstractDimmingWidget::overlayDefaultOpacity() {
	return overlay_opacity;
}

void AbstractDimmingWidget::setVisible(bool visible) {
	QWidget::setVisible(visible);
}

void AbstractDimmingWidget::setHidden(bool visible) {
	QWidget::setHidden(visible);
}

void AbstractDimmingWidget::show() {
	QWidget::show();
}

void AbstractDimmingWidget::hide() {
	QWidget::hide();
}

void AbstractDimmingWidget::setEventTransparent(bool transparent) {
	if (overlay_widget)
	overlay_widget->setAttribute(Qt::WA_TransparentForMouseEvents, transparent);
}

void AbstractDimmingWidget::parentResizeEvent() {
	adjustSize();

	if (this->isHidden())
		return;

	if (overlay_enabled) {
		overlay_widget->resize(parent_widget->size());
		if (this->last_state != OUT) {
			overlay_just_resize = true;
			delayed = sceneX;
			finalize();
		}
	}
}

void AbstractDimmingWidget::eventCaptured(QEvent* evt) {
	if (evt->type() == QEvent::Resize)
		parentResizeEvent();
}

void AbstractDimmingWidget::resizeEvent(QResizeEvent* event) {
	if (this->isVisible())
		animate();

	QWidget::resizeEvent(event);
}

#include "AbstractDimmingWidget.moc"