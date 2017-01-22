// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#pragma once

#include <QtCore/qhash.h>



template <class Key, class T>
class LruCache
{
    struct Node {
        inline Node() : keyPtr(0), t(0), c(0), p(0), n(0) {}
        inline Node(T *data, int cost)
            : keyPtr(0), t(data), c(cost), p(0), n(0) {}
        const Key *keyPtr; T *t; int c; Node *p,*n; //-V122
    };
    Node *f, *l; //-V122
    QHash<Key, Node> hash;
    QSet<T*> removed;
    int mx, total;

    inline void unlink(Node &n) {
        if (n.p) n.p->n = n.n;
        if (n.n) n.n->p = n.p;
        if (l == &n) l = n.p;
        if (f == &n) f = n.n;
        total -= n.c;
        T *obj = n.t;
        hash.remove(*n.keyPtr);
        removed << obj;
    }
    inline T *relink(const Key &key) {
        auto i = hash.find(key);
        if (i == hash.constEnd())
            return 0;

        Node &n = *i;
        if (f != &n) {
            if (n.p) n.p->n = n.n;
            if (n.n) n.n->p = n.p;
            if (l == &n) l = n.p;
            n.p = 0;
            n.n = f;
            f->p = &n;
            f = &n;
        }
        return n.t;
    }

    Q_DISABLE_COPY(LruCache)

public:
    inline explicit LruCache(int maxCost = 100) Q_DECL_NOTHROW;
    inline ~LruCache() { clear(); qDeleteAll(removed); }

    inline int maxCost() const { return mx; }
    void setMaxCost(int m);
    inline int totalCost() const { return total; }

    inline int size() const { return hash.size(); }
    inline int count() const { return hash.size(); }
    inline bool isEmpty() const { return hash.isEmpty(); }
    inline QList<Key> keys() const { return hash.keys(); }

    QList<T*> valuesAfter(const Key &key) const;
    QList<T*> removedValues();

    void clear();

    bool insert(const Key &key, T *object, int cost = 1);
    T *object(const Key &key) const;
    inline bool contains(const Key &key) const { return hash.contains(key); }
    T *operator[](const Key &key) const;

    bool remove(const Key &key);
    T *take(const Key &key);

private:
    void trim(int m);
};

template <class Key, class T>
inline LruCache<Key, T>::LruCache(int amaxCost) Q_DECL_NOTHROW
    : f(0), l(0), mx(amaxCost), total(0) {}

template <class Key, class T>
inline void LruCache<Key,T>::clear()
{ while (f) { removed << f->t; f = f->n; }
 hash.clear(); l = 0; total = 0; }

template <class Key, class T>
inline void LruCache<Key,T>::setMaxCost(int m)
{ mx = m; trim(mx); }

template<class Key, class T>
QList<T*> LruCache<Key, T>::valuesAfter(const Key& key) const
{
    QList<T*> values;
    values.reserve(count());
    auto node = f;
    while (node && *node->keyPtr != key)
    {
        values << node->t;
        node = node->n;
    }
    return values;
}

template<class Key, class T>
QList<T*> LruCache<Key, T>::removedValues()
{
    const auto list = QList<T*>::fromSet(removed);
    removed.clear();
    return list;
}

template <class Key, class T>
inline T *LruCache<Key,T>::object(const Key &key) const
{ return const_cast<LruCache<Key,T>*>(this)->relink(key); }

template <class Key, class T>
inline T *LruCache<Key,T>::operator[](const Key &key) const
{ return object(key); }

template <class Key, class T>
inline bool LruCache<Key,T>::remove(const Key &key)
{
    auto i = hash.find(key);
    if (i == hash.constEnd()) {
        return false;
    } else {
        unlink(*i);
        return true;
    }
}

template <class Key, class T>
inline T *LruCache<Key,T>::take(const Key &key)
{
    typename QHash<Key, Node>::iterator i = hash.find(key);
    if (i == hash.end())
        return 0;

    Node &n = *i;
    T *t = n.t;
    n.t = 0;
    unlink(n);
    return t;
}

template <class Key, class T>
bool LruCache<Key,T>::insert(const Key &akey, T *aobject, int acost)
{
    if (contains(akey))
    {
        auto& node = hash[akey];
        auto old = node.t;
        Q_ASSERT(old == aobject);
        if (old == aobject)
        {
            relink(akey);

            total -= node.c;
            total += acost;
            node.c = acost;

            if (acost > mx) {
                removed << aobject;
                return false;
            }
            trim(mx - acost);

            return true;
        }

        remove(akey);
    }

    if (acost > mx) {
        removed << aobject;
        return false;
    }
    trim(mx - acost);
    Node sn(aobject, acost);
    typename QHash<Key, Node>::iterator i = hash.insert(akey, sn);
    total += acost;
    Node *n = &i.value();
    n->keyPtr = &i.key();
    if (f) f->p = n;
    n->n = f;
    f = n;
    if (!l) l = f;
    return true;
}

template <class Key, class T>
void LruCache<Key,T>::trim(int m)
{
    Node *n = l;
    while (n && total > m) {
        Node *u = n;
        n = n->p;
        unlink(*u);
    }
}
