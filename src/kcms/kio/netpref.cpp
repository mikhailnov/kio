
// Own
#include "netpref.h"

// Qt
#include <QCheckBox>
#include <QGroupBox>
#include <KPluralHandlingSpinBox>
#include <QFormLayout>

// KDE
#include <ioslave_defaults.h>
#include <KLocalizedString>
#include <KPluralHandlingSpinBox>
#include <QDialog>
#include <kconfig.h>
#include <kpluginfactory.h>
#include <KConfigGroup>

// Local
#include "ksaveioconfig.h"

#define MAX_TIMEOUT_VALUE  3600

K_PLUGIN_FACTORY_DECLARATION(KioConfigFactory)

KIOPreferences::KIOPreferences(QWidget *parent, const QVariantList &)
               :KCModule(/*KioConfigFactory::componentData(),*/ parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout( this );
    mainLayout->setMargin(0);
    gb_Timeout = new QGroupBox( i18n("Timeout Values"), this );
    gb_Timeout->setWhatsThis( i18np("Here you can set timeout values. "
                                    "You might want to tweak them if your "
                                    "connection is very slow. The maximum "
                                    "allowed value is 1 second." ,
                                    "Here you can set timeout values. "
                                    "You might want to tweak them if your "
                                    "connection is very slow. The maximum "
                                    "allowed value is %1 seconds.", MAX_TIMEOUT_VALUE));
    mainLayout->addWidget( gb_Timeout );

    QFormLayout* timeoutLayout = new QFormLayout(gb_Timeout);
    sb_socketRead = new KPluralHandlingSpinBox( this );
    sb_socketRead->setSuffix( ki18np( " second", " seconds" ) );
    connect(sb_socketRead, SIGNAL(valueChanged(int)), SLOT(configChanged()));
    timeoutLayout->addRow(i18n( "Soc&ket read:" ), sb_socketRead);

    sb_proxyConnect = new KPluralHandlingSpinBox( this );
    sb_proxyConnect->setValue(0);
    sb_proxyConnect->setSuffix( ki18np( " second", " seconds" ) );
    connect(sb_proxyConnect, SIGNAL(valueChanged(int)), SLOT(configChanged()));
    timeoutLayout->addRow(i18n( "Pro&xy connect:" ), sb_proxyConnect);

    sb_serverConnect = new KPluralHandlingSpinBox( this );
    sb_serverConnect->setValue(0);
    sb_serverConnect->setSuffix( ki18np( " second", " seconds" ) );
    connect(sb_serverConnect, SIGNAL(valueChanged(int)), SLOT(configChanged()));
    timeoutLayout->addRow(i18n("Server co&nnect:"), sb_serverConnect);

    sb_serverResponse = new KPluralHandlingSpinBox( this );
    sb_serverResponse->setValue(0);
    sb_serverResponse->setSuffix( ki18np( " second", " seconds" ) );
    connect(sb_serverResponse, SIGNAL(valueChanged(int)), SLOT(configChanged()));
    timeoutLayout->addRow(i18n("&Server response:"), sb_serverResponse);

    gb_Ftp = new QGroupBox( i18n( "FTP Options" ), this );
    mainLayout->addWidget( gb_Ftp );
    QVBoxLayout* ftpLayout = new QVBoxLayout(gb_Ftp);

    cb_ftpEnablePasv = new QCheckBox( i18n( "Enable passive &mode (PASV)" ), this );
    cb_ftpEnablePasv->setWhatsThis( i18n("Enables FTP's \"passive\" mode. "
                                         "This is required to allow FTP to "
                                         "work from behind firewalls.") );
    connect(cb_ftpEnablePasv, SIGNAL(toggled(bool)), SLOT(configChanged()));
    ftpLayout->addWidget(cb_ftpEnablePasv);

    cb_ftpMarkPartial = new QCheckBox( i18n( "Mark &partially uploaded files" ), this );
    cb_ftpMarkPartial->setWhatsThis( i18n( "<p>Marks partially uploaded FTP "
                                           "files.</p><p>When this option is "
                                           "enabled, partially uploaded files "
                                           "will have a \".part\" extension. "
                                           "This extension will be removed "
                                           "once the transfer is complete.</p>") );
    connect(cb_ftpMarkPartial, SIGNAL(toggled(bool)), SLOT(configChanged()));
    ftpLayout->addWidget(cb_ftpMarkPartial);

    mainLayout->addStretch( 1 );
}

KIOPreferences::~KIOPreferences()
{
}

void KIOPreferences::load()
{
  KProtocolManager proto;

  sb_socketRead->setRange( MIN_TIMEOUT_VALUE, MAX_TIMEOUT_VALUE );
  sb_serverResponse->setRange( MIN_TIMEOUT_VALUE, MAX_TIMEOUT_VALUE );
  sb_serverConnect->setRange( MIN_TIMEOUT_VALUE, MAX_TIMEOUT_VALUE );
  sb_proxyConnect->setRange( MIN_TIMEOUT_VALUE, MAX_TIMEOUT_VALUE );

  sb_socketRead->setValue( proto.readTimeout() );
  sb_serverResponse->setValue( proto.responseTimeout() );
  sb_serverConnect->setValue( proto.connectTimeout() );
  sb_proxyConnect->setValue( proto.proxyConnectTimeout() );

  KConfig config( "kio_ftprc", KConfig::NoGlobals );
  cb_ftpEnablePasv->setChecked( !config.group("").readEntry( "DisablePassiveMode", false ) );
  cb_ftpMarkPartial->setChecked( config.group("").readEntry( "MarkPartial", true ) );
  emit changed( false );
}

void KIOPreferences::save()
{
  KSaveIOConfig::setReadTimeout( sb_socketRead->value() );
  KSaveIOConfig::setResponseTimeout( sb_serverResponse->value() );
  KSaveIOConfig::setConnectTimeout( sb_serverConnect->value() );
  KSaveIOConfig::setProxyConnectTimeout( sb_proxyConnect->value() );

  KConfig config("kio_ftprc", KConfig::NoGlobals);
  config.group("").writeEntry( "DisablePassiveMode", !cb_ftpEnablePasv->isChecked() );
  config.group("").writeEntry( "MarkPartial", cb_ftpMarkPartial->isChecked() );
  config.sync();

  KSaveIOConfig::updateRunningIOSlaves(this);

  emit changed( false );
}

void KIOPreferences::defaults()
{
  sb_socketRead->setValue( DEFAULT_READ_TIMEOUT );
  sb_serverResponse->setValue( DEFAULT_RESPONSE_TIMEOUT );
  sb_serverConnect->setValue( DEFAULT_CONNECT_TIMEOUT );
  sb_proxyConnect->setValue( DEFAULT_PROXY_CONNECT_TIMEOUT );

  cb_ftpEnablePasv->setChecked( true );
  cb_ftpMarkPartial->setChecked( true );

  emit changed(true);
}

QString KIOPreferences::quickHelp() const
{
  return i18n("<h1>Network Preferences</h1>Here you can define"
              " the behavior of KDE programs when using Internet"
              " and network connections. If you experience timeouts"
              " or use a modem to connect to the Internet, you might"
              " want to adjust these settings." );
}

