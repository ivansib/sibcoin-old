include $(top_srcdir)/src/Makefile.include

AM_CPPFLAGS += -I$(top_srcdir)/src \
  -I$(top_builddir)/src/qt \
  -I$(top_builddir)/src/qt/forms \
  $(PROTOBUF_CFLAGS) \
  $(QR_CFLAGS)
bin_PROGRAMS = sibcoin-qt
noinst_LIBRARIES = libdashqt.a
SUBDIRS = . $(BUILD_TEST_QT)
DIST_SUBDIRS = . test

# sibcoin qt core #
#QT_TS = \
#  locale/sibcoin_bar.ts \
#  locale/sibcoin_bg.ts \
#  locale/sibcoin_ca.ts \
#  locale/sibcoin_cmn.ts \
#  locale/sibcoin_cs.ts \
#  locale/sibcoin_da.ts \
#  locale/sibcoin_de.ts \
#  locale/sibcoin_el.ts \
#  locale/sibcoin_en.ts \
#  locale/sibcoin_eo.ts \
#  locale/sibcoin_es.ts \
#  locale/sibcoin_fi.ts \
#  locale/sibcoin_fr.ts \
#  locale/sibcoin_hu_HU.ts \
#  locale/sibcoin_it.ts \
#  locale/sibcoin_lv_LV.ts \
#  locale/sibcoin_nb.ts \
#  locale/sibcoin_nl.ts \
#  locale/sibcoin_pl.ts \
#  locale/sibcoin_pt.ts \
#  locale/sibcoin_pt_BR.ts \
#  locale/sibcoin_ru.ts \
#  locale/sibcoin_sk.ts \
#  locale/sibcoin_sv.ts \
#  locale/sibcoin_tr.ts \
#  locale/sibcoin_vi.ts \
#  locale/sibcoin_zh_CN.ts \
#  locale/sibcoin_zh_HK.ts

QT_TS = \
  locale/sibcoin_en.ts \
  locale/sibcoin_ru.ts

QT_FORMS_UI = \
  forms/aboutdialog.ui \
  forms/addressbookpage.ui \
  forms/askpassphrasedialog.ui \
  forms/coincontroldialog.ui \
  forms/darksendconfig.ui \
  forms/editaddressdialog.ui \
  forms/genandprintdialog.ui \
  forms/goodsdialog.ui \
  forms/exchangedialog.ui \
  forms/tableoffersdialog.ui \
  forms/helpmessagedialog.ui \
  forms/intro.ui \
  forms/openuridialog.ui \
  forms/optionsdialog.ui \
  forms/overviewpage.ui \
  forms/receivecoinsdialog.ui \
  forms/receiverequestdialog.ui \
  forms/rpcconsole.ui \
  forms/sendcoinsdialog.ui \
  forms/sendcoinsentry.ui \
  forms/signverifymessagedialog.ui \
  forms/transactiondescdialog.ui \
  forms/offerdetailsview.ui \
  forms/offerdetailseditor.ui \
  forms/settingsdialog.ui \
  forms/filterlist.ui

QT_MOC_CPP = \
  moc_addressbookpage.cpp \
  moc_addresstablemodel.cpp \
  moc_askpassphrasedialog.cpp \
  moc_bitcoinaddressvalidator.cpp \
  moc_bitcoinamountfield.cpp \
  moc_bitcoingui.cpp \
  moc_bitcoinunits.cpp \
  moc_clientmodel.cpp \
  moc_coincontroldialog.cpp \
  moc_coincontroltreewidget.cpp \
  moc_csvmodelwriter.cpp \
  moc_darksendconfig.cpp \
  moc_editaddressdialog.cpp \
  moc_genandprintdialog.cpp \
  moc_goodsdialog.cpp \
  moc_guiutil.cpp \
  moc_intro.cpp \
  moc_macdockiconhandler.cpp \
  moc_macnotificationhandler.cpp \
  moc_notificator.cpp \
  moc_openuridialog.cpp \
  moc_optionsdialog.cpp \
  moc_optionsmodel.cpp \
  moc_overviewpage.cpp \
  moc_paymentserver.cpp \
  moc_qvalidatedlineedit.cpp \
  moc_qvaluecombobox.cpp \
  moc_receivecoinsdialog.cpp \
  moc_receiverequestdialog.cpp \
  moc_recentrequeststablemodel.cpp \
  moc_rpcconsole.cpp \
  moc_sendcoinsdialog.cpp \
  moc_sendcoinsentry.cpp \
  moc_sibmodel.cpp \
  moc_signverifymessagedialog.cpp \
  moc_splashscreen.cpp \
  moc_trafficgraphwidget.cpp \
  moc_transactiondesc.cpp \
  moc_transactiondescdialog.cpp \
  moc_transactionfilterproxy.cpp \
  moc_transactiontablemodel.cpp \
  moc_transactionview.cpp \
  moc_utilitydialog.cpp \
  moc_walletframe.cpp \
  moc_walletmodel.cpp \
  moc_walletview.cpp

