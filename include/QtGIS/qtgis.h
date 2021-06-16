/****************************************************************************
 *                                                                          *
 * Copyright (C) 2015 Neutrino International Inc.                           *
 *                                                                          *
 * Author : Brian Lin <lin.foxman@gmail.com>, Skype: wolfram_lin            *
 *                                                                          *
 ****************************************************************************/

#ifndef QT_GIS_H
#define QT_GIS_H

#include <QtManagers>

QT_BEGIN_NAMESPACE

#ifndef QT_STATIC
#    if defined(QT_BUILD_QTGIS_LIB)
#      define Q_GIS_EXPORT Q_DECL_EXPORT
#    else
#      define Q_GIS_EXPORT Q_DECL_IMPORT
#    endif
#else
#    define Q_GIS_EXPORT
#endif

namespace N
{

class Q_GIS_EXPORT ContinentLists         ;
class Q_GIS_EXPORT AreaLists              ;

namespace Map
{

class Q_GIS_EXPORT Adapter                ;
class Q_GIS_EXPORT Geometry               ;
class Q_GIS_EXPORT Point                  ;
class Q_GIS_EXPORT CirclePoint            ;
class Q_GIS_EXPORT ImagePoint             ;
class Q_GIS_EXPORT FixedImageOverlay      ;
class Q_GIS_EXPORT Curve                  ;
class Q_GIS_EXPORT LineString             ;
class Q_GIS_EXPORT Layer                  ;
class Q_GIS_EXPORT GeometryLayer          ;
class Q_GIS_EXPORT MapLayer               ;
class Q_GIS_EXPORT LayerManager           ;
class Q_GIS_EXPORT ImageManager           ;
class Q_GIS_EXPORT MapNetwork             ;
class Q_GIS_EXPORT EmptyAdapter           ;
class Q_GIS_EXPORT TileAdapter            ;
class Q_GIS_EXPORT WmsAdapter             ;
class Q_GIS_EXPORT GoogleAdapter          ;
class Q_GIS_EXPORT GoogleSatelliteAdapter ;
class Q_GIS_EXPORT OsmAdapter             ;
class Q_GIS_EXPORT YahooAdapter           ;
class Q_GIS_EXPORT OpenAerialAdapter      ;
class Q_GIS_EXPORT MapControl             ;

}

/*****************************************************************************
 *                                                                           *
 *                              Geographic widgets                           *
 *                                                                           *
 *****************************************************************************/

class Q_GIS_EXPORT ContinentLists : public TreeDock
{
  Q_OBJECT
  public:

    explicit     ContinentLists  (StandardConstructor) ;
    virtual     ~ContinentLists  (void) ;

  protected:

    QPoint dragPoint ;

    virtual void Configure       (void) ;
    virtual bool FocusIn         (void) ;

  private:

  public slots:

    virtual void List            (void) ;
    virtual void Copy            (void) ;

  protected slots:

    bool         Menu            (QPoint pos) ;

  private slots:

  signals:

};

class Q_GIS_EXPORT AreaLists : public TreeDock
{
  Q_OBJECT
  public:

    explicit AreaLists           (StandardConstructor) ;
    virtual ~AreaLists           (void);

  protected:

    virtual void Configure       (void) ;
    virtual bool FocusIn         (void) ;

  private:

  public slots:

    virtual bool startup         (void) ;

    virtual void Insert          (void) ;
    virtual void Delete          (void) ;

  protected slots:

    virtual bool Menu            (QPoint pos) ;
    virtual void doubleClicked   (QTreeWidgetItem * item,int column) ;
    virtual void nameFinished    (void) ;
    virtual void englishFinished (void) ;

  private slots:

  signals:

};


namespace Map
{

class Q_GIS_EXPORT Adapter : public QObject
{
  friend class Layer;
  Q_OBJECT
  public:

