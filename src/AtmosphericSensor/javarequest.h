#ifndef JAVAREQUEST_H
#define JAVAREQUEST_H

#include "declaration.h"

void turnGpsOn()
{
    //push window settings to allow location access
    const QAndroidJniObject action_location = QAndroidJniObject::getStaticObjectField("android/provider/Settings","ACTION_LOCATION_SOURCE_SETTINGS","Ljava/lang/String;");
    const QAndroidIntent intent(action_location.toString());
    QtAndroid::startActivity(intent.handle(),0);
}

bool getAccess()
{
    //check location access
    const QAndroidJniObject locString = QAndroidJniObject::getStaticObjectField("android/content/Context","LOCATION_SERVICE","Ljava/lang/String;");
    const QAndroidJniObject locService = QtAndroid::androidContext().callObjectMethod("getSystemService","(Ljava/lang/String;)Ljava/lang/Object;",locString.object<jstring>());
    QAndroidJniObject provider = QAndroidJniObject::getStaticObjectField("android/location/LocationManager","GPS_PROVIDER","Ljava/lang/String;");
    bool access = bool(locService.callMethod<jboolean>("isProviderEnabled","(Ljava/lang/String;)Z",provider.object<jstring>()));
    if(!access)
        return false;
    return true;
}

void checkPermissions()
{
    //check permissions
    const QVector<QString> permissions({"android.permission.BLUETOOTH","android.permission.ACCESS_FINE_LOCATION"});

    for(const QString &permission : permissions)
    {
        auto result = QtAndroid::checkPermission(permission);
        if(result == QtAndroid::PermissionResult::Denied)
        {
            auto resultHash = QtAndroid::requestPermissionsSync(QStringList({permission}));
            if(resultHash[permission] == QtAndroid::PermissionResult::Denied)
                QMessageBox::information(nullptr,"Permission",permission + " denied");
        }
    }
}

#endif // JAVAREQUEST_H
