#ifndef PALETTEEDITOR_H
#define PALETTEEDITOR_H

#include <vector>

using namespace std;

#include <QColor>
#include <QImage>
#include <QPen>
#include <QBrush>
#include <QWidget>

using Model = vector<vector<bool>>;

class PaletteEditor : public QWidget
{
    Q_OBJECT

public:
    PaletteEditor(QWidget *parent = 0);

    void setDimensions(int rows, int cols){
        m_rows = rows;
        m_columns = cols;
    }

    Q_SLOT void clear();
//    QSize sizeHint() const;

    Model getModel(){return m_model;}
    Model getBigModel(){return m_bigModel;}

    void loadModel(vector<vector<bool>> model, bool isBig)
    {
        m_model = model;
        if (isBig)
            m_bigModel = model;
    }

    void resizePalette(int rows, int columns);


protected:

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void drawAtPixel(const QPoint& pos, bool value);

private:

    int boxSize();
    void swapAtPixel(const QPoint& pos, bool trackSame);

    QColor color;
    QPen m_penRed;

    int m_rows = 32;
    int m_columns = 32;

    int m_lastR = -1;
    int m_lastC = -1;

    Model m_model;
    Model m_bigModel;
};


#endif // PALETTEEDITOR_H
