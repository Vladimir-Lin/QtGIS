#include <qtgis.h>

N::Map::CirclePoint::CirclePoint(qreal x, qreal y, int radius, QString name, Alignment alignment, QPen* pen)
    : Point(x, y, name, alignment)
{
    size = QSize(radius, radius);
    mypixmap = new QPixmap(radius+1, radius+1);
    mypixmap->fill(Qt::transparent);
    QPainter painter(mypixmap);
    if (pen != 0)
    {
        painter.setPen(*pen);
    }
    painter.drawEllipse(0,0,radius, radius);
}

N::Map::CirclePoint::CirclePoint(qreal x, qreal y, QString name, Alignment alignment, QPen* pen)
    : Point(x, y, name, alignment)
{
    int radius = 10;
    size = QSize(radius, radius);
    mypixmap = new QPixmap(radius+1, radius+1);
    mypixmap->fill(Qt::transparent);
    QPainter painter(mypixmap);
    if (pen != 0)
    {
        painter.setPen(*pen);
    }
    painter.drawEllipse(0,0,radius, radius);
}

N::Map::CirclePoint::~CirclePoint()
{
  if (NotNull(mypixmap)) {
    delete mypixmap      ;
  }                      ;
  mypixmap = NULL        ;
}

void N::Map::CirclePoint::setPen(QPen* pen)
{
  mypen     = pen                                                  ;
  mypixmap  = new QPixmap ( size.width() + 1 , size.height() + 1 ) ;
  mypixmap -> fill        ( Qt::transparent                      ) ;
  //////////////////////////////////////////////////////////////////
  QPainter painter        ( mypixmap                             ) ;
  painter . setPen        ( *pen                                 ) ;
  painter . drawEllipse   ( 0 , 0 , size.width() , size.height() ) ;
}