                    Adapter (const QString & host          ,
                             const QString & serverPath    ,
                             int             tilesize      ,
                             int             minZoom = 0   ,
                             int             maxZoom = 0 ) ;
    virtual        ~Adapter             (void) ;

    QString         host                (void) const ;
    int             tilesize            (void) const ;
    int             minZoom             (void) const ;
    int             maxZoom             (void) const ;
    int             currentZoom         (void) const ;
    virtual int     adaptedZoom         (void)const ;
    virtual QPoint  coordinateToDisplay (const QPointF & coordinate) const = 0 ;
    virtual QPointF displayToCoordinate (const QPoint  & point     ) const = 0 ;

  protected:

    virtual void    zoom_in             (void) = 0;
    virtual void    zoom_out            (void) = 0;
    virtual bool    isValid             (int x,int y,int z) const = 0 ;
    virtual QString query               (int x,int y,int z) const = 0 ;

    QSize   size          ;
    QString myhost        ;
    QString serverPath    ;
    int     mytilesize    ;
    int     min_zoom      ;
    int     max_zoom      ;
    int     current_zoom  ;
    int     param1        ;
    int     param2        ;
    int     param3        ;
    int     param4        ;
    int     param5        ;
    int     param6        ;
    QString sub1          ;
    QString sub2          ;
    QString sub3          ;
    QString sub4          ;
    QString sub5          ;
    QString sub6          ;
    int     order[3][2]   ;
    int     middle_x      ;
    int     middle_y      ;
    qreal   numberOfTiles ;
    QLocale loc           ;

};

class Q_GIS_EXPORT Layer : public QObject
{
  Q_OBJECT
  public:

    friend class LayerManager;

    enum LayerType  {
      MapLayer      , /*!< uses the MapAdapter to display maps, only gets refreshed when a new offscreen image is needed */
      GeometryLayer   /*!< gets refreshed everytime when a geometry changes */
    }               ;

             Layer (QString        layername           ,
                    Adapter      * mapadapter          ,
                    enum LayerType layertype           ,
                    bool           takeevents = true ) ;
             Layer (const Layer & old) ;
    virtual ~Layer (void);

    QString          layername       (void) const ;
    const Adapter *  mapadapter      (void) const ;
    void             addGeometry     (Geometry * geometry) ;
    void             removeGeometry  (Geometry * geometry) ;
    void             clearGeometries (void) ;
    bool             isVisible       (void) const ;
    Layer::LayerType layertype       (void) const ;
    void             setMapAdapter   (Adapter * mapadapter) ;
    Layer &          operator =      (const Layer & rhs) ;

  protected:

    void  moveWidgets        (const QPoint mapmiddle_px) const ;
    void  drawYourImage      (QPainter* painter,const QPoint mapmiddle_px) const ;
    void  drawYourGeometries (QPainter* painter,const QPoint mapmiddle_px,QRect viewport) const ;
    void  setSize            (QSize size) ;
    QRect offscreenViewport  (void) const ;
    bool  takesMouseEvents   (void) const ;
    void  mouseEvent         (const QMouseEvent *,const QPoint mapmiddle_px) ;
    void  zoomIn             (void) const ;
    void  zoomOut            (void) const ;
    void  _draw              (QPainter * painter,const QPoint mapmiddle_px) const ;

  private:

    bool              visible             ;
    QString           mylayername         ;
    LayerType         mylayertype         ;
    QSize             size                ;
    QPoint            screenmiddle        ;
    QList<Geometry *> geometries          ;
    Adapter        *  mapAdapter          ;
    bool              takeevents          ;
    mutable QRect     myoffscreenViewport ;

  public slots:

    void setVisible (bool visible) ;

  protected slots:

  private slots:

  signals:

    void geometryClicked (Geometry * geometry,QPoint point) ;
    void updateRequest   (QRectF rect) ;
    void updateRequest   (void) ;

};

class Q_GIS_EXPORT GeometryLayer : public Layer
{
  Q_OBJECT
  public:

