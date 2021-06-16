#include <qtgis.h>

N::Map::Layer:: Layer(QString layername,Adapter * mapadapter,enum LayerType layertype,bool takeevents)
              : visible             ( true           )
              , mylayername         ( layername      )
              , mylayertype         ( layertype      )
              , mapAdapter          ( mapadapter     )
              , takeevents          ( takeevents     )
              , myoffscreenViewport ( QRect(0,0,0,0) )
{
  //qDebug() << "creating new Layer: " << layername << ", type: " << contents;
  //qDebug() << this->layertype;
}

N::Map::Layer::Layer (const Layer & old)
{
  visible             = old . visible             ;
  mylayername         = old . mylayername         ;
  mylayertype         = old . mylayertype         ;
  size                = old . size                ;
  screenmiddle        = old . screenmiddle        ;
  geometries          = old . geometries          ;
  mapAdapter          = old . mapAdapter          ;
  takeevents          = old . takeevents          ;
  myoffscreenViewport = old . myoffscreenViewport ;
}

N::Map::Layer::~Layer(void)
{
  if (NotNull(mapAdapter)) {
    delete mapAdapter      ;
  }                        ;
}

void N::Map::Layer::setSize(QSize size)
{
  this->size = size;
  screenmiddle = QPoint(size.width()/2, size.height()/2);
  //QMatrix mat;
  //mat.translate(480/2, 640/2);
  //mat.rotate(45);
  //mat.translate(-480/2,-640/2);
  //screenmiddle = mat.map(screenmiddle);
}

QString N::Map::Layer::layername(void) const
{
  return mylayername ;
}

const N::Map::Adapter* N::Map::Layer::mapadapter(void) const
{
  return mapAdapter ;
}

void N::Map::Layer::setVisible(bool visible)
{
  Iamv(visible) = visible ;
  emit updateRequest()    ;
}

void N::Map::Layer::addGeometry(Geometry* geom)
{
    //qDebug() << geom->getName() << ", " << geom->getPoints().at(0)->getWidget();

    geometries.append(geom);
    emit(updateRequest(geom->boundingBox()));
    //a geometry can request a redraw, e.g. when its position has been changed
    connect(geom, SIGNAL(updateRequest(QRectF)),
            this, SIGNAL(updateRequest(QRectF)));
}

void N::Map::Layer::removeGeometry(Geometry* geometry)
{
    for (int i=0; i<geometries.count(); i++)
    {
        if (geometry == geometries.at(i))
        {
            disconnect(geometry);
            geometries.removeAt(i);
            //delete geometry;
        }
    }
}

void N::Map::Layer::clearGeometries(void)
{
  foreach(Geometry *geometry, geometries) {
    disconnect(geometry);
  }
  geometries.clear();
}

bool N::Map::Layer::isVisible(void) const
{
  return visible ;
}

void N::Map::Layer::zoomIn(void) const
{
  mapAdapter->zoom_in() ;
}

void N::Map::Layer::zoomOut(void) const
{
  mapAdapter->zoom_out() ;
}

void N::Map::Layer::mouseEvent(const QMouseEvent * evnt,const QPoint mapmiddle_px)
{
    if (takesMouseEvents())
    {
        if (evnt->button() == Qt::LeftButton && evnt->type() == QEvent::MouseButtonPress)
        {
            // check for collision
            QPointF c = mapAdapter->displayToCoordinate(QPoint(evnt->x()-screenmiddle.x()+mapmiddle_px.x(),
                                                               evnt->y()-screenmiddle.y()+mapmiddle_px.y()));
            Point* tmppoint = new Point(c.x(), c.y());
            for (int i=0; i<geometries.count(); i++)
            {
                if (geometries.at(i)->isVisible() && geometries.at(i)->Touches(tmppoint, mapAdapter))

                    //if (geometries.at(i)->Touches(c, mapAdapter))
                {
                    emit(geometryClicked(geometries.at(i), QPoint(evnt->x(), evnt->y())));
                }
            }
            delete tmppoint;
        }
    }
}

bool N::Map::Layer::takesMouseEvents(void) const
{
  return takeevents ;
}

void N::Map::Layer::drawYourImage(QPainter* painter, const QPoint mapmiddle_px) const
{
    if (mylayertype == MapLayer)
    {
        //qDebug() << ":: " << mapmiddle_px;
        //QMatrix mat;
        //mat.translate(480/2, 640/2);
        //mat.rotate(45);
        //mat.translate(-480/2,-640/2);

        //mapmiddle_px = mat.map(mapmiddle_px);
        //qDebug() << ":: " << mapmiddle_px;
        _draw(painter, mapmiddle_px);
    }

    drawYourGeometries(painter, QPoint(mapmiddle_px.x()-screenmiddle.x(), mapmiddle_px.y()-screenmiddle.y()), myoffscreenViewport);
}

void N::Map::Layer::drawYourGeometries(QPainter* painter, const QPoint mapmiddle_px, QRect viewport) const
{
    QPoint offset;
    if (mylayertype == MapLayer)
        offset = mapmiddle_px;
    else
        offset = mapmiddle_px-screenmiddle;

    painter->translate(-mapmiddle_px+screenmiddle);
    for (int i=0; i<geometries.count(); i++)
    {
        geometries.at(i)->draw(painter, mapAdapter, viewport, offset);
    }
    painter->translate(mapmiddle_px-screenmiddle);

}

