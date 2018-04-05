#ifndef SETTINGSMODEL_H
#define SETTINGSMODEL_H

#include <QMimeData>
#include <QDataStream>
#include <QAbstractTableModel>
#include <QStringList>

class SettingsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    SettingsModel(const int &columnEdit, QObject *parent = nullptr);
    virtual ~SettingsModel() {}

    virtual int columnCount(const QModelIndex &parent) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    virtual Qt::DropActions supportedDropActions() const;

    virtual QStringList mimeTypes() const;
    virtual QMimeData *mimeData(const QModelIndexList &indexes) const;

    int columnEdit() const;

protected:
    QStringList listHead;
    const int m_columnEdit;

Q_SIGNALS:
    void dataChanged();
};

#endif
