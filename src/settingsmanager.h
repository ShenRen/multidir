#pragma once

#include <QSettings>

class SettingsManager
{
public:
  enum Type
  {
    OpenConsoleCommand, RunInConsoleCommand, EditorCommand,
    CheckUpdates, StartInBackground, ImageCacheSize,
    GroupIds, TabIds, TabSwitchOrder, Translation,
    ShowFreeSpace, ShowFilesInfo, ShowSelectionInfo,
    Style,
    TypeCount
  };

  static void triggerUpdate ();
  static void subscribeForUpdates (QObject *object, const QString &method =
                                     QLatin1String("updateSettings"));

  static void setPortable (bool isPortable);

  SettingsManager ();

  QVariant get (Type type) const;
  void set (Type type, const QVariant &value);

private:
  QSettings settings_;
};
