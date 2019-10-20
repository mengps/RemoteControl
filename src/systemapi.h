#ifndef SYSTEMAPI_H
#define SYSTEMAPI_H

#include <QPixmap>

class SystemApi
{
public:
    /** System Event */
    static void mousePress(const QPointF &pos);
    static void mouseRelease(const QPointF &pos);
    static void mouseMove(const QPointF &pos);

    /** System Tools */
    static QPixmap grabScreen();
    static QPixmap grabCursor();
};

#endif
