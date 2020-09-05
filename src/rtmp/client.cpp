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

using namespace RTMP;

Client::Client(const QString &hostName, QObject *parent)
    : QObject(parent)
    , mHostName(hostName)
    , mProtocol(&mSocket)
{
    connect(&mSocket, &QTcpSocket::connected, this, &Client::onConnected);
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wused-but-marked-unused"
    connect(&mSocket, qOverload<QAbstractSocket::SocketError>(&QTcpSocket::error),
            this, &Client::onSocketError);
#pragma clang diagnostic pop

    connect(&mProtocol, &Protocol::error, this, &Client::onProtocolError);
}

void Client::onConnected()
{
    mProtocol.startHandshake();
}

void Client::onSocketError()
{
    //...
}

void Client::onProtocolError(const QString &errorMessage)
{
    //...
}
