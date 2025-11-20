/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *OpenButton;
    QAction *NewButton;
    QAction *HelpInfo;
    QAction *Developer;
    QAction *action;
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_4;
    QTabWidget *tabWidget;
    QWidget *templates;
    QHBoxLayout *horizontalLayout_3;
    QListWidget *templatesWidget;
    QWidget *documents;
    QHBoxLayout *horizontalLayout_2;
    QListWidget *documentsWidget;
    QMenuBar *menuBar;
    QMenu *menu;
    QMenu *menu_2;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 600);
        OpenButton = new QAction(MainWindow);
        OpenButton->setObjectName(QString::fromUtf8("OpenButton"));
        NewButton = new QAction(MainWindow);
        NewButton->setObjectName(QString::fromUtf8("NewButton"));
        HelpInfo = new QAction(MainWindow);
        HelpInfo->setObjectName(QString::fromUtf8("HelpInfo"));
        Developer = new QAction(MainWindow);
        Developer->setObjectName(QString::fromUtf8("Developer"));
        action = new QAction(MainWindow);
        action->setObjectName(QString::fromUtf8("action"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        templates = new QWidget();
        templates->setObjectName(QString::fromUtf8("templates"));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(templates->sizePolicy().hasHeightForWidth());
        templates->setSizePolicy(sizePolicy);
        horizontalLayout_3 = new QHBoxLayout(templates);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        templatesWidget = new QListWidget(templates);
        templatesWidget->setObjectName(QString::fromUtf8("templatesWidget"));

        horizontalLayout_3->addWidget(templatesWidget);

        tabWidget->addTab(templates, QString());
        documents = new QWidget();
        documents->setObjectName(QString::fromUtf8("documents"));
        sizePolicy.setHeightForWidth(documents->sizePolicy().hasHeightForWidth());
        documents->setSizePolicy(sizePolicy);
        horizontalLayout_2 = new QHBoxLayout(documents);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        documentsWidget = new QListWidget(documents);
        documentsWidget->setObjectName(QString::fromUtf8("documentsWidget"));

        horizontalLayout_2->addWidget(documentsWidget);

        tabWidget->addTab(documents, QString());

        horizontalLayout_4->addWidget(tabWidget);


        gridLayout->addLayout(horizontalLayout_4, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 800, 21));
        menu = new QMenu(menuBar);
        menu->setObjectName(QString::fromUtf8("menu"));
        menu_2 = new QMenu(menuBar);
        menu_2->setObjectName(QString::fromUtf8("menu_2"));
        MainWindow->setMenuBar(menuBar);

        menuBar->addAction(menu->menuAction());
        menuBar->addAction(menu_2->menuAction());
        menu->addAction(OpenButton);
        menu->addAction(NewButton);
        menu->addAction(action);
        menu_2->addAction(HelpInfo);
        menu_2->addAction(Developer);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        OpenButton->setText(QCoreApplication::translate("MainWindow", "\320\224\320\276\320\261\320\260\320\262\320\270\321\202\321\214 \320\264\320\276\320\272\321\203\320\274\320\265\320\275\321\202", nullptr));
#if QT_CONFIG(tooltip)
        OpenButton->setToolTip(QCoreApplication::translate("MainWindow", "\320\224\320\276\320\261\320\260\320\262\320\270\321\202\321\214 \321\201\321\203\321\211\320\265\321\201\321\202\320\262\321\203\321\216\321\211\320\270\320\271 \320\264\320\276\320\272\321\203\320\274\320\265\320\275\321\202", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        OpenButton->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
        NewButton->setText(QCoreApplication::translate("MainWindow", "\320\224\320\276\320\261\320\260\320\262\320\270\321\202\321\214 \321\210\320\260\320\261\320\273\320\276\320\275", nullptr));
#if QT_CONFIG(tooltip)
        NewButton->setToolTip(QCoreApplication::translate("MainWindow", "\320\224\320\276\320\261\320\260\320\262\320\270\321\202\321\214 \321\210\320\260\320\261\320\273\320\276 \320\270\320\267 \320\264\320\270\321\201\320\272\320\260", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        NewButton->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+N", nullptr));
#endif // QT_CONFIG(shortcut)
        HelpInfo->setText(QCoreApplication::translate("MainWindow", "\320\244\320\276\321\200\320\274\320\260\321\202\320\270\321\200\320\276\320\262\320\260\320\275\320\270\320\265", nullptr));
        Developer->setText(QCoreApplication::translate("MainWindow", "\320\240\320\260\320\267\321\200\320\260\320\261\320\276\321\202\321\207\320\270\320\272", nullptr));
        action->setText(QCoreApplication::translate("MainWindow", "\320\235\320\276\320\262\321\213\320\271 \321\210\320\260\320\261\320\273\320\276\320\275", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(templates), QCoreApplication::translate("MainWindow", "\320\250\320\260\320\261\320\273\320\276\320\275\321\213", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(documents), QCoreApplication::translate("MainWindow", "\320\224\320\276\320\272\321\203\320\274\320\275\321\202\321\213", nullptr));
        menu->setTitle(QCoreApplication::translate("MainWindow", "\320\244\320\260\320\271\320\273", nullptr));
        menu_2->setTitle(QCoreApplication::translate("MainWindow", "\320\241\320\277\321\200\320\260\320\262\320\272\320\260", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
