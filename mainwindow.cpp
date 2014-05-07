#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "predictor.h"

static Predictor predictor;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString str = this->ui->textEdit->toPlainText();
    this->ui->listWidget->clear();
    this->ui->listWidget->addItem("Processing...");
    this->ui->listWidget->repaint();
    this->ui->listWidget->repaint(); // One time repaint is now enough somehow
    QVector<QString> words = predictor.correct(str);
    this->ui->listWidget->clear();
    for (int i = 0; i < words.size(); i++) {
        this->ui->listWidget->addItem(words[i]);
    }
}

void MainWindow::on_textEdit_textChanged()
{
    QString str = this->ui->textEdit->toPlainText();
    this->ui->listWidget->clear();
    this->ui->listWidget->addItem("Processing...");
    //QMessageBox::information(0, "error", "Here");
    QVector<QString> words = predictor.predict(str);
    this->ui->listWidget->clear();
    for (int i = 0; i < words.size(); i++) {
        this->ui->listWidget->addItem(words[i]);
    }
}
