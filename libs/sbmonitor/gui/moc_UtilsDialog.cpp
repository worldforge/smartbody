/****************************************************************************
** Meta object code from reading C++ file 'UtilsDialog.h'
**
** Created: Tue May 29 15:01:03 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "UtilsDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'UtilsDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_UtilsDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x08,
      29,   12,   12,   12, 0x08,
      47,   12,   12,   12, 0x08,
      63,   12,   12,   12, 0x08,
      81,   12,   12,   12, 0x08,
     101,   12,   12,   12, 0x08,
     122,   12,   12,   12, 0x08,
     142,   12,   12,   12, 0x08,
     156,   12,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_UtilsDialog[] = {
    "UtilsDialog\0\0GazeAtPressed()\0"
    "StopGazePressed()\0RunBmlPressed()\0"
    "PlayAnimPressed()\0SetPosturePressed()\0"
    "SpeakButtonPressed()\0QueryAnimsPressed()\0"
    "FilterAnims()\0Refresh()\0"
};

void UtilsDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        UtilsDialog *_t = static_cast<UtilsDialog *>(_o);
        switch (_id) {
        case 0: _t->GazeAtPressed(); break;
        case 1: _t->StopGazePressed(); break;
        case 2: _t->RunBmlPressed(); break;
        case 3: _t->PlayAnimPressed(); break;
        case 4: _t->SetPosturePressed(); break;
        case 5: _t->SpeakButtonPressed(); break;
        case 6: _t->QueryAnimsPressed(); break;
        case 7: _t->FilterAnims(); break;
        case 8: _t->Refresh(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData UtilsDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject UtilsDialog::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_UtilsDialog,
      qt_meta_data_UtilsDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &UtilsDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *UtilsDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *UtilsDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UtilsDialog))
        return static_cast<void*>(const_cast< UtilsDialog*>(this));
    return QObject::qt_metacast(_clname);
}

int UtilsDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
