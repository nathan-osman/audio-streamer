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

#include "client.h"

Client::Client(QObject *parent)
    : QObject(parent)
    , mProtocol(&mSocket)
    , mActive(false)
{
    connect(&mSocket, &QTcpSocket::connected, this, &Client::onConnected);
    connect(&mSocket, qOverload<QAbstractSocket::SocketError>(&QTcpSocket::error),
            this, &Client::onSocketError);

    connect(&mProtocol, &Protocol::handshakeCompleted, this, &Client::onHandshakeCompleted);
    connect(&mProtocol, &Protocol::error, this, &Client::onProtocolError);
}

void Client::start(const QString &hostName)
{
    mActive = true;

    emit log(LogType::Info, QString("connecting to %1...").arg(hostName));
    mSocket.connectToHost(hostName, 1935);
}

void Client::stop()
{
    mActive = false;

    emit log(LogType::Info, QString("disconnecting from host..."));
    mSocket.disconnectFromHost();
}

void Client::onConnected()
{
    emit log(LogType::Success, "connected to host");
    mProtocol.startHandshake();
}

void Client::onHandshakeCompleted()
{
    emit log(LogType::Success, "RTMP handshake completed");

    // TODO: send next chunks
}

void Client::onProtocolError(const QString &errorMessage)
{
    //...
}

void Client::onSocketError()
{
    //...
}
