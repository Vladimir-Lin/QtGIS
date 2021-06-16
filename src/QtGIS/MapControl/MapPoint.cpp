#include <qtgis.h>

N::Map::Point:: Point(void)
{
}

N::Map::Point:: Point    (const Point& point)
              : Geometry (point.name     () )
              , X        (point.longitude() )
              , Y        (point.latitude () )
{
  visible   = point.isVisible() ;
  mywidget  = 0                 ;
  mypixmap  = 0                 ;
  mypen     = point.mypen       ;
  homelevel = -1                ;
  minsize   = QSize(-1,-1)      ;
  maxsize   = QSize(-1,-1)      ;
}

N::Map::Point:: Point       (qreal x,qreal y,QString name,enum Alignment alignment)
              : Geometry    (name     )
              , X           (x        )
              , Y           (y        )
              , myalignment (alignment)
{
  GeometryType = "Point"      ;
  mywidget     = 0            ;
  mypixmap     = 0            ;
  visible      = true         ;
  homelevel    = -1           ;
  minsize      = QSize(-1,-1) ;
  maxsize      = QSize(-1,-1) ;
}

N::Map::Point:: Point(qreal x, qreal y, QWidget* widget, QString name, enum Alignment alignment)
              : Geometry    (name     )
              , X           (x        )
              , Y           (y        )
              , mywidget    (widget   )
              , myalignment (alignment)
{
    // Point(x, y, name, alignment);
  GeometryType = "Point"        ;
  mypixmap     = 0              ;
  visible      = true           ;
  size         = widget->size() ;
  homelevel    = -1             ;
  minsize      = QSize(-1,-1)   ;
  maxsize      = QSize(-1,-1)   ;
  mywidget    -> show (     )   ;
}

N::Map::Point:: Point(qreal x, qreal y, QPixmap* pixmap, QString name, enum Alignment alignment)
              : Geometry    (name     )
              , X           (x        )
              , Y           (y        )
              , mypixmap    (pixmap   )
              , myalignment (alignment)
{
  GeometryType = "Point"        ;
  mywidget     = 0              ;
  visible      = true           ;
  size         = pixmap->size() ;
  homelevel    = -1             ;
  minsize      = QSize(-1,-1)   ;
  maxsize      = QSize(-1,-1)   ;
}

/*
    Point& N::Map::Point::operator=(const Point& rhs)
    {
    if (this == &rhs)
    return *this;
    else
    {
    X = rhs.X;
    Y = rhs.Y;
    size = rhs.size;

    mywidget = rhs.mywidget;
    mypixmap = rhs.mypixmap;
    alignment = rhs.alignment;
    homelevel = rhs.homelevel;
    minsize = rhs.minsize;
    maxsize = rhs.maxsize;
}
}
*/

N::Map::Point::~Point(void)
{
  delete mywidget ;
  delete mypixmap ;
}

void N::Map::Point::setVisible(bool visible)
{
  Iamv(visible) = visible         ;
  if (mywidget !=0)               {
    mywidget->setVisible(visible) ;
  }                               ;
}

QRectF N::Map::Point::boundingBox(void)
{
  //TODO: have to be calculated in relation to alignment...
  return QRectF(QPointF(X, Y), displaysize) ;
}

qreal N::Map::Point::longitude(void) const
{
  return X ;
}

qreal N::Map::Point::latitude(void) const
{
  return Y ;
}

QPointF N::Map::Point::coordinate(void) const
{
  return QPointF(X,Y) ;
}

void N::Map::Point::draw(QPainter      * painter    ,
                         const Adapter * mapadapter ,
                         const QRect   & viewport   ,
                         const QPoint    offset     )
{
  if (!visible) return ;

  if (homelevel > 0) {
    int currentzoom = mapadapter->maxZoom() < mapadapter->minZoom() ? mapadapter->minZoom() - mapadapter->currentZoom() : mapadapter->currentZoom();

    // int currentzoom = mapadapter->getZoom();
    int diffzoom = homelevel-currentzoom;
    int viewheight = size.height();
    int viewwidth = size.width();
    int dfz       = 1 ;
    if (diffzoom>0)                         {
      for (int j=0;j<diffzoom;j++) dfz *= 2 ;
    }                                       ;
    viewheight = int ( viewheight / dfz )   ;
    viewwidth  = int ( viewwidth  / dfz )   ;

    if (minsize.height()!= -1 && viewheight < minsize.height())
        viewheight = minsize.height();
    else
    if (maxsize.height() != -1 && viewheight > maxsize.height())
            viewheight = maxsize.height();


        if (minsize.width()!= -1 && viewwidth < minsize.width())
            viewwidth = minsize.width();
        else if (maxsize.width() != -1 && viewwidth > maxsize.width())
            viewwidth = maxsize.width();


        displaysize = QSize(viewwidth, viewheight);
    }
    else
    {
        displaysize = size;
    }


    if (mypixmap !=0)
    {
        const QPointF c = QPointF(X, Y);
        QPoint point = mapadapter->coordinateToDisplay(c);

        if (viewport.contains(point))
        {
            QPoint alignedtopleft = alignedPoint(point);
            painter->drawPixmap(alignedtopleft.x(), alignedtopleft.y(), displaysize.width(), displaysize.height(), *mypixmap);
        }

    }
    else if (mywidget!=0)
    {
        drawWidget(mapadapter, offset);
    }

}

