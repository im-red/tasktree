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

#include <QString>

#include <vector>
#include <memory>

class Task
{
public:
    enum Type
    {
        Idle,
        Fork,
        Exec
    };

public:
    Task(Type t, int id, int pid, const QString &comm, int64_t startTime, bool kthread);

    int id() const;
    int pid() const;
    QString comm() const;
    int64_t startTime() const;
    int64_t stopTime() const;
    int parentId() const;
    int preExecId() const;
    int postExecId() const;
    std::vector<int> childrenId() const;
    bool kthread() const;

    int childrenId(int i) const { assert(i < childrenCount()); return m_childrenId[static_cast<size_t>(i)]; }
    int childrenCount() const { return static_cast<int>(m_childrenId.size()); }

    void setStopTime(const int64_t &stopTime);
    void setParentId(int parentId);
    void setPreExecId(int preExecId);
    void setPostExecId(int postExecId);

    void addChild(int id);
    int64_t duration() const;

    QString description() const;
    QString dump() const;

private:
    Type m_type;

    // pid is unique only in living task. It can be duplicated
    // with a history task. So we need a id field.
    int m_id;
    int m_pid;

    QString m_comm;

    // microsecond from system boot
    int64_t m_startTime;
    int64_t m_stopTime;

    // it is NOT the pid
    int m_parentId;
    int m_preExecId;
    int m_postExecId;
    std::vector<int> m_childrenId;

    bool m_kthread;
};

