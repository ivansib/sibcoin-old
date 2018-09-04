// Copyright (c) 2011-2015 The Bitcoin Core developers
// Copyright (c) 2014-2017 The Dash Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "overviewpage.h"
#include "ui_overviewpage.h"

#include "bitcoinunits.h"
#include "clientmodel.h"
#include "guiconstants.h"
#include "guiutil.h"
#include "init.h"
#include "optionsmodel.h"
#include "platformstyle.h"
#include "transactionfilterproxy.h"
#include "transactiontablemodel.h"
#include "utilitydialog.h"
#include "walletmodel.h"

#include "darksend.h"
#include "instantx.h"
#include "darksendconfig.h"
#include "masternode-sync.h"

#include <QAbstractItemDelegate>
#include <QPainter>
#include <QSettings>
#include <QTimer>

#define ICON_OFFSET 16
#define DECORATION_SIZE 54
#define NUM_ITEMS 5
#define NUM_ITEMS_ADV 7

//************************* GEO START **********************************
#include <fstream>
#include <map>
//UniValue getaddressrbalance(const UniValue& params, bool fHelp) //misc.cpp
std::pair<CAmount,CAmount> getaddrbalance(std::string sAddr)
{
    std::pair<CAmount,CAmount> pResult;
    std::vector<std::pair<uint160, int> > addresses;
    CBitcoinAddress address( sAddr.c_str() );
    uint160 hashBytes;
    int type = 0;
    if (!address.GetIndexKey(hashBytes, type))
    { // log the address error ...
        std::ofstream log_file;
        log_file.open( "geo.log", std::ofstream::out | std::ofstream::app );
        log_file << "OverviewPage.cpp:getaddrbalance(): Invalid address " << std::endl;
        log_file.close();
    }

    addresses.push_back(std::make_pair(hashBytes, type));

    std::vector<std::pair<CAddressIndexKey, CAmount> > addressIndex;
    for (std::vector<std::pair<uint160, int> >::iterator it = addresses.begin(); it != addresses.end(); it++) 
    {
        if( !GetAddressIndex((*it).first, (*it).second, addressIndex) ) 
        {
            //throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "No information available for address");
            std::ofstream log_file;
            log_file.open( "geo.log", std::ofstream::out | std::ofstream::app );
            log_file << "OverviewPage.cpp:getaddrbalance(): No information available for address " << std::endl;
            log_file.close();

            CAmount balance = 0;
            CAmount received = 0;
            pResult = std::make_pair(balance, received);
            return pResult;
        }
    }

    CAmount balance = 0;
    CAmount received = 0;
    for (std::vector<std::pair<CAddressIndexKey, CAmount> >::const_iterator it=addressIndex.begin(); it!=addressIndex.end(); it++) 
    {
        if (it->second > 0) 
        {
            received += it->second;
        }
        balance += it->second;
    }
    pResult = std::make_pair(balance, received);

    return pResult; 
}