             GeometryLayer (QString   layername           ,
                            Adapter * mapadapter          ,
                            bool      takeevents = true ) ;
    virtual ~GeometryLayer (void) ;

  protected:

  private:

};

class Q_GIS_EXPORT MapLayer : public Layer
{
  Q_OBJECT
  public:

             MapLayer (QString   layername           ,
                       Adapter * mapadapter          ,
                       bool      takeevents = true ) ;
    virtual ~MapLayer (void) ;

  protected:

  private:

};

class Q_GIS_EXPORT Geometry : public QObject
{
  Q_OBJECT
  public:

    friend class LineString ;

    explicit Geometry (QString name = QString()) ;
    virtual ~Geometry (void);

    QString GeometryType;

    bool                      Equals           (Geometry * geom) ;
    QString                   toString         (void) ;
    QString                   name             (void) const ;
    Geometry               *  parentGeometry   (void) const ;
    bool                      isVisible        (void) const ;
    void                      setName          (QString name) ;
    QPen                   *  pen              (void) const ;

    virtual QRectF            boundingBox      (void) = 0 ;
    virtual QList<Point    *> points           (void) = 0 ;

    virtual bool              Touches          (Point         * geom             ,
                                                const Adapter * mapadapter ) = 0 ;
    virtual void              draw             (QPainter      * painter      ,
                                                const Adapter * mapadapter   ,
                                                const QRect   & viewport     ,
                                                const QPoint    offset ) = 0 ;

    virtual bool              hasPoints        (void) const ;
    virtual bool              hasClickedPoints (void) const ;
    virtual void              setPen           (QPen * pen) ;
    virtual QList<Geometry *> clickedPoints    (void) ;

  protected:

    QPen  * mypen   ;
    bool    visible ;
    QString myname  ;

    void setParentGeometry (Geometry* geom) ;

  private:

    Geometry * myparentGeometry ;

    Geometry(const Geometry & old);
    Geometry & operator = (const Geometry & rhs) ;

  public slots:

    virtual void setVisible(bool visible);

  protected slots:

  private slots:

  signals:

    void updateRequest   (Geometry * geom);
    void updateRequest   (QRectF     rect);
    void geometryClicked (Geometry * geometry,QPoint point) ;
    void positionChanged (Geometry * geom);

};

class Q_GIS_EXPORT Point : public Geometry
{
  Q_OBJECT
  public:

    friend class Layer      ;
    friend class LineString ;

    //! sets where the point should be aligned
    enum Alignment  {
        TopLeft     , /*!< Align on TopLeft*/
        TopRight    , /*!< Align on TopRight*/
        BottomLeft  , /*!< Align on BottomLeft*/
        BottomRight , /*!< Align on BottomRight*/
        Middle        /*!< Align on Middle*/
    }               ;

             Point (void) ;
    explicit Point (const Point & ) ;
             Point (qreal x,qreal y,QString name = QString(),enum Alignment alignment=Middle) ;
             Point (qreal x,qreal y,QWidget * widget,QString name = QString(),enum Alignment alignment = Middle) ;
             Point (qreal x,qreal y,QPixmap * pixmap,QString name = QString(),enum Alignment alignment = Middle) ;
    virtual ~Point (void) ;

    virtual QRectF         boundingBox  (void) ;

    qreal                  longitude    (void) const ;
    qreal                  latitude     (void) const ;
    QPointF                coordinate   (void) const ;
    virtual QList<Point *> points       (void) ;
    QWidget             *  widget       (void) ;
    QPixmap             *  pixmap       (void) ;

    void                   setBaselevel (int   zoomlevel) ;
    void                   setMinsize   (QSize minsize) ;
    void                   setMaxsize   (QSize maxsize) ;

    Point::Alignment       alignment    (void) const ;

  protected:

