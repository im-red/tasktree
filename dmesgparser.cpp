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

#include "dmesgparser.h"

#include <QStringList>

DmesgParser::DmesgParser(TaskModel &model)
    : m_model(model)
{

}

void DmesgParser::parse(const QString &dmesg)
{
    m_model.clear();

    QStringList list = dmesg.split('\n');
    for (QString &s : list)
    {
        parseOneLine(s);
    }
}

void DmesgParser::parseOneLine(const QString &s)
{
    int timeBegin = s.indexOf('[');
    int timeEnd = s.indexOf(']');
    if (timeBegin < 0 || timeEnd <= 1)
    {
        return;
    }

    QString time = s.mid(timeBegin + 1, timeEnd - timeBegin - 1).trimmed();

    bool ok = true;
    double dTime = time.toDouble(&ok);
    if (!ok)
    {
        return;
    }

    int64_t i64Time = static_cast<int64_t>(dTime * 1000000);
    QString body = s.mid(timeEnd + 2);

    const QString FORK_PREFIX = "FORK|";
    const QString EXEC_PREFIX = "EXEC|";
    const QString EXIT_PREFIX = "EXIT|";

    if (body.startsWith(FORK_PREFIX))
    {
        parseForkLine(i64Time, body);
    }
    else if (body.startsWith(EXEC_PREFIX))
    {
        parseExecLine(i64Time, body);
    }
    else if (body.startsWith(EXIT_PREFIX))
    {
        parseExitLine(i64Time, body);
    }
    else
    {
        // do nothing
    }
}

void DmesgParser::parseForkLine(int64_t time, const QString &s)
{
    // FORK|570|VBoxService|=>|571|0
    QStringList list = s.split('|');
    assert(list.size() >= 6);

    int ppid = list[1].toInt();
    int pid = list[4].toInt();
    bool kthread = (list[5].toInt() != 0);

    m_model.addForkTask(pid, ppid, list[2], time, kthread);
}

void DmesgParser::parseExecLine(int64_t time, const QString &s)
{
    // EXEC|569|S35vboxadd-serv|=|grep
    QStringList list = s.split('|');
    assert(list.size() >= 5);

    int pid = list[1].toInt();

    m_model.addExecTask(pid, list[4], time);
}

void DmesgParser::parseExitLine(int64_t time, const QString &s)
{
    // EXIT|568|lsmod
    QStringList list = s.split('|');
    assert(list.size() >= 3);

    int pid = list[1].toInt();

    m_model.taskExit(pid, time);
}