BITCOIN_MM = \
  macdockiconhandler.mm \
  macnotificationhandler.mm

QT_MOC = \
  sibcoin.moc \
  intro.moc \
  overviewpage.moc \
  rpcconsole.moc

QT_QRC_CPP = qrc_sibcoin.cpp
QT_QRC = sibcoin.qrc

PROTOBUF_CC = paymentrequest.pb.cc
PROTOBUF_H = paymentrequest.pb.h
PROTOBUF_PROTO = paymentrequest.proto

BITCOIN_QT_H = \
  addressbookpage.h \
  addresstablemodel.h \
  askpassphrasedialog.h \
  bitcoinaddressvalidator.h \
  bitcoinamountfield.h \
  bitcoingui.h \
  bitcoinunits.h \
  clientmodel.h \
  coincontroldialog.h \
  coincontroltreewidget.h \
  csvmodelwriter.h \
  darksendconfig.h \
  editaddressdialog.h \
  genandprintdialog.h \
  goodsdialog.h \
  guiconstants.h \
  guiutil.h \
  intro.h \
  macdockiconhandler.h \
  macnotificationhandler.h \
  notificator.h \
  openuridialog.h \
  optionsdialog.h \
  optionsmodel.h \
  overviewpage.h \
  paymentrequestplus.h \
  paymentserver.h \
  qvalidatedlineedit.h \
  qvaluecombobox.h \
  receivecoinsdialog.h \
  receiverequestdialog.h \
  recentrequeststablemodel.h \
  rpcconsole.h \
  sendcoinsdialog.h \
  sendcoinsentry.h \
  sibmodel.h \
  signverifymessagedialog.h \
  splashscreen.h \
  trafficgraphwidget.h \
  transactiondesc.h \
  transactiondescdialog.h \
  transactionfilterproxy.h \
  transactionrecord.h \
  transactiontablemodel.h \
  transactionview.h \
  utilitydialog.h \
  walletframe.h \
  walletmodel.h \
  walletmodeltransaction.h \
  walletview.h \
  winshutdownmonitor.h

RES_ICONS = \
  res/icons/add.png \
  res/icons/address-book.png \
  res/icons/bitcoin.ico \
  res/icons/bitcoin.png \
  res/icons/bitcoin_testnet.ico \
  res/icons/bitcoin_testnet.png \
  res/icons/clock1.png \
  res/icons/clock2.png \
  res/icons/clock3.png \
  res/icons/clock4.png \
  res/icons/clock5.png \
  res/icons/configure.png \
  res/icons/connect0_16.png \
  res/icons/connect1_16.png \
  res/icons/connect2_16.png \
  res/icons/connect3_16.png \
  res/icons/connect4_16.png \
  res/icons/debugwindow.png \
  res/icons/edit.png \
  res/icons/editcopy.png \
  res/icons/editpaste.png \
  res/icons/export.png \
  res/icons/filesave.png \
  res/icons/history.png \
  res/icons/key.png \
  res/icons/lock_closed.png \
  res/icons/lock_open.png \
  res/icons/overview.png \
  res/icons/qrcode.png \
  res/icons/quit.png \
  res/icons/receive.png \
  res/icons/remove.png \
  res/icons/send.png \
  res/icons/synced.png \
  res/icons/toolbar.png \
  res/icons/toolbar_testnet.png \
  res/icons/transaction0.png \
  res/icons/transaction2.png \
  res/icons/transaction_conflicted.png \
  res/icons/tx_inout.png \
  res/icons/tx_input.png \
  res/icons/tx_output.png \
  res/icons/tx_mined.png \
  res/icons/drkblue_editpaste.png \
  res/icons/drkblue_address-book.png \
  res/icons/drkblue_editcopy.png \
  res/icons/drkblue_remove.png \
  res/icons/browse.png