    qreal     X           ;
    qreal     Y           ;
    QSize     size        ;
    QWidget * mywidget    ;
    QPixmap * mypixmap    ;
    Alignment myalignment ;
    int       homelevel   ;
    QSize     displaysize ;
    QSize     minsize     ;
    QSize     maxsize     ;

    void         drawWidget    (const Adapter * mapadapter  ,
                                const QPoint    offset    ) ;
    virtual void draw          (QPainter      * painter     ,
                                const Adapter * mapadapter  ,
                                const QRect   & viewport    ,
                                const QPoint    offset    ) ;
    QPoint       alignedPoint  (const QPoint    point       ) const ;

    virtual bool Touches       (Point * geom,const Adapter * mapadapter) ;

  public slots:

    void         setCoordinate (QPointF point) ;
    virtual void setVisible    (bool visible) ;

  protected slots:

  private slots:

  signals:

};

class Q_GIS_EXPORT CirclePoint : public Point
{
  Q_OBJECT
  public:

             CirclePoint (qreal     x                       ,
                          qreal     y                       ,
                          QString   name      = QString ( ) ,
                          Alignment alignment = Middle      ,
                          QPen    * pen       = 0         ) ;
             CirclePoint (qreal     x                       ,
                          qreal     y                       ,
                          int       radius    = 10          ,
                          QString   name      = QString ( ) ,
                          Alignment alignment = Middle      ,
                          QPen    * pen       = 0         ) ;
    virtual ~CirclePoint (void) ;

    virtual void setPen  (QPen * pen) ;

};

class Q_GIS_EXPORT ImagePoint : public Point
{
  Q_OBJECT
  public:

             ImagePoint (qreal     x                     ,
                         qreal     y                     ,
                         QString   filename              ,
                         QString   name      = QString() ,
                         Alignment alignment = Middle  ) ;
             ImagePoint (qreal     x                     ,
                         qreal     y                     ,
                         QPixmap * pixmap                ,
                         QString   name      = QString() ,
                         Alignment alignment = Middle  ) ;
    virtual ~ImagePoint(void) ;

};

class Q_GIS_EXPORT FixedImageOverlay : public ImagePoint
{
  Q_OBJECT
  public:

             FixedImageOverlay (qreal     x_upperleft        ,
                                qreal     y_upperleft        ,
                                qreal     x_lowerright       ,
                                qreal     y_lowerright       ,
                                QString   filename           ,
                                QString   name = QString() ) ;
             FixedImageOverlay (qreal     x_upperleft        ,
                                qreal     y_upperleft        ,
                                qreal     x_lowerright       ,
                                qreal     y_lowerright       ,
                                QPixmap * pixmap             ,
                                QString   name = QString() ) ;
    virtual ~FixedImageOverlay (void) ;

    virtual void draw          (QPainter      * painter    ,
                                const Adapter * mapadapter ,
                                const QRect   & viewport   ,
                                const QPoint    offset   ) ;

  protected:

    qreal x_lowerright ;
    qreal y_lowerright ;

  private:

};

class Q_GIS_EXPORT Curve : public Geometry
{
  Q_OBJECT
  public:

    double Length ;

             Curve (QString name = QString()) ;
    virtual ~Curve (void) ;

  protected:

  private:

};

class Q_GIS_EXPORT LineString : public Curve
{
  Q_OBJECT
  public:

             LineString (void) ;
             LineString (const QList<Point *> points           ,
                         QString              name = QString() ,
                         QPen               * pen  = 0       ) ;
    virtual ~LineString (void) ;

    QList<Point *>            points           (void) ;
    void                      addPoint         (Point * point) ;
    void                      setPoints        (QList<Point *> points) ;
    int                       numberOfPoints   (void) const ;
    virtual QRectF            boundingBox      (void) ;
    virtual bool              hasPoints        (void) const ;
    virtual bool              hasClickedPoints (void) const ;
    virtual QList<Geometry *> clickedPoints    (void) ;

