#include <qtgis.h>

N::Map::MapControl:: MapControl   ( QWidget * parent , Plan * p )
                   : Widget       (           parent ,        p )
                   , scaleVisible ( true                        )
{
  mymousemode   = Panning      ;
  size          = sizeHint ( ) ;
  Configure ( )                ;
}

N::Map::MapControl:: MapControl   ( QSize s , MouseMode mousemode )
                   : Widget       ( NULL    , NULL                )
                   , size         ( s                             )
                   , mymousemode  ( mousemode                     )
                   , scaleVisible ( true                          )
{
  Configure ( ) ;
}

N::Map::MapControl::~MapControl(void)
{
  if (NotNull(layermanager)) {
    delete layermanager      ;
  }                          ;
  layermanager = NULL        ;
}

QSize N::Map::MapControl::sizeHint(void) const
{
  return QSize ( 640 , 480 ) ;
}

void N::Map::MapControl::Configure(void)
{
  lastResize    = nTimeNow                                           ;
  resizeAction  = false                                              ;
  layermanager  = new LayerManager ( this , size )                   ;
  screen_middle = QPoint(size.width()/2, size.height()/2)            ;
  mousepressed  = false                                              ;
  proxyHost     = "http://127.0.0.1"                                 ;
  proxyPort     = 8580                                               ;
  ////////////////////////////////////////////////////////////////////
  setFocusPolicy ( Qt::WheelFocus )                                  ;
  setFunction    ( 10001 , true   )                                  ;
  setFunction    ( 10002 , true   )                                  ;
  setFunction    ( 10003 , true   )                                  ;
  ////////////////////////////////////////////////////////////////////
  nConnect ( ImageManager::instance() , SIGNAL(imageReceived   ())   ,
             this                     , SLOT  (updateRequestNew()) ) ;
  nConnect ( ImageManager::instance() , SIGNAL(loadingFinished ())   ,
             this                     , SLOT  (loadingFinished ()) ) ;
  this -> setMaximumSize ( size.width() + 1 , size.height() + 1 )    ;
  ////////////////////////////////////////////////////////////////////
  MapNetwork * mn = ImageManager::instance()->Loader()               ;
  if ( IsNull(mn) ) return                                           ;
  nConnect ( mn   , SIGNAL ( startLoading () )                       ,
             this , SLOT   ( LoadingStart () )                     ) ;
  nConnect ( mn   , SIGNAL ( stopLoading  () )                       ,
             this , SLOT   ( LoadingStop  () )                     ) ;
}

bool N::Map::MapControl::FocusIn(void)
{
  AssignAction ( Label   , windowTitle      ( ) ) ;
  LinkAction   ( Refresh , updateRequestNew ( ) ) ;
  LinkAction   ( Export  , Export           ( ) ) ;
  LinkAction   ( ZoomIn  , zoomIn           ( ) ) ;
  LinkAction   ( ZoomOut , zoomOut          ( ) ) ;
  return true                                     ;
}

QPointF N::Map::MapControl::currentCoordinate(void) const
{
  if (IsNull(layermanager)) return QPointF ( 0 , 0 ) ;
  return layermanager -> currentCoordinate (       ) ;
}

N::Map::Layer * N::Map::MapControl::layer(const QString& layername) const
{
  return layermanager -> layer ( layername ) ;
}

QStringList N::Map::MapControl::layers(void) const
{
  return layermanager -> layers ( ) ;
}

int N::Map::MapControl::numberOfLayers(void) const
{
  return layermanager -> layers ( ) . size ( ) ;
}

void N::Map::MapControl::followGeometry(const Geometry * geom) const
{
  nConnect ( geom , SIGNAL ( positionChanged(Geometry*) )   ,
             this , SLOT   ( positionChanged(Geometry*) ) ) ;
}

void N::Map::MapControl::positionChanged(Geometry * geom)
{
  QPoint start = layermanager->layer()->mapadapter()->coordinateToDisplay(currentCoordinate());
  QPoint dest  = layermanager->layer()->mapadapter()->coordinateToDisplay(((Point*)geom)->coordinate());
  QPoint step  = (dest-start);
  layermanager->scrollView(step);
//  setView(geom);
  update();
}

