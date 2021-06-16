#include <qtgis.h>

N::Map::ImageManager * N::Map::ImageManager::m_Instance = NULL ;

N::Map::ImageManager:: ImageManager        ( QObject * parent     )
                     : QObject             (           parent     )
                     , emptyPixmap         ( QPixmap(1,1)         )
                     , net                 ( new MapNetwork(this) )
                     , doPersistentCaching ( false                )
{
  emptyPixmap.fill(Qt::transparent)        ;
  if (QPixmapCache::cacheLimit() <= 20000) {
     QPixmapCache::setCacheLimit(20000)    ; // in kb
  }                                        ;
}

N::Map::ImageManager::~ImageManager(void)
{
  if ( NotNull(m_Instance) ) {
    delete m_Instance        ;
    m_Instance = NULL        ;
  }                          ;
  delete net                 ;
  net = NULL                 ;
}

N::Map::ImageManager * N::Map::ImageManager::instance(void)
{
  if ( IsNull(m_Instance) )           {
    m_Instance = new ImageManager ( ) ;
  }                                   ;
  return m_Instance                   ;
}

N::Map::MapNetwork * N::Map::ImageManager::Loader(void)
{
  return net ;
}

QPixmap N::Map::ImageManager::getImage(const QString & host, const QString& url)
{
  QPixmap pm                                                     ;
  //pm.fill(Qt::black);
  //is image cached (memory) or currently loading?
  if (!QPixmapCache::find(url, pm) && !net->imageIsLoading(url)) {
//  if (!images.contains(url) && !net->imageIsLoading(url))
    //image cached (persistent)?
    if ( doPersistentCaching && tileExist(url) )                 {
      loadTile ( url , pm )                                      ;
      QPixmapCache::insert(url.toLatin1().toBase64(), pm)        ;
    } else                                                       {
      //load from net, add empty image
      net->loadImage(host, url)                                  ;
      //QPixmapCache::insert(url, emptyPixmap);
      return emptyPixmap                                         ;
    }                                                            ;
  }                                                              ;
  return pm                                                      ;
}

QPixmap N::Map::ImageManager::prefetchImage(const QString & host,const QString & url)
{
  prefetch . append ( url ) ;
  return getImage ( host , url ) ;
}

void N::Map::ImageManager::receivedImage(const QPixmap pixmap,const QString & url)
{
  QPixmapCache :: insert ( url , pixmap )      ;
//images[url] = pixmap;
// needed?
  if (doPersistentCaching && !tileExist(url) ) {
    saveTile ( url , pixmap )                  ;
  }                                            ;
//((Layer*)this->parent())->imageReceived();
  if (!prefetch.contains(url))                 {
    emit imageReceived ( )                     ;
  } else                                       {
    prefetch . takeAt (prefetch.indexOf(url))  ;
  }                                            ;
}

void N::Map::ImageManager::loadingQueueEmpty(void)
{
  emit loadingFinished() ;
  //((Layer*)this->parent())->removeZoomImage();
  //qDebug() << "size of image-map: " << images.size();
  //qDebug() << "size: " << QPixmapCache::cacheLimit();
}

void N::Map::ImageManager::abortLoading(void)
{
  net -> abortLoading ( ) ;
}

void N::Map::ImageManager::setProxy(QString host,int port)
{
  net -> setProxy ( host , port ) ;
}

void N::Map::ImageManager::setCacheDir(const QDir & path)
{
  doPersistentCaching = true                          ;
  cacheDir            = path                          ;
  if ( ! cacheDir . exists ( ) )                      {
    cacheDir . mkpath ( cacheDir . absolutePath ( ) ) ;
  }                                                   ;
}

QString N::Map::ImageManager::absolutePath(QString tileName)
{
  QString Path                           ;
  tileName . replace ( "/" , "-" )       ;
  Path  = cacheDir.absolutePath()        ;
  Path += "/"                            ;
  Path += tileName.toLatin1().toBase64() ;
  return Path                            ;
}

bool N::Map::ImageManager::saveTile(QString tileName,QPixmap tileData)
{
  QString Path = absolutePath ( tileName )  ;
  QFile file ( Path )                       ;
  if ( ! file.open(QIODevice::ReadWrite) )  {
    return false                            ;
  }                                         ;
  ///////////////////////////////////////////
  QByteArray bytes                          ;
  QBuffer    buffer ( &bytes )              ;
  buffer   . open  ( QIODevice::WriteOnly ) ;
  tileData . save  ( &buffer , "PNG"      ) ;
  file     . write ( bytes                ) ;
  file     . close (                      ) ;
  return true                               ;
}

bool N::Map::ImageManager::loadTile(QString tileName,QPixmap & tileData)
{
  QString Path = absolutePath ( tileName )   ;
  QFile   file ( Path )                      ;
  if (!file.open(QIODevice::ReadOnly ))      {
    return false                             ;
  }                                          ;
  tileData . loadFromData ( file.readAll() ) ;
  file     . close        (                ) ;
  return true;
}

bool N::Map::ImageManager::tileExist(QString tileName)
{
  QString Path = absolutePath ( tileName ) ;
  QFile   file                ( Path     ) ;
  return  file . exists       (          ) ;
}