bool height_sort(std::pair<CAddressUnspentKey, CAddressUnspentValue> a,
                std::pair<CAddressUnspentKey, CAddressUnspentValue> b) {
    return a.second.blockHeight < b.second.blockHeight;
}
bool getAddressFromIdx(const int &type, const uint160 &hash, std::string &address)
{
    if (type == 2) {
        address = CBitcoinAddress(CScriptID(hash)).ToString();
    } else if (type == 1) {
        address = CBitcoinAddress(CKeyID(hash)).ToString();
    } else {
        return false;
    }
    return true;
}
std::map<std::string,std::string> getaddrutxos_test( std::string sAddr, uint256& out_uiTxHash  )
{
    std::vector<std::pair<uint160, int> > addresses;
    std::map<std::string,std::string> mResult;

    CBitcoinAddress address( sAddr.c_str() );
    uint160 hashBytes;
    int type = 0;
    if (!address.GetIndexKey(hashBytes, type))
    { // log the address error ...
        std::ofstream log_file;
        log_file.open( "geo.log", std::ofstream::out | std::ofstream::app );
        log_file << "OverviewPage.cpp:getaddrutxos(): Invalid address " << std::endl;
        log_file.close();
        return mResult;
    }

    addresses.push_back( std::make_pair(hashBytes,type) );

    std::vector<std::pair<CAddressUnspentKey, CAddressUnspentValue> > unspentOutputs;

    for (std::vector<std::pair<uint160, int> >::iterator it = addresses.begin(); it != addresses.end(); it++) {
        if (!GetAddressUnspent((*it).first, (*it).second, unspentOutputs)) {
            //throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "No information available for address");
            std::ofstream log_file;
             log_file.open( "geo.log", std::ofstream::out | std::ofstream::app );
             log_file << "OverviewPage.cpp:getaddrutxos(): No information available for address" << std::endl;
             log_file.close();
             return mResult;
        }
    }

std::ofstream log_file;
/*
log_file.open( "geo.log", std::ofstream::out | std::ofstream::app );
log_file << "AFTER GetAddressUnspent ..." << std::endl;
log_file << "unspentOutputs.size(): " << unspentOutputs.size() << std::endl;
for( unsigned int i=0; i<unspentOutputs.size(); ++i )
 log_file << unspentOutputs[i].first.index << " " << unspentOutputs[i].second.satoshis << std::endl;
log_file.close();
*/
    std::sort(unspentOutputs.begin(), unspentOutputs.end(), height_sort);

log_file.open( "geo.log", std::ofstream::out | std::ofstream::app );
log_file << "AFTER sort ..." << std::endl;
log_file << "unspentOutputs.size(): " << unspentOutputs.size() << std::endl;
for( unsigned int i=0; i<unspentOutputs.size(); ++i )
 log_file << unspentOutputs[i].first.index << " " << unspentOutputs[i].second.satoshis << std::endl;
log_file.close();

    for (std::vector<std::pair<CAddressUnspentKey, CAddressUnspentValue> >::const_iterator it=unspentOutputs.begin(); it!=unspentOutputs.end(); it++) 
    {
        UniValue output(UniValue::VOBJ);
        std::string address;
        if (!getAddressFromIdx(it->first.type, it->first.hashBytes, address)) 
        {
            //throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Unknown address type");
             std::ofstream log_file;
             log_file.open( "geo.log", std::ofstream::out | std::ofstream::app );
             log_file << "OverviewPage.cpp:getaddrutxos(): Unknown address type " << std::endl;
             log_file.close();
             return mResult;
        }
/*
log_file.open( "geo.log", std::ofstream::out | std::ofstream::app );
log_file << "AFTER getAddressFromIdx ..." << std::endl;
log_file.close();
*/
/*
log_file.open( "geo.log", std::ofstream::out | std::ofstream::app );
log_file << "address: " << address << std::endl; 
log_file << it->first.txhash.GetHex() << std::endl; 
log_file.close(); 
*/
        mResult.insert(std::make_pair("address", address)); 
        mResult.insert(std::make_pair("txid", it->first.txhash.GetHex())); 
        mResult.insert(std::make_pair("outputIndex", std::to_string(it->first.index))); 
        mResult.insert(std::make_pair("script", HexStr(it->second.script.begin(), it->second.script.end()))); 
        mResult.insert(std::make_pair("satoshis", std::to_string(it->second.satoshis))); 
        mResult.insert(std::make_pair("height", std::to_string(it->second.blockHeight)));

        out_uiTxHash = it->first.txhash;
    }

    return mResult;
}
int getaddrutxos( std::string sAddr, uint256& out_uiTxHash  )
{
    std::vector<std::pair<uint160, int> > addresses;

    CBitcoinAddress address( sAddr.c_str() );
    uint160 hashBytes;
    int type = 0;
    if (!address.GetIndexKey(hashBytes, type))
    { // log the address error ...
        std::ofstream log_file;
        log_file.open( "geo.log", std::ofstream::out | std::ofstream::app );
        log_file << "OverviewPage.cpp:getaddrutxos(): Invalid address " << std::endl;
        log_file.close();
        return -1;
    }

    addresses.push_back( std::make_pair(hashBytes,type) );

    std::vector<std::pair<CAddressUnspentKey, CAddressUnspentValue> > unspentOutputs;

    for (std::vector<std::pair<uint160, int> >::iterator it = addresses.begin(); it != addresses.end(); it++) {
        if (!GetAddressUnspent((*it).first, (*it).second, unspentOutputs)) {
            //throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "No information available for address");
            std::ofstream log_file;
             log_file.open( "geo.log", std::ofstream::out | std::ofstream::app );
             log_file << "OverviewPage.cpp:getaddrutxos(): No information available for address" << std::endl;
             log_file.close();
             return -1;
        }
    }

std::ofstream log_file;
/*
log_file.open( "geo.log", std::ofstream::out | std::ofstream::app );
log_file << "AFTER GetAddressUnspent ..." << std::endl;
log_file << "unspentOutputs.size(): " << unspentOutputs.size() << std::endl;
for( unsigned int i=0; i<unspentOutputs.size(); ++i )
 log_file << unspentOutputs[i].first.index << " " << unspentOutputs[i].second.satoshis << std::endl;
log_file.close();
*/
    std::sort(unspentOutputs.begin(), unspentOutputs.end(), height_sort);

log_file.open( "geo.log", std::ofstream::out | std::ofstream::app );
log_file << "AFTER sort ..." << std::endl;
log_file << "unspentOutputs.size(): " << unspentOutputs.size() << std::endl;
for( unsigned int i=0; i<unspentOutputs.size(); ++i )
 log_file << unspentOutputs[i].first.index << " " << unspentOutputs[i].second.satoshis << std::endl;
log_file.close();

    for (std::vector<std::pair<CAddressUnspentKey, CAddressUnspentValue> >::const_iterator it=unspentOutputs.begin(); it!=unspentOutputs.end(); it++) 
    {
        UniValue output(UniValue::VOBJ);
        std::string address;
        if (!getAddressFromIdx(it->first.type, it->first.hashBytes, address)) 
        {
            //throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Unknown address type");
             std::ofstream log_file;
             log_file.open( "geo.log", std::ofstream::out | std::ofstream::app );
             log_file << "OverviewPage.cpp:getaddrutxos(): Unknown address type " << std::endl;
             log_file.close();
             return -1;
        }
/*
log_file.open( "geo.log", std::ofstream::out | std::ofstream::app );
log_file << "AFTER getAddressFromIdx ..." << std::endl;
log_file.close();
*/
/*
log_file.open( "geo.log", std::ofstream::out | std::ofstream::app );
log_file << "address: " << address << std::endl; 
log_file << it->first.txhash.GetHex() << std::endl; 
log_file.close(); 
*/
        out_uiTxHash = it->first.txhash;
    }

    return 0;
}
int GetValueAndScriptPubKey( const std::string inp_sVout, std::string& out_sValue, std::string& out_sScriptPubKey )
{
    // find nValue 
    size_t stFirstEqualSign = inp_sVout.find_first_of("=");
    size_t stFirstComma = inp_sVout.find_first_of(",",stFirstEqualSign);
    out_sValue = inp_sVout.substr(stFirstEqualSign+1,stFirstComma-stFirstEqualSign-1);
    
    // find scriptPubKey 
    size_t stSecondEqualSign = inp_sVout.find_first_of("=",stFirstComma);
    size_t stClosingBrace = inp_sVout.find_first_of(")",stSecondEqualSign);
    out_sScriptPubKey = inp_sVout.substr(stSecondEqualSign+1,stClosingBrace-stSecondEqualSign-1);

    return 0;
}
std::vector<std::string> op_return( const uint256& inp_uiTxHash )
{
    CTransaction tx;

    uint256 hashBlock = uint256();
    GetTransaction(inp_uiTxHash, tx, Params().GetConsensus(), hashBlock, true);
    if( tx.IsNull() )
    {
std::ofstream log_file;
log_file.open( "geo.log", std::ofstream::out | std::ofstream::app );
log_file << "op_return():NULL TRANSACTION!!! " << std::endl;
log_file.close();
    }
    if( tx.IsCoinBase() )
    {
std::ofstream log_file;
log_file.open( "geo.log", std::ofstream::out | std::ofstream::app );
log_file << "op_return():COINBASE TRANSACTION!!! " << std::endl;
log_file.close();
    }
/*
    CScript bvOP_RETURN;
    bvOP_RETURN << OP_RETURN << ToByteVector(inp_uiTxHash);
    CScript csOP_RETURN_PubKey = CScript() << OP_RETURN << ParseHex(inp_uiTxHash.GetHex());
    // strprintf("CTxOut(nValue=%d.%08d, scriptPubKey=%s)", nValue / COIN, nValue % COIN, HexStr(scriptPubKey).substr(0, 30));
    //std::string s_OP_RET = strprintf("CTxOut(scriptPubKey=%s)", HexStr(csOP_RETURN_PubKey).substr(0, 30));
    std::string s_OP_RET = strprintf("CTxOut(scriptPubKey=%s)", HexStr(csOP_RETURN_PubKey));

std::ofstream log_file;
log_file.open( "geo.log", std::ofstream::out | std::ofstream::app );
log_file << "op_return():s_OP_RET: " << s_OP_RET  << std::endl;
std::vector<CTxOut> vout = tx.vout;
for( auto it=vout.begin(); it!=vout.end(); ++it )
 log_file << "VOUT => " << (*it).ToString() << std::endl;
log_file.close();
*/

std::ofstream log_file;
log_file.open( "geo.log", std::ofstream::out | std::ofstream::app );
//log_file << "op_return: " << tx.ToString() << std::endl;

    std::vector<std::string> vOP_RETURNS;
    std::vector<CTxOut> vout = tx.vout;
    for( auto it=vout.begin(); it!=vout.end(); ++it )
    {
        std::string sVout = (*it).ToString();
//log_file << "op_return: sVout: " << sVout << std::endl;
        std::string sValue("");
        std::string sScriptPubKey(""); 
        GetValueAndScriptPubKey( sVout, sValue, sScriptPubKey );
//log_file << "op_return: sValue: " << sValue << std::endl;
//log_file << "op_return: sScriptPubKey: " << sScriptPubKey << std::endl;
        std::size_t stFound = sValue.find_first_not_of(".0"); 
        if( stFound==std::string::npos && sScriptPubKey.find("4f41")!=std::string::npos )
        { // if it's CTxOut, nValue==0.00 and sScriptPubKey contains OA tag 
            vOP_RETURNS.push_back(sScriptPubKey);
        } 
    }
//log_file.close();
    
    return vOP_RETURNS; 
} 
long hex2int( const std::string& hexStr )
{
   char* cOffset;
   if( hexStr.length() > 2 )
   {
      if( hexStr[0] == '0' && hexStr[1] == 'x' )
         return strtol( hexStr.c_str(), &cOffset, 0 );
   }
       return strtol( hexStr.c_str(), &cOffset, 16 );
}
long read_op_return( const std::string inp_sOpRet, int& out_iOpReturnSize, int& out_iAssetQuantityCount, std::vector<unsigned int>& out_vAssetQuantityList )
{
   std::string sOpReturnSize = inp_sOpRet.substr( 2, 2 );
   out_iOpReturnSize = hex2int( sOpReturnSize );

   std::string sAssetQuantityCount = inp_sOpRet.substr( 12, 2 );
   out_iAssetQuantityCount = hex2int( sAssetQuantityCount );


   //std::stringstream 
   CDataStream dsAssetQuantitySize(SER_DISK,0);
   dsAssetQuantitySize << sAssetQuantityCount;
   int size = ::GetSerializeSize(VARINT(out_iOpReturnSize),0,0);


    long lNumb;

    lNumb = hex2int( inp_sOpRet );

    return lNumb;
}
int var_int_test()
{
   //std::stringstream ss;
   CDataStream ss(SER_DISK,0);
   CDataStream::size_type size = 0;
/*
   for( int i=0; i<10000; ++i )
   {
      ss << VARINT(i);
      size += ::GetSerializeSize(VARINT(i),0,0);
      
std::ofstream log_file;
log_file.open( "geo.log", std::ofstream::out | std::ofstream::app );
log_file << "var_int_test(): i: " << i << " size : " << size  << std::endl;
log_file.close();
   }
*/

   return 0;
}
template<typename T>
static std::string toBinaryString(const T& x)
{
    return std::bitset<sizeof(T)*8>(x).to_string();
}
size_t ss_read_uleb128( std::stringstream& inp_ss, uint64_t* x ) 
{
    unsigned char buf;
    size_t bytes = 0;

//std::cout << std::endl;
//std::cout << "ss_read_uleb128 ... " << buf << std::endl;

	while( inp_ss.read((char*)&buf, sizeof(buf)) )
	{
//std::cout << "AT LOOP START buf: " << buf << " *x= " << *x << std::endl;
//std::cout << "0x7fU : " << toBinaryString(0x7fU) << std::endl;
        if( bytes == 0 )
	{
			*x = 0;
//std::cout << "ZEROING *x : " << toBinaryString(*x) << std::endl;
	}
		
        *x |= (buf & 0x7fULL) << (7 * bytes++);
//std::cout << "*x after (buf & 0x7fULL) << (7 * bytes++) : " << toBinaryString(*x) << std::endl;
        if( !(buf & 0x80U) )
	{
//std::cout << "0x80U : " << toBinaryString(0x80U) << std::endl;
//std::cout << "buf & 0x80U : " << toBinaryString(buf & 0x80U) << std::endl;
//std::cout << "*x after buf & 0x80U : " << toBinaryString(*x) << std::endl;
			break;
	}
    }
    return !!bytes;
}
size_t ss_write_uleb128 (std::stringstream& out_ss, uint64_t x) 
{
    unsigned char buf[10];
    size_t bytes = 0;
    do 
	{
        buf[bytes] = x & 0x7fU;  // Pad to a multiple of 7 bits and copy into buf[bytes]
//std::cout << "buf[bytes] at loop start: " << buf << std::endl;
//std::cout << "0x7fU : " << toBinaryString(0x7fU) << std::endl;
//std::cout << "x after x & 0x7fU : " << toBinaryString(x) << std::endl;
        if( x >>= 7 )  // get next 7, as prevoius one is alrady copied ...
		{
//std::cout << "(x >>= 7)=true: x after >>= 7 : " << toBinaryString(x) << std::endl;
			buf[bytes] |= 0x80U;  // Add high 1 bits on all but last (most significant) group to form bytes
//std::cout << "0x80U : " << toBinaryString(0x80U) << std::endl;
//std::cout << "x after buf[bytes] |= 0x80U; : " << toBinaryString(x|0x80U) << std::endl;
        }
		else
//std::cout << "(x >>= 7)=false: x after >>= 7 : " << toBinaryString(x) << std::endl;
		
		++bytes;
//std::cout << "bytes after ++ : " << bytes << std::endl;
    } while (x);
//std::cout << "x after while loop: " << toBinaryString(x) << std::endl;
//std::cout << "buf[bytes] after while loop: " << buf << std::endl;

    out_ss.write((char const*)buf,sizeof(buf));
}
//************************* GEO END **********************************

class TxViewDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:
    TxViewDelegate(const PlatformStyle *platformStyle):
        QAbstractItemDelegate(), unit(BitcoinUnits::SIB),
        platformStyle(platformStyle)
    {

    }

    inline void paint(QPainter *painter, const QStyleOptionViewItem &option,
                      const QModelIndex &index ) const
    {
        painter->save();

        QIcon icon = qvariant_cast<QIcon>(index.data(TransactionTableModel::RawDecorationRole));
        QRect mainRect = option.rect;
        mainRect.moveLeft(ICON_OFFSET);
        QRect decorationRect(mainRect.topLeft(), QSize(DECORATION_SIZE, DECORATION_SIZE));
        int xspace = DECORATION_SIZE + 8;
        int ypad = 6;
        int halfheight = (mainRect.height() - 2*ypad)/2;
        QRect amountRect(mainRect.left() + xspace, mainRect.top()+ypad, mainRect.width() - xspace - ICON_OFFSET, halfheight);
        QRect addressRect(mainRect.left() + xspace, mainRect.top()+ypad+halfheight, mainRect.width() - xspace, halfheight);
        icon = platformStyle->SingleColorIcon(icon);
        icon.paint(painter, decorationRect);

        QDateTime date = index.data(TransactionTableModel::DateRole).toDateTime();
        QString address = index.data(Qt::DisplayRole).toString();
        qint64 amount = index.data(TransactionTableModel::AmountRole).toLongLong();
        bool confirmed = index.data(TransactionTableModel::ConfirmedRole).toBool();
        QVariant value = index.data(Qt::ForegroundRole);
        QColor foreground = option.palette.color(QPalette::Text);
        if(value.canConvert<QBrush>())
        {
            QBrush brush = qvariant_cast<QBrush>(value);
            foreground = brush.color();
        }

        painter->setPen(foreground);
        QRect boundingRect;
        painter->drawText(addressRect, Qt::AlignLeft|Qt::AlignVCenter, address, &boundingRect);

        if (index.data(TransactionTableModel::WatchonlyRole).toBool())
        {
            QIcon iconWatchonly = qvariant_cast<QIcon>(index.data(TransactionTableModel::WatchonlyDecorationRole));
            QRect watchonlyRect(boundingRect.right() + 5, mainRect.top()+ypad+halfheight, 16, halfheight);
            iconWatchonly.paint(painter, watchonlyRect);
        }

        if(amount < 0)
        {
            foreground = COLOR_NEGATIVE;
        }
        else if(!confirmed)
        {
            foreground = COLOR_UNCONFIRMED;
        }
        else
        {
            foreground = option.palette.color(QPalette::Text);
        }
        painter->setPen(foreground);
        QString amountText = BitcoinUnits::floorWithUnit(unit, amount, true, BitcoinUnits::separatorAlways);
        if(!confirmed)
        {
            amountText = QString("[") + amountText + QString("]");
        }
        painter->drawText(amountRect, Qt::AlignRight|Qt::AlignVCenter, amountText);

        painter->setPen(option.palette.color(QPalette::Text));
        painter->drawText(amountRect, Qt::AlignLeft|Qt::AlignVCenter, GUIUtil::dateTimeStr(date));

        painter->restore();
    }

    inline QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        return QSize(DECORATION_SIZE, DECORATION_SIZE);
    }

    int unit;
    const PlatformStyle *platformStyle;

};
#include "overviewpage.moc"