void N::Map::MapControl::moveTo(QPointF coordinate)
{
  target = coordinate                          ;
  steps  = 25                                  ;
  if (moveMutex.tryLock())                     {
    QTimer::singleShot(40, this, SLOT(tick())) ;
  }   else                                     {
//    stopMove(coordinate);
  }                                            ;
}

void N::Map::MapControl::tick(void)
{
  QPoint  start = layermanager->layer()->mapadapter()->coordinateToDisplay(currentCoordinate());
  QPoint  dest = layermanager->layer()->mapadapter()->coordinateToDisplay(target);
  QPoint  step = (dest-start)/steps;
  QPointF next = currentCoordinate()- step;
//  setView(Coordinate(next.x(), next.y()));
  layermanager->scrollView(step);
  update();
  steps--;
  if ( steps > 0 ) {
    QTimer::singleShot(40, this, SLOT(tick()));
  } else {
    moveMutex.unlock();
  }
}

void N::Map::MapControl::drawMeasurement(QPainter & painter)
{
  if ( ! scaleVisible    ) return                                        ;
  if ( currentZoom() < 0 ) return                                        ;
  QList<double> distanceList                                             ;
  distanceList << 5000000
               << 2000000
               << 1000000
               << 1000000
               << 1000000
               <<  100000
               <<  100000
               <<   50000
               <<   50000
               <<   10000
               <<   10000
               <<   10000
               <<    1000
               <<    1000
               <<     500
               <<     200
               <<     100
               <<      50
               <<      25                                                ;
  ////////////////////////////////////////////////////////////////////////
  if ( distanceList.count() < currentZoom() ) return                     ;
  int zl = 18 - currentZoom ( )                                          ;
  if ( zl < 0 ) return                                                   ;
  ////////////////////////////////////////////////////////////////////////
  double line                                                            ;
  line = distanceList.at( currentZoom() ) / ( 1 << zl ) / 0.597164       ;
  ////////////////////////////////////////////////////////////////////////
  painter . setPen ( Qt::black )                                         ;
  ////////////////////////////////////////////////////////////////////////
  QPoint p1 ( 10,size.height()-20                                    )   ;
  QPoint p2 ((int)line,size.height()-20                              )   ;
  QLine  L1 ( p1 , p2                                                )   ;
  QLine  L2 ( 10,size.height()-15, 10,size.height()-25               )   ;
  QLine  L3 ( (int)line,size.height()-15, (int)line,size.height()-25 )   ;
  painter . drawLine ( L1 )                                              ;
  painter . drawLine ( L2 )                                              ;
  painter . drawLine ( L3 )                                              ;
  ////////////////////////////////////////////////////////////////////////
  QString distance                                                       ;
  QString measure                                                        ;
  if ( distanceList . at ( currentZoom() ) >= 1000)                      {
    measure  = QVariant( distanceList.at(currentZoom())/1000).toString() ;
    distance = tr("%1 km").arg(measure)                                  ;
  } else                                                                 {
    measure  = QVariant( distanceList.at(currentZoom()) ).toString()     ;
    distance = tr("%1 m").arg(measure)                                   ;
  }                                                                      ;
  painter.drawText(QPoint((int)line+10,size.height()-15), distance)      ;
}

