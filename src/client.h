/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Nathan Osman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>

#include "log.h"
#include "protocol.h"

/**
 * @brief Implementation of an RTMP client for streaming audio
 */
class Client : public QObject
{
    Q_OBJECT

public:

    explicit Client(QObject *parent = nullptr);

    void start(const QString &hostName);
    void stop();

    inline bool isActive() const { return mActive; }

signals:

    void log(LogType logType, const QString &message);

private slots:

    void onConnected();
    void onHandshakeCompleted();
    void onProtocolError(const QString &errorMessage);
    void onSocketError();

private:

    QTcpSocket mSocket;
    Protocol mProtocol;

    bool mActive;
};

#endif // CLIENT_H
