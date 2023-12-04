#include "qwaylandextsessionlocksurface.h"
#include "window.h"

#include <QtWaylandClient/private/qwaylandshellsurface_p.h>
#include <QtWaylandClient/private/qwaylandsurface_p.h>
#include <QtWaylandClient/private/qwaylandwindow_p.h>
#include <private/qwaylandscreen_p.h>

namespace ExtSessionLockV1Qt {

QWaylandExtLockSurface::QWaylandExtLockSurface(QWaylandExtSessionLockManagerIntegration *manager,
                                               QtWaylandClient::QWaylandWindow *window)
  : QtWaylandClient::QWaylandShellSurface(window)
  , QtWayland::ext_session_lock_surface_v1()
{
    ExtSessionLockV1Qt::Window *inteface = Window::get(window->window());
    Q_ASSERT(inteface);
    connect(
      manager,
      &QWaylandExtSessionLockManagerIntegration::requestLock,
      this,
      [window, manager, inteface, this] {
          if (m_isLocked) {
              return;
          }
          m_isLocked = true;
          init(manager->m_lock->get_lock_surface(window->wlSurface(),
                                                 window->waylandScreen()->output()));
          connect(inteface, &Window::requestUnlock, this, [manager] {
              manager->m_lock->unlock_and_destroy();
          });
      },
      Qt::QueuedConnection);
}

QWaylandExtLockSurface::~QWaylandExtLockSurface()
{
    if (isInitialized()) {
        destroy();
    }
}

void
QWaylandExtLockSurface::ext_session_lock_surface_v1_configure(uint32_t serial,
                                                              uint32_t width,
                                                              uint32_t height)
{
    ack_configure(serial);
    m_peddingSize = QSize(width, height);
    if (!m_configured) {
        m_configured = true;
        window()->resizeFromApplyConfigure(m_peddingSize);
        window()->handleExpose(QRect(QPoint(), m_peddingSize));
    } else {
        window()->applyConfigureWhenPossible();
    }
}

void
QWaylandExtLockSurface::applyConfigure()
{
    window()->resizeFromApplyConfigure(m_peddingSize);
}
}
