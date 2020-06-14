#include <qwt_plot_picker.h>
class QwtPlotCurve;
class QwtPlotItem;
class SAXYDataTracker: public QwtPlotPicker
{
    Q_OBJECT
public:
    SAXYDataTracker(QWidget * canvas);
protected:
    virtual QwtText trackerTextF(const QPointF & pos) const;
    virtual QRect trackerRect(const QFont & font) const;
    virtual void drawRubberBand (QPainter *painter) const;
    void calcClosestPoint(const QPoint& pos);
private:
    ///
    /// \brief 记录最近点的信息
    ///
    class closePoint
    {
    public:
        closePoint();
        QwtPlotCurve * curve() const{return this->m_curve;}
        void setCurve(QwtPlotCurve * cur);
        bool isValid() const;
        QPointF getClosePoint() const;
        int index() const{return this->m_index;}
        void setIndex(int i){this->m_index = i;}
        double distace() const{return this->m_distace;}
        void setDistace(double d){this->m_distace = d;}
        void setInvalid();
    private:
        QwtPlotCurve *m_curve;
        int m_index;
        double m_distace;
    };
    closePoint m_closePoint;
private slots:

public slots:
    void itemAttached(QwtPlotItem* plotItem,bool on);
    //捕获鼠标移动的槽
    void mouseMove(const QPoint &pos);
};
