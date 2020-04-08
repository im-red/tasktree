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

#include "taskmodel.h"

#include <QDebug>

#include <assert.h>

using namespace std;

TaskModel::TaskModel()
{
    addIdleTask();
}

TaskModel::~TaskModel()
{

}

void TaskModel::clear()
{
    m_tasks.clear();
    m_pid2id.clear();

    addIdleTask();
}

void TaskModel::addForkTask(int pid, int ppid, const QString &comm, int64_t startTime)
{
    // qDebug() << pid << ppid << comm << startTime;

    if (pid == 0)
    {
        qDebug() << "ignore idle task:" << pid << ppid << comm << startTime;
        return;
    }
    assert(m_pid2id[ppid].size() != 0);

    int parentId = m_pid2id[ppid].back();
    int id = static_cast<int>(m_tasks.size());

    m_tasks.emplace_back(Task::Fork, id, pid, comm, startTime);
    m_pid2id[pid].push_back(id);

    m_tasks[static_cast<size_t>(id)].setParentId(parentId);
    m_tasks[static_cast<size_t>(parentId)].addChild(id);
}

void TaskModel::addExecTask(int pid, const QString &comm, int64_t startTime)
{
    // qDebug() << pid << comm << startTime;

    assert(m_pid2id[pid].size() != 0);

    int id = static_cast<int>(m_tasks.size());
    int preExecId = m_pid2id[pid].back();

    m_tasks.emplace_back(Task::Exec, id, pid, comm, startTime);
    m_pid2id[pid].push_back(id);

    m_tasks[static_cast<size_t>(id)].setPreExecId(preExecId);
    m_tasks[static_cast<size_t>(preExecId)].setPostExecId(id);
    m_tasks[static_cast<size_t>(preExecId)].setStopTime(startTime);
}

void TaskModel::taskExit(int pid, int64_t stopTime)
{
    // qDebug() << pid << stopTime;

    assert(m_pid2id[pid].size() != 0);

    int id = m_pid2id[pid].back();
    m_tasks[static_cast<size_t>(id)].setStopTime(stopTime);
}

Task TaskModel::rootTask() const
{
    assert(m_tasks.size() > 0);
    return m_tasks[0];
}

const Task &TaskModel::task(int id) const
{
    assert(m_tasks.size() > static_cast<size_t>(id));
    return m_tasks[static_cast<size_t>(id)];
}

QString TaskModel::dump() const
{
    QString result;
    const size_t size = m_tasks.size();
    for (size_t i = 0; i < size; i++)
    {
        result += m_tasks[i].dump() + "\n";
    }
    return result;
}

QString TaskModel::dumpTree() const
{
    return "";
}

void TaskModel::addIdleTask()
{
    int id = static_cast<int>(m_tasks.size());

    m_tasks.emplace_back(Task::Idle, id, 0, "idle", 0);
    m_pid2id[id].push_back(id);
}
