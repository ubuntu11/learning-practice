/********************************************************************************
** Form generated from reading UI file 'mbmslink.ui'
**
** Created by: Qt User Interface Compiler version 5.12.11
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MBMSLINK_H
#define UI_MBMSLINK_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_MBMSLink
{
public:
    QGridLayout *gridLayout_2;
    QDialogButtonBox *buttonBox;
    QGridLayout *gridLayout;
    QLineEdit *lineEdit;
    QSpacerItem *verticalSpacer;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *horizontalSpacer;
    QLabel *label;
    QSpacerItem *verticalSpacer_2;
    QLineEdit *lineEdit_2;
    QLabel *label_2;

    void setupUi(QDialog *MBMSLink)
    {
        if (MBMSLink->objectName().isEmpty())
            MBMSLink->setObjectName(QString::fromUtf8("MBMSLink"));
        MBMSLink->resize(515, 241);
        gridLayout_2 = new QGridLayout(MBMSLink);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        buttonBox = new QDialogButtonBox(MBMSLink);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout_2->addWidget(buttonBox, 1, 1, 1, 1);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        lineEdit = new QLineEdit(MBMSLink);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        gridLayout->addWidget(lineEdit, 1, 2, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 3, 2, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 1, 3, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 1, 0, 1, 1);

        label = new QLabel(MBMSLink);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 1, 1, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_2, 0, 2, 1, 1);

        lineEdit_2 = new QLineEdit(MBMSLink);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));
        lineEdit_2->setEnabled(true);
        lineEdit_2->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(lineEdit_2, 2, 2, 1, 1);

        label_2 = new QLabel(MBMSLink);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 2, 1, 1, 1);


        gridLayout_2->addLayout(gridLayout, 0, 1, 1, 1);


        retranslateUi(MBMSLink);
        QObject::connect(buttonBox, SIGNAL(accepted()), MBMSLink, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), MBMSLink, SLOT(reject()));

        QMetaObject::connectSlotsByName(MBMSLink);
    } // setupUi

    void retranslateUi(QDialog *MBMSLink)
    {
        MBMSLink->setWindowTitle(QApplication::translate("MBMSLink", "Dialog", nullptr));
        label->setText(QApplication::translate("MBMSLink", "ID", nullptr));
        label_2->setText(QApplication::translate("MBMSLink", "soc_sys", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MBMSLink: public Ui_MBMSLink {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MBMSLINK_H