  protected:

    virtual bool              Touches (Geometry * geom,const Adapter * mapadapter) ;
    virtual bool              Touches (Point    * geom,const Adapter * mapadapter) ;
    virtual void              draw    (QPainter      * painter    ,
                                       const Adapter * mapadapter ,
                                       const QRect   & screensize ,
                                       const QPoint    offset   ) ;

  private:

    QList < Point    * > vertices      ;
    QList < Geometry * > touchedPoints ;

};

class Q_GIS_EXPORT ImageManager : public QObject
{
  Q_OBJECT
  public:

             ImageManager          (QObject            * parent = 0) ;
             ImageManager          (const ImageManager & im        ) ;
    virtual ~ImageManager          (void) ;

    static ImageManager * instance (void) ;
    ImageManager & operator =      (const ImageManager & im) ;
    MapNetwork   * Loader          (void) ;
    QPixmap getImage               (const QString & host  ,const QString & path) ;
    QPixmap prefetchImage          (const QString & host  ,const QString & path) ;
    void    receivedImage          (const QPixmap   pixmap,const QString & url ) ;

    void loadingQueueEmpty         (void) ;
    void abortLoading              (void) ;
    void setProxy                  (QString host, int port) ;
    void setCacheDir               (const QDir & path) ;

  protected:

    QPixmap               emptyPixmap         ;
    MapNetwork          * net                 ;
    QStringList           prefetch            ;
    QDir                  cacheDir            ;
    bool                  doPersistentCaching ;
    static ImageManager * m_Instance          ;

    QString absolutePath           (QString tileName) ;
    bool    saveTile               (QString tileName,QPixmap   tileData) ;
    bool    loadTile               (QString tileName,QPixmap & tileData) ;
    bool    tileExist              (QString tileName) ;

  private:

  signals:

    void imageReceived             (void) ;
    void loadingFinished           (void) ;

};

class Q_GIS_EXPORT MapNetwork : public QObject
                              , public Thread
{
  Q_OBJECT
  public:

                 MapNetwork      (ImageManager     * image) ;
                 MapNetwork      (const MapNetwork & old  ) ;
    virtual     ~MapNetwork      (void) ;

    MapNetwork & operator     =  (const MapNetwork & rhs) ;
    void         loadImage       (const QString & host,const QString & url) ;
    bool         imageIsLoading  (QString url) ;
    bool         isBusy          (void) ;
    bool         viaProxy        (void) ;

  protected:

    ImageManager * image       ;
    QMap<int,QUrl> loadingMap  ;
    CUIDs          loadingIDs  ;
    QMutex         vectorMutex ;
    bool           hasProxy    ;
    QString        ProxyHost   ;
    int            ProxyPort   ;
    int            dlTime      ;

    virtual void run             (int Type,ThreadData * data) ;

    void         pendingDownload (void) ;
    void         Download        (QUrl & url) ;

  private:

  public slots:

    virtual void abortLoading    (void) ;
    virtual void setProxy        (QString host, int port);
    virtual void disableProxy    (void) ;

  protected slots:

  private slots:

    void relayStart              (void) ;
    void relayStop               (void) ;

  signals:

    void sendStart               (void) ;
    void sendStop                (void) ;
    void startLoading            (void) ;
    void stopLoading             (void) ;

};

class Q_GIS_EXPORT EmptyAdapter : public Adapter
{
  Q_OBJECT
  public:

             EmptyAdapter (int tileSize = 256  ,
                           int minZoom  = 0    ,
                           int maxZoom  = 17 ) ;
    virtual ~EmptyAdapter (void) ;

    virtual QPoint  coordinateToDisplay (const QPointF &) const ;
    virtual QPointF displayToCoordinate (const QPoint  &) const ;

    qreal PI;

  protected:

    qreal           rad_deg          (qreal x) const;
    qreal           deg_rad          (qreal x) const;

