#ifndef FILTERITEMEDITOR_H
#define FILTERITEMEDITOR_H

#include <QDialog>

namespace Ui {
    class FilterItemEditor;
}

class FilterItemEditor : public QDialog
{
    Q_OBJECT

public:
    FilterItemEditor(QDialog *parent = nullptr);
    ~FilterItemEditor();

private:
    Ui::FilterItemEditor *ui;

private Q_SLOTS:
    void clickedOk();

Q_SIGNALS:
    void newItem(const QString &item);
};

#endif
