#include <qtgis.h>

N::Map::WmsAdapter:: WmsAdapter ( QString host       ,
                                  QString serverPath ,
                                  int     tilesize   )
                   : Adapter    (         host       ,
                                          serverPath ,
                                          tilesize   ,
                                          0          ,
                                          17         )
{
    // param1 = serverPath.indexOf("%1");
    // param2 = serverPath.indexOf("%2");
    // param3 = serverPath.indexOf("%3");
    // param4 = serverPath.indexOf("%4");
    // param5 = serverPath.indexOf("%5");
    // param6 = serverPath.lastIndexOf("%5");

    // this->serverPath = serverPath.replace(param6, 2, QString().setNum(tilesize)).replace(param5, 2, QString().setNum(tilesize));

    // sub1 = serverPath.mid(0, param1);
    // sub2 = serverPath.mid(param1+2, param2-param1-2);
    // sub3 = serverPath.mid(param2+2, param3-param2-2);
    // sub4 = serverPath.mid(param3+2, param4-param3-2);
    // sub5 = serverPath.mid(param4+2);

    this->serverPath.append("&WIDTH=").append(loc.toString(tilesize))
            .append("&HEIGHT=").append(loc.toString(tilesize))
            .append("&BBOX=");
    numberOfTiles    = 1    << current_zoom  ;
    coord_per_x_tile = 360.0 / numberOfTiles ;
    coord_per_y_tile = 180.0 / numberOfTiles ;
}


N::Map::WmsAdapter::~WmsAdapter(void)
{
}

QPoint N::Map::WmsAdapter::coordinateToDisplay(const QPointF & coordinate) const
{
    qreal x = (coordinate.x()+180) * (numberOfTiles*mytilesize)/360.; // coord to pixel!
    qreal y = -1*(coordinate.y()-90) * (numberOfTiles*mytilesize)/180.; // coord to pixel!
    return QPoint(int(x), int(y));
}

QPointF N::Map::WmsAdapter::displayToCoordinate(const QPoint & point) const
{
    qreal lon = (point.x()*(360./(numberOfTiles*mytilesize)))-180;
    qreal lat = -(point.y()*(180./(numberOfTiles*mytilesize)))+90;
    return QPointF(lon, lat);
}

void N::Map::WmsAdapter::zoom_in(void)
{
  current_zoom    += 1                     ;
  numberOfTiles    = ( 1 << current_zoom ) ;
  coord_per_x_tile = 360.0 / numberOfTiles ;
  coord_per_y_tile = 180.0 / numberOfTiles ;
}

void N::Map::WmsAdapter::zoom_out(void)
{
    current_zoom    -= 1                     ;
    numberOfTiles    = ( 1 << current_zoom ) ;
    coord_per_x_tile = 360.0 / numberOfTiles ;
    coord_per_y_tile = 180.0 / numberOfTiles ;
}

bool N::Map::WmsAdapter::isValid(int /*x*/, int /*y*/, int /*z*/) const
{
    // if (x>0 && y>0 && z>0)
    {
        return true;
    }
    // return false;
}

QString N::Map::WmsAdapter::query(int i, int j, int /*z*/) const
{
    return getQ(-180+i*coord_per_x_tile,
                90-(j+1)*coord_per_y_tile,
                -180+i*coord_per_x_tile+coord_per_x_tile,
                90-(j+1)*coord_per_y_tile+coord_per_y_tile);
}

QString N::Map::WmsAdapter::getQ(qreal ux, qreal uy, qreal ox, qreal oy) const
{
    return QString().append(serverPath)
            .append(loc.toString(ux)).append(",")
            .append(loc.toString(uy)).append(",")
            .append(loc.toString(ox)).append(",")
            .append(loc.toString(oy));
}