    virtual bool    isValid          (int x,int y,int z) const ;
    virtual void    zoom_in          (void) ;
    virtual void    zoom_out         (void) ;
    virtual QString query            (int x,int y,int z) const ;
    virtual int     tilesonzoomlevel (int zoomlevel) const ;
    virtual int     xoffset          (int x) const ;
    virtual int     yoffset          (int y) const ;

  private:

};

class Q_GIS_EXPORT TileAdapter : public Adapter
{
  Q_OBJECT
  public:

             TileAdapter (const QString & host           ,
                          const QString & serverPath     ,
                          int             tilesize       ,
                          int             minZoom = 0    ,
                          int             maxZoom = 17 ) ;
    virtual ~TileAdapter (void) ;

    virtual QPoint  coordinateToDisplay (const QPointF &) const ;
    virtual QPointF displayToCoordinate (const QPoint  &) const ;

    qreal PI;

  protected:

    qreal           rad_deg          (qreal x) const ;
    qreal           deg_rad          (qreal x) const ;

    virtual bool    isValid          (int x,int y,int z) const ;
    virtual void    zoom_in          (void) ;
    virtual void    zoom_out         (void) ;
    virtual QString query            (int x,int y,int z) const ;
    virtual int     tilesonzoomlevel (int zoomlevel) const ;
    virtual int     xoffset          (int x) const ;
    virtual int     yoffset          (int y) const ;

};

class Q_GIS_EXPORT WmsAdapter : public Adapter
{
  public:

             WmsAdapter (QString host             ,
                         QString serverPath       ,
                         int     tilesize = 256 ) ;
    virtual ~WmsAdapter (void) ;

    virtual QPoint  coordinateToDisplay (const QPointF &) const ;
    virtual QPointF displayToCoordinate (const QPoint  &) const ;

  protected:

    virtual void    zoom_in  (void) ;
    virtual void    zoom_out (void) ;
    virtual QString query    (int x,int y,int z) const ;
    virtual bool    isValid  (int x,int y,int z) const ;

  private:

    virtual QString getQ     (qreal ux,qreal uy,qreal ox,qreal oy) const ;

    qreal coord_per_x_tile ;
    qreal coord_per_y_tile ;
};

class Q_GIS_EXPORT GoogleAdapter : public TileAdapter
{
  Q_OBJECT
  public:

             GoogleAdapter(void) ;
    virtual ~GoogleAdapter(void) ;

  protected:

  private:

};

class Q_GIS_EXPORT GoogleSatelliteAdapter : public TileAdapter
{
  Q_OBJECT
  public:

             GoogleSatelliteAdapter (void) ;
    virtual ~GoogleSatelliteAdapter (void) ;

    virtual QPoint  coordinateToDisplay (const QPointF &) const ;
    virtual QPointF displayToCoordinate (const QPoint  &) const ;

    QString         getHost   (void) const ;

  protected:

    virtual void    zoom_in   (void) ;
    virtual void    zoom_out  (void) ;
    virtual QString query     (int x,int y,int z) const ;
    virtual bool    isValid   (int x,int y,int z) const ;

  private:

    virtual QString getQ      (qreal longitude, qreal latitude, int zoom) const ;

    qreal getMercatorLatitude (qreal YCoord) const ;
    qreal getMercatorYCoord   (qreal lati  ) const ;

    qreal coord_per_x_tile ;
    qreal coord_per_y_tile ;
    int   srvNum           ;

};

class Q_GIS_EXPORT OsmAdapter : public TileAdapter
{
  Q_OBJECT
  public:

             OsmAdapter (void) ;
    virtual ~OsmAdapter (void) ;

};

class Q_GIS_EXPORT YahooAdapter : public TileAdapter
{
  Q_OBJECT
  public:

             YahooAdapter        (void) ;
             YahooAdapter        (QString host,QString url) ;
    virtual ~YahooAdapter        (void) ;

