#include "qwaylandextsessionlockmanagerintegration.h"
#include "interfaces/command.h"
#include "qwaylandextsessionlocksurface.h"
#include "wayland-ext-session-lock-v1-client-protocol.h"

#include <QtWaylandClient/private/qwaylanddisplay_p.h>
#include <QtWaylandClient/private/qwaylandintegration_p.h>
#include <QtWaylandClient/private/qwaylandwindow_p.h>

namespace ExtSessionLockV1Qt {
QWaylandExtSessionLockManagerIntegration::
    QWaylandExtSessionLockManagerIntegration()
    : QtWaylandClient::QWaylandShellIntegrationTemplate<
          QWaylandExtSessionLockManagerIntegration>(1),
      m_lock(new QtWayland::ext_session_lock_v1()) {
  connect(Command::instance(), &Command::requestUnlock, this, [this] {
    if (m_lock && m_lock->isInitialized()) {
      m_lock->unlock_and_destroy();
      // We are required to do round trip, so the compositor would be informed.
      QtWaylandClient::QWaylandIntegration::instance()
          ->display()
          ->forceRoundTrip();
      m_lock = nullptr;
    }
  });

  connect(Command::instance(), &Command::requestLock, this, [this] {
    if (!m_lock->isInitialized()) {
      m_lock->init(lock());
    }
    Q_EMIT requestLock();
  });
}

QWaylandExtSessionLockManagerIntegration::
    ~QWaylandExtSessionLockManagerIntegration() {
  if (object() && ext_session_lock_manager_v1_get_version(object()) >=
                      EXT_SESSION_LOCK_MANAGER_V1_DESTROY_SINCE_VERSION) {
    ext_session_lock_manager_v1_destroy(object());
  }
}

void *QWaylandExtSessionLockManagerIntegration::nativeResourceForWindow(
    const QByteArray &resource, QWindow *window) {
  if (auto waylandWindow =
          static_cast<QtWaylandClient::QWaylandWindow *>(window->handle())) {
    if (auto shellSurface = qobject_cast<QWaylandExtLockSurface *>(
            waylandWindow->shellSurface())) {
      return shellSurface->object();
    }
  }
  return nullptr;
}

QtWaylandClient::QWaylandShellSurface *
QWaylandExtSessionLockManagerIntegration::createShellSurface(
    QtWaylandClient::QWaylandWindow *window) {
  return new QWaylandExtLockSurface(this, window);
}
} // namespace ExtSessionLockV1Qt
