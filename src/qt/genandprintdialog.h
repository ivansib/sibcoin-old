// Copyright (c) 2011-2013 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef GENANDPRINTDIALOG_H
#define GENANDPRINTDIALOG_H

#if defined(HAVE_CONFIG_H)
#include "config/sibcoin-config.h"
#endif


#include <QDialog>

class WalletModel;
class CKey;

namespace Ui {
    class GenAndPrintDialog;
}

/** Multifunctional dialog to ask for passphrases. Used for encryption, unlocking, and changing the passphrase.
 */
class GenAndPrintDialog : public QDialog
{
    Q_OBJECT

public:
    enum Mode {
#ifdef ENABLE_PAPERWALLET
        Export, /**< Ask passphrase, generate key pair and print them out */
#endif
        Import  /**< Ask passphrase and load priv key from QR code */
    };

    explicit GenAndPrintDialog(Mode mode, QWidget *parent);
    ~GenAndPrintDialog();

    void accept();

    void setModel(WalletModel *model);
    QString getURI();

private:
    Ui::GenAndPrintDialog *ui;
    Mode mode;
    WalletModel *model;
    bool fCapsLock;
    std::string salt;
    QString uri;

    QString cryptedKey(const CKey &secret, const std::string &address, const QString &passwd);
   
private Q_SLOTS:
    void textChanged();

    /** Print button clicked */
    void on_printButton_clicked();
    /** Import button clicked */
    void on_importButton_clicked();
    
protected:
    bool event(QEvent *event);
    bool eventFilter(QObject *object, QEvent *event);
    std::string getCurrentKeyPair();
    void printAsQR(QPainter &painter, QString &vchKey, int place);

};

#endif // GENANDPRINTDIALOG_H