void N::Map::MapControl::paintEvent(QPaintEvent* evnt)
{
  QWidget::paintEvent(evnt);
  if (IsNull(layermanager)) return ;
  ///////////////////////////////////////////////////////////
  QPainter painter(this);
  // painter.translate(150,190);
  // painter.scale(0.5,0.5);
  // painter.setClipRect(0,0, size.width(), size.height());
  // painter.setViewport(10000000000,0,size.width(),size.height());
  /*
  // rotating
      rotation = 45;
      painter.translate(256,256);
      painter.rotate(rotation);
      painter.translate(-256,-256);
  */
  layermanager->drawImage(&painter);
  layermanager->drawGeoms(&painter);
  drawMeasurement ( painter ) ;
  // added by wolf
  // draw scale
  if (isFunction(10001))                                       {
    painter.drawLine(screen_middle.x(), screen_middle.y()-10   ,
                     screen_middle.x(), screen_middle.y()+10 ) ; // |
    painter.drawLine(screen_middle.x()-10, screen_middle.y()   ,
                     screen_middle.x()+10, screen_middle.y() ) ; // -
  }                                                            ;
    // int cross_x = int(layermanager->getMapmiddle_px().x())%256;
    // int cross_y = int(layermanager->getMapmiddle_px().y())%256;
    // painter.drawLine(screen_middle.x()-cross_x+cross_x, screen_middle.y()-cross_y+0,
    //   screen_middle.x()-cross_x+cross_x, screen_middle.y()-cross_y+256); // |
    // painter.drawLine(screen_middle.x()-cross_x+0, screen_middle.y()-cross_y+cross_y,
    //   screen_middle.x()-cross_x+256, screen_middle.y()-cross_y+cross_y); // -
  if (isFunction(10002))                               {
    painter.drawRect(0,0, size.width(), size.height()) ;
  }                                                    ;
  /*
  // rotating
    painter.setMatrix(painter.matrix().inverted());
  //qt = painter.transform();
     qm = painter.combinedMatrix();
  */
  if ( mousepressed && mymousemode == Dragging ) {
    QRect rect = QRect(pre_click_px, current_mouse_pos);
    painter.drawRect(rect);
  }
  emit viewChanged ( currentCoordinate() , currentZoom() ) ;
}

void N::Map::MapControl::contextMenuEvent(QContextMenuEvent * evnt)
{
  if (Menu(evnt->pos())) evnt->accept(); else
  QWidget::contextMenuEvent(evnt);
}

void N::Map::MapControl::resizeEvent(QResizeEvent * evnt)
{
  if (Relocation()) evnt->accept() ; else
  QWidget::resizeEvent(evnt)  ;
}

void N::Map::MapControl::focusInEvent(QFocusEvent * event)
{
  if (!focusIn (event)) QWidget::focusInEvent (event) ;
}

void N::Map::MapControl::focusOutEvent(QFocusEvent * event)
{
  if (!focusOut(event)) QWidget::focusOutEvent(event) ;
}

// mouse events
void N::Map::MapControl::mousePressEvent(QMouseEvent* evnt)
{
  if (IsNull(layermanager))                                                {
    evnt->accept()                                                         ;
    return                                                                 ;
  }                                                                        ;
  //rotating (experimental)
  // QMouseEvent* me = new QMouseEvent(evnt->type(), qm.map(QPoint(evnt->x(),evnt->y())), evnt->button(), evnt->buttons(), evnt->modifiers());
  // evnt = me;
  layermanager -> mouseEvent ( evnt )                                      ;
  if (layermanager->layers().size()>0)                                     {
    if ( evnt->button() == Qt::LeftButton )                                {
      mousepressed = true                                                  ;
      pre_click_px = evnt->pos()                                           ;
    }                                                                      ;
  }                                                                        ;
  // emit(mouseEvent(evnt));
  emit mouseEventCoordinate ( evnt , clickToWorldCoordinate(evnt->pos()) ) ;
}

void N::Map::MapControl::mouseReleaseEvent(QMouseEvent* evnt)
{
  mousepressed = false                                                 ;
  if ( mymousemode == Dragging )                                       {
    QPointF ulCoord      = clickToWorldCoordinate (pre_click_px      ) ;
    QPointF lrCoord      = clickToWorldCoordinate (current_mouse_pos ) ;
    QRectF  coordinateBB = QRectF ( ulCoord                            ,
                                    QSizeF( (lrCoord-ulCoord).x()      ,
                                            (lrCoord-ulCoord).y())   ) ;
    emit boxDragged ( coordinateBB )                                   ;
  }                                                                    ;
  emit mouseEventCoordinate(evnt, clickToWorldCoordinate(evnt->pos())) ;
}

void N::Map::MapControl::mouseMoveEvent(QMouseEvent* evnt)
{
  // emit(mouseEvent(evnt));
  /*
  // rotating
     QMouseEvent* me = new QMouseEvent(evnt->type(), qm.map(QPoint(evnt->x(),evnt->y())), evnt->button(), evnt->buttons(), evnt->modifiers());
     evnt = me;
  */
  if ( mousepressed && mymousemode == Panning ) {
     QPoint offset = pre_click_px - QPoint(evnt->x(), evnt->y());
     layermanager->scrollView(offset);
     pre_click_px = QPoint(evnt->x(), evnt->y());
  } else
  if ( mousepressed && mymousemode == Dragging ) {
     current_mouse_pos = QPoint(evnt->x(), evnt->y());
  }
  // emit(mouseEventCoordinate(evnt, clickToWorldCoordinate(evnt->pos())));
  update();
  // emit(mouseEventCoordinate(evnt, clickToWorldCoordinate(evnt->pos())));
}

