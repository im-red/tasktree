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

#include "task.h"

Task::Task(Type t, int id, int pid, const QString &comm, int64_t startTime)
    : m_type(t)
    , m_id(id)
    , m_pid(pid)
    , m_comm(comm)
    , m_startTime(startTime)
    , m_stopTime(-1)
    , m_parentId(-1)
    , m_preExecId(-1)
    , m_postExecId(-1)
{

}

int Task::id() const
{
    return m_id;
}

int Task::pid() const
{
    return m_pid;
}

QString Task::comm() const
{
    return m_comm;
}

int64_t Task::startTime() const
{
    return m_startTime;
}

int64_t Task::stopTime() const
{
    return m_stopTime;
}

void Task::setStopTime(const int64_t &stopTime)
{
    m_stopTime = stopTime;
}

int Task::parentId() const
{
    return m_parentId;
}

void Task::setParentId(int parentId)
{
    m_parentId = parentId;
}

int Task::postExecId() const
{
    return m_postExecId;
}

void Task::setPostExecId(int afterExecId)
{
    m_postExecId = afterExecId;
}

void Task::addChild(int id)
{
    m_childrenId.push_back(id);
}

std::vector<int> Task::childrenId() const
{
    return m_childrenId;
}

int64_t Task::duration() const
{
    int64_t result = -1;
    if (m_stopTime != -1)
    {
        result = m_stopTime - m_startTime;
    }
    return result;
}

QString Task::dump() const
{
    QString children = "(";
    if (m_childrenId.size() != 0)
    {
        children += QString::number(m_childrenId[0]);
        const size_t size = m_childrenId.size();
        for (size_t i = 1; i < size; i++)
        {
            children += ", " + QString::number(m_childrenId[i]);
        }
    }
    children += ")";

    return QString("type: %1, id: %2, pid: %3, comm: %4, "
                   "startTime: %5, stopTime: %6, "
                   "parentId: %7, preExecId: %8, postExecId: %9, "
                   "children: %10")
            .arg(m_type).arg(m_id).arg(m_pid).arg(m_comm)
            .arg(m_startTime).arg(m_stopTime)
            .arg(m_parentId).arg(m_preExecId).arg(m_postExecId)
            .arg(children);
}

int Task::preExecId() const
{
    return m_preExecId;
}

void Task::setPreExecId(int preExecId)
{
    m_preExecId = preExecId;
}
