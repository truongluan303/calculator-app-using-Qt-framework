#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QPushButton"
#include "QMessageBox"
#include <unordered_set>

QT_BEGIN_NAMESPACE
namespace Ui { class Calculator; }
QT_END_NAMESPACE


using namespace std;


enum NumberMode { decimal, binary, hexadecimal, floating_point };


class Calculator : public QMainWindow
{
    Q_OBJECT

public:
    Calculator(QWidget *parent = nullptr);
    ~Calculator();

private:
    Ui::Calculator *ui;

    string current_num;
    string prev_num;
    string operation;
    bool is_result;
    NumberMode mode;

    void initialize_components();
    void reset_operation_buttons_color();

    unordered_set<QPushButton*> numbers;
    unordered_set<QPushButton*> alphas;
    unordered_set<QPushButton*> operations;
    unordered_set<QPushButton*> trigs;

    const string ADD = "+";
    const string SUB = "-";
    const string MUL = "x";
    const string DIV = "/";


private slots:

    void calculator_button_clicked();
    void number_mode_toggled();

};

#endif // MAINWINDOW_H