QPointF N::Map::MapControl::clickToWorldCoordinate(QPoint click)
{
  // click coordinate to image coordinate
  QPoint mmp            ;
  QPoint displayToImage ;
  mmp             = layermanager->getMapmiddle_px() ;
  displayToImage  = click ;
  displayToImage -= screen_middle ;
  displayToImage += mmp ;
  // image coordinate to world coordinate
  return layermanager->layer()->mapadapter()->displayToCoordinate(displayToImage);
}

void N::Map::MapControl::updateRequest(QRect rect)
{
  update ( rect ) ;
}

void N::Map::MapControl::updateRequestNew(void)
{
  if ( IsNull(layermanager) ) return ;
//  if ( isBusy(            ) ) return ;
  layermanager->forceRedraw ( )      ;
  update                    ( )      ;
}

// slots
void N::Map::MapControl::zoomIn(void)
{
  if ( IsNull(layermanager) ) return ;
//  if ( isBusy(            ) ) return ;
  layermanager->zoomIn ( )           ;
  update               ( )           ;
}

void N::Map::MapControl::zoomOut(void)
{
  if ( IsNull(layermanager) ) return ;
//  if ( isBusy(            ) ) return ;
  layermanager -> zoomOut ( )        ;
  update                  ( )        ;
}

void N::Map::MapControl::setZoom(int zoomlevel)
{
  if ( IsNull(layermanager) ) return    ;
//  if ( isBusy(            ) ) return    ;
  layermanager -> setZoom ( zoomlevel ) ;
  update                  (           ) ;
}

int N::Map::MapControl::currentZoom(void) const
{
  if ( IsNull(layermanager) ) return 0   ;
  return layermanager -> currentZoom ( ) ;
}

void N::Map::MapControl::scrollLeft(int pixel)
{
  if ( IsNull(layermanager) ) return              ;
//  if ( isBusy(            ) ) return              ;
  layermanager -> scrollView ( QPoint(-pixel,0) ) ;
  update                     (                  ) ;
}

void N::Map::MapControl::scrollRight(int pixel)
{
  if ( IsNull(layermanager) ) return             ;
//  if ( isBusy(            ) ) return             ;
  layermanager -> scrollView ( QPoint(pixel,0) ) ;
  update                     (                 ) ;
}

void N::Map::MapControl::scrollUp(int pixel)
{
  if ( IsNull(layermanager) ) return              ;
//  if ( isBusy(            ) ) return              ;
  layermanager -> scrollView ( QPoint(0,-pixel) ) ;
  update                     (                  ) ;
}

void N::Map::MapControl::scrollDown(int pixel)
{
  if ( IsNull(layermanager) ) return             ;
//  if ( isBusy(            ) ) return             ;
  layermanager -> scrollView ( QPoint(0,pixel) ) ;
  update                     (                 ) ;
}

void N::Map::MapControl::scroll(const QPoint scroll)
{
  if ( IsNull(layermanager) ) return    ;
//  if ( isBusy(            ) ) return    ;
  layermanager -> scrollView ( scroll ) ;
  update                     (        ) ;
}

void N::Map::MapControl::setView(const QPointF & coordinate) const
{
  if ( IsNull(layermanager) ) return     ;
  layermanager -> setView ( coordinate ) ;
}

void N::Map::MapControl::setView(const QList<QPointF> coordinates) const
{
  if ( IsNull(layermanager) ) return      ;
  layermanager -> setView ( coordinates ) ;
}

void N::Map::MapControl::setViewAndZoomIn(const QList<QPointF> coordinates) const
{
  if ( IsNull(layermanager) ) return               ;
  layermanager -> setViewAndZoomIn ( coordinates ) ;
}

