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

#include "textlayouter.h"

#include <QDebug>

using namespace std;

struct TextLayoutData
{
    explicit TextLayoutData(int _id, int _x = 0) : id(_id), x(_x), nextLayoutChild(0) {}

    int id;
    int x;
    int nextLayoutChild;
};

TextLayouter::TextLayouter(const TaskModel &model)
    : m_model(model)
{

}

static void appendSpace(QString &s, int x)
{
    int delta = x - s.size();
    if (delta > 0)
    {
        s += QString(delta, ' ');
    }
}

static QString description(const Task &t)
{
    QString result = QString("[%1] %2").arg(t.pid()).arg(t.comm());
    if (t.duration() == -1)
    {
        result += "(living)";
    }
    return result;
}

QString TextLayouter::layout()
{
    vector<TextLayoutData> layouting;

    QString result;
    const int taskCount = m_model.taskCount();
    assert(taskCount >= 0);

    {
        TextLayoutData d(0);
        result += description(m_model.task(d.id)) + "\n";
        if (m_model.task(d.id).childrenId().size() != 0)
        {
            layouting.push_back(d);
        }
        else
        {
            qDebug() << "only a idle task";
        }
    }

    static const QString FORK_PREFIX = " \\_ ";
    static const QString EXEC_PREFIX = " -> ";
    static const QString PROCESSING_PREFIX = " |  ";

    size_t layoutingSize = 0;
    while ((layoutingSize = layouting.size()) != 0)
    {
        QString line;
        for (size_t i = 0; i < layoutingSize - 1; i++)
        {
            TextLayoutData &d = layouting[i];
            appendSpace(line, d.x);
            line += PROCESSING_PREFIX;
        }

        TextLayoutData &lastD = layouting.back();
        appendSpace(line, lastD.x);
        line += FORK_PREFIX;

        const Task &lastT = m_model.task(lastD.id);
        int curId = lastT.childrenId(lastD.nextLayoutChild);

        lastD.nextLayoutChild++;
        if (lastD.nextLayoutChild >= lastT.childrenCount())
        {
            layouting.pop_back();
        }

        while (curId != -1)
        {
            const Task &t = m_model.task(curId);
            TextLayoutData d(curId, line.size());
            if (t.childrenCount() > 0)
            {
                layouting.push_back(d);
            }
            line += description(t);

            curId = t.postExecId();
            if (curId != -1)
            {
                line += EXEC_PREFIX;
            }
        }
        result += line + "\n";
    }

    return result;
}
