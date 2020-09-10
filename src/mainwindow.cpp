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
#include <QCloseEvent>
#include <QDateTime>
#include <QGridLayout>
#include <QIcon>
#include <QMessageBox>
#include <QUrl>

#include "mainwindow.h"

const QString SettingDeviceName("deviceName");
const QString SettingGeometry("geometry");
const QString SettingHostName("hostName");
const QString SettingWindowState("windowState");

MainWindow::MainWindow()
    : mDeviceComboBox(new QComboBox)
    , mRefreshButton(new QPushButton(tr("Refresh")))
    , mHostNameEdit(new QLineEdit)
    , mConnectionButton(new QPushButton)
    , mLogEdit(new QTextEdit)
{
    mHostNameEdit->setPlaceholderText(tr("RTMP server URL"));
    mHostNameEdit->setText(mSettings.value(SettingHostName).toString());
    mLogEdit->setReadOnly(true);

    connect(mDeviceComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onDeviceChanged);

    connect(mRefreshButton, &QPushButton::clicked, this, &MainWindow::onRefreshClicked);
    connect(mConnectionButton, &QPushButton::clicked, this, &MainWindow::onConnectClicked);

    connect(&mRecorder, &Recorder::log, this, &MainWindow::onLog);
    connect(&mClient, &Client::log, this, &MainWindow::onLog);

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(mDeviceComboBox, 0, 0);
    gridLayout->addWidget(mRefreshButton, 0, 1);
    gridLayout->addWidget(mHostNameEdit, 1, 0);
    gridLayout->addWidget(mConnectionButton, 1, 1);
    gridLayout->addWidget(mLogEdit, 2, 0, 1, 2);

    QWidget *widget = new QWidget;
    widget->setLayout(gridLayout);
    setCentralWidget(widget);

    // Load the previous window state
    restoreGeometry(mSettings.value(SettingGeometry).toByteArray());
    restoreState(mSettings.value(SettingWindowState).toByteArray());

    setWindowTitle(tr("Audio Streamer"));
    setWindowIcon(QIcon(":/logo.png"));

    onRefreshClicked(true);
    toggleConnected(false);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (mClient.isActive() && QMessageBox::warning(
                this,
                tr("Warning"),
                tr("Connection is active. Are you sure?"),
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::No) == QMessageBox::No) {
        event->ignore();
    } else {
        mSettings.setValue(SettingDeviceName, mDeviceComboBox->currentText());
        mSettings.setValue(SettingGeometry, saveGeometry());
        mSettings.setValue(SettingHostName, mHostNameEdit->text());
        mSettings.setValue(SettingWindowState, saveState());
        QMainWindow::closeEvent(event);
    }
}

void MainWindow::onDeviceChanged()
{
    QAudioDeviceInfo audioDeviceInfo = mDeviceComboBox->currentData().value<QAudioDeviceInfo>();
    if (!audioDeviceInfo.isNull()) {
        mRecorder.setDevice(audioDeviceInfo);
    }

    //...
}

void MainWindow::onRefreshClicked(bool init)
{
    QString curDeviceName = init ?
                mSettings.value(SettingDeviceName).toString() :
                mDeviceComboBox->currentText();

    mDeviceComboBox->clear();
    foreach (QAudioDeviceInfo info, QAudioDeviceInfo::availableDevices(QAudio::AudioInput)) {
        if (!info.isNull()) {
            QString deviceName = QString("%1 (%2)")
                    .arg(info.deviceName())
                    .arg(info.realm());
            mDeviceComboBox->addItem(
                deviceName,
                QVariant::fromValue<QAudioDeviceInfo>(info)
            );
            if (deviceName == curDeviceName) {
                mDeviceComboBox->setCurrentIndex(mDeviceComboBox->count() - 1);
            }
        }
    }
}

void MainWindow::onConnectClicked()
{
    if (mClient.isActive()) {
        mClient.stop();
    } else {
        QUrl url(mHostNameEdit->text());
        mClient.start(url.host());
    }

    toggleConnected(mClient.isActive());
}

void MainWindow::onLog(LogType logType, const QString &message)
{
    QString formatColor;

    switch (logType) {
    case LogType::Info:
        formatColor = "#777";
        break;
    case LogType::Success:
        formatColor = "#070";
        break;
    case LogType::Error:
        formatColor = "#700";
        break;
    }

    mLogEdit->append(
        QString("[%1] <span style=\"color: %2;\">%3</span>")
            .arg(QDateTime::currentDateTime().toString())
            .arg(formatColor)
            .arg(message.toHtmlEscaped())
    );
}

void MainWindow::toggleConnected(bool connected)
{
    mDeviceComboBox->setEnabled(!connected);
    mRefreshButton->setEnabled(!connected);
    mHostNameEdit->setEnabled(!connected);
    mConnectionButton->setText(connected ? tr("Disconnect") : tr("Connect"));
}