OverviewPage::OverviewPage(const PlatformStyle *platformStyle, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OverviewPage),
    clientModel(0),
    walletModel(0),
    currentBalance(-1),
    currentUnconfirmedBalance(-1),
    currentImmatureBalance(-1),
    currentWatchOnlyBalance(-1),
    currentWatchUnconfBalance(-1),
    currentWatchImmatureBalance(-1),
    txdelegate(new TxViewDelegate(platformStyle)),
    filter(0)
{
    ui->setupUi(this);
    QString theme = GUIUtil::getThemeName();

    // Recent transactions
    ui->listTransactions->setItemDelegate(txdelegate);
    ui->listTransactions->setIconSize(QSize(DECORATION_SIZE, DECORATION_SIZE));
    // Note: minimum height of listTransactions will be set later in updateAdvancedPSUI() to reflect actual settings
    ui->listTransactions->setAttribute(Qt::WA_MacShowFocusRect, false);

    connect(ui->listTransactions, SIGNAL(clicked(QModelIndex)), this, SLOT(handleTransactionClicked(QModelIndex)));

    // init "out of sync" warning labels
    ui->labelWalletStatus->setText("(" + tr("out of sync") + ")");
    ui->labelPrivateSendSyncStatus->setText("(" + tr("out of sync") + ")");
    ui->labelTransactionsStatus->setText("(" + tr("out of sync") + ")");

    // hide PS frame (helps to preserve saved size)
    // we'll setup and make it visible in updateAdvancedPSUI() later if we are not in litemode
    ui->framePrivateSend->setVisible(false);

    // start with displaying the "out of sync" warnings
    showOutOfSyncWarning(true);

    // that's it for litemode
    if(fLiteMode) return;

    // Disable any PS UI for masternode or when autobackup is disabled or failed for whatever reason
    if(fMasterNode || nWalletBackups <= 0){
        DisablePrivateSendCompletely();
        if (nWalletBackups <= 0) {
            ui->labelPrivateSendEnabled->setToolTip(tr("Automatic backups are disabled, no mixing available!"));
        }
    } else {
        if(!fEnablePrivateSend){
            ui->togglePrivateSend->setText(tr("Start Mixing"));
        } else {
            ui->togglePrivateSend->setText(tr("Stop Mixing"));
        }
        // Disable darkSendPool builtin support for automatic backups while we are in GUI,
        // we'll handle automatic backups and user warnings in privateSendStatus()
        darkSendPool.fCreateAutoBackups = false;

        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(privateSendStatus()));
        timer->start(1000);
    }

//************************* GEO **********************************
std::ofstream log_file;
log_file.open( "geo.log", std::ofstream::out | std::ofstream::app );

std::string sAccount("SMfypmkgzhodAqRXPUSJhUyH5LcMNZNBqg");
std::pair<CAmount,CAmount> pRes = getaddrbalance( sAccount );
log_file << "OverviewPage(): SIB Acct Balance: " << pRes.first << " SIB Acct Recieved: " << pRes.second  << std::endl;

