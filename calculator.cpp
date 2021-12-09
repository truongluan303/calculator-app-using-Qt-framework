#include "calculator.h"
#include "ui_calculator.h"
#include <iostream>
#include <string>
#include <sstream>
#include <math.h>


using namespace std;



string to_decimal(string, NumberMode);

string to_binary(string, NumberMode);

string to_hex(string, NumberMode);

string trim_zeros(string);




Calculator::Calculator(QWidget *parent) : QMainWindow(parent), ui(new Ui::Calculator)
{
    ui->setupUi(this);
    setFixedSize(size());

    current_num = "0";
    prev_num = "";
    operation = "";
    is_result = false;
    mode = floating_point;

    initialize_components();
}




Calculator::~Calculator()
{
    delete ui;
}





/**
 * @brief handle the event when a push button on the calculator is clicked
 */
void Calculator::calculator_button_clicked()
{
    // get the button that was clicked
    QPushButton* button_clicked = qobject_cast<QPushButton*>(sender());
    string button_text = button_clicked->text().toStdString();

    bool update_display = true;

    // if the button clicked is number, alphas or a floating point
    if (numbers.find(button_clicked) != numbers.end() ||
        button_clicked == ui->point_button ||
        alphas.find(button_clicked) != alphas.end())
    {
        if (is_result)
        {
            current_num = "0";
            is_result = false;
        }
        if (button_clicked != ui->point_button &&
           (to_decimal(current_num, mode) == "0" || current_num.compare("0") == 0))
        {
            current_num = "";
        }
        if (button_clicked == ui->point_button)
        {
            if (current_num.find(".") != string::npos)
            {
                button_text = "";
            }
        }
        current_num += button_text;
    }

    // if button clicked is an operator
    else if (operations.find(button_clicked) != operations.end())
    {
        operation = button_text;
        update_display = false;
        if (is_result || prev_num == "")
        {
            prev_num = current_num;
            current_num = "0";
        }
        button_clicked->setStyleSheet(
            QString::fromStdString(
            "background-color:rgb(50,50,50); color:rgb(255,255,255);"));
    }

    // if the button clicked is a trigonometric operation
    else if (trigs.find(button_clicked) != trigs.end())
    {
        double value = stod(current_num);
        if (button_clicked == ui->sin_button)
        {
            value = sin(value);
        }
        else if (button_clicked == ui->cos_button)
        {
            value = cos(value);
        }
        else if (button_clicked == ui->tan_button)
        {
            value = tan(value);
        }
        current_num = to_string(value);
        is_result = true;
    }

    // if button clicked is the equal sign
    else if (button_clicked == ui->equal_button)
    {
        if (prev_num != "")
        {
            string number = current_num;
            if (mode != floating_point)
            {
                number = to_decimal(current_num, mode);
                prev_num = to_decimal(prev_num, mode);
            }

            double result = 0;
            double num1 = stod(prev_num);
            double num2 = stod(number);

            if (operation.compare(ADD) == 0)
                result = num1 + num2;
            else if (operation.compare(SUB) == 0)
                result = num1 - num2;
            else if (operation.compare(MUL) == 0)
                result = num1 * num2;
            else if (operation.compare(DIV) == 0)
                result = num1 / num2;

            number = to_string(result);

            if (mode == floating_point)
            {
                number = trim_zeros(number);
            }
            else
            {
                int value = round(stod(number));
                number = to_string(value);
            }

            is_result = true;
            prev_num = "";
            operation = "";

            current_num = number;

            if (mode != floating_point)
            {
                ui->dec_label->setText(QString::fromStdString(
                                           to_decimal(current_num, mode)));
                ui->bin_label->setText(QString::fromStdString(
                                           to_binary(current_num, mode)));
                ui->hex_label->setText(QString::fromStdString(
                                           to_hex(current_num, mode)));
                if (mode == binary)
                {
                    current_num = to_binary(number, decimal);
                }
                else if (mode == hexadecimal)
                {
                    current_num = to_hex(number, decimal);
                }
            }
        }
        else if (mode != floating_point)
        {
            string dec = to_decimal(current_num, mode);
            string bin = to_binary(current_num, mode);
            string hex = to_hex(current_num, mode);
            ui->dec_label->setText(QString::fromStdString(dec));
            ui->bin_label->setText(QString::fromStdString(bin));
            ui->hex_label->setText(QString::fromStdString(hex));
        }

        reset_operation_buttons_color();
    }

    // if button clicked is the sign button "+/-"
    else if (button_clicked == ui->sign_button)
    {
        if (stod(current_num) == 0)
        {
            return;
        }
        if (current_num[0] == '-')
        {
            current_num = current_num.substr(1, current_num.length());
        }
        else
        {
            current_num = '-' + current_num;
        }

        if (mode != floating_point)
        {
            string dec = to_decimal(current_num, mode);
            string bin = to_binary(current_num, mode);
            string hex = to_hex(current_num, mode);
            ui->dec_label->setText(QString::fromStdString(dec));
            ui->bin_label->setText(QString::fromStdString(bin));
            ui->hex_label->setText(QString::fromStdString(hex));
        }
    }

    // if button clicked is the inverse button "1/x"
    else if (button_clicked == ui->inverse_button)
    {
        double value = stod(current_num);
        if (value != 0)
        {
            value = 1 / value;
            current_num = to_string(value);
        }
    }

    // if button clicked is the percentage button
    else if (button_clicked == ui->percent_button)
    {
        if (stod(current_num) == 0)
        {
            return;
        }
        double value = stod(current_num);
        value = value / 100;
        current_num = trim_zeros(to_string(value));
    }

    // if button clicked is the delete button
    else if (button_clicked == ui->del_button)
    {
        if (current_num.length() < 2)
            current_num = "0";
        else
        {
            current_num = current_num.substr(0, current_num.length() - 1);
            if (current_num.compare("-") == 0)
            {
                current_num = "0";
            }
        }
    }

    // if button clicked is the clear button
    else if (button_clicked == ui->clear_button)
    {
        if (mode != floating_point)
        {
            ui->dec_label->setText(QString::fromStdString("0"));
            ui->bin_label->setText(QString::fromStdString("0"));
            ui->hex_label->setText(QString::fromStdString("0"));
        }
        current_num = "0";
        prev_num = "";
    }


    // if the update flag is on, then update the number displayed
    if (update_display)
    {
        ui->number_display->setText(QString::fromStdString(current_num));
    }
}





