#ifndef DRAGGABLENODEITEM_H
#define DRAGGABLENODEITEM_H

#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QPen>
#include <QBrush>

/**
 * @brief Custom draggable node item for the graph visualization
 */
class DraggableNodeItem : public QGraphicsEllipseItem {
public:
    DraggableNodeItem(qreal x, qreal y, qreal width, qreal height, 
                      const QPen& pen, const QBrush& brush, QGraphicsItem* parent = nullptr)
        : QGraphicsEllipseItem(x, y, width, height, parent) {
        setPen(pen);
        setBrush(brush);
        setFlag(QGraphicsItem::ItemIsMovable, true);
        setFlag(QGraphicsItem::ItemIsSelectable, true);
        setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
        setCursor(Qt::OpenHandCursor);
    }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override {
        setCursor(Qt::ClosedHandCursor);
        QGraphicsEllipseItem::mousePressEvent(event);
    }

    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override {
        setCursor(Qt::OpenHandCursor);
        QGraphicsEllipseItem::mouseReleaseEvent(event);
    }

    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override {
        if (change == ItemPositionHasChanged) {
            // Notify that position changed (could be used to update connected edges)
            update();
        }
        return QGraphicsEllipseItem::itemChange(change, value);
    }
};

#endif // DRAGGABLENODEITEM_H
