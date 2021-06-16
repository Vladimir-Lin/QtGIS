#include <qtgis.h>

N::Map::Geometry:: Geometry         ( QString name )
                 : GeometryType     ( "Geometry"   )
                 , myparentGeometry ( 0            )
                 , mypen            ( 0            )
                 , visible          ( true         )
                 , myname           ( name         )
{
}

N::Map::Geometry::~Geometry(void)
{
}

QString N::Map::Geometry::name(void) const
{
  return myname ;
}

N::Map::Geometry * N::Map::Geometry::parentGeometry(void) const
{
  return myparentGeometry ;
}

void N::Map::Geometry::setParentGeometry(Geometry * geom)
{
  myparentGeometry = geom ;
}

bool N::Map::Geometry::hasPoints(void) const
{
  return false ;
}

bool N::Map::Geometry::hasClickedPoints(void) const
{
  return false ;
}

QList<N::Map::Geometry *> N::Map::Geometry::clickedPoints(void)
{
  QList<Geometry *> tmp ;
  return tmp            ;
}

bool N::Map::Geometry::isVisible(void) const
{
  return visible ;
}

void N::Map::Geometry::setVisible(bool visible)
{
  Iamv(visible) = visible           ;
  emit updateRequest(boundingBox()) ;
}

void N::Map::Geometry::setName(QString name)
{
  myname = name;
}

void N::Map::Geometry::setPen(QPen * pen)
{
  mypen = pen ;
}

QPen * N::Map::Geometry::pen(void) const
{
  return mypen ;
}
