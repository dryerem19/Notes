#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "stringtable.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QString _fileName;
    StringTable _stringTable;

    bool _fieldIdEmpty{true};
    bool _fieldValueEmpty{true};

    void setupUI();
    void setupTable();
    void setupConnections();
    void refreshTable();
    void lockAddUpdateButton();
    void unlockAddUpdateButton();

    void saveFile(QString fileName);
    void showStatus(const QString &message, int timeout = 0);

private slots:
    void onAddUpdatePushButtonClicked();
    void onRemovePushButtonClicked();

    void onLoadActionTriggered();
    void onSaveActionTriggered();
    void onSaveAsActionTriggered();
};
#endif // MAINWINDOW_H
