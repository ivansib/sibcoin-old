// Copyright (c) 2015 The Sibcoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef OFFERMODEL_H // TODO: maybe this class isn't used
#define	OFFERMODEL_H

#include "offerdb.h"

#include <QObject>
#include <QAbstractTableModel>

//class QNetworkAccessManager;
//class QResource;


class OfferModel : public QAbstractTableModel
{
    Q_OBJECT
public:
	OfferModel(COfferDB *offerdb, QObject *parent);
	~OfferModel();


    // override QAbstractTableModel methods
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    //bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex & index) const ;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

private:
    COfferDB *offerDB;
//    void fetch();
//    void insertRow(const std::string hash_idx);
//    void deleteRow(const std::string hash_idx);
//    void updateRow(const std::string hash_idx);
};

/** Interface to Sibcoin Offer DB from Qt view code. */
//class OfferModel : public QObject
//{
//    Q_OBJECT
//
//public:
//    explicit OfferModel(COfferDB *sibdb, QObject *parent = 0);
//    ~OfferModel();
//
//
//
//    void fetch();
//    void insertRow(const std::string hash_idx);
//    void deleteRow(const std::string hash_idx);
//    void updateRow(const std::string hash_idx)
//
//private:
//    enum ST {
//        ST_INIT,
//        ST_LOADING_RCC,
//        ST_LOADING_MD5,
//        ST_LOADED,
//        ST_ERROR,
//    };
    
//    COfferDB *offerDB;
//    QByteArray rccData;
//    QString rccMD5;
    
//Q_SIGNALS:
//    void resourceReady(std::string res_root);
//
//
//public Q_SLOTS:
//    void replyFinished(QNetworkReply* p_reply);
//
//private:
//    void loadLocalResource();
//    void loadFromDB();
//    bool registerRes();
        
//};

#endif	/* SIBMODEL_H */

