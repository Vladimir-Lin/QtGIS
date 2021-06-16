#include <qtgis.h>

N::Map::ImagePoint:: ImagePoint ( qreal     x              ,
                                  qreal     y              ,
                                  QString   filename       ,
                                  QString   name           ,
                                  Alignment alignment      )
                   : Point      ( x , y , name , alignment )
{
  mypixmap = new QPixmap      ( filename ) ;
  size     = mypixmap -> size (          ) ;
}

N::Map::ImagePoint:: ImagePoint ( qreal     x              ,
                                  qreal     y              ,
                                  QPixmap * pixmap         ,
                                  QString   name           ,
                                  Alignment alignment      )
                   : Point      ( x , y , name , alignment )
{
  mypixmap = pixmap               ;
  size     = mypixmap -> size ( ) ;
}

N::Map::ImagePoint::~ImagePoint(void)
{
}
