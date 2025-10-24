#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    digitBtns = {
        {Qt::Key_0,ui->btnNum0},
        {Qt::Key_1,ui->btnNum1},
        {Qt::Key_2,ui->btnNum2},
        {Qt::Key_3,ui->btnNum3},
        {Qt::Key_4,ui->btnNum4},
        {Qt::Key_5,ui->btnNum5},
        {Qt::Key_6,ui->btnNum6},
        {Qt::Key_7,ui->btnNum7},
        {Qt::Key_8,ui->btnNum8},
        {Qt::Key_9,ui->btnNum9}
    };

    foreach(auto btn,digitBtns)
        connect(btn,SIGNAL(clicked()),this,SLOT(btnNumClicked()));

    connect(ui->btnPlus,SIGNAL(clicked()),this,SLOT(btnBinaryOperatorClicked()));
    connect(ui->btnSubtract,SIGNAL(clicked()),this,SLOT(btnBinaryOperatorClicked()));
    connect(ui->btnMultiple,SIGNAL(clicked()),this,SLOT(btnBinaryOperatorClicked()));
    connect(ui->btnDivide,SIGNAL(clicked()),this,SLOT(btnBinaryOperatorClicked()));

    connect(ui->btnInverse,SIGNAL(clicked()),this,SLOT(btnUnaryOperatorClicked()));
    connect(ui->btnPercentage,SIGNAL(clicked()),this,SLOT(btnUnaryOperatorClicked()));
    connect(ui->btnSquare,SIGNAL(clicked()),this,SLOT(btnUnaryOperatorClicked()));
    connect(ui->btnSqrt,SIGNAL(clicked()),this,SLOT(btnUnaryOperatorClicked()));
    connect(ui->btnSign,SIGNAL(clicked()),this,SLOT(btnUnaryOperatorClicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::btnNumClicked()   //数字按键
{
    QString num = qobject_cast<QPushButton *>(sender())->text();

    if(operand == '0' && num == '0')   //操作数为0时点击0，不变
        num = "";
    else if(operand == '0')   //操作数为0时点击非0数，操作数变为点击的数
        operand = num;
    else   //操作数不为0，将数字直接追加到操作数末尾
        operand += num;

    ui->display->setText(operand);
}

void MainWindow::btnBinaryOperatorClicked()   //双操作数运算符
{
    if(operand != ""){   //更新算式，记录并清空操作数和记录操作符
        opcode = qobject_cast<QPushButton *>(sender())->text();
        equation += operand + ' ' + opcode + ' ';
        ui->display2->setText(equation);
        operands.enqueue(operand);
        opcodes.enqueue(opcode);
        operand = "";
    }
    else if(equation != ""){   //更改算式和队列的操作符
        opcode = qobject_cast<QPushButton *>(sender())->text();
        equation = equation.left(equation.length() - 2) + opcode + ' ';
        ui->display2->setText(equation);
        opcodes.dequeue();
        opcodes.enqueue(opcode);
    }
    else{   //操作数和算式都为空，则第一个操作数为0
        operand = "0";
        operands.enqueue(operand);
        opcode = qobject_cast<QPushButton *>(sender())->text();
        equation += operand + ' ' + opcode + ' ';
        ui->display2->setText(equation);
        opcodes.enqueue(opcode);
        operand = "";
    }
}

void MainWindow::btnUnaryOperatorClicked()   //单操作数运算符
{
    if(operand == "" && !operands.isEmpty())   //如果操作数为空，则上一个操作数作为这一次运算的操作数
        operand = operands.back();

    opcode = qobject_cast<QPushButton *>(sender())->text();
    if(opcode == "%"){
        equation = equation.left(equation.length() - operand.length() + 1);
        QString result = operand + "%";
        operand = QString::number( operand.toDouble() / 100.0 );
        ui->display2->setText(equation + result);
        ui->display->setText(operand);
    }
    else if(opcode == "1/x"){
        if(operand == "0")
            ui->display->setText("0没有倒数");
        else{
            equation = equation.left(equation.length() - operand.length() + 1);
            QString result = "1/(" + operand + ")";
            operand = QString::number( 1.0 / operand.toDouble() );
            ui->display2->setText(equation + result);
            ui->display->setText(operand);
        }
    }
    else if(opcode == "x²"){
        equation = equation.left(equation.length() - operand.length() + 1);
        QString result = "(" + operand + ")²";
        operand = QString::number( pow(operand.toDouble(),2) );
        ui->display2->setText(equation + result);
        ui->display->setText(operand);
    }
    else if(opcode == "√"){
        equation = equation.left(equation.length() - operand.length() + 1);
        QString result = "sqrt(" + operand + ")";
        operand = QString::number( sqrt(operand.toDouble()) );
        ui->display2->setText(equation + result);
        ui->display->setText(operand);
    }
    else if(opcode == "+/-"){
        equation = equation.left(equation.length() - operand.length() + 1);
        QString result = "-(" + operand + ")";
        operand = QString::number( -1.0 * operand.toDouble() );
        ui->display2->setText(equation + result);
        ui->display->setText(operand);
    }
}

void MainWindow::on_btnPoint_clicked()   //小数点
{
    if(operand == "")   //操作数为空时默认为0
        operand = "0.";
    else if(!operand.contains('.'))
        operand += '.';
    ui->display->setText(operand);
}

void MainWindow::on_btnDel_clicked()   //退格
{
    operand = operand.left(operand.length() - 1);
    ui->display->setText(operand);
}

void MainWindow::on_btnClearAll_clicked()   //C
{
    ui->display->clear();
    ui->display2->clear();
    equation.clear();
    operands.clear();
    opcodes.clear();
    operand = "";
    opcode = "";
}

void MainWindow::on_btnClear_clicked()   //CE
{
    operand.clear();
    ui->display->setText(operand);
}

void MainWindow::on_btnEqual_clicked()   //等于号
{
    if(operand == "" && equation != "")   //如果按下四则运算符后没有输出操作数就直接按下等于号
        opcodes.dequeue();

    else if(operand != ""){   //更新算式，将最后一个操作数入队并清空
        equation += operand + " = ";
        ui->display2->setText(equation);
        operands.enqueue(operand);
        operand = "";
    }

    int size = operands.size();
    if(size < 2){   //如果操作数只有一个或者没有，则不用进行计算
        if(operands.isEmpty()){
            equation = "0 = ";
            opcodes.enqueue("=");
        }
        else{
            operand = operands.dequeue();
            equation = operand + " = ";
        }
        ui->display2->setText(equation);
        equation.clear();
    }

    else{   //计算算式
        double num1,num2;
        //考虑到四则运算的优先级，进行两轮计算，先处理乘除法，再处理加减法
        for(int i=1 ; i<size ; i++){
            opcode = opcodes.dequeue();
            if(opcode == '*'){
                num1 = operands.dequeue().toDouble();
                num2 = operands.dequeue().toDouble();
                num1 = num1 * num2;
                operands.prepend(QString::number(num1));   //计算结果重新回到队头
            }
            else if(opcode == '/'){
                num1 = operands.dequeue().toDouble();
                num2 = operands.dequeue().toDouble();

                if(num2 == 0){   //除数不能为0,清空操作数和运算符
                    ui->display->setText("除数不能为0");
                    operands.clear();
                    opcodes.clear();
                    break;
                }
                num1 = num1 / num2;
                operands.prepend(QString::number(num1));
            }
            else{
                operands.enqueue(operands.dequeue());
                opcodes.enqueue(opcode);
            }
        }
        operands.enqueue(operands.dequeue());   //将最后一个操作数也放入队尾

        //加减法运算
        if(operands.size() == 1){   //如果算式没有加减法，那么操作数会只剩一个
            operand = operands.front();
            ui->display->setText(operand);
            opcode = "";
            equation.clear();
        }
        else{
            num1 = operands.dequeue().toDouble();

            while(!operands.isEmpty()){
                opcode = opcodes.dequeue();
                if(opcode == '+'){
                    num2 = operands.dequeue().toDouble();
                    num1 = num1 + num2;
                }
                else if(opcode == '-'){
                    num2 = operands.dequeue().toDouble();
                    num1 = num1 - num2;
                }
            }
            operand = QString::number(num1);   //将运算结果作为下一次运算的第一个操作数
            ui->display->setText(operand);
            opcode = "";
            equation.clear();
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    foreach(auto btnKey,digitBtns.keys())
        if(event->key() == btnKey)
            digitBtns[btnKey]->animateClick();

    switch(event->key()){
    case Qt::Key_Plus : ui->btnPlus->animateClick(); break;
    case Qt::Key_Minus : ui->btnSubtract->animateClick(); break;
    case Qt::Key_Asterisk : ui->btnMultiple->animateClick(); break;
    case Qt::Key_Slash : ui->btnDivide->animateClick(); break;
    case Qt::Key_Equal :
    case Qt::Key_Return :
    case Qt::Key_Enter : ui->btnEqual->animateClick(); break;
    case Qt::Key_Period : ui->btnPoint->animateClick(); break;
    case Qt::Key_Backspace : ui->btnDel->animateClick(); break;
    case Qt::Key_Escape : ui->btnClearAll->animateClick(); break;
    case Qt::Key_AsciiCircum : ui->btnSquare->animateClick(); break;
    case Qt::Key_Percent : ui->btnPercentage->animateClick(); break;
    }
}