    bool isValid                 (int x,int y,int z) const ;

  protected:

    virtual int tilesonzoomlevel (int zoomlevel) const ;
    virtual int yoffset          (int y) const ;

};

class Q_GIS_EXPORT OpenAerialAdapter : public TileAdapter
{
  Q_OBJECT
  public:

             OpenAerialAdapter (void) ;
    virtual ~OpenAerialAdapter (void) ;

};

class Q_GIS_EXPORT LayerManager : public QObject
{
  Q_OBJECT
  public:

                LayerManager      (MapControl * control,QSize size) ;
                LayerManager      (const LayerManager & old) ;
    virtual    ~LayerManager      (void) ;

    LayerManager & operator =     (const LayerManager & rhs) ;

    QPointF     currentCoordinate (void) const ;
    QPixmap     getImage          (void) const ;

    Layer *     layer             (void) const ;
    Layer *     layer             (const QString & ) const ;
    QStringList layers            (void) const ;
    void        addLayer          (Layer * layer) ;

    void        setView           (const QPointF & coordinate) ;
    void        setView           (const QList<QPointF> coordinates) ;
    void        setViewAndZoomIn  (const QList<QPointF> coordinates) ;

    void        zoomIn            (void) ;
    void        zoomOut           (void) ;
    void        setZoom           (int zoomlevel) ;
    int         currentZoom       (void) const ;

    QRectF      getViewport       (void) const ;
    void        scrollView        (const QPoint & offset) ;

    void        mouseEvent        (const QMouseEvent * evnt) ;

    QPoint      getMapmiddle_px   (void) const ;

    void        forceRedraw       (void) ;
    void        removeZoomImage   (void) ;

    void        drawGeoms         (QPainter * painter);
    void        drawImage         (QPainter * painter);

  protected:

    void        newOffscreenImage (bool clearImage = true,bool showZoomImage = true) ;
    inline bool checkOffscreen    (void) const ;
    inline bool containsAll       (QList<QPointF> coordinates) const ;
    inline void moveWidgets       (void) ;
    inline void setMiddle         (QList<QPointF> coordinates) ;

    MapControl *   mapcontrol              ;
    QPoint         screenmiddle            ; // middle of the screen
    QPoint         scroll                  ; // scrollvalue of the offscreen image
    QPoint         zoomImageScroll         ; // scrollvalue of the zoom image
    QSize          size                    ; // widget size
    QSize          offSize                 ; // size of the offscreen image

    QPixmap        composedOffscreenImage  ;
    QPixmap        composedOffscreenImage2 ;
    QPixmap        zoomImage               ;

    QList<Layer *> mylayers                ;

    QPoint         mapmiddle_px            ; // projection-display coordinates
    QPointF        mapmiddle               ; // world coordinate
    QPoint         whilenewscroll          ;

  private:

  public slots:

    void updateRequest            (QRectF rect) ;
    void updateRequest            (void) ;
    void resize                   (QSize newSize) ;
    void takeLayer                (QString layer) ;

  protected slots:

  private slots:

  signals:

};

class Q_GIS_EXPORT MapControl : public Widget
{
  Q_OBJECT
  public:

    enum MouseMode
    {
        Panning  , /*!< The map is moved */
        Dragging , /*!< A rectangular can be drawn */
        None     , /*!< Mouse move events have no efect to the map */
    }            ;

    explicit MapControl (StandardConstructor) ;
             MapControl (QSize size,MouseMode mousemode = Panning) ;
             MapControl (const MapControl & old) ;
    virtual ~MapControl (void) ;

    virtual QSize sizeHint        (void) const ;

    void        addSource         (QString name) ;
    void        addLayer          (Layer * layer) ;
    Layer *     layer             (const QString & layername) const ;
    QStringList layers            (void) const ;
    int         numberOfLayers    (void) const ;
    QPointF     currentCoordinate (void) const ;
    int         currentZoom       (void) const ;