/**
 * @brief handle the event when a number mode radio button is clicked.
 */
void Calculator::number_mode_toggled()
{
    // get the button that was clicked
    QRadioButton* button_clicked = qobject_cast<QRadioButton*>(sender());

    // if float mode is chosen
    if (button_clicked == ui->float_mode)
    {
        if (mode != floating_point)
        {
            current_num = "0";
        }
        mode = floating_point;
        for (QPushButton* button : ui->buttons_container->findChildren<QPushButton*>())
        {
            button->setEnabled(true);
            if (alphas.find(button) != alphas.end())
            {
                // alphas are not allowed in floating point numbers
                button->setEnabled(false);
            }
        }
        // disable decimal, binary, and hex converting
        ui->dec_label->setText("");
        ui->bin_label->setText("");
        ui->hex_label->setText("");
    }

    else {
        if (mode == floating_point)
        {
            current_num = "0";
            prev_num = "";
            operation = "";
            is_result = false;
            mode = decimal;
            QMessageBox::information(this, tr("Non-floating-point Mode Notice"),
                tr("Any floating point number caused by division will be rounded to decimal"),
                QMessageBox::Ok);
        }

        // if decimal mode is chosen
        if (button_clicked == ui->decimal_mode)
        {
            current_num = to_decimal(current_num, mode);
            prev_num = to_decimal(prev_num, mode);
            mode = decimal;
            for (QPushButton* button : ui->buttons_container->findChildren<QPushButton*>())
            {
                button->setEnabled(false);
                // only numbers are allowed in decimal mode
                if (numbers.find(button) != numbers.end())
                {
                    button->setEnabled(true);
                }
            }
        }

        // if binary mode is chosen
        else if (button_clicked == ui->binary_mode)
        {
            current_num = to_binary(current_num, mode);
            prev_num = to_binary(current_num, mode);
            mode = binary;
            for (QPushButton* button : ui->buttons_container->findChildren<QPushButton*>())
            {
                button->setEnabled(false);
                // only 0 and 1 are allowed in binary mode
                if (button == ui->one_button || button == ui->zero_button)
                {
                    button->setEnabled(true);
                }
            }
        }

        // if hex mode is chosen
        else if (button_clicked == ui->hex_mode)
        {
            current_num = to_hex(current_num, mode);
            prev_num = to_hex(current_num, mode);
            mode = hexadecimal;
            for (QPushButton* button : ui->buttons_container->findChildren<QPushButton*>())
            {
                button->setEnabled(false);
                // alphas and numbers are allowed in hex
                if (alphas.find(button) != alphas.end() || numbers.find(button) != numbers.end())
                {
                    button->setEnabled(true);
                }
            }
        }
    }

    ui->number_display->setText(QString::fromStdString(current_num));
    is_result = true;
    prev_num = "";

    // basic operations, equal, clear, and del buttons must always be enabled
    for (QPushButton* op_button : operations)
    {
        op_button->setEnabled(true);
    }
    ui->sign_button->setEnabled(true);
    ui->equal_button->setEnabled(true);
    ui->del_button->setEnabled(true);
    ui->clear_button->setEnabled(true);
}





/**
 * @brief change the operation buttons to their original color
 */
void Calculator::reset_operation_buttons_color()
{
    for (QPushButton* button : operations)
    {
        button->setStyleSheet(
            QString::fromStdString(
            "background-color:rgb(100,100,100); color:rgb(255,255,255);"));
    }
}





/**
 * @brief initialize all the components on the main window
 */
