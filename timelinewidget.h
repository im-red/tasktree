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

#pragma once

#include "taskmodel.h"

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsItem>

#include <vector>

namespace Ui {
class TimeLineWidget;
}

class TimeLineWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TimeLineWidget(QWidget *parent = nullptr);
    ~TimeLineWidget() override;

    void setModel(const TaskModel &model);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    void on_buttonDebug_clicked();

private:
    void initConnection();

    // generate bright color according to the description of a task
    static QColor generateBrightColor(const QByteArray &ba);
    static QColor itemColor(const Task &t);

    QPointF scenePointOnViewCenter() const;

    int centerTask() const;
    void centerOnTask(int i);

    qreal unitWidth() const;
    qreal unitHeight() const;

    bool taskShouldShow(int i) const;
    bool textShouldShow() const;

    void clearScene();
    void initItems();
    void redrawScene();

    void updateRuler();

private:
    Ui::TimeLineWidget *ui;
    QGraphicsScene *m_scene;
    TaskModel m_model;
    int64_t m_maxStopTime;
    std::vector<QGraphicsRectItem *> m_rects;
    std::vector<QGraphicsTextItem *> m_texts;
};