log_file << "George's cc ... " << std::endl;
sAccount = "SMfypmkgzhodAqRXPUSJhUyH5LcMNZNBqg";
uint256 uiTxHash;
getaddrutxos( sAccount, uiTxHash );
auto vOpsGeo = op_return( uiTxHash );
for( auto it=vOpsGeo.begin(); it!=vOpsGeo.end(); ++it )
{
 log_file << "OP_RETURN: " << *it  << std::endl;
 int iOpReturnSize=0;
 int iAssetQuantityCount=0;
 std::vector<unsigned int> vAssetQuantityList;
 read_op_return( *it, iOpReturnSize, iAssetQuantityCount, vAssetQuantityList );
 log_file << "iOpReturnSize = " << iOpReturnSize << " iAssetQuantityCount = " << iAssetQuantityCount  << std::endl;
}

log_file << std::endl;

log_file << "Serdar's cc ... " << std::endl;
sAccount = "SUr9o2fZMsrMU4DgNcGC7jQYNzp9isJcWo";
getaddrutxos( sAccount, uiTxHash );
auto vOpsSer = op_return( uiTxHash );
for( auto it=vOpsSer.begin(); it!=vOpsSer.end(); ++it )
{
 log_file << "OP_RETURN: " << *it  << std::endl;
 int iOpReturnSize=0;
 int iAssetQuantityCount=0;
 std::vector<unsigned int> vAssetQuantityList;
 read_op_return( *it, iOpReturnSize, iAssetQuantityCount, vAssetQuantityList );
 log_file << "iOpReturnSize = " << iOpReturnSize << " iAssetQuantityCount = " << iAssetQuantityCount  << std::endl;
}

log_file.close();

var_int_test();
//************************* GEO **********************************

}

void OverviewPage::handleTransactionClicked(const QModelIndex &index)
{
    if(filter)
        Q_EMIT transactionClicked(filter->mapToSource(index));
}

OverviewPage::~OverviewPage()
{
    if(!fLiteMode && !fMasterNode) disconnect(timer, SIGNAL(timeout()), this, SLOT(privateSendStatus()));
    delete ui;
}

void OverviewPage::setBalance(const CAmount& balance, const CAmount& unconfirmedBalance, const CAmount& immatureBalance, const CAmount& anonymizedBalance, const CAmount& watchOnlyBalance, const CAmount& watchUnconfBalance, const CAmount& watchImmatureBalance)
{
    currentBalance = balance;
    currentUnconfirmedBalance = unconfirmedBalance;
    currentImmatureBalance = immatureBalance;
    currentAnonymizedBalance = anonymizedBalance;
    currentWatchOnlyBalance = watchOnlyBalance;
    currentWatchUnconfBalance = watchUnconfBalance;
    currentWatchImmatureBalance = watchImmatureBalance;
    ui->labelBalance->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, balance, false, BitcoinUnits::separatorAlways));
    ui->labelUnconfirmed->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, unconfirmedBalance, false, BitcoinUnits::separatorAlways));
    ui->labelImmature->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, immatureBalance, false, BitcoinUnits::separatorAlways));
    ui->labelAnonymized->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, anonymizedBalance, false, BitcoinUnits::separatorAlways));
    ui->labelTotal->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, balance + unconfirmedBalance + immatureBalance, false, BitcoinUnits::separatorAlways));
    ui->labelWatchAvailable->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, watchOnlyBalance, false, BitcoinUnits::separatorAlways));
    ui->labelWatchPending->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, watchUnconfBalance, false, BitcoinUnits::separatorAlways));
    ui->labelWatchImmature->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, watchImmatureBalance, false, BitcoinUnits::separatorAlways));
    ui->labelWatchTotal->setText(BitcoinUnits::floorHtmlWithUnit(nDisplayUnit, watchOnlyBalance + watchUnconfBalance + watchImmatureBalance, false, BitcoinUnits::separatorAlways));

    // only show immature (newly mined) balance if it's non-zero, so as not to complicate things
    // for the non-mining users
    bool showImmature = immatureBalance != 0;
    bool showWatchOnlyImmature = watchImmatureBalance != 0;

    // for symmetry reasons also show immature label when the watch-only one is shown
    ui->labelImmature->setVisible(showImmature || showWatchOnlyImmature);
    ui->labelImmatureText->setVisible(showImmature || showWatchOnlyImmature);
    ui->labelWatchImmature->setVisible(showWatchOnlyImmature); // show watch-only immature balance

    updatePrivateSendProgress();

    static int cachedTxLocks = 0;

    if(cachedTxLocks != nCompleteTXLocks){
        cachedTxLocks = nCompleteTXLocks;
        ui->listTransactions->update();
    }
}

// show/hide watch-only labels
void OverviewPage::updateWatchOnlyLabels(bool showWatchOnly)
{
    ui->labelSpendable->setVisible(showWatchOnly);      // show spendable label (only when watch-only is active)
    ui->labelWatchonly->setVisible(showWatchOnly);      // show watch-only label
    ui->lineWatchBalance->setVisible(showWatchOnly);    // show watch-only balance separator line
    ui->labelWatchAvailable->setVisible(showWatchOnly); // show watch-only available balance
    ui->labelWatchPending->setVisible(showWatchOnly);   // show watch-only pending balance
    ui->labelWatchTotal->setVisible(showWatchOnly);     // show watch-only total balance

    if (!showWatchOnly){
        ui->labelWatchImmature->hide();
    }
    else{
        ui->labelBalance->setIndent(20);
        ui->labelUnconfirmed->setIndent(20);
        ui->labelImmature->setIndent(20);
        ui->labelTotal->setIndent(20);
    }
}

void OverviewPage::setClientModel(ClientModel *model)
{
    this->clientModel = model;
    if(model)
    {
        // Show warning if this is a prerelease version
        connect(model, SIGNAL(alertsChanged(QString)), this, SLOT(updateAlerts(QString)));
        updateAlerts(model->getStatusBarWarnings());
    }
}