BITCOIN_QT_CPP = \
  sibcoin.cpp \
  bitcoinaddressvalidator.cpp \
  bitcoinamountfield.cpp \
  bitcoingui.cpp \
  bitcoinunits.cpp \
  clientmodel.cpp \
  csvmodelwriter.cpp \
  darksendconfig.cpp \
  guiutil.cpp \
  intro.cpp \
  notificator.cpp \
  optionsdialog.cpp \
  optionsmodel.cpp \
  qvalidatedlineedit.cpp \
  qvaluecombobox.cpp \
  rpcconsole.cpp \
  splashscreen.cpp \
  trafficgraphwidget.cpp \
  utilitydialog.cpp \
  winshutdownmonitor.cpp

if ENABLE_WALLET
BITCOIN_QT_CPP += \
  addressbookpage.cpp \
  addresstablemodel.cpp \
  askpassphrasedialog.cpp \
  coincontroldialog.cpp \
  coincontroltreewidget.cpp \
  darksendconfig.cpp \
  editaddressdialog.cpp \
  genandprintdialog.cpp \
  goodsdialog.cpp \
  openuridialog.cpp \
  overviewpage.cpp \
  paymentrequestplus.cpp \
  paymentserver.cpp \
  receivecoinsdialog.cpp \
  receiverequestdialog.cpp \
  recentrequeststablemodel.cpp \
  sibmodel.cpp \
  sendcoinsdialog.cpp \
  sendcoinsentry.cpp \
  signverifymessagedialog.cpp \
  transactiondesc.cpp \
  transactiondescdialog.cpp \
  transactionfilterproxy.cpp \
  transactionrecord.cpp \
  transactiontablemodel.cpp \
  transactionview.cpp \
  walletframe.cpp \
  walletmodel.cpp \
  walletmodeltransaction.cpp \
  walletview.cpp
endif

if ENABLE_DEX
QT_MOC_CPP += \
  moc_exchangedialog.cpp \
  moc_offermodel.cpp \
  moc_tableoffersdialog.cpp \
  moc_offerdetails.cpp \
  moc_convertdata.cpp \
  moc_offerdetailseditor.cpp \
  moc_tableoffersview.cpp \
  moc_tableofferseditor.cpp \
  moc_combobox.cpp \
  moc_pricespinbox.cpp \
  moc_offerdetailsview.cpp \
  moc_offerdetailscretor.cpp \
  moc_settingsdialog.cpp \
  moc_tablecountries.cpp \
  moc_countriesmodel.cpp \
  moc_settingsdelegate.cpp \
  moc_tablecurrencies.cpp \
  moc_settingsmodel.cpp \
  moc_currenciesmodel.cpp \
  moc_filterlist.cpp

BITCOIN_QT_H += \
  exchangedialog.h \
  offermodel.h \
  tableoffersdialog.h \
  offerdetails.h \
  convertdata.h \
  dto.h \
  offerdetailseditor.h \
  tableoffersview.h \
  tableofferseditor.h \
  combobox.h \
  pricespinbox.h \
  offerdetailsview.h \
  offerdetailscreator.h \
  settingsdialog.h \
  tablecountries.h \
  countriesmodel.h \
  settingsdelegate.h \
  tablecurrencies.h \
  settingsmodel.h \
  currenciesmodel.h \
  filterlist.h

BITCOIN_QT_CPP += \
  exchangedialog.cpp \
  offermodel.cpp \
  tableoffersdialog.cpp \
  offerdetails.cpp \
  convertdata.cpp \
  offerdetailseditor.cpp \
  tableoffersview.cpp \
  tableofferseditor.cpp \
  combobox.cpp \
  pricespinbox.cpp \
  offerdetailsview.cpp \
  offerdetailscreator.cpp \
  settingsdialog.cpp \
  tablecountries.cpp \
  countriesmodel.cpp \
  settingsdelegate.cpp \
  tablecurrencies.cpp \
  settingsmodel.cpp \
  currenciesmodel.cpp \
  filterlist.cpp
endif

RES_IMAGES = \
  res/images/about.png \
  res/images/splash.png \
  res/images/splash_testnet.png \
  res/images/dash_logo_horizontal.png \
  res/images/drkblue_downArrow.png \
  res/images/drkblue_downArrow_small.png \
  res/images/drkblue_upArrow_small.png \
  res/images/drkblue_leftArrow_small.png \
  res/images/drkblue_rightArrow_small.png \
  res/images/drkblue_qtreeview_selected.png \
  res/images/drkblue_walletFrame_bg.png \
  res/images/drkblue_walletFrame.png

