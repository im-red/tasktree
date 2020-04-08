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

#include "task.h"

#include <memory>
#include <map>

class TaskModel
{
public:
    TaskModel();
    ~TaskModel();

    void clear();

    void addForkTask(int pid, int ppid, const QString &comm, int64_t startTime);
    void addExecTask(int pid, const QString &comm, int64_t startTime);
    void taskExit(int pid, int64_t stopTime);

    Task rootTask() const;
    const Task &task(int id) const;
    int taskCount() const { return static_cast<int>(m_tasks.size()); }

    QString dump() const;
    QString dumpTree() const;

private:
    void addIdleTask();

private:
    // TaskData::m_id is always same with the index in m_tasks
    std::vector<Task> m_tasks;
    std::map<int, std::vector<int>> m_pid2id;
};