void N::Map::MapControl::setView(const Point * point) const
{
  if ( IsNull(layermanager) ) return              ;
  if ( IsNull(point       ) ) return              ;
  layermanager -> setView ( point->coordinate() ) ;
}

void N::Map::MapControl::loadingFinished(void)
{
  if ( IsNull(layermanager) ) return  ;
  layermanager -> removeZoomImage ( ) ;
}

void N::Map::MapControl::addLayer(Layer * layer)
{
  if ( IsNull(layermanager) ) return ;
  if ( IsNull(layer       ) ) return ;
  layermanager -> addLayer ( layer ) ;
}

void N::Map::MapControl::addSource(QString name)
{
  GoogleAdapter          * google     = NULL                      ;
  GoogleSatelliteAdapter * satellite  = NULL                      ;
  OsmAdapter             * osm        = NULL                      ;
  YahooAdapter           * yahoo      = NULL                      ;
  OpenAerialAdapter      * openaerial = NULL                      ;
  MapLayer               * map        = NULL                      ;
  QString                  n          = name . toLower ( )        ;
  /////////////////////////////////////////////////////////////////
  if ( "openstreetmap" == n )                                     {
    osm        = new OsmAdapter             (                   ) ;
    map        = new MapLayer               ( name , osm        ) ;
    ImageManager::instance()->Loader()->disableProxy()            ;
  } else
  if ( "satellite"     == n )                                     {
    satellite  = new GoogleSatelliteAdapter (                   ) ;
    map        = new MapLayer               ( name , satellite  ) ;
    if (isFunction(10003))                                        {
      ImageManager::instance()->Loader()->setProxy(proxyHost,proxyPort) ;
    } else                                                        {
      ImageManager::instance()->Loader()->disableProxy()          ;
    }                                                             ;
  } else
  if ( "openaerial"    == n )                                     {
    openaerial = new OpenAerialAdapter      (                   ) ;
    map        = new MapLayer               ( name , openaerial ) ;
    ImageManager::instance()->Loader()->disableProxy()            ;
  } else
  if ( "yahoo"         == n )                                     {
    yahoo      = new YahooAdapter           (                   ) ;
    map        = new MapLayer               ( name , yahoo      ) ;
    if (isFunction(10003))                                        {
      ImageManager::instance()->Loader()->setProxy(proxyHost,proxyPort) ;
    } else                                                        {
      ImageManager::instance()->Loader()->disableProxy()          ;
    }                                                             ;
  } else
  if ( "google"        == n )                                     {
    google     = new GoogleAdapter          (                   ) ;
    map        = new MapLayer               ( name , google     ) ;
    if (isFunction(10003))                                        {
      ImageManager::instance()->Loader()->setProxy(proxyHost,proxyPort) ;
    } else                                                        {
      ImageManager::instance()->Loader()->disableProxy()          ;
    }                                                             ;
  }                                                               ;
  /////////////////////////////////////////////////////////////////
  if ( NotNull(map) ) addLayer              ( map               ) ;
}

void N::Map::MapControl::setMouseMode(MouseMode mousemode)
{
  mymousemode = mousemode;
}

N::Map::MapControl::MouseMode N::Map::MapControl::mouseMode(void)
{
  return mymousemode ;
}

void N::Map::MapControl::stopFollowing(Geometry * geom)
{
  if ( IsNull(geom) ) return                                ;
  geom -> disconnect ( SIGNAL(positionChanged(Geometry*)) ) ;
}

void N::Map::MapControl::enablePersistentCache(const QDir & path)
{
  ImageManager :: instance ( ) -> setCacheDir ( path ) ;
}

void N::Map::MapControl::setProxy(QString host,int port)
{
  ImageManager :: instance ( ) -> setProxy ( host , port) ;
}

void N::Map::MapControl::showScale(bool show)
{
  scaleVisible = show ;
}

void N::Map::MapControl::resize(const QSize newSize)
{
  size = newSize                                                           ;
  screen_middle = QPoint (newSize.width()/2, newSize.height()/2)           ;
  setMaximumSize    ( newSize . width ( ) + 1 , newSize . height ( ) + 1 ) ;
  layermanager -> resize ( newSize )                                       ;
}

void N::Map::MapControl::LoadingStart(void)
{
  plan -> StartBusy ( ) ;
}

