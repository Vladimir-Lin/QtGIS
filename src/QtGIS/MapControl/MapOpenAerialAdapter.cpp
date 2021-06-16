#include <qtgis.h>

N::Map::OpenAerialAdapter:: OpenAerialAdapter(void)
                          : TileAdapter                                        (
                              "tile.openaerialmap.org"                         ,
                              "/tiles/1.0.0/openaerialmap-900913/%1/%2/%3.png" ,
                              256                                              ,
                              0                                                ,
                              17                                               )
{
}

N::Map::OpenAerialAdapter::~OpenAerialAdapter(void)
{
}