void N::Map::Point::drawWidget(const Adapter* mapadapter, const QPoint offset)
{
    const QPointF c = QPointF(X, Y);
    QPoint point = mapadapter->coordinateToDisplay(c);
    point -= offset;

    QPoint alignedtopleft = alignedPoint(point);
    mywidget->setGeometry(alignedtopleft.x(), alignedtopleft.y(), displaysize.width(), displaysize.height());
}

QPoint N::Map::Point::alignedPoint(const QPoint point) const
{
    QPoint alignedtopleft;
    if (myalignment == Middle)
    {
        alignedtopleft.setX(point.x()-displaysize.width()/2);
        alignedtopleft.setY(point.y()-displaysize.height()/2);
    }
    else if (myalignment == TopLeft)
    {
        alignedtopleft.setX(point.x());
        alignedtopleft.setY(point.y());
    }
    else if (myalignment == TopRight)
    {
        alignedtopleft.setX(point.x()-displaysize.width());
        alignedtopleft.setY(point.y());
    }
    else if (myalignment == BottomLeft)
    {
        alignedtopleft.setX(point.x());
        alignedtopleft.setY(point.y()-displaysize.height());
    }
    else if (myalignment == BottomRight)
    {
        alignedtopleft.setX(point.x()-displaysize.width());
        alignedtopleft.setY(point.y()-displaysize.height());
    }
    return alignedtopleft;
}


bool N::Map::Point::Touches(Point* p, const Adapter* mapadapter)
{
    if (this->isVisible() == false)
        return false;
    if (mypixmap == 0)
        return false;

    QPointF c = p->coordinate();
    // coordinate to pixel
    QPoint pxOfPoint = mapadapter->coordinateToDisplay(c);
    // size/2 Pixel toleranz aufaddieren
    QPoint p1;
    QPoint p2;

    switch (myalignment)
    {
                    case Middle:
        p1 = pxOfPoint - QPoint(displaysize.width()/2,displaysize.height()/2);
        p2 = pxOfPoint + QPoint(displaysize.width()/2,displaysize.height()/2);
        break;
                    case TopLeft:
        p1 = pxOfPoint - QPoint(displaysize.width(),displaysize.height());
        p2 = pxOfPoint;
        break;
                    case TopRight:
        p1 = pxOfPoint - QPoint(0, displaysize.height());
        p2 = pxOfPoint + QPoint(displaysize.width(),0);
        break;
                    case BottomLeft:
        p1 = pxOfPoint - QPoint(displaysize.width(), 0);
        p2 = pxOfPoint + QPoint(0, displaysize.height());
        break;
                    case BottomRight:
        p1 = pxOfPoint;
        p2 = pxOfPoint + QPoint(displaysize.width(), displaysize.height());
        break;
    }

    // calculate "Bounding Box" in coordinates
    QPointF c1 = mapadapter->displayToCoordinate(p1);
    QPointF c2 = mapadapter->displayToCoordinate(p2);


    if(this->longitude()>=c1.x() && this->longitude()<=c2.x())
    {
        if (this->latitude()<=c1.y() && this->latitude()>=c2.y())
        {
            emit(geometryClicked(this, QPoint(0,0)));
            return true;
        }
    }
    return false;
}

void N::Map::Point::setCoordinate(QPointF point)
{
  // emit(updateRequest(this));
  // emit(updateRequest(QRectF(X, Y, size.width(), size.height())));
  X = point.x();
  Y = point.y();
  // emit(updateRequest(this));
  emit updateRequest(QRectF(X, Y, size.width(), size.height())) ;
  emit positionChanged(this) ;
}

QList<N::Map::Point*> N::Map::Point::points(void)
{
  //TODO: assigning temp?!
  QList<Point*> points ;
  points.append(this)  ;
  return points        ;
}

QWidget * N::Map::Point::widget(void)
{
  return mywidget ;
}

QPixmap * N::Map::Point::pixmap(void)
{
  return mypixmap ;
}

void N::Map::Point::setBaselevel(int zoomlevel)
{
  homelevel = zoomlevel;
}

void N::Map::Point::setMinsize(QSize minsize)
{
  Iamv(minsize) = minsize;
}

void N::Map::Point::setMaxsize(QSize maxsize)
{
  Iamv(maxsize) = maxsize ;
}

N::Map::Point::Alignment N::Map::Point::alignment(void) const
{
  return myalignment ;
}
