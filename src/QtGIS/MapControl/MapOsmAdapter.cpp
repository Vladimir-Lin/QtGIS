#include <qtgis.h>

N::Map::OsmAdapter:: OsmAdapter (void)
                   : TileAdapter                (
                       "tile.openstreetmap.org" ,
                       "/%1/%2/%3.png"          ,
                       256                      ,
                       0                        ,
                       17                       )
{
}

N::Map::OsmAdapter::~OsmAdapter(void)
{
}
