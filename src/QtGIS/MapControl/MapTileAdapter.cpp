#include <qtgis.h>

N::Map::TileAdapter:: TileAdapter ( const QString & host       ,
                                    const QString & serverPath ,
                                    int             tilesize   ,
                                    int             minZoom    ,
                                    int             maxZoom    )
                    : Adapter     (                 host       ,
                                                    serverPath ,
                                                    tilesize   ,
                                                    minZoom    ,
                                                    maxZoom    )
{
    PI = acos(-1.0);
    /*
        Initialize the "substring replace engine". First the string replacement
        in getQuery was made by QString().arg() but this was very slow. So this
        splits the servers path into substrings and when calling getQuery the
        substrings get merged with the parameters of the URL.
        Pretty complicated, but fast.
    */
    param1 = serverPath.indexOf("%1");
    param2 = serverPath.indexOf("%2");
    param3 = serverPath.indexOf("%3");

    int min = param1 < param2 ? param1 : param2;
    min = param3 < min ? param3 : min;

    int max = param1 > param2 ? param1 : param2;
    max = param3 > max ? param3 : max;

    int middle = param1+param2+param3-min-max;

    order[0][0] = min;
    if (min == param1)
        order[0][1] = 0;
    else if (min == param2)
        order[0][1] = 1;
    else
        order[0][1] = 2;

    order[1][0] = middle;
    if (middle == param1)
        order[1][1] = 0;
    else if (middle == param2)
        order[1][1] = 1;
    else
        order[1][1] = 2;

    order[2][0] = max;
    if (max == param1)
        order[2][1] = 0;
    else if(max == param2)
        order[2][1] = 1;
    else
        order[2][1] = 2;

    int zoom = max_zoom < min_zoom ? min_zoom - current_zoom : current_zoom;
    numberOfTiles = tilesonzoomlevel(zoom);
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
}

N::Map::TileAdapter::~TileAdapter(void)
{
}

//TODO: pull out
void N::Map::TileAdapter::zoom_in(void)
{
    if (min_zoom > max_zoom)
    {
        //current_zoom = current_zoom-1;
        current_zoom = current_zoom > max_zoom ? current_zoom-1 : max_zoom;
    }
    else if (min_zoom < max_zoom)
    {
        //current_zoom = current_zoom+1;
        current_zoom = current_zoom < max_zoom ? current_zoom+1 : max_zoom;
    }

    int zoom = max_zoom < min_zoom ? min_zoom - current_zoom : current_zoom;
    numberOfTiles = tilesonzoomlevel(zoom);

}

void N::Map::TileAdapter::zoom_out(void)
{
    if (min_zoom > max_zoom)
    {
        //current_zoom = current_zoom+1;
        current_zoom = current_zoom < min_zoom ? current_zoom+1 : min_zoom;
    }
    else if (min_zoom < max_zoom)
    {
        //current_zoom = current_zoom-1;
        current_zoom = current_zoom > min_zoom ? current_zoom-1 : min_zoom;
    }

    int zoom = max_zoom < min_zoom ? min_zoom - current_zoom : current_zoom;
    numberOfTiles = tilesonzoomlevel(zoom);
}

qreal N::Map::TileAdapter::deg_rad(qreal x) const
{
    return x * (PI/180.0);
}

qreal N::Map::TileAdapter::rad_deg(qreal x) const
{
    return x * (180/PI);
}

QString N::Map::TileAdapter::query(int x, int y, int z) const
{
    x = xoffset(x);
    y = yoffset(y);

    int a[3] = {z, x, y};
    return QString(serverPath).replace(order[2][0],2, loc.toString(a[order[2][1]]))
            .replace(order[1][0],2, loc.toString(a[order[1][1]]))
            .replace(order[0][0],2, loc.toString(a[order[0][1]]));

}

QPoint N::Map::TileAdapter::coordinateToDisplay(const QPointF& coordinate) const
{
    qreal x = (coordinate.x()+180) * (numberOfTiles*mytilesize)/360.; // coord to pixel!
    qreal y = (1-(log(tan(PI/4+deg_rad(coordinate.y())/2)) /PI)) /2  * (numberOfTiles*mytilesize);

    return QPoint(int(x), int(y));
}

QPointF N::Map::TileAdapter::displayToCoordinate(const QPoint& point) const
{
    qreal longitude = (point.x()*(360/(numberOfTiles*mytilesize)))-180;
    qreal latitude = rad_deg(atan(sinh((1-point.y()*(2/(numberOfTiles*mytilesize)))*PI)));

    return QPointF(longitude, latitude);

}

bool N::Map::TileAdapter::isValid(int x, int y, int z) const
{
    if (max_zoom < min_zoom)
    {
        z= min_zoom - z;
    }
    int Z = 1 ;
    if (z>0) Z <<= z ;
    Z -= 1 ;
    if ( ( x < 0 ) || ( x > Z )  ||
         ( y < 0 ) || ( y > Z ) ) {
        return false;
    }
    return true;

}

int N::Map::TileAdapter::tilesonzoomlevel(int z) const
{
  int Z = 1        ;
  if (z>0) Z <<= z ;
  return Z         ;
}

int N::Map::TileAdapter::xoffset(int x) const
{
  return x ;
}

int N::Map::TileAdapter::yoffset(int y) const
{
  return y ;
}
