#include <qtgis.h>

N::Map::FixedImageOverlay:: FixedImageOverlay          (
                              qreal   xupperleft       ,
                              qreal   yupperleft       ,
                              qreal   xlowerright      ,
                              qreal   ylowerright      ,
                              QString filename         ,
                              QString name             )
                          : ImagePoint                 (
                              xupperleft               ,
                              yupperleft               ,
                              filename                 ,
                              name                     ,
                              TopLeft                  )
                          , x_lowerright ( xlowerright )
                          , y_lowerright ( ylowerright )
{
  mypixmap = new QPixmap      ( filename ) ;
  size     = mypixmap -> size (          ) ;
}

N::Map::FixedImageOverlay:: FixedImageOverlay          (
                              qreal     xupperleft     ,
                              qreal     yupperleft     ,
                              qreal     xlowerright    ,
                              qreal     ylowerright    ,
                              QPixmap * pixmap         ,
                              QString   name           )
                          : ImagePoint                 (
                              xupperleft               ,
                              yupperleft               ,
                              pixmap                   ,
                              name                     ,
                              TopLeft                  )
                          , x_lowerright ( xlowerright )
                          , y_lowerright ( ylowerright )
{
  mypixmap = pixmap               ;
  size     = mypixmap -> size ( ) ;
}

N::Map::FixedImageOverlay::~FixedImageOverlay(void)
{
}

void N::Map::FixedImageOverlay::draw (
       QPainter      * painter       ,
       const Adapter * mapadapter    ,
       const QRect   & viewport      ,
       const QPoint    offset        )
{ Q_UNUSED ( viewport )                                               ;
  Q_UNUSED ( offset   )                                               ;
  if ( ! visible        ) return                                      ;
  if ( IsNull(mypixmap) ) return                                      ;
  /////////////////////////////////////////////////////////////////////
  const QPointF c          = QPointF ( X            , Y             ) ;
  const QPointF c2         = QPointF ( x_lowerright , y_lowerright  ) ;
  QPoint        topleft    = mapadapter -> coordinateToDisplay ( c  ) ;
  QPoint        lowerright = mapadapter -> coordinateToDisplay ( c2 ) ;
  /////////////////////////////////////////////////////////////////////
  painter -> drawPixmap                                               (
    topleft    . x ( )                                                ,
    topleft    . y ( )                                                ,
    lowerright . x ( ) - topleft . x ( )                              ,
    lowerright . y ( ) - topleft . y ( )                              ,
    *mypixmap                                                       ) ;
}
