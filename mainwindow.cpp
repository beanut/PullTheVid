#include "mainwindow.h"
#include "./ui_mainwindow.h"

// Layout Items
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFileDialog>

// File system
#include <QFile>

#include <QDebug>
#include <QRegularExpression>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , process(new QProcess(this))
{
    ui->setupUi(this);

    // Download status label
    statusLabel = new QLabel("Waiting for input...");
    // Download Button
    downloadButton = new QPushButton("Download", this);
    connect(downloadButton, &QPushButton::clicked, this, &MainWindow::download_clicked);

    // URL Input
    urlLineEdit = new QLineEdit(this);
    urlLineEdit->setPlaceholderText("Enter URL...");

    // Format Selector
    formatSelector = new QComboBox(this);
    formatSelector->addItems({"mp4", "avi", "flv", "gif", "mkv", "mov", "webm", "aac", "aiff", "alac",
                              "flac", "m4a", "mka", "mp3", "ogg", "opus", "vorbis", "wav"});

    // Layout
    QVBoxLayout* vStack = new QVBoxLayout;
    QHBoxLayout* hStack = new QHBoxLayout;

    vStack->addStretch();
    vStack->addWidget(urlLineEdit);
    vStack->addWidget(formatSelector);
    vStack->addWidget(downloadButton);
    vStack->addStretch();
    vStack->addWidget(statusLabel);
    vStack->addStretch();

    hStack->addStretch();
    hStack->addLayout(vStack);
    hStack->addStretch();

    ui->centralwidget->setLayout(hStack);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::download_clicked() {
    qDebug() << "Download clicked!";
    processCore();
}

void MainWindow::makeExecutable(const QString &filePath) {
    QFile file(filePath);

    if (!file.exists()) {
        qDebug() << "File does not exist:" << filePath;
        return;
    }

    // Get current permissions
    QFile::Permissions permissions = file.permissions();

    // Add executable permission for the owner
    permissions |= QFile::ExeUser;

    // Set the new permissions
    if (file.setPermissions(permissions)) {
        qDebug() << "Executable permission set for:" << filePath;
    } else {
        qDebug() << "Failed to set executable permission for:" << filePath;
    }
}

void MainWindow::processCore() {
    QString videoUrl = urlLineEdit->text();

    if (videoUrl.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a URL.");
        return;
    }

    QString selectedFormat = formatSelector->currentText();
    QString corePath = QCoreApplication::applicationDirPath() + "/yt-dlp";

#ifdef Q_OS_WIN
    corePath += ".exe";
#endif

    makeExecutable(corePath);

    qDebug() << "Core Path: " << corePath;


    // Check if yt-dlp exists
    if (!QFile::exists(corePath)) {
        QMessageBox::warning(this, "Error", "Couldn't locate yt-dlp in " + corePath);
        return;
    }

    QString savePath = QFileDialog::getExistingDirectory(this, "Select save directory");
    if (savePath.isEmpty()) return;

    // Debugging: Print arguments
    QStringList args;
    args << "-f" << selectedFormat
         << "--merge-output-format" << selectedFormat
         << "--no-part"
         << "--progress"
         << "-o" << savePath + "/%(title)s.%(ext)s"
         << videoUrl;

    qDebug() << "Arguments: " << args;

    statusLabel->setText("Processing...");

    // Connect process signals
    connect(process, &QProcess::readyReadStandardOutput, this, &MainWindow::processDownloadOutput);
    connect(process, &QProcess::readyReadStandardError, this, [this]() {
        QString errorOutput = process->readAllStandardError();
        qDebug() << "Error: " << errorOutput;
        statusLabel->setText("Error: " + errorOutput);
    });
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this](int exitCode, QProcess::ExitStatus status) {
                if (exitCode == 0)
                    statusLabel->setText("Download complete!");
                else
                    statusLabel->setText("Download failed.");
            });

    // Start the process
    process->start(corePath, args);

    if (!process->waitForStarted()) {
        QMessageBox::critical(this, "Error", "Failed to start yt-dlp process. Ensure yt-dlp is in the correct directory and has execute permissions.");
        return;
    }
}

void MainWindow::processDownloadOutput() {
    QString output = process->readAllStandardOutput();
    qDebug() << output;

    QRegularExpression regex(R"(\[download\]\s+(\d+)%\s+)");
    QRegularExpressionMatch match = regex.match(output);
    if (match.hasMatch()) {
        int progress = match.captured(1).toInt();
        statusLabel->setText("Progress: " + QString::number(progress));
    }
}