RES_CSS = \
  res/css/drkblue.css

RES_MOVIES = $(wildcard res/movies/spinner-*.png)

SIBCOIN_RC = res/sibcoin-qt-res.rc

libdashqt_a_CPPFLAGS = $(AM_CPPFLAGS) $(QT_INCLUDES) \
  -I$(top_srcdir)/src/qt/forms $(QT_DBUS_INCLUDES)
libdashqt_a_SOURCES = $(BITCOIN_QT_CPP) $(BITCOIN_QT_H) $(QT_FORMS_UI) \
  $(QT_QRC) $(QT_TS) $(PROTOBUF_PROTO) $(RES_ICONS) $(RES_IMAGES) $(RES_MOVIES) $(RES_CSS)

nodist_libdashqt_a_SOURCES = $(QT_MOC_CPP) $(QT_MOC) $(PROTOBUF_CC) \
  $(PROTOBUF_H) $(QT_QRC_CPP)

BUILT_SOURCES = $(nodist_libdashqt_a_SOURCES)

#Generating these with a half-written protobuf header leads to wacky results.
#This makes sure it's done.
$(QT_MOC): $(PROTOBUF_H)
$(QT_MOC_CPP): $(PROTOBUF_H)

# sibcoin-qt binary #
sibcoin_qt_CPPFLAGS = $(AM_CPPFLAGS) $(QT_INCLUDES) \
  -I$(top_srcdir)/src/qt/forms
sibcoin_qt_SOURCES = sibcoin.cpp
if TARGET_DARWIN
  sibcoin_qt_SOURCES += $(BITCOIN_MM)
endif
if TARGET_WINDOWS
  sibcoin_qt_SOURCES += $(SIBCOIN_RC)
endif
sibcoin_qt_LDADD = libdashqt.a $(LIBBITCOIN_SERVER)
if ENABLE_WALLET
sibcoin_qt_LDADD += $(LIBBITCOIN_WALLET)
endif
sibcoin_qt_LDADD += $(LIBBITCOIN_CLI) $(LIBBITCOIN_COMMON) $(LIBLEVELDB) $(LIBMEMENV) \
  $(BOOST_LIBS) $(QT_LIBS) $(QT_DBUS_LIBS) $(QR_LIBS) $(PROTOBUF_LIBS) $(BDB_LIBS)
sibcoin_qt_LDFLAGS = $(QT_LDFLAGS)

# forms/foo.h -> forms/ui_foo.h
QT_FORMS_H=$(join $(dir $(QT_FORMS_UI)),$(addprefix ui_, $(notdir $(QT_FORMS_UI:.ui=.h))))

#locale/foo.ts -> locale/foo.qm
QT_QM=$(QT_TS:.ts=.qm)

.PHONY: FORCE
.SECONDARY: $(QT_QM)

sibcoinstrings.cpp: FORCE
	$(MAKE) -C $(top_srcdir)/src qt/sibcoinstrings.cpp

translate: sibcoinstrings.cpp $(QT_FORMS_UI) $(QT_FORMS_UI) $(BITCOIN_QT_CPP) $(BITCOIN_QT_H) $(BITCOIN_MM)
	@test -n $(LUPDATE) || echo "lupdate is required for updating translations"
	@QT_SELECT=$(QT_SELECT) $(LUPDATE) $^ -locations relative -no-obsolete -ts locale/sibcoi_en.ts  

$(QT_QRC_CPP): $(QT_QRC) $(QT_QM) $(QT_FORMS_H) $(RES_ICONS) $(RES_IMAGES) $(RES_MOVIES) $(RES_CSS) $(PROTOBUF_H)
	@cd $(abs_srcdir); test -f $(RCC) && QT_SELECT=$(QT_SELECT) $(RCC) -name sibcoin -o $(abs_builddir)/$@ $< || \
	  echo error: could not build $@
	$(SED) -e '/^\*\*.*Created:/d' $@ > $@.n && mv $@{.n,}
	$(SED) -e '/^\*\*.*by:/d' $@  > $@.n && mv $@{.n,}

CLEANFILES = $(BUILT_SOURCES) $(QT_QM) $(QT_FORMS_H) *.gcda *.gcno
