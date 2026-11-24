#pragma once

#include <QDialog>
#include <QString>

class Document;

class DocumentWindow : public QDialog {
    Q_OBJECT

public:
    DocumentWindow(QWidget *parent, const Document &document);

private:
    void openDocument();
    void exportDocument();

    QString sourcePath_;
    QString displayName_;
};
