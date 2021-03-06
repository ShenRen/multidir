#pragma once

#include <QDialog>

class QFileInfo;
class QLabel;
class QPushButton;
class QCheckBox;

class FileConflictResolver : public QDialog
{
Q_OBJECT
public:
  enum Resolution : int
  {
    Pending = 0,
    Source = 1 << 0,
    Target = 1 << 1,
    Rename = 1 << 2,
    Merge = 1 << 3,
    Abort = 1 << 4,
    All = 1 << 5
  };

  explicit FileConflictResolver (QWidget *parent = nullptr);

public slots:
  void resolve (const QFileInfo &source, const QFileInfo &target, int action, int *result);

private:
  QLabel *sourceLabel_;
  QLabel *targetLabel_;
  QPushButton *source_;
  QPushButton *target_;
  QPushButton *rename_;
  QPushButton *merge_;
  QPushButton *abort_;
  QCheckBox *applyToAll_;
};