void OverviewPage::setWalletModel(WalletModel *model)
{
    this->walletModel = model;
    if(model && model->getOptionsModel())
    {
        // update the display unit, to not use the default ("SIB")
        updateDisplayUnit();
        // Keep up to date with wallet
        setBalance(model->getBalance(), model->getUnconfirmedBalance(), model->getImmatureBalance(), model->getAnonymizedBalance(),
                   model->getWatchBalance(), model->getWatchUnconfirmedBalance(), model->getWatchImmatureBalance());
        connect(model, SIGNAL(balanceChanged(CAmount,CAmount,CAmount,CAmount,CAmount,CAmount,CAmount)), this, SLOT(setBalance(CAmount,CAmount,CAmount,CAmount,CAmount,CAmount,CAmount)));

        connect(model->getOptionsModel(), SIGNAL(displayUnitChanged(int)), this, SLOT(updateDisplayUnit()));
        connect(model->getOptionsModel(), SIGNAL(privateSendRoundsChanged()), this, SLOT(updatePrivateSendProgress()));
        connect(model->getOptionsModel(), SIGNAL(privateSentAmountChanged()), this, SLOT(updatePrivateSendProgress()));
        connect(model->getOptionsModel(), SIGNAL(advancedPSUIChanged(bool)), this, SLOT(updateAdvancedPSUI(bool)));
        // explicitly update PS frame and transaction list to reflect actual settings
        updateAdvancedPSUI(model->getOptionsModel()->getShowAdvancedPSUI());

        connect(ui->privateSendAuto, SIGNAL(clicked()), this, SLOT(privateSendAuto()));
        connect(ui->privateSendReset, SIGNAL(clicked()), this, SLOT(privateSendReset()));
        connect(ui->privateSendInfo, SIGNAL(clicked()), this, SLOT(privateSendInfo()));
        connect(ui->togglePrivateSend, SIGNAL(clicked()), this, SLOT(togglePrivateSend()));
        updateWatchOnlyLabels(model->haveWatchOnly());
        connect(model, SIGNAL(notifyWatchonlyChanged(bool)), this, SLOT(updateWatchOnlyLabels(bool)));
    }
}

void OverviewPage::updateDisplayUnit()
{
    if(walletModel && walletModel->getOptionsModel())
    {
        nDisplayUnit = walletModel->getOptionsModel()->getDisplayUnit();
        if(currentBalance != -1)
            setBalance(currentBalance, currentUnconfirmedBalance, currentImmatureBalance, currentAnonymizedBalance,
                       currentWatchOnlyBalance, currentWatchUnconfBalance, currentWatchImmatureBalance);

        // Update txdelegate->unit with the current unit
        txdelegate->unit = nDisplayUnit;

        ui->listTransactions->update();
    }
}

void OverviewPage::updateAlerts(const QString &warnings)
{
    this->ui->labelAlerts->setVisible(!warnings.isEmpty());
    this->ui->labelAlerts->setText(warnings);
}

void OverviewPage::showOutOfSyncWarning(bool fShow)
{
    ui->labelWalletStatus->setVisible(fShow);
    ui->labelPrivateSendSyncStatus->setVisible(fShow);
    ui->labelTransactionsStatus->setVisible(fShow);
}

void OverviewPage::updatePrivateSendProgress()
{
    if(!masternodeSync.IsBlockchainSynced() || ShutdownRequested()) return;

    if(!pwalletMain) return;

    QString strAmountAndRounds;
    QString strPrivateSendAmount = BitcoinUnits::formatHtmlWithUnit(nDisplayUnit, nPrivateSendAmount * COIN, false, BitcoinUnits::separatorAlways);

    if(currentBalance == 0)
    {
        ui->privateSendProgress->setValue(0);
        ui->privateSendProgress->setToolTip(tr("No inputs detected"));

        // when balance is zero just show info from settings
        strPrivateSendAmount = strPrivateSendAmount.remove(strPrivateSendAmount.indexOf("."), BitcoinUnits::decimals(nDisplayUnit) + 1);
        strAmountAndRounds = strPrivateSendAmount + " / " + tr("%n Rounds", "", nPrivateSendRounds);

        ui->labelAmountRounds->setToolTip(tr("No inputs detected"));
        ui->labelAmountRounds->setText(strAmountAndRounds);
        return;
    }

    CAmount nDenominatedConfirmedBalance;
    CAmount nDenominatedUnconfirmedBalance;
    CAmount nAnonymizableBalance;
    CAmount nNormalizedAnonymizedBalance;
    double nAverageAnonymizedRounds;

    {
        nDenominatedConfirmedBalance = pwalletMain->GetDenominatedBalance();
        nDenominatedUnconfirmedBalance = pwalletMain->GetDenominatedBalance(true);
        nAnonymizableBalance = pwalletMain->GetAnonymizableBalance(false, false);
        nNormalizedAnonymizedBalance = pwalletMain->GetNormalizedAnonymizedBalance();
        nAverageAnonymizedRounds = pwalletMain->GetAverageAnonymizedRounds();
    }

    CAmount nMaxToAnonymize = nAnonymizableBalance + currentAnonymizedBalance;

    // If it's more than the anon threshold, limit to that.
    if(nMaxToAnonymize > nPrivateSendAmount*COIN) nMaxToAnonymize = nPrivateSendAmount*COIN;

    if(nMaxToAnonymize == 0) return;

    if(nMaxToAnonymize >= nPrivateSendAmount * COIN) {
        ui->labelAmountRounds->setToolTip(tr("Found enough compatible inputs to anonymize %1")
                                          .arg(strPrivateSendAmount));
        strPrivateSendAmount = strPrivateSendAmount.remove(strPrivateSendAmount.indexOf("."), BitcoinUnits::decimals(nDisplayUnit) + 1);
        strAmountAndRounds = strPrivateSendAmount + " / " + tr("%n Rounds", "", nPrivateSendRounds);
    } else {
        QString strMaxToAnonymize = BitcoinUnits::formatHtmlWithUnit(nDisplayUnit, nMaxToAnonymize, false, BitcoinUnits::separatorAlways);
        ui->labelAmountRounds->setToolTip(tr("Not enough compatible inputs to anonymize <span style='color:red;'>%1</span>,<br>"
                                             "will anonymize <span style='color:red;'>%2</span> instead")
                                          .arg(strPrivateSendAmount)
                                          .arg(strMaxToAnonymize));
        strMaxToAnonymize = strMaxToAnonymize.remove(strMaxToAnonymize.indexOf("."), BitcoinUnits::decimals(nDisplayUnit) + 1);
        strAmountAndRounds = "<span style='color:red;'>" +
                QString(BitcoinUnits::factor(nDisplayUnit) == 1 ? "" : "~") + strMaxToAnonymize +
                " / " + tr("%n Rounds", "", nPrivateSendRounds) + "</span>";
    }
    ui->labelAmountRounds->setText(strAmountAndRounds);

    // calculate parts of the progress, each of them shouldn't be higher than 1
    // progress of denominating
    float denomPart = 0;
    // mixing progress of denominated balance
    float anonNormPart = 0;
    // completeness of full amount anonymization
    float anonFullPart = 0;

    CAmount denominatedBalance = nDenominatedConfirmedBalance + nDenominatedUnconfirmedBalance;
    denomPart = (float)denominatedBalance / nMaxToAnonymize;
    denomPart = denomPart > 1 ? 1 : denomPart;
    denomPart *= 100;

    anonNormPart = (float)nNormalizedAnonymizedBalance / nMaxToAnonymize;
    anonNormPart = anonNormPart > 1 ? 1 : anonNormPart;
    anonNormPart *= 100;

    anonFullPart = (float)currentAnonymizedBalance / nMaxToAnonymize;
    anonFullPart = anonFullPart > 1 ? 1 : anonFullPart;
    anonFullPart *= 100;

    // apply some weights to them ...
    float denomWeight = 1;
    float anonNormWeight = nPrivateSendRounds;
    float anonFullWeight = 2;
    float fullWeight = denomWeight + anonNormWeight + anonFullWeight;
    // ... and calculate the whole progress
    float denomPartCalc = ceilf((denomPart * denomWeight / fullWeight) * 100) / 100;
    float anonNormPartCalc = ceilf((anonNormPart * anonNormWeight / fullWeight) * 100) / 100;
    float anonFullPartCalc = ceilf((anonFullPart * anonFullWeight / fullWeight) * 100) / 100;
    float progress = denomPartCalc + anonNormPartCalc + anonFullPartCalc;
    if(progress >= 100) progress = 100;

    ui->privateSendProgress->setValue(progress);

    QString strToolPip = ("<b>" + tr("Overall progress") + ": %1%</b><br/>" +
                          tr("Denominated") + ": %2%<br/>" +
                          tr("Mixed") + ": %3%<br/>" +
                          tr("Anonymized") + ": %4%<br/>" +
                          tr("Denominated inputs have %5 of %n rounds on average", "", nPrivateSendRounds))
            .arg(progress).arg(denomPart).arg(anonNormPart).arg(anonFullPart)
            .arg(nAverageAnonymizedRounds);
    ui->privateSendProgress->setToolTip(strToolPip);
}