void Calculator::initialize_components()
{
    QList<QPushButton *> all_buttons = ui->buttons_container->findChildren<QPushButton*>();
    for (QPushButton* button : all_buttons)
    {
        // make the font size bigger
        QFont font = button->font();
        font.setPointSize(15);
        font.setWeight(100);
        button->setFont(font);

        // handle click event
        connect(button, SIGNAL(clicked()), this, SLOT(calculator_button_clicked()));
    }

    numbers.insert(ui->zero_button);
    numbers.insert(ui->one_button);
    numbers.insert(ui->two_button);
    numbers.insert(ui->three_button);
    numbers.insert(ui->four_button);
    numbers.insert(ui->five_button);
    numbers.insert(ui->six_button);
    numbers.insert(ui->seven_button);
    numbers.insert(ui->eight_button);
    numbers.insert(ui->nine_button);

    alphas.insert(ui->a_button);
    alphas.insert(ui->b_button);
    alphas.insert(ui->c_button);
    alphas.insert(ui->d_button);
    alphas.insert(ui->e_button);
    alphas.insert(ui->f_button);
    for (QPushButton* button : alphas)
        button->setEnabled(false);

    operations.insert(ui->add_button);
    operations.insert(ui->sub_button);
    operations.insert(ui->mul_button);
    operations.insert(ui->div_button);

    trigs.insert(ui->sin_button);
    trigs.insert(ui->cos_button);
    trigs.insert(ui->tan_button);

    QList<QRadioButton*> radio_buttons = ui->number_mode_panel->findChildren<QRadioButton*>();
    for (QRadioButton* radio_button : radio_buttons)
    {
        // handle toggle event
        connect(radio_button, SIGNAL(clicked()), this, SLOT(number_mode_toggled()));
    }

    ui->float_mode->setChecked(true);
}





/**
 * @brief convert a number to decimal
 * @param current_num - the number to be converted
 * @param mode - the mode that the given number is in
 * @return the string version of the converted number
 */
string to_decimal(string current_num, NumberMode mode)
{
    string result;
    bool is_negative = false;
    if (current_num[0] == '-')
    {
        current_num = current_num.substr(1, current_num.length() - 1);
        is_negative = true;
    }

    if (mode == decimal)
    {
        result = current_num;
    }

    // convert binary to decimal
    else if (mode == binary)
    {
        int dec_number = stoi(current_num, 0, 2);
        result = to_string(dec_number);
    }

    // convert hex to decimal
    else if (mode == hexadecimal)
    {
        stringstream stream;
        int dec_number;
        stream << current_num;
        stream >> hex >> dec_number;
        result = to_string(dec_number);
    }

    if (is_negative)
    {
        result = '-' + result;
    }
    return result;
}





/**
 * @brief convert a number to binary
 * @param current_num - the number to be converted
 * @param mode - the mode that the given number is in
 * @return the string version of the converted number
 */
string to_binary(string current_num, NumberMode mode)
{
    string result;
    bool is_negative = false;
    if (current_num[0] == '-')
    {
        current_num = current_num.substr(1, current_num.length() - 1);
        is_negative = true;
    }

    // convert decimal to binary
    if (mode == decimal)
    {
        int dec = stoi(current_num);
        if (dec == 0)
        {
            result = "0";
        }
        while (dec != 0)
        {
            result = ((dec % 2 == 0) ? "0" : "1") + result;
            dec /= 2;
        }
    }

    else if (mode == binary)
    {
        result = current_num;
    }

    // convert hex to binary
    else if (mode == hexadecimal)
    {
        // convert from hex to decimal then convert from decimal to binary
        string dec = to_decimal(current_num, hexadecimal);
        result = to_binary(dec, decimal);
    }

    if (is_negative)
    {
        result = '-' + result;
    }

    return result;
}





/**
 * @brief convert a number to hexadecimal
 * @param current_num - the number to be converted
 * @param mode - the mode that the given number is in
 * @return the string version of the converted number
 */
string to_hex(string current_num, NumberMode mode)
{
    string result;
    bool is_negative = false;
    if (current_num[0] == '-')
    {
        current_num = current_num.substr(1, current_num.length() - 1);
        is_negative = true;
    }

    // convert decimal to hex
    if (mode == decimal)
    {
        int dec = stoi(current_num);
        stringstream stream;
        stream << hex << dec;
        stream >> result;
    }

    // convert binary to hex
    else if (mode == binary)
    {
        // convert from binary to decimal then from decimal to hex
        string dec = to_decimal(current_num, binary);
        result = to_hex(dec, decimal);
    }

    else if (mode == hexadecimal)
    {
        result = current_num;
    }

    // make upper case
    transform(result.begin(), result.end(), result.begin(), ::toupper);

    if (is_negative)
    {
        result = '-' + result;
    }
    return result;
}





/**
 * @brief remove the trailing zeros in a float
 * @param flnum - the string version of the float
 * @return the trimmed float
 */
string trim_zeros(string flnum)
{
    // remove the trailing zeros
    flnum = flnum.substr(0, flnum.find_last_not_of('0') + 1);
    if (flnum.compare("0") == 0)
    {
        return "0";
    }
    if (flnum.find('.') == flnum.size() - 1)
    {
        // if floating point is the last char then remove it
        flnum = flnum.substr(0, flnum.size() - 1);
    }
    return flnum;
}