void N::Map::MapControl::LoadingStop(void)
{
  plan -> StopBusy  ( ) ;
}

bool N::Map::MapControl::isBusy(void)
{
  MapNetwork * mn = ImageManager::instance()->Loader() ;
  if ( IsNull(mn) ) return false                       ;
  return mn -> isBusy ( )                              ;
}

bool N::Map::MapControl::Relocation(void)
{
  QRect g = geometry ( ) ;
  resize ( g.size() )    ;
  return true            ;
}

QString N::Map::MapControl::TwoDigits(int v)
{
  return QString("%1%2").arg(v/10).arg(v%10) ;
}

QString N::Map::MapControl::DegreeToString(double v)
{
  QChar   DS ( (ushort)0x00B0 ) ;
  QString m                     ;
  ///////////////////////////////
  int     D[4]                  ;
  D[0]  = (int)v                ;
  v    -= D[0]                  ;
  v    *=  60.0                 ;
  D[1]  = (int)v                ;
  v    -= D[1]                  ;
  v    *=  60.0                 ;
  D[2]  = (int)v                ;
  v    -= D[2]                  ;
  v    *= 100.0                 ;
  D[3]  = (int)v                ;
  ///////////////////////////////
  m = tr("%1%2%3'%4.%5\""       )
      . arg ( D[0]              )
      . arg ( DS                )
      . arg ( TwoDigits(D[1])   )
      . arg ( TwoDigits(D[2])   )
      . arg ( TwoDigits(D[3]) ) ;
  ///////////////////////////////
  return m                      ;
}

