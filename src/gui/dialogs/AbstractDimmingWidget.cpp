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

#include "globals.h"

#include "AbstractDimmingWidget.h"

AbstractDimmingWidget::AbstractDimmingWidget(QWidget * ov, QWidget* parent, Qt::WindowFlags f): QWidget(parent, f),
	duration(2000), overlay(ov), parent_widget(parent), init_finished(false),
	overlay_animated(true), overlay_enabled(true), overlay_styled(true),
	overlay_opacity(200), overlay_just_resize(false), overlay_dont_animate(false),
	animate_direction(FINISHED), last_state(OUT), animate_move_direction(QTimeLine::Forward),
	animate_start(TOPCENTER), animate_pause(MIDCENTER), animate_stop(BOTCENTER),
	animate_duration(1400),  /*disable_parent_in_shown(false),*/
	sceneX(new QTimeLine), sceneY(new QTimeLine), sceneF(new QTimeLine), effect(NULL)
{
	this->hide();

	overlay->setAttribute(Qt::WA_TransparentForMouseEvents);
	overlay->hide();

	setEnabled(true);
	updateParentResizeEvent();

	if (overlay_enabled)
		setOverlay(overlay_animated, overlay_styled);

	setAnimationCurve(animation);
	setDuration(duration, animate_duration);
	setOverlayOpacity();

	// Finalize
	connect(sceneX, SIGNAL(finished()), this, SLOT(finalize()));

	connect(sceneX, SIGNAL(frameChanged(int)), this, SLOT(moveFrame(int)));
	connect(sceneY, SIGNAL(frameChanged(int)), this, SLOT(moveFrame(int)));
	connect(sceneF, SIGNAL(frameChanged(int)), this, SLOT(setOverlayStyle(int)));

	init_finished = true;
}

AbstractDimmingWidget::~AbstractDimmingWidget() {
	delete sceneX;
	delete sceneY;
	delete sceneF;

	delete effect;
}

void AbstractDimmingWidget::updateParentResizeEvent() {
// 	resizeEvent(NULL);
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

void AbstractDimmingWidget::setOverlay(bool animated, bool use_style) {
	// Resize the overlay with parent's size
	overlay_enabled = true;
	overlay_animated = animated;
	overlay_styled = use_style;
	sceneF->setUpdateInterval(20);

	if (overlay_styled) {
		if (animated) {
			// Register animation range for overlay fade-in/out effect
		} else {
			// Set overlay opacity
			setOverlayStyle(overlay_opacity);
		}
	} else {
		overlay->setAutoFillBackground(true);
	}
}

void AbstractDimmingWidget::disableOverlay() {
	overlay_enabled = false;
}

void AbstractDimmingWidget::setOverlayStyle(int opacity) {
	overlay->setStyleSheet(QString("QWidget { background-color: rgba(0, 0, 0, %1) }").arg(opacity));
}

QTimeLine * AbstractDimmingWidget::animate(QTimeLine * start_after) {
	if (start_after) {
		if (start_after->state() == QTimeLine::Running) {
			disconnect(start_after, SIGNAL(finished()), this, SLOT(animate()));
			connect(start_after, SIGNAL(finished()), this, SLOT(animate()));
			return NULL;
		}
	}
	return NULL;
}

QTimeLine * AbstractDimmingWidget::animate() {
	overlay->resize(parent_widget->size());

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

	// Calculate new positions for given points
	int limits[CURRENT+1][2] = {
		{ 0, 0 }, { p_width/2 - _width/2, 0 }, { p_width - _width, 0 },
		{ 0, p_height/2 - _height/2 }, { p_width/2 - _width/2, p_height/2 - _height/2 }, { p_width - _width, p_height/2 - _height/2 },
		{ 0, p_height - _height }, { p_width/2 - _width/2, p_height - _height }, { p_width - _width, p_height - _height },
		{ this->x(), this->y() }
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
		move(limits[animate_start][0], limits[animate_start][1]);
		
		// Set calculated start and stop positions
		sceneX->setFrameRange(limits[animate_start][0], limits[animate_stop][0]);
		sceneY->setFrameRange(limits[animate_start][1], limits[animate_stop][1]);
	}

	// Move the widget to calculated start position

	// Run predefined callback functions for given direction
// 	runCallBacks(animate_direction);
// 	disable_parent();

	// Show/hide overlay if overlay enabled
	if (overlay_enabled) {
		overlay->show();
	}
	else
		overlay->hide();

	if (overlay_dont_animate) {
		overlay->setHidden(animate_direction == OUT);
		setHidden(animate_direction == OUT);
// 		runCallBacks(FINISHED);
	} else {
		// Start the animation!
		if (sceneX->state() == QTimeLine::NotRunning) {

			sceneX->start();
			sceneY->start();
			if (!overlay_just_resize) {
				// The animation will just work for repositioning the widget,
				// so we dont need overlay fade animation
				sceneF->start();
			}
		}
	}

	// Return the X coordinate timeline obj to use as reference for next animation
	return sceneX;
}

void AbstractDimmingWidget::finalize() {
	last_state = animate_direction;
	animate_direction = FINISHED;

	if (animate_direction == FINISHED) {
		updateParentResizeEvent();
	}

	if (last_state == IN) {
	}
	else
	if (last_state == OUT) {
	}

	overlay->setHidden(last_state == OUT);
}

void AbstractDimmingWidget::moveFrame(int frame) {
	if (sender() == sceneX)
		this->move(frame, y());
	if (sender() == sceneY)
		this->move(x(), frame);
}

void AbstractDimmingWidget::disable_parent() {
	// FIXME: WTF it is?

// 	if (disable_parent_in_shown) {
// 		for (QObjectList::const_iterator it = parent_widget->children().begin(); it != parent_widget->children().end(); ++it)
// 			if (*it != this and !(*it)->inherits("QLayout"))
// 				((QWidget *)(*it))->setEnabled(last_direction != IN);
// // 		parent.c
// // 		for item in self.__parent.children():
// // 		if not item == self and not item.inherits("QLayout"):
// // 			try:
// // 			item.setEnabled(not self.__last_direction == IN)
// // 			except:
// // 			pass
// 	}
}

void AbstractDimmingWidget::setAnimationCurve(QEasingCurve curve) {
	sceneX->setEasingCurve(curve);
	sceneY->setEasingCurve(curve);
}

void AbstractDimmingWidget::go() {
	if (init_finished and (last_state != IN)) {
		animate_direction = IN;
		animate();
	}
}

void AbstractDimmingWidget::og() {
	if (init_finished and (last_state != OUT)) {
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

void AbstractDimmingWidget::setOverlayOpacity(int opacity) {
	overlay_opacity = opacity;
	sceneF->setFrameRange(0, opacity);
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


#include "AbstractDimmingWidget.moc"