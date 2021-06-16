#include <qtgis.h>

N::Map::EmptyAdapter:: EmptyAdapter (int tileSize,int minZoom,int maxZoom)
                     : Adapter      ( "" , "" , 256 , minZoom,    maxZoom)
{ Q_UNUSED                         ( tileSize ) ;
  PI            = acos             ( -1.0     ) ;
  numberOfTiles = tilesonzoomlevel ( minZoom  ) ;
}

N::Map::EmptyAdapter::~EmptyAdapter(void)
{
}

void N::Map::EmptyAdapter::zoom_in(void)
{
  if ( current_zoom < max_zoom )                    {
    current_zoom = current_zoom + 1                 ;
  }                                                 ;
  numberOfTiles = tilesonzoomlevel ( current_zoom ) ;
}

void N::Map::EmptyAdapter::zoom_out(void)
{
  if ( current_zoom > min_zoom )                    {
    current_zoom = current_zoom - 1                 ;
  }                                                 ;
  numberOfTiles = tilesonzoomlevel ( current_zoom ) ;
}

qreal N::Map::EmptyAdapter::deg_rad(qreal x) const
{
  return x * ( PI / 180.0 ) ;
}

qreal N::Map::EmptyAdapter::rad_deg(qreal x) const
{
  return x * ( 180.0 / PI ) ;
}

QString N::Map::EmptyAdapter::query(int x,int y,int z) const
{ Q_UNUSED ( x ) ;
  Q_UNUSED ( y ) ;
  Q_UNUSED ( z ) ;
  return ""      ;
}

QPoint N::Map::EmptyAdapter::coordinateToDisplay(const QPointF & coordinate) const
{
  qreal x = (coordinate.x()+180) * (numberOfTiles*mytilesize)/360.; // coord to pixel!
  qreal y = (1-(log(tan(PI/4+deg_rad(coordinate.y())/2)) /PI)) /2  * (numberOfTiles*mytilesize);
  return QPoint(int(x), int(y));
}

QPointF N::Map::EmptyAdapter::displayToCoordinate(const QPoint & point) const
{
  qreal longitude = (point.x()*(360/(numberOfTiles*mytilesize)))-180;
  qreal latitude = rad_deg(atan(sinh((1-point.y()*(2/(numberOfTiles*mytilesize)))*PI)));
  return QPointF(longitude, latitude);
}

bool N::Map::EmptyAdapter::isValid(int x,int y,int z) const
{
  if ( max_zoom < min_zoom )    {
    z = min_zoom - z            ;
  }                             ;
  int Z = 1                     ;
  if (z>0) Z <<= z              ;
  Z -= 1                        ;
  if ( ( x < 0 ) || ( x > Z )  ||
       ( y < 0 ) || ( y > Z ) ) {
    return false                ;
  }                             ;
  return true                   ;
}

int N::Map::EmptyAdapter::tilesonzoomlevel(int z) const
{
  int Z = 1        ;
  if (z>0) Z <<= z ;
  return Z         ;
}

int N::Map::EmptyAdapter::xoffset(int x) const
{
  return x;
}

int N::Map::EmptyAdapter::yoffset(int y) const
{
  return y;
}
