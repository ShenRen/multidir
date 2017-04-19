#include "copypaste.h"

#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QFileInfo>
#include <QDir>
#include <QDrag>

#include <QDebug>

namespace
{
const QString kdeCut = QLatin1String ("application/x-kde-cutselection");
const QByteArray kdeCutValue = "1";
const QString gnomeCut = QLatin1String ("x-special/gnome-copied-files");
const QByteArray gnomeCutValue = "cut\n";
const QByteArray gnomeCopyValue = "copy\n";
const QString winCut = QLatin1String ("Preferred DropEffect");
const QByteArray winCutValue = QByteArray::fromHex ("02000000");
}

CopyPaste::CopyPaste ()
{

}

void CopyPaste::copy (const QList<QFileInfo> &sources) const
{
  auto mime = new QMimeData;
  const auto urls = this->urls (sources);
  mime->setUrls (urls);
#ifdef Q_OS_LINUX
  mime->setData (gnomeCut, gnomeCopyValue + names (urls).join ("\n").toUtf8 ());
#endif
  QApplication::clipboard ()->setMimeData (mime);
}

void CopyPaste::cut (const QList<QFileInfo> &sources) const
{
  auto mime = new QMimeData;
  const auto urls = this->urls (sources);
  mime->setUrls (urls);
#ifdef Q_OS_LINUX
  mime->setData (kdeCut, kdeCutValue);
  mime->setData (gnomeCut, gnomeCutValue + names (urls).join ("\n").toUtf8 ());
#elif Q_OS_WIN
  mime->setData (winCut, winCutValue);
#endif
  QApplication::clipboard ()->setMimeData (mime);
}

void CopyPaste::paste (const QFileInfo &target) const
{
  if (auto mime = QApplication::clipboard ()->mimeData (QClipboard::Clipboard))
  {
    auto isCut = this->isCut (*mime);
    paste (mime->urls (), target, isCut ? Qt::MoveAction : Qt::CopyAction);
  }
}

bool CopyPaste::paste (const QList<QUrl> &urls, const QFileInfo &target, Qt::DropAction action) const
{
  auto infos = this->infos (urls);
  if (infos.isEmpty ())
  {
    return true;
  }

  auto targetPath = (target.isDir () ? target.absoluteFilePath () : target.absolutePath ())
                    + QDir::separator ();
  bool ok = true;
  switch (action)
  {
    case Qt::CopyAction:
      for (const auto &i: infos)
      {
        ok &= QFile::copy (i.absoluteFilePath (), uniqueFilePath (targetPath, i.fileName ()));
      }
      break;

    case Qt::LinkAction:
      for (const auto &i: infos)
      {
        ok &= QFile::link (i.absoluteFilePath (), targetPath + i.fileName ());
      }
      break;

    case Qt::MoveAction:
      for (const auto &i: infos)
      {
        ok &= QFile::rename (i.absoluteFilePath (), targetPath + i.fileName ());
      }
      break;

    default:
      return false;
  }
  return ok;
}

bool CopyPaste::isCut (const QMimeData &mime) const
{
#ifdef Q_OS_LINUX
  return (mime.data (kdeCut) == kdeCutValue ||
          mime.data (gnomeCut).startsWith (gnomeCutValue));
#elif Q_OS_WIN
  return (mime.data (winCut) == winCutValue);
#endif
}

QString CopyPaste::uniqueFilePath (const QString &targetPath, const QString &name) const
{
  auto targetName = name;
  while (QFile::exists (targetPath + targetName))
  {
    targetName = QObject::tr ("copy_") + targetName;
  }
  return targetPath + targetName;
}

QStringList CopyPaste::names (const QList<QUrl> &urls) const
{
  QStringList result;
  result.reserve (urls.size ());
  for (const auto &i: urls)
  {
    if (i.isLocalFile ())
    {
      result << i.toString ();
    }
  }
  return result;
}

QList<QUrl> CopyPaste::urls (const QList<QFileInfo> &infos) const
{
  QList<QUrl> urls;
  urls.reserve (infos.size ());
  for (const auto &i: infos)
  {
    urls << QUrl::fromLocalFile (i.absoluteFilePath ());
  }
  return urls;
}

QList<QFileInfo> CopyPaste::infos (const QList<QUrl> &urls) const
{
  QList<QFileInfo> result;
  result.reserve (urls.size ());
  for (const auto &i: urls)
  {
    if (i.isLocalFile ())
    {
      result << i.toLocalFile ();
    }
  }
  return result;
}