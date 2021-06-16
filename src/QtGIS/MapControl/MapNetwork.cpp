#include <qtgis.h>

N::Map::MapNetwork:: MapNetwork ( ImageManager * p )
                   : QObject    ( NULL             )
                   , Thread     (                  )
                   , image      (                p )
                   , hasProxy   ( false            )
                   , ProxyHost  ( ""               )
                   , ProxyPort  ( 0                )
                   , dlTime     ( 480              )
{
  nConnect ( this , SIGNAL ( sendStart () )   ,
             this , SLOT   ( relayStart() ) ) ;
  nConnect ( this , SIGNAL ( sendStop  () )   ,
             this , SLOT   ( relayStop () ) ) ;
}

N::Map::MapNetwork:: MapNetwork ( const MapNetwork & old )
                   : QObject    ( old . parent ( )       )
                   , Thread     (                        )
                   , image      ( old . image            )
                   , hasProxy   ( old . hasProxy         )
                   , ProxyHost  ( old . ProxyHost        )
                   , ProxyPort  ( old . ProxyPort        )
                   , dlTime     ( old . dlTime           )
{
  nConnect ( this , SIGNAL ( sendStart () )   ,
             this , SLOT   ( relayStart() ) ) ;
  nConnect ( this , SIGNAL ( sendStop  () )   ,
             this , SLOT   ( relayStop () ) ) ;
}

N::Map::MapNetwork::~MapNetwork(void)
{
}

void N::Map::MapNetwork::loadImage(const QString & host,const QString & url)
{
  QUrl U                           ;
  bool loading = false             ;
  int  id      = 0                 ;
  loading = (loadingMap.count()>0) ;
  U . setScheme ( "http" )         ;
  U . setHost   ( host   )         ;
  U . setPath   ( url    )         ;
  if (loadingIDs.count()>0)        {
    id = loadingIDs.last()         ;
  }                                ;
  id++                             ;
  vectorMutex.lock()               ;
  loadingIDs << id                 ;
  loadingMap [ id ] = U            ;
  vectorMutex . unlock ( )         ;
  if ( ! loading ) start ( 1001 )  ;
}

void N::Map::MapNetwork::abortLoading(void)
{
  if (vectorMutex.tryLock()) {
    loadingIDs  . clear  ( ) ;
    loadingMap  . clear  ( ) ;
    vectorMutex . unlock ( ) ;
  }                          ;
}

void N::Map::MapNetwork::relayStart(void)
{
  emit startLoading ( ) ;
}

void N::Map::MapNetwork::relayStop(void)
{
  emit stopLoading ( ) ;
}

bool N::Map::MapNetwork::imageIsLoading(QString url)
{
  return loadingMap . values ( ) . contains ( url ) ;
}

void N::Map::MapNetwork::setProxy(QString host,int port)
{
  hasProxy  = true ;
  ProxyHost = host ;
  ProxyPort = port ;
}

void N::Map::MapNetwork::disableProxy(void)
{
  hasProxy = false ;
}

void N::Map::MapNetwork::run(int Type,ThreadData * data)
{ Q_UNUSED ( data )       ;
  switch   ( Type )       {
    case     1001         :
      pendingDownload ( ) ;
    break                 ;
  }                       ;
}

bool N::Map::MapNetwork::viaProxy(void)
{
  return hasProxy ;
}

bool N::Map::MapNetwork::isBusy(void)
{
  return ( loadingIDs.count() > 0 ) ;
}

void N::Map::MapNetwork::pendingDownload(void)
{
  emit sendStart ( )               ;
  while ( loadingIDs.count() > 0 ) {
    int  id                        ;
    QUrl u                         ;
    vectorMutex.lock()             ;
    id = loadingIDs.first()        ;
    u  = loadingMap [ id ]         ;
    vectorMutex . unlock ( )       ;
    ////////////////////////////////
    Download ( u )                 ;
    ////////////////////////////////
    vectorMutex.lock()             ;
    if (loadingIDs.count()>0)      {
      loadingIDs.takeAt(0)         ;
    }                              ;
    vectorMutex . unlock ( )       ;
  }                                ;
  image -> loadingQueueEmpty ( )   ;
  emit sendStop  ( )               ;
}

void N::Map::MapNetwork::Download(QUrl & url)
{
  QByteArray Blob                                  ;
  QBuffer    IO ( &Blob )                          ;
  Ftp        ftp                                   ;
  QUrl       ProxyUrl                              ;
  QString    agent                                 ;
  QString    proxyUrl                              ;
  int        DT     = dlTime * 1000                ;
  agent = N::XML::UserAgents [ rand() % 40 ]       ;
  ftp . Requests [ "user-agent" ] = agent          ;
  if (hasProxy)                                    {
    ProxyUrl . setScheme ( "http"    )             ;
    ProxyUrl . setHost   ( ProxyHost )             ;
    ProxyUrl . setPort   ( ProxyPort )             ;
    proxyUrl = QString("%1:%2").arg(ProxyHost).arg(ProxyPort) ;
    ftp . Requests [ "proxy" ] = proxyUrl          ;
  }                                                ;
  //////////////////////////////////////////////////
  if (NotNull(AppPlan) && AppPlan->Verbose>75)     {
    AppPlan->Debug ( 76 , url.toString() )         ;
  }                                                ;
  //////////////////////////////////////////////////
  bool dn = false                                  ;
  IO . open  ( QIODevice::WriteOnly )              ;
  dn = ftp . download ( url , IO , DT )            ;
  IO . close (                      )              ;
  if ( ! dn ) return                               ;
  if ( Blob  . size ( ) <= 0 ) return              ;
  //////////////////////////////////////////////////
  QPixmap pm                                       ;
  if (pm.loadFromData(Blob))                       {
    QString path = url . path ( )                  ;
    image -> receivedImage ( pm , path )           ;
  }                                                ;
}
