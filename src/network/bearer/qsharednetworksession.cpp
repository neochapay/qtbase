/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtNetwork module of the Qt Toolkit.
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

#include "qsharednetworksession_p.h"
#include "qbearerengine_p.h"
#include <QThreadStorage>

#ifndef QT_NO_BEARERMANAGEMENT

QT_BEGIN_NAMESPACE

QThreadStorage<QSharedNetworkSessionManager *> tls;

inline QSharedNetworkSessionManager* sharedNetworkSessionManager()
{
    QSharedNetworkSessionManager* rv = tls.localData();
    if (!rv) {
        rv = new QSharedNetworkSessionManager;
        tls.setLocalData(rv);
    }
    return rv;
}

static void doDeleteLater(QObject* obj)
{
    obj->deleteLater();
}

QSharedPointer<QNetworkSession> QSharedNetworkSessionManager::getSession(const QNetworkConfiguration &config)
{
    QSharedNetworkSessionManager *m(sharedNetworkSessionManager());
    //if already have a session, return it
    if (m->sessions.contains(config)) {
        QSharedPointer<QNetworkSession> p = m->sessions.value(config).toStrongRef();
        if (!p.isNull())
            return p;
    }
    //otherwise make one
    QSharedPointer<QNetworkSession> session(new QNetworkSession(config), doDeleteLater);
    m->sessions[config] = session;
    return session;
}

void QSharedNetworkSessionManager::setSession(const QNetworkConfiguration &config, QSharedPointer<QNetworkSession> session)
{
    QSharedNetworkSessionManager *m(sharedNetworkSessionManager());
    m->sessions[config] = session;
}

uint qHash(const QNetworkConfiguration& config)
{
    return ((uint)config.type()) | (((uint)config.bearerType()) << 8) | (((uint)config.purpose()) << 16);
}

QT_END_NAMESPACE

#endif // QT_NO_BEARERMANAGEMENT