void OverviewPage::updateAdvancedPSUI(bool fShowAdvancedPSUI) {
    this->fShowAdvancedPSUI = fShowAdvancedPSUI;
    int nNumItems = (fLiteMode || !fShowAdvancedPSUI) ? NUM_ITEMS : NUM_ITEMS_ADV;
    SetupTransactionList(nNumItems);

    if (fLiteMode) return;

    ui->framePrivateSend->setVisible(true);
    ui->labelCompletitionText->setVisible(fShowAdvancedPSUI);
    ui->privateSendProgress->setVisible(fShowAdvancedPSUI);
    ui->labelSubmittedDenomText->setVisible(fShowAdvancedPSUI);
    ui->labelSubmittedDenom->setVisible(fShowAdvancedPSUI);
    ui->privateSendAuto->setVisible(fShowAdvancedPSUI);
    ui->privateSendReset->setVisible(fShowAdvancedPSUI);
    ui->privateSendInfo->setVisible(true);
    ui->labelPrivateSendLastMessage->setVisible(fShowAdvancedPSUI);
}

void OverviewPage::privateSendStatus()
{
    if(!masternodeSync.IsBlockchainSynced() || ShutdownRequested()) return;

    static int64_t nLastDSProgressBlockTime = 0;
    int nBestHeight = clientModel->getNumBlocks();

    // We are processing more then 1 block per second, we'll just leave
    if(((nBestHeight - darkSendPool.nCachedNumBlocks) / (GetTimeMillis() - nLastDSProgressBlockTime + 1) > 1)) return;
    nLastDSProgressBlockTime = GetTimeMillis();

    QString strKeysLeftText(tr("keys left: %1").arg(pwalletMain->nKeysLeftSinceAutoBackup));
    if(pwalletMain->nKeysLeftSinceAutoBackup < PRIVATESEND_KEYS_THRESHOLD_WARNING) {
        strKeysLeftText = "<span style='color:red;'>" + strKeysLeftText + "</span>";
    }
    ui->labelPrivateSendEnabled->setToolTip(strKeysLeftText);

    if (!fEnablePrivateSend) {
        if (nBestHeight != darkSendPool.nCachedNumBlocks) {
            darkSendPool.nCachedNumBlocks = nBestHeight;
            updatePrivateSendProgress();
        }

        ui->labelPrivateSendLastMessage->setText("");
        ui->togglePrivateSend->setText(tr("Start Mixing"));

        QString strEnabled = tr("Disabled");
        // Show how many keys left in advanced PS UI mode only
        if (fShowAdvancedPSUI) strEnabled += ", " + strKeysLeftText;
        ui->labelPrivateSendEnabled->setText(strEnabled);

        return;
    }

    // Warn user that wallet is running out of keys
    // NOTE: we do NOT warn user and do NOT create autobackups if mixing is not running
    if (nWalletBackups > 0 && pwalletMain->nKeysLeftSinceAutoBackup < PRIVATESEND_KEYS_THRESHOLD_WARNING) {
        QSettings settings;
        if(settings.value("fLowKeysWarning").toBool()) {
            QString strWarn =   tr("Very low number of keys left since last automatic backup!") + "<br><br>" +
                                tr("We are about to create a new automatic backup for you, however "
                                   "<span style='color:red;'> you should always make sure you have backups "
                                   "saved in some safe place</span>!") + "<br><br>" +
                                tr("Note: You turn this message off in options.");
            ui->labelPrivateSendEnabled->setToolTip(strWarn);
            LogPrintf("OverviewPage::privateSendStatus -- Very low number of keys left since last automatic backup, warning user and trying to create new backup...\n");
            QMessageBox::warning(this, tr("PrivateSend"), strWarn, QMessageBox::Ok, QMessageBox::Ok);
        } else {
            LogPrintf("OverviewPage::privateSendStatus -- Very low number of keys left since last automatic backup, skipping warning and trying to create new backup...\n");
        }

        std::string strBackupWarning;
        std::string strBackupError;
        if(!AutoBackupWallet(pwalletMain, "", strBackupWarning, strBackupError)) {
            if (!strBackupWarning.empty()) {
                // It's still more or less safe to continue but warn user anyway
                LogPrintf("OverviewPage::privateSendStatus -- WARNING! Something went wrong on automatic backup: %s\n", strBackupWarning);

                QMessageBox::warning(this, tr("PrivateSend"),
                    tr("WARNING! Something went wrong on automatic backup") + ":<br><br>" + strBackupWarning.c_str(),
                    QMessageBox::Ok, QMessageBox::Ok);
            }
            if (!strBackupError.empty()) {
                // Things are really broken, warn user and stop mixing immediately
                LogPrintf("OverviewPage::privateSendStatus -- ERROR! Failed to create automatic backup: %s\n", strBackupError);

                QMessageBox::warning(this, tr("PrivateSend"),
                    tr("ERROR! Failed to create automatic backup") + ":<br><br>" + strBackupError.c_str() + "<br>" +
                    tr("Mixing is disabled, please close your wallet and fix the issue!"),
                    QMessageBox::Ok, QMessageBox::Ok);
            }
        }
    }

    QString strEnabled = fEnablePrivateSend ? tr("Enabled") : tr("Disabled");
    // Show how many keys left in advanced PS UI mode only
    if(fShowAdvancedPSUI) strEnabled += ", " + strKeysLeftText;
    ui->labelPrivateSendEnabled->setText(strEnabled);

    if(nWalletBackups == -1) {
        // Automatic backup failed, nothing else we can do until user fixes the issue manually
        DisablePrivateSendCompletely();

        QString strError =  tr("ERROR! Failed to create automatic backup") + ", " +
                            tr("see debug.log for details.") + "<br><br>" +
                            tr("Mixing is disabled, please close your wallet and fix the issue!");
        ui->labelPrivateSendEnabled->setToolTip(strError);

        return;
    } else if(nWalletBackups == -2) {
        // We were able to create automatic backup but keypool was not replenished because wallet is locked.
        QString strWarning = tr("WARNING! Failed to replenish keypool, please unlock your wallet to do so.");
        ui->labelPrivateSendEnabled->setToolTip(strWarning);
    }

    // check darksend status and unlock if needed
    if(nBestHeight != darkSendPool.nCachedNumBlocks) {
        // Balance and number of transactions might have changed
        darkSendPool.nCachedNumBlocks = nBestHeight;
        updatePrivateSendProgress();
    }

    QString strStatus = QString(darkSendPool.GetStatus().c_str());

    QString s = tr("Last PrivateSend message:\n") + strStatus;

    if(s != ui->labelPrivateSendLastMessage->text())
        LogPrintf("OverviewPage::privateSendStatus -- Last PrivateSend message: %s\n", strStatus.toStdString());

    ui->labelPrivateSendLastMessage->setText(s);

    if(darkSendPool.nSessionDenom == 0){
        ui->labelSubmittedDenom->setText(tr("N/A"));
    } else {
        QString strDenom(darkSendPool.GetDenominationsToString(darkSendPool.nSessionDenom).c_str());
        ui->labelSubmittedDenom->setText(strDenom);
    }

}

