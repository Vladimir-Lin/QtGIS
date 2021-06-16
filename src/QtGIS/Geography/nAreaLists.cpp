#include <qtgis.h>

N::AreaLists:: AreaLists ( QWidget * parent , Plan * p )
             : TreeDock  (           parent ,        p )
{
  Configure ( ) ;
}

N::AreaLists::~AreaLists(void)
{
}

void N::AreaLists::Configure(void)
{
  NewTreeWidgetItem            ( head                  )         ;
  head -> setText              ( 0 , tr("Name"   )     )         ;
  head -> setText              ( 1 , tr("English")     )         ;
  head -> setText              ( 2 , ""                )         ;
  for (int i=0;i<2;i++)                                          {
    head -> setTextAlignment   ( i , Qt::AlignCenter   )         ;
  }                                                              ;
  setWindowTitle               ( tr("Area lists" )     )         ;
  setDragDropMode              ( NoDragDrop            )         ;
  setRootIsDecorated           ( false                 )         ;
  setAlternatingRowColors      ( true                  )         ;
  setSelectionMode             ( SingleSelection       )         ;
  setColumnCount               ( 3                     )         ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded )         ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded )         ;
  assignHeaderItems            ( head                  )         ;
  plan -> setFont              ( this                  )         ;
  ////////////////////////////////////////////////////////////////
  connect(this,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int))   ,
          this,SLOT  (doubleClicked    (QTreeWidgetItem*,int)) ) ;
}

bool N::AreaLists::FocusIn(void)
{
  connectAction(Menus::Refresh,this,SLOT(startup        ())) ;
  connectAction(Menus::Insert ,this,SLOT(Insert         ())) ;
  connectAction(Menus::Delete ,this,SLOT(Delete         ())) ;
  connectAction(Menus::Copy   ,this,SLOT(CopyToClipboard())) ;
  return true                                                          ;
}

bool N::AreaLists::startup(void)
{
  clear            (                         ) ;
  EnterSQL         ( SC , plan->sql          ) ;
    QString Q                                  ;
    UUIDs   Uuids                              ;
    SUID    uuid                               ;
    Uuids = SC.Uuids                           (
              PlanTable(Area)                  ,
              "uuid","order by id desc"      ) ;
    foreach (uuid,Uuids)                       {
      QString name = SC.getName                (
                       PlanTable(Names)        ,
                       "uuid"                  ,
                       plan->LanguageId,uuid ) ;
      QString english                          ;
      Q = SC.sql.SelectFrom                    (
            "english"                          ,
            PlanTable(Area)                    ,
            SC.WhereUuid(uuid)               ) ;
      if (SC.Fetch(Q)) english = SC.String(0)  ;
      NewTreeWidgetItem(item)                  ;
      item->setData(0,Qt::UserRole,uuid)       ;
      item->setText(0,name             )       ;
      item->setText(1,english          )       ;
      addTopLevelItem(item)                    ;
    }                                          ;
  LeaveSQL         ( SC , plan->sql          ) ;
  SuitableColumns  (                         ) ;
  Alert            ( Done                    ) ;
  return true                                  ;
}

void N::AreaLists::Insert(void)
{
  NewTreeWidgetItem(item)         ;
  item->setData(0,Qt::UserRole,0) ;
  insertTopLevelItem(0,item)      ;
  doubleClicked(item,0)           ;
}

void N::AreaLists::Delete(void)
{
}

void N::AreaLists::doubleClicked(QTreeWidgetItem * item,int column)
{
  QLineEdit * line                         ;
  switch (column)                          {
    case 0                                 :
      line = setLineEdit                   (
               item,column                 ,
               SIGNAL(editingFinished())   ,
               SLOT  (nameFinished   ()) ) ;
      line->setFocus( Qt::TabFocusReason ) ;
    break                                  ;
    case 1                                 :
      line = setLineEdit                   (
               item,column                 ,
               SIGNAL(editingFinished())   ,
               SLOT  (englishFinished()) ) ;
      line->setFocus( Qt::TabFocusReason ) ;
    break                                  ;
  }                                        ;
}

