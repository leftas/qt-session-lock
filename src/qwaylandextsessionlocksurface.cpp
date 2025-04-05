#include "qwaylandextsessionlocksurface.h"

#include <QtWaylandClient/private/qwaylandscreen_p.h>
#include <QtWaylandClient/private/qwaylandshellsurface_p.h>
#include <QtWaylandClient/private/qwaylandsurface_p.h>
#include <QtWaylandClient/private/qwaylandwindow_p.h>
#include <qpa/qwindowsysteminterface.h>

namespace ExtSessionLockV1Qt {

QWaylandExtLockSurface::QWaylandExtLockSurface(
    QWaylandExtSessionLockManagerIntegration *manager,
    QtWaylandClient::QWaylandWindow *window)
    : QtWaylandClient::QWaylandShellSurface(window),
      QtWayland::ext_session_lock_surface_v1() {
  window->waylandSurface()->commit();
  connect(
      manager, &QWaylandExtSessionLockManagerIntegration::requestLock, this,
      [window, manager, this] {
        if (m_isLocked) {
          return;
        }
        m_isLocked = true;
        init(manager->m_lock->get_lock_surface(
            window->wlSurface(), window->waylandScreen()->output()));
      },
      Qt::QueuedConnection);
}

QWaylandExtLockSurface::~QWaylandExtLockSurface() {
  if (isInitialized()) {
    destroy();
  }
}

void QWaylandExtLockSurface::ext_session_lock_surface_v1_configure(
    uint32_t serial, uint32_t width, uint32_t height) {
  ack_configure(serial);
  m_peddingSize = QSize(width, height);
  if (!m_configured) {
    m_configured = true;
    window()->resizeFromApplyConfigure(m_peddingSize);
    window()->updateExposure();
  } else {
    window()->applyConfigureWhenPossible();
  }
}

void QWaylandExtLockSurface::setXdgActivationToken(const QString& token)  {
    (void)token;
    return;
}

void QWaylandExtLockSurface::applyConfigure() {
  window()->resizeFromApplyConfigure(m_peddingSize);
}

} // namespace ExtSessionLockV1Qt
