#include "SAXYDataTracker.h"
#include <qwt_plot_item.h>
#include <qwt_plot_curve.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_dict.h>
#include <qwt_plot.h>
#include <qwt_painter.h>
#include <algorithm>
#include <numeric>
SAXYDataTracker::SAXYDataTracker(QWidget* canvas) :
    QwtPlotPicker( canvas )
{
    setTrackerMode( QwtPlotPicker::ActiveOnly );
    setRubberBand( UserRubberBand  );
    setStateMachine( new QwtPickerTrackerMachine() );
    connect (this,&QwtPicker::moved,this,&SAXYDataTracker::mouseMove);
    if(plot ())
        connect (plot(),&QwtPlot::itemAttached,this,&SAXYDataTracker::itemAttached );
}

QwtText SAXYDataTracker::trackerTextF(const QPointF& pos) const
{
    QwtText trackerText;
    if(!m_closePoint.isValid ())
        return trackerText;
    trackerText.setColor( Qt::black );
    QColor lineColor = m_closePoint.curve()->pen ().color ();
    QColor bkColor(lineColor);
    bkColor.setAlpha (30);
    trackerText.setBorderPen( m_closePoint.curve()->pen () );
    trackerText.setBackgroundBrush( bkColor );
    QPointF point = m_closePoint.getClosePoint ();
    QString info = QStringLiteral("<font color=\"%1\">y:%2</font><br>")
                   .arg(lineColor.name ()).arg(point.y ())
                   +
                   QStringLiteral("<font color=\"%1\">x:%2</font>")
                                      .arg(lineColor.name ()).arg(point.x ());
    trackerText.setText( info );
    trackerText.setBorderRadius (5);
    return trackerText;

}

QRect SAXYDataTracker::trackerRect(const QFont& font) const
{
    QRect r = QwtPlotPicker::trackerRect( font );
    r += QMargins(5,5,5,5);
    return r;
}

void SAXYDataTracker::drawRubberBand(QPainter* painter) const
{
    if ( !isActive() || rubberBand() == NoRubberBand ||
        rubberBandPen().style() == Qt::NoPen )
    {
        return;
    }
    if(!m_closePoint.isValid ())
        return;
    QPolygon pickedPoint = pickedPoints ();
    if(pickedPoint.count () < 1)
        return;
    //获取鼠标的客户坐标位置
    const QPoint pos = pickedPoint[0];
    const QPointF closePoint = m_closePoint.getClosePoint ();
    const QPoint cvp = transform (closePoint);
    QwtPainter::drawLine (painter,pos,cvp);
    QRect r(0,0,10,10);
    r.moveCenter (cvp);
    QwtPainter::drawRect (painter,r);
    //QwtPainter::drawEllipse (painter,r);
}

void SAXYDataTracker::calcClosestPoint(const QPoint& pos)
{
    const QwtPlotItemList curveItems =
        plot()->itemList( QwtPlotItem::Rtti_PlotCurve );
    if(curveItems.size () <= 0)
        return;
    //把屏幕坐标转换为图形的数值坐标
    QPointF mousePoint = invTransform(pos);
    //记录最短的距离，默认初始化为double的最大值
    double distance = std::numeric_limits<double>::max ();
    //记录前一次最近点的曲线指针
    QwtPlotCurve * oldCur = m_closePoint.curve ();
    for ( int i = 0; i < curveItems.size(); ++i )
    {
        QwtPlotCurve * cur = static_cast<QwtPlotCurve *>( curveItems[i] );
        int index = cur->closestPoint (pos);
        if(-1 == index)
            continue;
        QPointF p = cur->sample (index);
        QLineF lp(p,mousePoint);
        double l = lp.length ();
        if(l < distance)
        {
            m_closePoint.setDistace(l);
            m_closePoint.setIndex(index);
            m_closePoint.setCurve(cur);
            distance = l;
        }
    }
    //说明最近点的曲线更换了，标记线的颜色换为当前曲线的颜色
    if(m_closePoint.isValid () && oldCur!=m_closePoint.curve ())
    {
        QPen p(m_closePoint.curve ()->pen ());
        p.setWidth (1);
        setRubberBandPen (p);
    }
}

void SAXYDataTracker::mouseMove(const QPoint& pos)
{
    calcClosestPoint(pos);
}

void SAXYDataTracker::itemAttached(QwtPlotItem* plotItem, bool on)
{
    if(!on)
    {
        if(QwtPlotItem::Rtti_PlotCurve == plotItem->rtti ())
        {
            QwtPlotCurve * cur = static_cast<QwtPlotCurve *>( plotItem);
            if(cur == m_closePoint.curve())
                m_closePoint.setInvalid ();
        }
    }
}

SAXYDataTracker::closePoint::closePoint()
  :m_curve(nullptr)
  ,m_index(-1)
  ,m_distace(std::numeric_limits<double>::max ())
{

}

void SAXYDataTracker::closePoint::setCurve(QwtPlotCurve* cur)
{
    this->m_curve = cur;
}

bool SAXYDataTracker::closePoint::isValid() const
{
    return ((this->curve() != nullptr) && (this->index() > 0));
}

QPointF SAXYDataTracker::closePoint::getClosePoint() const
{
    if(isValid ())
        return this->curve()->sample (this->index());
    return QPointF();
}

void SAXYDataTracker::closePoint::setInvalid()
{
    setCurve (nullptr);
    setIndex (-1);
    setDistace (std::numeric_limits<double>::max ());
}
