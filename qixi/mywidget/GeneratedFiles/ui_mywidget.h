/********************************************************************************
** Form generated from reading UI file 'mywidget.ui'
**
** Created by: Qt User Interface Compiler version 5.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MYWIDGET_H
#define UI_MYWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_mywidgetClass
{
public:
    QLabel *label;
    QTextEdit *textEditConnect;
    QLabel *label_2;
    QLineEdit *CarNo;
    QFrame *frame;
    QFrame *frame_2;
    QFrame *frame_3;
    QWidget *formLayoutWidget;
    QGridLayout *gridLayout;
    QPushButton *manualstart_2;
    QPushButton *pushButtoninit;
    QPushButton *ConNectPLC;
    QPushButton *manualstop_2;
    QPushButton *pushButtonstop;
    QHBoxLayout *horizontalLayout;

    void setupUi(QWidget *mywidgetClass)
    {
        if (mywidgetClass->objectName().isEmpty())
            mywidgetClass->setObjectName(QStringLiteral("mywidgetClass"));
        mywidgetClass->resize(586, 402);
        label = new QLabel(mywidgetClass);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(210, 10, 151, 16));
        textEditConnect = new QTextEdit(mywidgetClass);
        textEditConnect->setObjectName(QStringLiteral("textEditConnect"));
        textEditConnect->setGeometry(QRect(340, 70, 241, 311));
        label_2 = new QLabel(mywidgetClass);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 40, 36, 16));
        CarNo = new QLineEdit(mywidgetClass);
        CarNo->setObjectName(QStringLiteral("CarNo"));
        CarNo->setGeometry(QRect(50, 40, 133, 20));
        frame = new QFrame(mywidgetClass);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setGeometry(QRect(0, 70, 331, 311));
        frame->setFrameShape(QFrame::Box);
        frame->setFrameShadow(QFrame::Raised);
        frame->setMidLineWidth(0);
        frame_2 = new QFrame(frame);
        frame_2->setObjectName(QStringLiteral("frame_2"));
        frame_2->setGeometry(QRect(0, 80, 111, 231));
        frame_2->setFrameShape(QFrame::Box);
        frame_2->setFrameShadow(QFrame::Raised);
        frame_3 = new QFrame(frame);
        frame_3->setObjectName(QStringLiteral("frame_3"));
        frame_3->setGeometry(QRect(220, 80, 111, 231));
        frame_3->setFrameShape(QFrame::Box);
        frame_3->setFrameShadow(QFrame::Raised);
        formLayoutWidget = new QWidget(frame);
        formLayoutWidget->setObjectName(QStringLiteral("formLayoutWidget"));
        formLayoutWidget->setGeometry(QRect(0, 0, 331, 311));
        gridLayout = new QGridLayout(formLayoutWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        manualstart_2 = new QPushButton(formLayoutWidget);
        manualstart_2->setObjectName(QStringLiteral("manualstart_2"));

        gridLayout->addWidget(manualstart_2, 1, 0, 1, 1);

        pushButtoninit = new QPushButton(formLayoutWidget);
        pushButtoninit->setObjectName(QStringLiteral("pushButtoninit"));

        gridLayout->addWidget(pushButtoninit, 0, 1, 1, 1);

        ConNectPLC = new QPushButton(formLayoutWidget);
        ConNectPLC->setObjectName(QStringLiteral("ConNectPLC"));

        gridLayout->addWidget(ConNectPLC, 1, 2, 1, 1);

        manualstop_2 = new QPushButton(formLayoutWidget);
        manualstop_2->setObjectName(QStringLiteral("manualstop_2"));

        gridLayout->addWidget(manualstop_2, 2, 0, 1, 1);

        pushButtonstop = new QPushButton(formLayoutWidget);
        pushButtonstop->setObjectName(QStringLiteral("pushButtonstop"));

        gridLayout->addWidget(pushButtonstop, 3, 1, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));

        gridLayout->addLayout(horizontalLayout, 1, 1, 1, 1);


        retranslateUi(mywidgetClass);

        QMetaObject::connectSlotsByName(mywidgetClass);
    } // setupUi

    void retranslateUi(QWidget *mywidgetClass)
    {
        mywidgetClass->setWindowTitle(QApplication::translate("mywidgetClass", "mywidget", Q_NULLPTR));
        label->setText(QApplication::translate("mywidgetClass", "\344\270\212\350\241\214\346\260\224\351\232\231\347\263\273\347\273\237\346\225\260\346\215\256\351\207\207\351\233\206", Q_NULLPTR));
        label_2->setText(QApplication::translate("mywidgetClass", "\350\275\246\345\217\267\357\274\232", Q_NULLPTR));
        manualstart_2->setText(QApplication::translate("mywidgetClass", "\346\211\213\345\212\250\345\274\200\345\247\213", Q_NULLPTR));
        pushButtoninit->setText(QApplication::translate("mywidgetClass", "\345\210\235\345\247\213\345\214\226", Q_NULLPTR));
        ConNectPLC->setText(QApplication::translate("mywidgetClass", "\350\277\236\346\216\245plc", Q_NULLPTR));
        manualstop_2->setText(QApplication::translate("mywidgetClass", "\346\211\213\345\212\250\347\273\223\346\235\237", Q_NULLPTR));
        pushButtonstop->setText(QApplication::translate("mywidgetClass", "\345\201\234\346\255\242\351\207\207\351\233\206", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class mywidgetClass: public Ui_mywidgetClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MYWIDGET_H