void N::Map::Layer::_draw(QPainter * painter, const QPoint mapmiddle_px) const
{
    // screen middle rotieren...

    int tilesize = mapAdapter->tilesize();
    int cross_x = int(mapmiddle_px.x())%tilesize; // position on middle tile
    int cross_y = int(mapmiddle_px.y())%tilesize;
    //qDebug() << screenmiddle << " - " << cross_x << ", " << cross_y;

    // calculate how many surrounding tiles have to be drawn to fill the display
    int space_left = screenmiddle.x() - cross_x;
    int tiles_left = space_left/tilesize;
    if (space_left>0)
        tiles_left+=1;

    int space_above = screenmiddle.y() - cross_y;
    int tiles_above = space_above/tilesize;
    if (space_above>0)
        tiles_above+=1;

    int space_right = screenmiddle.x() - (tilesize-cross_x);
    int tiles_right = space_right/tilesize;
    if (space_right>0)
        tiles_right+=1;

    int space_bottom = screenmiddle.y() - (tilesize-cross_y);
    int tiles_bottom = space_bottom/tilesize;
    if (space_bottom>0)
        tiles_bottom+=1;

    //int tiles_displayed = 0;
    int mapmiddle_tile_x = mapmiddle_px.x()/tilesize;
    int mapmiddle_tile_y = mapmiddle_px.y()/tilesize;

    const QPoint from = QPoint((-tiles_left+mapmiddle_tile_x)*tilesize, (-tiles_above+mapmiddle_tile_y)*tilesize);
    const QPoint to = QPoint((tiles_right+mapmiddle_tile_x+1)*tilesize, (tiles_bottom+mapmiddle_tile_y+1)*tilesize);

    myoffscreenViewport = QRect(from, to);

    // for the EmptyMapAdapter no tiles should be loaded and painted.
    if (mapAdapter->host() == "")
    {
        return;
    }

    if (mapAdapter->isValid(mapmiddle_tile_x, mapmiddle_tile_y, mapAdapter->currentZoom()))
    {
        painter->drawPixmap(-cross_x+size.width(),
                            -cross_y+size.height(),
                            ImageManager::instance()->getImage(mapAdapter->host(), mapAdapter->query(mapmiddle_tile_x, mapmiddle_tile_y, mapAdapter->currentZoom())));
    }

    for (int i=-tiles_left+mapmiddle_tile_x; i<=tiles_right+mapmiddle_tile_x; i++)
    {
        for (int j=-tiles_above+mapmiddle_tile_y; j<=tiles_bottom+mapmiddle_tile_y; j++)
        {
            // check if image is valid
            if (!(i==mapmiddle_tile_x && j==mapmiddle_tile_y))
                if (mapAdapter->isValid(i, j, mapAdapter->currentZoom()))
                {

                painter->drawPixmap(((i-mapmiddle_tile_x)*tilesize)-cross_x+size.width(),
                                    ((j-mapmiddle_tile_y)*tilesize)-cross_y+size.height(),
                                    ImageManager::instance()->getImage(mapAdapter->host(), mapAdapter->query(i, j, mapAdapter->currentZoom())));
                //if (QCoreApplication::hasPendingEvents())
                //  QCoreApplication::processEvents();
            }
        }
    }


    // PREFETCHING
    int upper = mapmiddle_tile_y-tiles_above-1;
    int right = mapmiddle_tile_x+tiles_right+1;
    int left = mapmiddle_tile_x-tiles_right-1;
    int lower = mapmiddle_tile_y+tiles_bottom+1;

    int j = upper;
    for (int i=left; i<=right; i++)
    {
        if (mapAdapter->isValid(i, j, mapAdapter->currentZoom()))
            ImageManager::instance()->prefetchImage(mapAdapter->host(), mapAdapter->query(i, j, mapAdapter->currentZoom()));
    }
    j = lower;
    for (int i=left; i<=right; i++)
    {
        if (mapAdapter->isValid(i, j, mapAdapter->currentZoom()))
            ImageManager::instance()->prefetchImage(mapAdapter->host(), mapAdapter->query(i, j, mapAdapter->currentZoom()));
    }
    int i = left;
    for (int j=upper+1; j<=lower-1; j++)
    {
        if (mapAdapter->isValid(i, j, mapAdapter->currentZoom()))
            ImageManager::instance()->prefetchImage(mapAdapter->host(), mapAdapter->query(i, j, mapAdapter->currentZoom()));
    }
    i = right;
    for (int j=upper+1; j<=lower-1; j++)
    {
        if (mapAdapter->isValid(i, j, mapAdapter->currentZoom()))
            ImageManager::instance()->prefetchImage(mapAdapter->host(), mapAdapter->query(i, j, mapAdapter->currentZoom()));
    }
}

QRect N::Map::Layer::offscreenViewport(void) const
{
  return myoffscreenViewport ;
}

void N::Map::Layer::moveWidgets(const QPoint mapmiddle_px) const
{
    for (int i=0; i<geometries.count(); i++)
    {
        const Geometry* geom = geometries.at(i);
        if (geom->GeometryType == "Point")
        {
            if (((Point*)geom)->widget()!=0)
            {
                QPoint topleft_relative = QPoint(mapmiddle_px-screenmiddle);
                ((Point*)geom)->drawWidget(mapAdapter, topleft_relative);
            }
        }
    }
}

N::Map::Layer::LayerType N::Map::Layer::layertype(void) const
{
  return mylayertype ;
}

void N::Map::Layer::setMapAdapter(Adapter * mapadapter)
{
  mapAdapter = mapadapter ;
}

N::Map::Layer & N::Map::Layer::operator = (const Layer & rhs)
{
  visible             = rhs . visible             ;
  mylayername         = rhs . mylayername         ;
  mylayertype         = rhs . mylayertype         ;
  size                = rhs . size                ;
  screenmiddle        = rhs . screenmiddle        ;
  geometries          = rhs . geometries          ;
  mapAdapter          = rhs . mapAdapter          ;
  takeevents          = rhs . takeevents          ;
  myoffscreenViewport = rhs . myoffscreenViewport ;
  return (*this)                                  ;
}
