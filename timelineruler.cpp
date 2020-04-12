/*********************************************************************************
 * MIT License
 *
 * Copyright (c) 2020 Jia Lihong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ********************************************************************************/

#include "timelineruler.h"

#include <QPainter>
#include <QDebug>

#include <cmath>

TimeLineRuler::TimeLineRuler(QWidget *parent) : QWidget(parent)
{

}

int64_t TimeLineRuler::startTime() const
{
    return m_startTime;
}

void TimeLineRuler::setStartTime(const int64_t &startTime)
{
    m_startTime = startTime;
    update();
}

int64_t TimeLineRuler::stopTime() const
{
    return m_stopTime;
}

void TimeLineRuler::setStopTime(const int64_t &stopTime)
{
    m_stopTime = stopTime;
    update();
}

int TimeLineRuler::startX() const
{
    return m_startX;
}

void TimeLineRuler::setStartX(int startX)
{
    m_startX = startX;
    update();
}

int TimeLineRuler::stopX() const
{
    return m_stopX;
}

void TimeLineRuler::setStopX(int stopX)
{
    m_stopX = stopX;
    update();
}

void TimeLineRuler::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawLine(m_startX, height() / 2, m_startX, height());
    p.drawLine(m_stopX, height() / 2, m_stopX, height());

    drawNumber(p);
}

void TimeLineRuler::drawNumber(QPainter &painter)
{
    QString startTimeText = QString::number(m_startTime / 1000000.0, 'f', 3) + " s";
    QString stopTimeText = QString::number(m_stopTime / 1000000.0, 'f', 3) + " s";

    QFontMetrics fm(painter.font());
    int stopTimeWidth = fm.width(stopTimeText);

    painter.drawText(m_startX, height() / 2, startTimeText);
    painter.drawText(m_stopX - stopTimeWidth, height() / 2, stopTimeText);
}