QString N::Map::MapControl::PositionToString(QPointF p)
{
  QString m                                             ;
  QString X                                             ;
  QString Y                                             ;
  QString XD                                            ;
  QString YD                                            ;
  double  x = p.x()                                     ;
  double  y = p.y()                                     ;
  ///////////////////////////////////////////////////////
  if (x<0)                                              {
    XD = tr("West" )                                    ;
    x  = -x                                             ;
  } else                                                {
    XD = tr("East" )                                    ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  if (y<0)                                              {
    YD = tr("South")                                    ;
    y  = -y                                             ;
  } else                                                {
    YD = tr("North")                                    ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  X = DegreeToString ( x )                              ;
  Y = DegreeToString ( y )                              ;
  ///////////////////////////////////////////////////////
  m = tr("%1 %2 , %3 %4").arg(X).arg(XD).arg(Y).arg(YD) ;
  return m                                              ;
}

void N::Map::MapControl::Export(void)
{
  QString filename = QString("%1.png").arg(windowTitle()) ;
  filename = QFileDialog::getSaveFileName                 (
               this                                       ,
               tr("Export image")                         ,
               plan->Temporary(filename)                  ,
               tr("Portable Network Graphics (*.png)")  ) ;
  if (filename.length()<=0) return                        ;
  QImage I ( geometry().size () , QImage::Format_ARGB32 ) ;
  QPainter p                                              ;
  p . begin ( &I       )                                  ;
  render    ( &p       )                                  ;
  p . end   (          )                                  ;
  I . save  ( filename )                                  ;
}

bool N::Map::MapControl::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                                                         ;
  ///////////////////////////////////////////////////////////////////////////////////
  QMenu     * mc = NULL                                                             ;
  QMenu     * md = NULL                                                             ;
  QMenu     * mp = NULL                                                             ;
  QAction   * aa = NULL                                                             ;
  QLineEdit * phost                                                                 ;
  QSpinBox  * pport                                                                 ;
  QPointF     pf                                                                    ;
  QString     mpf                                                                   ;
  ///////////////////////////////////////////////////////////////////////////////////
  pf  = clickToWorldCoordinate ( pos )                                              ;
  mpf = PositionToString       ( pf  )                                              ;
  ///////////////////////////////////////////////////////////////////////////////////
  aa  = mm . add     ( 101 , mpf           )                                        ;
  aa -> setEnabled   ( false               )                                        ;
  mm  . addSeparator (                     )                                        ;
  mm  . add          ( 101 , tr("Refresh") )                                        ;
  mm  . add          ( 102 , tr("Export" ) )                                        ;
  mm  . addSeparator (                     )                                        ;
  ///////////////////////////////////////////////////////////////////////////////////
  mc  = mm.addMenu   ( tr("Edit mode")                                            ) ;
  mm  . add          ( mc , 601 , tr("Spot")                                      ) ;
  mm  . add          ( mc , 301 , tr("Pan" ) , true , ( mymousemode == Panning  ) ) ;
  mm  . add          ( mc , 302 , tr("Drag") , true , ( mymousemode == Dragging ) ) ;
  mm  . add          ( mc , 303 , tr("None") , true , ( mymousemode == None     ) ) ;
  ///////////////////////////////////////////////////////////////////////////////////
  md  = mm.addMenu   ( tr("Drawings" )                                            ) ;
  mm  . add          ( md , 401 , tr("Scale" ) , true , scaleVisible              ) ;
  mm  . add          ( md , 402 , tr("Axis"  ) , true , isFunction(10001)         ) ;
  mm  . add          ( md , 403 , tr("Border") , true , isFunction(10002)         ) ;
  ///////////////////////////////////////////////////////////////////////////////////
  phost   = new QLineEdit ( )                                                       ;
  pport   = new QSpinBox  ( )                                                       ;
  phost  -> setText  ( proxyHost           )                                        ;
  pport  -> setRange ( 1 , 65535           )                                        ;
  pport  -> setValue ( proxyPort           )                                        ;
  mp  = mm.addMenu   ( tr("Proxy")         )                                        ;
  mm  . add          ( mp                                                           ,
                       501                                                          ,
                       tr("Use proxy")                                              ,
                       true                                                         ,
                       ImageManager::instance()->Loader()->viaProxy()             ) ;
  mm  . add          ( mp , 502 , phost    )                                        ;
  mm  . add          ( mp , 503 , pport    )                                        ;
  ///////////////////////////////////////////////////////////////////////////////////
  mm  . setFont      ( plan                )                                        ;
  aa = mm . exec     (                     )                                        ;
  proxyHost = phost -> text  ( )                                                    ;
  proxyPort = pport -> value ( )                                                    ;
  if (IsNull(aa)) return true                                                       ;
  ///////////////////////////////////////////////////////////////////////////////////
  switch (mm[aa])                                                                   {
    case 101                                                                        :
      updateRequestNew ( )                                                          ;
    return true                                                                     ;
    case 102                                                                        :
      Export           ( )                                                          ;
    return true                                                                     ;
    case 301                                                                        :
      mymousemode = Panning                                                         ;
    return true                                                                     ;
    case 302                                                                        :
      mymousemode = Dragging                                                        ;
    return true                                                                     ;
    case 303                                                                        :
      mymousemode = None                                                            ;
    return true                                                                     ;
    case 401                                                                        :
      scaleVisible = aa->isChecked()                                                ;
    return true                                                                     ;
    case 402                                                                        :
      setFunction( 10001 , aa->isChecked() )                                        ;
    return true                                                                     ;
    case 403                                                                        :
      setFunction( 10002 , aa->isChecked() )                                        ;
    return true                                                                     ;
    case 501                                                                        :
      if (aa->isChecked())                                                          {
        ImageManager::instance()->Loader()->setProxy    ( proxyHost , proxyPort   ) ;
      } else                                                                        {
        ImageManager::instance()->Loader()->disableProxy(                         ) ;
      }                                                                             ;
    return true                                                                     ;
    case 601                                                                        :
      if ( ( mpf.length() > 0 ) && ( layers().count() > 0 ) )                       {
        QStringList    lay = layers ( )                                             ;
        QPixmap     *  pix = new QPixmap ( ":/images/nail.png" )                    ;
        QPen        *  pen = new QPen ( QColor ( 0 , 0 , 255 , 100 ) )              ;
        QList<Point *> points                                                       ;
        points . append ( new ImagePoint(pf.x(),pf.y(),pix,mpf,Point::BottomLeft) ) ;
        pen   -> setWidth ( 5 )                                                     ;
        LineString * ls = new LineString ( points , mpf , pen )                     ;
        layer ( lay.first() ) -> addGeometry ( ls )                                 ;
        update ( )                                                                  ;
      }                                                                             ;
    return true                                                                     ;
  }                                                                                 ;
  return true                                                                       ;
}