void N::AreaLists::nameFinished(void)
{
  if (IsNull(ItemEditing)) return                     ;
  QLineEdit * line = Casting(QLineEdit,ItemWidget)    ;
  if (IsNull(line)) return                            ;
  SUID    uuid = nTreeUuid(ItemEditing,0)             ;
  QString name = line->text()                         ;
  EnterSQL         ( SC , plan->sql          )        ;
    QString Q                                         ;
    if (uuid<=0)                                      {
      if (name.length()>0)                            {
        uuid = SC.Unique                              (
                 PlanTable(MajorUuid)                 ,
                 "uuid",9712                        ) ;
        SC.assureUuid(PlanTable(MajorUuid)            ,
          uuid,Types::Area                ) ;
        SC.insertUuid(PlanTable(Area),uuid,"uuid")    ;
        Q = SC.NameSyntax(PlanTable(Names))           ;
        SC.insertName(Q,uuid,plan->LanguageId,name)   ;
        ItemEditing -> setData ( 0,Qt::UserRole,uuid) ;
        ItemEditing -> setText ( 0 , name )           ;
        Alert ( Done  )                               ;
      } else                                          {
        Alert ( Error )                               ;
      }                                               ;
    } else                                            {
      Q = SC.NameUpdate(PlanTable(Names),"name")      ;
      SC.insertName(Q,uuid,plan->LanguageId,name)     ;
      ItemEditing -> setText ( 0 , name )             ;
      Alert ( Done )                                  ;
    }                                                 ;
  LeaveSQL         ( SC , plan->sql          )        ;
  removeOldItem    ( true , 0                )        ;
}

void N::AreaLists::englishFinished(void)
{
  if (IsNull(ItemEditing)) return                  ;
  QLineEdit * line = Casting(QLineEdit,ItemWidget) ;
  if (IsNull(line)) return                         ;
  SUID    uuid    = nTreeUuid(ItemEditing,0)       ;
  QString english = line->text()                   ;
  EnterSQL         ( SC , plan->sql              ) ;
    QString Q                                      ;
    if (uuid>0)                                    {
      Q = SC.sql.Update                            (
            PlanTable(Area)                        ,
            "where uuid = :UUID"                   ,
            1,"english"                          ) ;
      SC . Prepare ( Q                           ) ;
      SC . Bind    ( "uuid"   ,uuid              ) ;
      SC . Bind    ( "english",english.toUtf8()  ) ;
      SC . Exec    (                             ) ;
      ItemEditing -> setText ( 1 , english       ) ;
    }                                              ;
  LeaveSQL         ( SC , plan->sql              ) ;
  removeOldItem    ( true , 0                    ) ;
  Alert            ( Done                        ) ;
}

bool N::AreaLists::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                                     ;
  QMdiSubWindow * mdi    = Casting ( QMdiSubWindow , parent() ) ;
  QDockWidget   * dock   = Casting ( QDockWidget   , parent() ) ;
  QAction    * aa                                               ;
  if (NotNull(dock)) mm.add(1000001,tr("Move to window area"))  ;
  if (NotNull(mdi )) mm.add(1000002,tr("Move to dock area"  ))  ;
  mm.setFont ( plan )                                           ;
  aa = mm.exec()                                                ;
  if (IsNull(aa)) return true                                   ;
  switch (mm[aa])                                               {
    case 1000001                                                :
      emit attachMdi ( this , 0 )                               ;
    break                                                       ;
    case 1000002                                                :
      emit attachDock                                           (
        this , windowTitle()                                    ,
        Qt::RightDockWidgetArea                                 ,
        Qt::LeftDockWidgetArea  | Qt::RightDockWidgetArea       |
        Qt::TopDockWidgetArea   | Qt::BottomDockWidgetArea    ) ;
    break                                                       ;
    default                                                     :
    break                                                       ;
  }                                                             ;
  return true                                                   ;
}
