#include <qtgis.h>

N::Map::Adapter:: Adapter    ( const QString & host       ,
                               const QString & serverPath ,
                               int             tilesize   ,
                               int             minZoom    ,
                               int             maxZoom    )
                : myhost     ( host                       )
                , serverPath ( serverPath                 )
                , mytilesize ( tilesize                   )
                , min_zoom   ( minZoom                    )
                , max_zoom   ( maxZoom                    )
{
  current_zoom = min_zoom          ;
  loc          = QLocale::system() ;
}

N::Map::Adapter::~Adapter(void)
{
}

QString N::Map::Adapter::host(void) const
{
  return myhost ;
}

int N::Map::Adapter::tilesize(void) const
{
  return mytilesize ;
}

int N::Map::Adapter::minZoom(void) const
{
  return min_zoom ;
}

int N::Map::Adapter::maxZoom(void) const
{
  return max_zoom ;
}

int N::Map::Adapter::currentZoom(void) const
{
  return current_zoom ;
}

int N::Map::Adapter::adaptedZoom(void) const
{
  return max_zoom < min_zoom     ?
         min_zoom - current_zoom :
         current_zoom            ;
}
