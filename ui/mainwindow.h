#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>
#include <QTreeWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QVBoxLayout>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:

};
#endif // MAINWINDOW_H
