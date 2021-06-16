#include <qtgis.h>

N::Map::YahooAdapter:: YahooAdapter(void)
                     : TileAdapter                     (
                         "png.maps.yimg.com"           ,
                         "/png?v=3.1.0&x=%2&y=%3&z=%1" ,
                         256                           ,
                         17                            ,
                         0                             )
{
  int zoom = max_zoom < min_zoom ? min_zoom - current_zoom : current_zoom;
  numberOfTiles = 1 << ( zoom + 1 ) ;
}

N::Map::YahooAdapter:: YahooAdapter(QString host,QString url               )
                     : TileAdapter (        host,        url, 256 , 17 , 0 )
{
  int zoom = max_zoom < min_zoom ? min_zoom - current_zoom : current_zoom ;
  numberOfTiles = 1 << ( zoom + 1 ) ;
}

N::Map::YahooAdapter::~YahooAdapter(void)
{
}

bool N::Map::YahooAdapter::isValid(int /*x*/, int /*y*/, int /*z*/) const
{
  return true;
}

int N::Map::YahooAdapter::tilesonzoomlevel(int zoomlevel) const
{
  return 1 << ( zoomlevel + 1 ) ;
}

int N::Map::YahooAdapter::yoffset(int y) const
{
  int zoom = max_zoom < min_zoom ? min_zoom - current_zoom : current_zoom;
  int tiles = 1 << zoom ;
  y = tiles - 1 - y ;
  return int(y);
}
