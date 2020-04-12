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

#include "timelinewidget.h"
#include "ui_timelinewidget.h"

#include <QCryptographicHash>
#include <QGraphicsTextItem>
#include <QDebug>
#include <QScrollBar>

#include <algorithm>

using namespace std;

TimeLineWidget::TimeLineWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TimeLineWidget)
    , m_scene(new QGraphicsScene(this))
    , m_maxStopTime(0)
{
    ui->setupUi(this);

    ui->gvTimeline->setScene(m_scene);
    ui->gvTimeline->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    ui->gvTimeline->setDragMode(QGraphicsView::ScrollHandDrag);

    initConnection();

    setModel(m_model);
}

TimeLineWidget::~TimeLineWidget()
{
    delete ui;
}

void TimeLineWidget::setModel(const TaskModel &model)
{
    m_model = model;

    clearScene();
    initItems();
    redrawScene();
}

void TimeLineWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateRuler();
}

void TimeLineWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    updateRuler();
}

void TimeLineWidget::on_buttonDebug_clicked()
{
    QScrollBar *sb = ui->gvTimeline->horizontalScrollBar();

    qDebug() << sb->minimum() << sb->maximum() << sb->value();
}

void TimeLineWidget::initConnection()
{
    connect(ui->cbHideKthread, &QCheckBox::toggled, this, &TimeLineWidget::redrawScene);
    connect(ui->sliderWidth, &QSlider::valueChanged, this, &TimeLineWidget::redrawScene);
    connect(ui->sliderHeight, &QSlider::valueChanged, this, &TimeLineWidget::redrawScene);
    connect(ui->gvTimeline->horizontalScrollBar(), &QScrollBar::valueChanged, this, &TimeLineWidget::updateRuler);
    connect(ui->gvTimeline->horizontalScrollBar(), &QScrollBar::rangeChanged, this, &TimeLineWidget::updateRuler);
}

QColor TimeLineWidget::generateBrightColor(const QByteArray &ba)
{
    assert(ba.size() > 0);

    const uint8_t a = static_cast<uint8_t>(ba.at(0 % ba.size()));
    const uint8_t b = static_cast<uint8_t>(ba.at(1 % ba.size()));
    const uint8_t c = static_cast<uint8_t>(ba.at(2 % ba.size()));

    static const qreal H_MIN = 0.0;
    static const qreal H_MAX = 1.0;
    static const qreal S_MIN = 0.0;
    static const qreal S_MAX = 1.0;
    static const qreal V_MIN = 0.7;
    static const qreal V_MAX = 1.0;

    const qreal h = H_MIN + (H_MAX - H_MIN) * a / 256;
    const qreal s = S_MIN + (S_MAX - S_MIN) * b / 256;
    const qreal v = V_MIN + (V_MAX - V_MIN) * c / 256;

    QColor result;
    result.setHsvF(h, s, v);
    return result;
}

QColor TimeLineWidget::itemColor(const Task &t)
{
    QString desc = t.description();
    QByteArray ba = QCryptographicHash::hash(desc.toUtf8(), QCryptographicHash::Md5);
    return generateBrightColor(ba);
}

QPointF TimeLineWidget::scenePointOnViewCenter() const
{
    const int centerX = ui->gvTimeline->viewport()->width() / 2;
    const int centerY = ui->gvTimeline->viewport()->height() / 2;

    const QPoint viewCenter(centerX, centerY);

    return ui->gvTimeline->mapToScene(viewCenter);
}

int TimeLineWidget::centerTask() const
{
    const QPointF sceneCenter = scenePointOnViewCenter();
    const qreal y = sceneCenter.y();
    int result = 0;
    for (int i = 0; i < m_model.taskCount(); i++)
    {
        const QGraphicsRectItem *rect = m_rects[static_cast<size_t>(i)];
        if (rect->isVisible())
        {
            const qreal rectY = rect->rect().y();
            const qreal rectH = rect->rect().height();

            if ((rectY <= y) && (rectY + rectH >= y))
            {
                result = i;
                break;
            }
        }
    }
    return result;
}

void TimeLineWidget::centerOnTask(int i)
{
    const QGraphicsRectItem *rect = m_rects[static_cast<size_t>(i)];
    if (rect->isVisible())
    {
        const qreal centerX = rect->rect().x();
        const qreal centerY = rect->rect().y() + rect->rect().height() / 2;

        ui->gvTimeline->centerOn(centerX, centerY);
    }
}