    void setView                  (const QPointF      & coordinate  ) const ;
    void setView                  (const QList<QPointF> coordinates ) const ;
    void setView                  (const Point        * point       ) const ;
    void setViewAndZoomIn         (const QList<QPointF> coordinates ) const ;
    void followGeometry           (const Geometry     * geometry    ) const ;
    void stopFollowing            (Geometry           * geometry    ) ;
    void moveTo                   (QPointF              coordinate  ) ;
    void setMouseMode             (MouseMode            mousemode   ) ;

    MapControl::MouseMode mouseMode (void) ;

    void enablePersistentCache    (const QDir & path= QDir ( QDir::homePath() + QString("/QMapControl.cache") ) ) ;

    void setProxy                 (QString host,int port) ;
    void showScale                (bool show) ;
    virtual bool isBusy           (void) ;

  protected:

    virtual void Configure         (void) ;
    virtual bool FocusIn           (void) ;

    virtual void paintEvent        (QPaintEvent       * evnt) ;

    virtual void contextMenuEvent  (QContextMenuEvent * evnt) ;
    virtual void resizeEvent       (QResizeEvent      * evnt) ;

    virtual void focusInEvent      (QFocusEvent       * evnt) ;
    virtual void focusOutEvent     (QFocusEvent       * evnt) ;

    virtual void mousePressEvent   (QMouseEvent       * evnt) ;
    virtual void mouseReleaseEvent (QMouseEvent       * evnt) ;
    virtual void mouseMoveEvent    (QMouseEvent       * evnt) ;

  private:

    LayerManager * layermanager      ;
    QPoint         screen_middle     ; // middle of the widget (half size)
    QPoint         pre_click_px      ; // used for scrolling (MouseMode Panning)
    QPoint         current_mouse_pos ; // used for scrolling and dragging (MouseMode Panning/Dragging)
    QSize          size              ; // size of the widget
    bool           mousepressed      ;
    MouseMode      mymousemode       ;
    bool           scaleVisible      ;
    bool           m_loadingFlag     ;
    QMutex         moveMutex         ; // used for method moveTo()
    QPointF        target            ; // used for method moveTo()
    int            steps             ; // used for method moveTo()
    QDateTime      lastResize        ;
    bool           resizeAction      ;
    QString        proxyHost         ;
    int            proxyPort         ;

    QPointF clickToWorldCoordinate (QPoint click) ;
    MapControl & operator=    (const MapControl & rhs) ;

    void drawMeasurement      (QPainter & painter) ;
    QString TwoDigits         (int v) ;
    QString DegreeToString    (double v) ;
    QString PositionToString  (QPointF p) ;

  public slots:

    virtual bool Relocation   (void) ;
    virtual void Export       (void) ;
    void zoomIn               (void) ;
    void zoomOut              (void) ;
    void setZoom              (int zoomlevel) ;
    void scrollLeft           (int pixel = 10) ;
    void scrollRight          (int pixel = 10) ;
    void scrollUp             (int pixel = 10) ;
    void scrollDown           (int pixel = 10) ;
    void scroll               (const QPoint scroll) ;
    void updateRequest        (QRect rect) ;
    void updateRequestNew     (void) ;
    void resize               (const QSize newSize) ;

  protected slots:

    virtual bool Menu         (QPoint pos) ;

    void tick                 (void) ;
    void loadingFinished      (void) ;
    void positionChanged      (Geometry * geom) ;

    void LoadingStart         (void) ;
    void LoadingStop          (void) ;

  private slots:

  signals:

    void mouseEventCoordinate (const QMouseEvent * evnt , const QPointF coordinate) ;
    void boxDragged           (const QRectF) ;
    void geometryClicked      (Geometry      * geometry   , QPoint coord_px) ;
    void viewChanged          (const QPointF & coordinate , int    zoom    ) ;

};

}

}

QT_END_NAMESPACE

#endif
