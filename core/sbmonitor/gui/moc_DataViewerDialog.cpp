/****************************************************************************
** Meta object code from reading C++ file 'DataViewerDialog.h'
**
** Created: Tue May 29 15:01:03 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "DataViewerDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DataViewerDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DataViewerDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x08,
      40,   17,   17,   17, 0x08,
      65,   17,   17,   17, 0x08,
      96,   17,   17,   17, 0x08,
     130,   17,   17,   17, 0x08,
     162,   17,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DataViewerDialog[] = {
    "DataViewerDialog\0\0AddSelectedChannels()\0"
    "RemoveSelectedChannels()\0"
    "ChangedSelectedMotion(QString)\0"
    "ChangedSelectedCharacter(QString)\0"
    "ChangedRotationDisplay(QString)\0"
    "Refresh()\0"
};

void DataViewerDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        DataViewerDialog *_t = static_cast<DataViewerDialog *>(_o);
        switch (_id) {
        case 0: _t->AddSelectedChannels(); break;
        case 1: _t->RemoveSelectedChannels(); break;
        case 2: _t->ChangedSelectedMotion((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->ChangedSelectedCharacter((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->ChangedRotationDisplay((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->Refresh(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DataViewerDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DataViewerDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_DataViewerDialog,
      qt_meta_data_DataViewerDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DataViewerDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DataViewerDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DataViewerDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DataViewerDialog))
        return static_cast<void*>(const_cast< DataViewerDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int DataViewerDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
