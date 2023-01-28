/****************************************************************************
** Meta object code from reading C++ file 'glwidget.h'
**
** Created: Tue May 29 15:01:03 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "glwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'glwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GLWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      35,   10,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
      84,   73,    9,    9, 0x0a,
     133,    9,    9,    9, 0x0a,
     167,  150,    9,    9, 0x0a,
     242,  230,    9,    9, 0x0a,
     299,  291,    9,    9, 0x0a,
     320,  291,    9,    9, 0x0a,
     345,  291,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GLWidget[] = {
    "GLWidget\0\0jointOwner,jointSelected\0"
    "JointPicked(const Pawn*,const Joint*)\0"
    "dlg,result\0OnCloseSettingsDialog(const SettingsDialog*,int)\0"
    "ToggleFreeLook()\0current,previous\0"
    "sceneTreeCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)\0"
    "item,column\0"
    "sceneTreeItemDoubleClicked(QTreeWidgetItem*,int)\0"
    "enabled\0ToggleShowAxes(bool)\0"
    "ToggleShowEyeBeams(bool)\0"
    "ToggleAllowBoneUpdates(bool)\0"
};

void GLWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        GLWidget *_t = static_cast<GLWidget *>(_o);
        switch (_id) {
        case 0: _t->JointPicked((*reinterpret_cast< const Pawn*(*)>(_a[1])),(*reinterpret_cast< const Joint*(*)>(_a[2]))); break;
        case 1: _t->OnCloseSettingsDialog((*reinterpret_cast< const SettingsDialog*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->ToggleFreeLook(); break;
        case 3: _t->sceneTreeCurrentItemChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QTreeWidgetItem*(*)>(_a[2]))); break;
        case 4: _t->sceneTreeItemDoubleClicked((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: _t->ToggleShowAxes((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->ToggleShowEyeBeams((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->ToggleAllowBoneUpdates((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData GLWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GLWidget::staticMetaObject = {
    { &QGLWidget::staticMetaObject, qt_meta_stringdata_GLWidget,
      qt_meta_data_GLWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GLWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GLWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GLWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GLWidget))
        return static_cast<void*>(const_cast< GLWidget*>(this));
    return QGLWidget::qt_metacast(_clname);
}

int GLWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void GLWidget::JointPicked(const Pawn * _t1, const Joint * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
