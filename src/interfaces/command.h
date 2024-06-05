#pragma once

#include "sessionlockqtinterface_export.h"
#include <QObject>
namespace ExtSessionLockV1Qt {

class SESSIONLOCKQTINTERFACE_EXPORT Command : public QObject {
  Q_OBJECT
public:
  static Command *instance();

  void unlockScreen();
  void lockScreen();

signals:
  void requestUnlock();
  void requestLock();

private:
  explicit Command(QObject *parent = nullptr);
};
} // namespace ExtSessionLockV1Qt
