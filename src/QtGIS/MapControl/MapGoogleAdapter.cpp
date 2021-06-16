#include <qtgis.h>

N::Map::GoogleAdapter:: GoogleAdapter (void)
                      : TileAdapter                     (
                          "mt2.google.com"              ,
                          "/mt?n=404&x=%2&y=%3&zoom=%1" ,
                          256                           ,
                          17                            ,
                          0                             )
{
}

N::Map::GoogleAdapter::~GoogleAdapter(void)
{
}
