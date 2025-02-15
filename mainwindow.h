#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void download_clicked();
    void processDownloadOutput();
    void processCore();
    void makeExecutable(const QString &filePath);

private:
    Ui::MainWindow *ui;
    QLabel* statusLabel;
    QLineEdit* urlLineEdit;
    QPushButton* downloadButton;
    QComboBox* formatSelector;
    QProcess* process;
};
#endif // MAINWINDOW_H