qreal TimeLineWidget::unitWidth() const
{
    return ui->sliderWidth->value() * 0.00001;
}

qreal TimeLineWidget::unitHeight() const
{
    return ui->sliderHeight->value();
}

bool TimeLineWidget::taskShouldShow(int i) const
{
    const Task &t = m_model.task(i);
    const bool hideKthread = ui->cbHideKthread->isChecked();
    if (hideKthread && t.kthread())
    {
        return false;
    }
    return true;
}

bool TimeLineWidget::textShouldShow() const
{
    return unitHeight() >= 20;
}

static int64_t maxStopTime(const TaskModel &model)
{
    int64_t result = 0;
    for (int i = 0; i < model.taskCount(); i++)
    {
        int64_t t = model.task(i).stopTime();
        result = max(result, t);
    }
    return result;
}

void TimeLineWidget::clearScene()
{
    m_scene->clear();
    m_maxStopTime = maxStopTime(m_model);
}

void TimeLineWidget::initItems()
{
    m_rects.clear();
    m_texts.clear();

    m_rects.reserve(static_cast<size_t>(m_model.taskCount()));
    m_texts.reserve(static_cast<size_t>(m_model.taskCount()));

    for (int i = 0; i < m_model.taskCount(); i++)
    {
        QColor c = itemColor(m_model.task(i));

        QGraphicsRectItem *rect = m_scene->addRect(0, 0, 0, 0, QPen(c), QBrush(c));
        rect->hide();
        m_rects.push_back(rect);

        QGraphicsTextItem *text = m_scene->addText(m_model.task(i).description());
        text->hide();
        m_texts.push_back(text);
    }
}

void TimeLineWidget::redrawScene()
{
    int oldCenterTask = centerTask();

    const qreal unitW = unitWidth();
    const qreal unitH = unitHeight();

    const bool showText = textShouldShow();

    int curRow = -1;

    qreal sceneW = m_maxStopTime * unitW;

    for (int i = 0; i < m_model.taskCount(); i++)
    {
        QGraphicsRectItem *rect = m_rects[static_cast<size_t>(i)];
        QGraphicsTextItem *text = m_texts[static_cast<size_t>(i)];
        if (taskShouldShow(i))
        {
            curRow++;

            rect->show();
            text->setVisible(showText);

            const Task &t = m_model.task(i);

            const qreal x = t.startTime() * unitW;
            const qreal y = curRow * unitH;
            const qreal w = t.duration() > 0 ? t.duration() * unitW : sceneW * 10;
            const qreal h = unitH;

            rect->setRect(x, y, w, h);
            text->setPos(x, y);

            sceneW = max(sceneW, x + text->boundingRect().width());
        }
        else
        {
            rect->hide();
            text->hide();
        }
    }
    const qreal sceneH = (curRow + 1) * unitH;
    m_scene->setSceneRect(0, 0, sceneW, sceneH);

    centerOnTask(oldCenterTask);

    updateRuler();
}

void TimeLineWidget::updateRuler()
{
    const int sceneWidth = static_cast<int>(m_scene->width());
    const int viewWidth = ui->gvTimeline->viewport()->width();

    if (sceneWidth <= viewWidth)
    {
        ui->widgetRuler->setStartX((viewWidth - sceneWidth) / 2);
        ui->widgetRuler->setStopX((viewWidth - sceneWidth) / 2 + sceneWidth);
        ui->widgetRuler->setStartTime(0);
        ui->widgetRuler->setStopTime(static_cast<int>(sceneWidth / unitWidth()));
    }
    else
    {
        ui->widgetRuler->setStartX(0);
        ui->widgetRuler->setStopX(viewWidth);
        const qreal sceneStartX = ui->gvTimeline->mapToScene(0, 0).x();
        const qreal sceneStopX = ui->gvTimeline->mapToScene(viewWidth, 0).x();
        const int startTime = static_cast<int>(sceneStartX / unitWidth());
        const int stopTime = static_cast<int>(sceneStopX / unitWidth());
        ui->widgetRuler->setStartTime(startTime);
        ui->widgetRuler->setStopTime(stopTime);
    }
}
