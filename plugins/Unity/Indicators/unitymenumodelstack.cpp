/*
 * Copyright 2013 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *      Nick Dedekind <nick.dedekind@canonical.com>
 */

#include "unitymenumodelstack.h"

#include <QDebug>
#include <unitymenumodel.h>

class UnityMenuModelEntry : public QObject {
    Q_OBJECT
public:
    UnityMenuModelEntry(UnityMenuModel* model, UnityMenuModel* parentModel, int index)
    : m_model(model),
      m_parentModel(parentModel),
      m_index(index)
    {
        if (m_parentModel) {
            QObject::connect(m_parentModel, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(onRowsInserted(QModelIndex, int, int)));
            QObject::connect(m_parentModel, SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(onRowsRemoved(QModelIndex, int, int)));
            QObject::connect(m_parentModel, SIGNAL(modelReset()), this, SLOT(onModelReset()));
        }
    }

    UnityMenuModel* model() const { return m_model; }

private Q_SLOTS:
    void onRowsInserted(const QModelIndex&, int start, int end)
    {
        int delta = end-start + 1;
        if (start <= m_index) {
            m_index += delta;
        }
    }

    void onRowsRemoved(const QModelIndex&, int start, int end)
    {
        int delta = end-start + 1;
        if (start <= m_index) {
            if (start + delta >= m_index) {
                // in the range removed
                Q_EMIT remove();
                disconnect(m_parentModel, 0, this, 0);
            } else {
                m_index -= delta;
            }
        }
    }

Q_SIGNALS:
    void remove();

private Q_SLOTS:
    void onModelReset()
    {
        Q_EMIT remove();
        disconnect(m_parentModel, 0, this, 0);
    }

private:
    UnityMenuModel* m_model;
    UnityMenuModel* m_parentModel;
    int m_index;
};

UnityMenuModelStack::UnityMenuModelStack(QObject* parent)
    : QObject(parent)
{
}

UnityMenuModelStack::~UnityMenuModelStack()
{
    qDeleteAll(m_menuModels);
    m_menuModels.clear();
}

UnityMenuModel* UnityMenuModelStack::head() const
{
    return !m_menuModels.isEmpty() ? m_menuModels.first()->model() : NULL;
}

void UnityMenuModelStack::setHead(UnityMenuModel* model)
{
    if (head() != model) {
        qDeleteAll(m_menuModels);
        m_menuModels.clear();

        push(model, 0);
        Q_EMIT headChanged(model);
    }
}

UnityMenuModel* UnityMenuModelStack::tail() const
{
    return !m_menuModels.isEmpty() ? m_menuModels.last()->model() : NULL;
}

void UnityMenuModelStack::push(UnityMenuModel* model, int index)
{
    UnityMenuModelEntry* entry = new UnityMenuModelEntry(model, tail(), index);
    QObject::connect(entry, SIGNAL(remove()), SLOT(onRemove()));

    m_menuModels << entry;
    Q_EMIT tailChanged(model);
}

UnityMenuModel* UnityMenuModelStack::pop()
{
    if (m_menuModels.isEmpty()) {
        return NULL;
    }
    UnityMenuModelEntry* entry = m_menuModels.takeLast();
    UnityMenuModel* model = entry->model();
    entry->deleteLater();

    Q_EMIT tailChanged(tail());
    if (m_menuModels.isEmpty()) {
        Q_EMIT headChanged(NULL);
    }

    return model;
}

void UnityMenuModelStack::onRemove()
{
    UnityMenuModelEntry* removed = qobject_cast<UnityMenuModelEntry*>(sender());
    if (!m_menuModels.contains(removed))
        return;

    for (int i = m_menuModels.count() -1; i >= 0; i--) {
        UnityMenuModelEntry* entry = m_menuModels[i];
        pop();
        if (entry == removed) {
            break;
        }
    }
}

#include "unitymenumodelstack.moc"
