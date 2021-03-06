#include <qtgis.h>

N::Map::GoogleSatelliteAdapter:: GoogleSatelliteAdapter(void)
                               : TileAdapter                (
                                   "kh2.google.com"         ,
                                   "/kh?n=404&v=8&t=trtqtt" ,
                                   256                      ,
                                   0                        ,
                                   19                       )
{
  // name = "googlesat";
  numberOfTiles    = 1    << current_zoom  ;
  coord_per_x_tile = 360.0 / numberOfTiles ;
  coord_per_y_tile = 180.0 / numberOfTiles ;
}

N::Map::GoogleSatelliteAdapter::~GoogleSatelliteAdapter(void)
{
}

QString N::Map::GoogleSatelliteAdapter::getHost(void) const
{
  int random = qrand() % 4                      ;
  return QString("kh%1.google.com").arg(random) ;
}

QPoint N::Map::GoogleSatelliteAdapter::coordinateToDisplay(const QPointF& coordinate) const
{
    //double x = ((coordinate.x()+180)*(tilesize*numberOfTiles/360));
    //double y = (((coordinate.y()*-1)+90)*(tilesize*numberOfTiles/180));

    qreal x = (coordinate.x()+180.) * (numberOfTiles*mytilesize)/360.;		// coord to pixel!
    //double y = -1*(coordinate.y()-90) * (numberOfTiles*tilesize)/180.;	// coord to pixel!
    qreal y = (getMercatorYCoord(coordinate.y())-M_PI) * -1 * (numberOfTiles*mytilesize)/(2*M_PI);	// coord to pixel!
    return QPoint(int(x), int(y));
}

QPointF N::Map::GoogleSatelliteAdapter::displayToCoordinate(const QPoint& point) const
{
  qreal lon = (   ( point.x() / ( mytilesize * numberOfTiles ) ) * 360.0 ) - 180.0        ;
  qreal lat = ( ( ( point.y() / ( mytilesize * numberOfTiles ) ) * 180.0 ) -  90.0 ) * -1 ;
  //qreal lon = (point.x()*(360./(numberOfTiles*mytilesize)))-180.0;
  //double lat = -(point.y()*(180./(numberOfTiles*tilesize)))+90;
  //qreal lat = getMercatorLatitude(point.y()*-1*(2*M_PI/(numberOfTiles*tilesize)) + M_PI);
  //qreal lat = lat *180./M_PI;
  return QPointF(lon, lat);
}

qreal N::Map::GoogleSatelliteAdapter::getMercatorLatitude(qreal YCoord) const
{
    //http://welcome.warnercnr.colostate.edu/class_info/nr502/lg4/projection_mathematics/converting.html
    if (YCoord > M_PI) return 9999.;
    if (YCoord < -M_PI) return -9999.;

    qreal t = atan(exp(YCoord));
    qreal res = (2.*(t))-(M_PI/2.);
    return res;
}

qreal N::Map::GoogleSatelliteAdapter::getMercatorYCoord(qreal lati) const
{
    qreal lat = lati;

    // conversion degre=>radians
    qreal phi = M_PI * lat / 180;

    qreal res;
    //double temp = Math.Tan(Math.PI / 4 - phi / 2);
    //res = Math.Log(temp);
    res = 0.5 * log((1 + sin(phi)) / (1 - sin(phi)));

    return res;
}

void N::Map::GoogleSatelliteAdapter::zoom_in(void)
{
  current_zoom    += 1                     ;
  numberOfTiles    = 1    << current_zoom  ;
  coord_per_x_tile = 360.0 / numberOfTiles ;
  coord_per_y_tile = 180.0 / numberOfTiles ;
}

void N::Map::GoogleSatelliteAdapter::zoom_out(void)
{
  current_zoom    -= 1                     ;
  numberOfTiles    = 1    << current_zoom  ;
  coord_per_x_tile = 360.0 / numberOfTiles ;
  coord_per_y_tile = 180.0 / numberOfTiles ;
}

bool N::Map::GoogleSatelliteAdapter::isValid(int x, int y, int z) const
{
    if ((x>=0 && x < numberOfTiles) && (y>=0 && y < numberOfTiles) && z>=0)
    {
        return true;
    }
    return false;
}

QString N::Map::GoogleSatelliteAdapter::query(int i, int j, int z) const
{
  return getQ ( -180 +  i    * coord_per_x_tile ,
                  90 - (j+1) * coord_per_y_tile ,
                z                             ) ;
}

QString N::Map::GoogleSatelliteAdapter::getQ(qreal longitude, qreal latitude, int zoom) const
{
    qreal xmin=-180;
    qreal xmax=180;
    qreal ymin=-90;
    qreal ymax=90;

    qreal xmoy=0;
    qreal ymoy=0;
    QString location="t";

    //Google uses a latitude divided by 2;
    qreal halflat = latitude;

    for (int i = 0; i < zoom; i++)
    {
        xmoy = (xmax + xmin) / 2;
        ymoy = (ymax + ymin) / 2;
        if (halflat >= ymoy) //upper part (q or r)
        {
            ymin = ymoy;
            if (longitude < xmoy)
            { /*q*/
                location+= "q";
                xmax = xmoy;
            }
            else
            {/*r*/
                location+= "r";
                xmin = xmoy;
            }
        }
        else //lower part (t or s)
        {
            ymax = ymoy;
            if (longitude < xmoy)
            { /*t*/

                location+= "t";
                xmax = xmoy;
            }
            else
            {/*s*/
                location+= "s";
                xmin = xmoy;
            }
        }
    }
    return QString("/kh?n=404&v=24&t=%1").arg(location);
}
