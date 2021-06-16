#include <qtgis.h>

#define TABLES(ID) plan->Tables[Tables::ID]

N::ContinentLists:: ContinentLists (QWidget * parent,Plan * p)
                  : TreeDock       (          parent,       p)
{
  Configure ( ) ;
}

N::ContinentLists::~ContinentLists(void)
{
}

void N::ContinentLists::Configure(void)
{
  NewTreeWidgetItem            ( head                  ) ;
  head -> setText              ( 0 , tr("Continent")   ) ;
  setWindowTitle               ( tr("Continent lists") ) ;
  setDragDropMode              ( NoDragDrop            ) ;
  setRootIsDecorated           ( false                 ) ;
  setAlternatingRowColors      ( true                  ) ;
  setSelectionMode             ( SingleSelection       ) ;
  setColumnCount               ( 1                     ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded ) ;
  assignHeaderItems            ( head                  ) ;
  plan -> setFont              ( this                  ) ;
  PassDragDrop = false                                   ;
}

bool N::ContinentLists::FocusIn(void)
{
  connectAction(Menus::Refresh,this,SLOT(List())) ;
  connectAction(Menus::Copy   ,this,SLOT(Copy())) ;
  return true                                               ;
}

bool N::ContinentLists::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                                        ;
  QMdiSubWindow * mdi    = qobject_cast<QMdiSubWindow *>(parent()) ;
  QDockWidget   * dock   = qobject_cast<QDockWidget   *>(parent()) ;
  QPoint          global = mapToGlobal(pos)                        ;
  QTreeWidgetItem * IT = currentItem()                             ;
  QAction       * a                                                ;
  mm.add(102,tr("Refresh"))                                        ;
  if (NotNull(dock)) mm.add(1000001,tr("Move to window area"))     ;
  if (NotNull(mdi )) mm.add(1000002,tr("Move to dock area"  ))     ;
  mm . setFont( plan )                                             ;
  a = mm.exec(global)                                              ;
  if (IsNull(a)) return true                                       ;
  switch (mm[a])                                                   {
    case 102                                                       :
      List ( )                                                     ;
    break                                                          ;
    case 1000001                                                   :
      emit attachMdi (this,Qt::Vertical)                           ;
    break                                                          ;
    case 1000002                                                   :
      emit attachDock                                              (
        this                                                       ,
        windowTitle()                                              ,
        Qt::RightDockWidgetArea                                    ,
        Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea           ) ;
    break                                                          ;
    default                                                        :
    break                                                          ;
  }                                                                ;
  return true                                                      ;
}

void N::ContinentLists::List(void)
{
  EnterSQL ( SC , plan->sql )                            ;
    QString Q                                            ;
    QString name                                         ;
    SUID    uuid                                         ;
    UUIDs   Uuids = SC.Uuids                             (
      TABLES(Continents) , "uuid" , "order by id asc"  ) ;
    clear ( )                                            ;
    foreach (uuid,Uuids)                                 {
      NewTreeWidgetItem ( IT )                           ;
      name = SC.getName ( TABLES(Names)                  ,
        "uuid",plan->LanguageId,uuid               )     ;
      IT -> setData   ( 0,Qt::UserRole,uuid        )     ;
      IT -> setText   ( 0,name                     )     ;
      addTopLevelItem ( IT                         )     ;
    }                                                    ;
  LeaveSQL ( SC , plan->sql )                            ;
  Alert    ( Done           )                            ;
}

void N::ContinentLists::Copy(void)
{
  QTreeWidgetItem * item = currentItem()  ;
  if (IsNull(item)) return                ;
  QMimeData * mime = new QMimeData()      ;
  mime->setText  ( item->text(0)        ) ;
  qApp->clipboard()->setMimeData ( mime ) ;
}
