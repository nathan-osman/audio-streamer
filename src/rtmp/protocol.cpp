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

#include <cstring>

#include <QDateTime>
#include <QtEndian>

#include "protocol.h"

using namespace RTMP;

const quint8 Version = 0x03;

struct Handshake1
{
    quint8 version;
    quint32 time;
    quint32 zero;
    quint8 random[1528];
};

struct Handshake2
{
    quint32 time;
    quint32 time2;
    quint8 random[1528];
};

struct Chunk
{
    //...
};

inline quint32 currentTimestamp()
{
    return static_cast<quint32>(QDateTime::currentMSecsSinceEpoch());
}

Protocol::Protocol(QIODevice *device, QObject *parent)
    : QObject(parent)
    , mDevice(device)
    , mState(StateNone)
{
    connect(mDevice, &QIODevice::readyRead, this, &Protocol::onReadyRead);
}

void Protocol::startHandshake()
{
    mEpoch = currentTimestamp();

    // Send the C0 and C1 packets
    Handshake1 handshake1{
        Version,
        qToBigEndian<quint32>(mEpoch),
        0,
        {0}
    };

    mDevice->write(reinterpret_cast<const char*> (&handshake1), sizeof (Handshake1));
    mState = StateVersionSent;
}

void Protocol::onReadyRead()
{
    mReadBuffer.append(mDevice->readAll());

    switch (mState) {
    case StateNone:
        break;
    case StateVersionSent:
        if (static_cast<size_t> (mReadBuffer.size()) >= sizeof (Handshake1)) {
            processVersion();
        }
        break;
    case StateAckSent:
        if (static_cast<size_t> (mReadBuffer.size()) >= sizeof (Handshake2)) {
            processAck();
        }
        break;
    }
}

void Protocol::processVersion()
{
    // Read the S0 and S1 packets
    const Handshake1 *handshake1 = reinterpret_cast<const Handshake1*>(mReadBuffer.constData());
    mReadBuffer.remove(0, sizeof (Handshake1));

    // Confirm that version 3+ is supported
    if (handshake1->version < Version) {
        emit error(tr("invalid version %1 specified").arg(handshake1->version));
        return;
    }

    // Create the C2 packet
    Handshake2 handshake2{
        qToBigEndian<quint32>(mEpoch),
        qToBigEndian<quint32>(currentTimestamp()),
        {0}
    };
    memcpy(handshake2.random, handshake1->random, sizeof (Handshake2::random));

    mDevice->write(reinterpret_cast<const char*> (&handshake2), sizeof (Handshake2));
    mState = StateAckSent;
}

void Protocol::processAck()
{
    // Nothing is done with the ACK, so discard it
    mReadBuffer.remove(0, sizeof (Handshake2));

    emit handshakeCompleted();

    // TODO: update the state
}

