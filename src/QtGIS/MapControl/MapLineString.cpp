#include <qtgis.h>

#define GTS "LineString"

N::Map::LineString:: LineString (void)
                   : Curve      (    )
{
  GeometryType = GTS ;
}

N::Map::LineString:: LineString ( const QList<Point *> points ,
                                  QString              name   ,
                                  QPen              *  pen    )
                   : Curve      ( name                        )
{
  mypen        = pen      ;
  GeometryType = GTS      ;
  setPoints    ( points ) ;
}

N::Map::LineString::~LineString(void)
{
}

void N::Map::LineString::addPoint(Point * point)
{
  vertices . append ( point ) ;
}

QList<N::Map::Point *> N::Map::LineString::points(void)
{
  return vertices ;
}

void N::Map::LineString::setPoints(QList<Point *> points)
{
  for (int i = 0 ; i < points . size ( ) ; i++ )    {
    points . at ( i ) -> setParentGeometry ( this ) ;
  }                                                 ;
  vertices = points                                 ;
}

void N::Map::LineString::draw     (
       QPainter      * painter    ,
       const Adapter * mapadapter ,
       const QRect   & screensize ,
       const QPoint    offset     )
{
  if ( ! visible ) return                                   ;
  QPolygon p = QPolygon()                                   ;
  QPointF  c                                                ;
  ///////////////////////////////////////////////////////////
  for (int i=0; i<vertices.size(); i++)                     {
     c = vertices[i]->coordinate()                          ;
     p . append ( mapadapter -> coordinateToDisplay ( c ) ) ;
  }                                                         ;
  ///////////////////////////////////////////////////////////
  if ( NotNull(mypen) )                                     {
    painter -> save   (        )                            ;
    painter -> setPen ( *mypen )                            ;
  }                                                         ;
  ///////////////////////////////////////////////////////////
  painter -> drawPolyline ( p )                             ;
  if ( NotNull(mypen) )                                     {
    painter -> restore ( )                                  ;
  }                                                         ;
  ///////////////////////////////////////////////////////////
  for (int i=0; i<vertices.size(); i++)                     {
    vertices [ i ] -> draw                                  (
      painter                                               ,
      mapadapter                                            ,
      screensize                                            ,
      offset                                              ) ;
  }                                                         ;
}

int N::Map::LineString::numberOfPoints(void) const
{
  return vertices . count ( ) ;
}

bool N::Map::LineString::Touches(Point * geom,const Adapter * mapadapter)
{
  bool touches = false                             ;
  touchedPoints . clear ( )                        ;
  for (int i=0; i<vertices.count(); i++)           {
    if (vertices.at(i)->Touches(geom, mapadapter)) {
      touchedPoints.append(vertices.at(i))         ;
      touches = true                               ;
    }                                              ;
  }                                                ;
  //////////////////////////////////////////////////
  if ( touches )                                   {
    emit geometryClicked ( this , QPoint(0,0) )    ;
  }                                                ;
  //////////////////////////////////////////////////
  return touches                                   ;
}

bool N::Map::LineString::Touches(Geometry * geom,const Adapter * mapadapter)
{ Q_UNUSED ( geom         ) ;
  Q_UNUSED ( mapadapter   ) ;
  touchedPoints . clear ( ) ;
  return false              ;
}

QList<N::Map::Geometry *> N::Map::LineString::clickedPoints(void)
{
  return touchedPoints ;
}

bool N::Map::LineString::hasPoints(void) const
{
  return vertices . size ( ) > 0 ? true : false ;
}

bool N::Map::LineString::hasClickedPoints(void) const
{
  return touchedPoints . size ( ) > 0 ? true : false ;
}

QRectF N::Map::LineString::boundingBox(void)
{
  qreal minlon =  180                                                  ;
  qreal maxlon = -180                                                  ;
  qreal minlat =   90                                                  ;
  qreal maxlat = - 90                                                  ;
  //////////////////////////////////////////////////////////////////////
  for (int i=0; i<vertices.size(); i++)                                {
    Point * tmp = vertices . at ( i )                                  ;
    if ( tmp -> longitude ( ) < minlon ) minlon = tmp -> longitude ( ) ;
    if ( tmp -> longitude ( ) > maxlon ) maxlon = tmp -> longitude ( ) ;
    if ( tmp -> latitude  ( ) < minlat ) minlat = tmp -> latitude  ( ) ;
    if ( tmp -> latitude  ( ) > maxlat ) maxlat = tmp -> latitude  ( ) ;
  }                                                                    ;
  //////////////////////////////////////////////////////////////////////
  QPointF min  = QPointF ( minlon   , minlat   )                       ;
  QPointF max  = QPointF ( maxlon   , maxlat   )                       ;
  QPointF dist = max - min                                             ;
  QSizeF  si   = QSizeF  ( dist.x() , dist.y() )                       ;
  //////////////////////////////////////////////////////////////////////
  return QRectF          ( min      , si       )                       ;
}
