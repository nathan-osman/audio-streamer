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

#include <QAudioDeviceInfo>
#include <QGridLayout>
#include <QIcon>

#include "mainwindow.h"

const QString SettingGeometry("geometry");
const QString SettingWindowState("windowState");

MainWindow::MainWindow()
    : mDeviceComboBox(new QComboBox)
    , mRefreshButton(new QPushButton(tr("Refresh")))
    , mHostNameEdit(new QLineEdit)
    , mConnectionButton(new QPushButton)
    , mLogEdit(new QTextEdit)
{
    mHostNameEdit->setPlaceholderText(tr("RTMP server URL"));

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(mDeviceComboBox, 0, 0);
    gridLayout->addWidget(mRefreshButton, 0, 1);
    gridLayout->addWidget(mHostNameEdit, 1, 0);
    gridLayout->addWidget(mConnectionButton, 1, 1);
    gridLayout->addWidget(mLogEdit, 2, 0, 1, 2);

    connect(mRefreshButton, &QPushButton::clicked, this, &MainWindow::repopulateAudioDevices);

    QWidget *widget = new QWidget;
    widget->setLayout(gridLayout);
    setCentralWidget(widget);

    // Load the previous window state
    restoreGeometry(mSettings.value(SettingGeometry).toByteArray());
    restoreState(mSettings.value(SettingWindowState).toByteArray());

    setWindowTitle(tr("Audio Streamer"));
    setWindowIcon(QIcon(":/logo.png"));

    repopulateAudioDevices();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    mSettings.setValue(SettingGeometry, saveGeometry());
    mSettings.setValue(SettingWindowState, saveState());
    QMainWindow::closeEvent(event);
}

void MainWindow::repopulateAudioDevices()
{
    mDeviceComboBox->clear();
    foreach (QAudioDeviceInfo info, QAudioDeviceInfo::availableDevices(QAudio::AudioInput)) {
        if (!info.isNull()) {
            mDeviceComboBox->addItem(
                QString("%1 (%2)")
                    .arg(info.deviceName())
                    .arg(info.realm()),
                QVariant::fromValue<QAudioDeviceInfo>(info)
            );
        }
    }
}