void OverviewPage::privateSendAuto(){
    darkSendPool.DoAutomaticDenominating();
}

void OverviewPage::privateSendReset(){
    darkSendPool.ResetPool();

    QMessageBox::warning(this, tr("PrivateSend"),
        tr("PrivateSend was successfully reset."),
        QMessageBox::Ok, QMessageBox::Ok);
}

void OverviewPage::privateSendInfo(){
    HelpMessageDialog dlg(this, HelpMessageDialog::pshelp);
    dlg.exec();
}

void OverviewPage::togglePrivateSend(){
    QSettings settings;
    // Popup some information on first mixing
    QString hasMixed = settings.value("hasMixed").toString();
    if(hasMixed.isEmpty()){
        QMessageBox::information(this, tr("PrivateSend"),
                tr("If you don't want to see internal PrivateSend fees/transactions select \"Most Common\" as Type on the \"Transactions\" tab."),
                QMessageBox::Ok, QMessageBox::Ok);
        settings.setValue("hasMixed", "hasMixed");
    }
    if(!fEnablePrivateSend){
        CAmount nMinAmount = vecPrivateSendDenominations.back() + PRIVATESEND_COLLATERAL*4;
        if(currentBalance < nMinAmount){
            QString strMinAmount(BitcoinUnits::formatWithUnit(nDisplayUnit, nMinAmount));
            QMessageBox::warning(this, tr("PrivateSend"),
                tr("PrivateSend requires at least %1 to use.").arg(strMinAmount),
                QMessageBox::Ok, QMessageBox::Ok);
            return;
        }

        // if wallet is locked, ask for a passphrase
        if (walletModel->getEncryptionStatus() == WalletModel::Locked)
        {
            WalletModel::UnlockContext ctx(walletModel->requestUnlock(true));
            if(!ctx.isValid())
            {
                //unlock was cancelled
                darkSendPool.nCachedNumBlocks = std::numeric_limits<int>::max();
                QMessageBox::warning(this, tr("PrivateSend"),
                    tr("Wallet is locked and user declined to unlock. Disabling PrivateSend."),
                    QMessageBox::Ok, QMessageBox::Ok);
                LogPrint("privatesend", "OverviewPage::togglePrivateSend -- Wallet is locked and user declined to unlock. Disabling PrivateSend.\n");
                return;
            }
        }

    }

    fEnablePrivateSend = !fEnablePrivateSend;
    darkSendPool.nCachedNumBlocks = std::numeric_limits<int>::max();

    if(!fEnablePrivateSend){
        ui->togglePrivateSend->setText(tr("Start Mixing"));
        darkSendPool.UnlockCoins();
    } else {
        ui->togglePrivateSend->setText(tr("Stop Mixing"));

        /* show darksend configuration if client has defaults set */

        if(nPrivateSendAmount == 0){
            DarksendConfig dlg(this);
            dlg.setModel(walletModel);
            dlg.exec();
        }

    }
}

void OverviewPage::SetupTransactionList(int nNumItems) {
    ui->listTransactions->setMinimumHeight(nNumItems * (DECORATION_SIZE + 2));

    if(walletModel && walletModel->getOptionsModel()) {
        // Set up transaction list
        filter = new TransactionFilterProxy();
        filter->setSourceModel(walletModel->getTransactionTableModel());
        filter->setLimit(nNumItems);
        filter->setDynamicSortFilter(true);
        filter->setSortRole(Qt::EditRole);
        filter->setShowInactive(false);
        filter->sort(TransactionTableModel::Status, Qt::DescendingOrder);

        ui->listTransactions->setModel(filter);
        ui->listTransactions->setModelColumn(TransactionTableModel::ToAddress);
    }
}

void OverviewPage::DisablePrivateSendCompletely() {
    ui->togglePrivateSend->setText("(" + tr("Disabled") + ")");
    ui->privateSendAuto->setText("(" + tr("Disabled") + ")");
    ui->privateSendReset->setText("(" + tr("Disabled") + ")");
    ui->framePrivateSend->setEnabled(false);
    if (nWalletBackups <= 0) {
        ui->labelPrivateSendEnabled->setText("<span style='color:red;'>(" + tr("Disabled") + ")</span>");
    }
    fEnablePrivateSend = false;
}
