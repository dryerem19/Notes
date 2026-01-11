#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setupUI();
    setupConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    showStatus("Готов к работе");
    ui->saveFileAction->setEnabled(false);
    setupTable();
}

void MainWindow::setupTable()
{
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void MainWindow::setupConnections()
{
    connect(ui->loadFileAction, &QAction::triggered, this, &MainWindow::onLoadActionTriggered);
    connect(ui->saveFileAction, &QAction::triggered, this, &MainWindow::onSaveActionTriggered);
    connect(ui->saveFileAsAction, &QAction::triggered, this, &MainWindow::onSaveAsActionTriggered);

    connect(ui->updPushButton, &QPushButton::clicked, this, &MainWindow::onAddUpdatePushButtonClicked);
    connect(ui->removePushButton, &QPushButton::clicked, this, &MainWindow::onRemovePushButtonClicked);

    connect(ui->tableWidget, &QTableWidget::itemSelectionChanged, this, [this](){
        int currentRow = ui->tableWidget->currentRow();
        if (currentRow != -1) {
            ui->removePushButton->setEnabled(true);
        } else {
            ui->removePushButton->setEnabled(false);
        }
    });

    connect(ui->idLineEdit, &QLineEdit::textChanged, this, [this](){
       QString tid = ui->idLineEdit->text().trimmed();
       if (!tid.isEmpty()) {
           _fieldIdEmpty = false;
           unlockAddUpdateButton();
       } else {
           lockAddUpdateButton();
       }
    });

    connect(ui->plainTextEdit, &QPlainTextEdit::textChanged, this, [this](){
        QString tvalue = ui->plainTextEdit->toPlainText().trimmed();
        if (!tvalue.isEmpty()) {
            _fieldValueEmpty = false;
            unlockAddUpdateButton();
        } else {
            lockAddUpdateButton();
        }
    });
}

void MainWindow::refreshTable()
{
    ui->tableWidget->setRowCount(0);

    QStringList ids = _stringTable.getAllIds();
    ui->tableWidget->setRowCount(ids.size());

    for (int row = 0; row < ids.size(); row++)
    {
        QString id = ids[row];
        QString value = _stringTable.getString(id);

        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(id));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(value));
    }
}

void MainWindow::lockAddUpdateButton()
{
    _fieldIdEmpty = true;
    _fieldValueEmpty = true;
    ui->updPushButton->setEnabled(false);
}

void MainWindow::unlockAddUpdateButton()
{
    if (_fieldIdEmpty || _fieldValueEmpty) {
        return;
    }

    ui->updPushButton->setEnabled(true);
}

void MainWindow::saveFile(QString fileName)
{
    if (_stringTable.size() == 0) {
        QMessageBox::information(this, tr("Информация"), tr("Нет данных для сохранения!"));
        return;
    }

    if (fileName.isEmpty()) {
        QMessageBox::critical(this, tr("Ошибка"),  tr("Некорректный путь сохранения!"));
        return;
    }

    if (!fileName.endsWith(".json", Qt::CaseInsensitive)) {
        fileName += ".json";
    }

    if (_stringTable.saveToFile(fileName)) {
        _fileName = fileName;
        ui->saveFileAction->setEnabled(true);
        showStatus(QString("Файл %1 был успешно сохранен").arg(_fileName));
    } else {
        showStatus(QString("Ошибка! Не удалось сохранить файл %1").arg(_fileName));
    }
}

void MainWindow::showStatus(const QString &message, int timeout)
{
    ui->statusbar->showMessage(message, timeout);
}

void MainWindow::onAddUpdatePushButtonClicked()
{
    QString id = ui->idLineEdit->text().trimmed();
    QString value = ui->plainTextEdit->toPlainText().trimmed();

    if (id.isEmpty()) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Поле ИД не может быть пустым!"));
        return;
    }

    if (value.isEmpty()) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Поле Значение не может быть пустым!"));
        return;
    }

    _stringTable.addString(id, value);
    showStatus(QString("Добавлено/обновлено поле с ID %1").arg(id));
    refreshTable();

    ui->idLineEdit->clear();
    ui->plainTextEdit->clear();
    ui->idLineEdit->setFocus();

    lockAddUpdateButton();
}

void MainWindow::onRemovePushButtonClicked()
{
    int currentRow = ui->tableWidget->currentRow();

    if (currentRow == -1) {
        QMessageBox::information(this, tr("Информация"), tr("Пожалуйста, выберите строку для удаления."));
        return;
    }

    QString id = ui->tableWidget->item(currentRow, 0)->text();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Подтвердите удаление"),
                                  QString("Вы уверены, что хотите удалить '%1'?").arg(id),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (_stringTable.removeString(id)) {
            refreshTable();
            showStatus(QString("Удалено поле с ID %1").arg(id));
        }
    }
}

void MainWindow::onLoadActionTriggered()
{
    QMessageBox::StandardButton reply = QMessageBox::Yes;
    if (!_fileName.isEmpty())
    {
        reply = QMessageBox::question(this, tr("Вы пытаетесь открыть другой файл"),
                                      QString("Несохраненные изменения могут быть потеряны! Продолжить?"),
                                      QMessageBox::Yes | QMessageBox::No);
    }

    if (reply == QMessageBox::No) {
        return;
    }

    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Открыть файл таблицы строк"), "", "JSON Files (*.json);;All Files (*)");

    if (fileName.isEmpty()) {
        return;
    }

    if (_stringTable.loadFromFile(fileName)) {
        refreshTable();
        _fileName = fileName;
        ui->saveFileAction->setEnabled(true);
        showStatus(QString("Файл %1 был открыт").arg(_fileName));
    } else {
        showStatus(QString("Ошибка! Не удалось открыть файл %1").arg(_fileName));
    }
}

void MainWindow::onSaveActionTriggered()
{
    if (!_fileName.isEmpty()) {
        saveFile(_fileName);
    }
}

void MainWindow::onSaveAsActionTriggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить файл таблицы строк как..."),
                               QDir::currentPath(),
                               tr("JSON (*.json)"));
    if (fileName.isEmpty()) {
        return;
    }

    saveFile(fileName);
}

