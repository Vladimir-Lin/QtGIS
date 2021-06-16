#include <qtgis.h>

N::Map::GeometryLayer:: GeometryLayer          (
                          QString   layername  ,
                          Adapter * mapadapter ,
                          bool      takeevents )
                      : Layer                  (
                          layername            ,
                          mapadapter           ,
                          Layer::GeometryLayer ,
                          takeevents           )
{
}


N::Map::GeometryLayer::~GeometryLayer(void)
{
}
