#include <qtgis.h>

N::Map::MapLayer:: MapLayer ( QString   layername  ,
                              Adapter * mapadapter ,
                              bool      takeevents )
                 : Layer    ( layername            ,
                              mapadapter           ,
                              Layer::MapLayer      ,
                              takeevents           )
{
}

N::Map::MapLayer::~MapLayer(void)
{
}
