#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QComboBox>
#include "dex/dexdto.h"

using namespace dex;

class ComboBox : public QComboBox
{
    Q_OBJECT
public:
    ComboBox(QWidget *parent = nullptr);

    void setCurrentData(const QString &data);

    template <typename type>
    void addData(const type &data)
    {
        auto it = data.begin();

        QMap<QString, QString> sort;
        while (it != data.end()) {
            sort[tr(it->second.name.c_str())] = toString(it->first);
            ++it;
        }

        auto itSort = sort.begin();
        addItem(tr("All"));
        while (itSort != sort.end()) {
            addItem(itSort.key(), itSort.value());
            ++itSort;
        }
    }

private:
    QString toString(const std::string &str) const;
    QString toString(const uint8_t &i) const;
};

#endif
