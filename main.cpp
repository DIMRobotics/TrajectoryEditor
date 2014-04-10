//========================================================================
//! @file       Main.cpp
//{=======================================================================
//!
//! @brief      <Заголовок>\n
//! @brief      <Подзаголовок>
//!
//! @version    [Version 0.01 alpha, build 1]
//! @author     Copyright (C) <Автор>, <Год> (<Имя> <Почта>)
//! @date       <Дата>
//!
//! @par        Протестировано
//!           - (TODO: список платформ)
//!
//! @todo     - (TODO: список ближайших планов по этому файлу)
//!
//! @bug      - (TODO: список найденных ошибок в этом файле)
//!
//! @par        История изменений файла
//!           - Версия 0.01 Alpha
//!             - Только что созданный файл
//!
//}=======================================================================

#include "TXLib.h"
#include <fstream>
#include <cmath>

#define ROBOT_RADIUS 45
#define CENTER_LINE_COLOR TX_YELLOW
#define MAIN_LINE_COLOR TX_BLUE
#define VERTEX_COLOR TX_LIGHTRED
#define CHOOSED_VERTEX_COLOR TX_GREEN

class Point
{
public:
    double x;
    double y;
    Point* next;
    Point* prev;

    Point (double x_, double y_, Point* prev_ = 0, Point* next_ = 0):
        x (x_),
        y (y_),
        prev (prev_),
        next (next_)
    {
    }

    ~Point ()
    {
    }
};

class Manager
{
public:
    bool mouseLButton, mouseRButton;
    Point* choosedVertex;
    Point* vertexList;
    Point* lastVertex;
    Point* drawingVertex;
    int radius;

    Manager ():
        mouseLButton (0),
        mouseRButton (0),
        choosedVertex (0),
        vertexList (0),
        lastVertex (0),
        drawingVertex (0),
        radius (10)
        
    {
    }

    ~Manager () {
        RemoveVertexList();
    }

    void RemoveVertexList() {
        Point *next = vertexList;

        while (next) {
            vertexList = next;
            next = next->next;
            delete vertexList;
        }
    }

    void LButtonDown ()
    {
        if (mouseRButton) return;

        if (vertexList && !mouseLButton && !choosedVertex)
        {
            Point* now = vertexList;
            int mX = 0, mY = 0;

            while (now)
            {
                mX = txMouseX ();
                mY = txMouseY ();
                if ((now -> x - mX) * (now -> x - mX) + (now -> y - mY) * (now -> y - mY) <= radius * radius)
                {
                    mouseLButton = 1;
                    choosedVertex = now;
                    return;
                }

                now = now -> next;
            }
        }

        if (mouseLButton && choosedVertex)
        {
            choosedVertex -> x = txMouseX ();
            choosedVertex -> y = txMouseY ();
            return;
        }

        if (!vertexList)
        {
            vertexList = new Point (txMouseX (), txMouseY ());
            choosedVertex = vertexList;
            lastVertex = vertexList;
            mouseLButton = 1;
            return;
        }

        if (vertexList)
        {
            lastVertex -> next = new Point (txMouseX(), txMouseY (), lastVertex);
            lastVertex = lastVertex -> next;
            choosedVertex = lastVertex;
            mouseLButton = 1;
        }

    }

    void LButtonUp ()
    {
        if (!mouseLButton) return;
        mouseLButton = 0;
        choosedVertex = 0;
    }

    void RButtonDown ()
    {
        if (!mouseRButton)
        {
            Point* now = vertexList;
            double mX = txMouseX (), mY = txMouseY ();

            while (now)
            {
                if ((now -> x - mX) * (now -> x - mX) + (now -> y - mY) * (now -> y - mY) <= radius * radius)
                {
                    Point* p = new Point (txMouseX(), txMouseY (), now, now -> next);
                    if (now -> next) now -> next -> prev = p;
                    now -> next = p;
                    mouseRButton = 1;
                    choosedVertex = now;
                    drawingVertex = p;
                    return;
                }

                now = now -> next;
            }
            return;
        }
        else
        {
            if (drawingVertex)
            {
                drawingVertex -> x = txMouseX ();
                drawingVertex -> y = txMouseY ();
                return;
            }
        }
    }

    void RButtonUp ()
    {
        if (!mouseRButton) return;

        double mX = txMouseX (), mY = txMouseY ();

        if ((choosedVertex -> x - mX) * (choosedVertex -> x - mX) + (choosedVertex -> y - mY) * (choosedVertex -> y - mY)
            <=
            radius * radius)
        {

            Point* now = drawingVertex;
            if (now -> prev) now -> prev -> next = now -> next;
            if (now -> next) now -> next -> prev = now -> prev;
            delete now;

            now = choosedVertex;
            if (now -> prev) now -> prev -> next = now -> next;
            if (now -> next) now -> next -> prev = now -> prev;
            if (now == vertexList) vertexList = now->next;
            delete now;
            mouseRButton = 0;
            drawingVertex = 0;
            choosedVertex = 0;
            return;
        }
        else
        {
            mouseRButton = 0;
            drawingVertex = 0;
            choosedVertex = 0;
            return;
        }
    }

    void Draw ()
    {
        Point* now = vertexList;

        if (now && !now -> next)
        {
            if (now == lastVertex)
                {
                    txSetColor (CHOOSED_VERTEX_COLOR);
                    txSetFillColor (CHOOSED_VERTEX_COLOR);
                }
                else
                {
                    txSetColor (VERTEX_COLOR);
                    txSetFillColor (VERTEX_COLOR);
                }

                txCircle (now -> x, now -> y, radius);

            return;
        }

        while (now)
        {
            if (now -> prev)
            {
                txSetColor (MAIN_LINE_COLOR, radius * 2);
                txSetFillColor (MAIN_LINE_COLOR);
                txLine (now -> prev -> x, now -> prev -> y, now -> x, now -> y);

                txSetColor (CENTER_LINE_COLOR, 1);
                txSetFillColor (CENTER_LINE_COLOR);
                txLine (now -> prev -> x, now -> prev -> y, now -> x, now -> y);

                if (now == lastVertex)
                {
                    txSetColor (CHOOSED_VERTEX_COLOR);
                    txSetFillColor (CHOOSED_VERTEX_COLOR);
                }
                else
                {
                    txSetColor (VERTEX_COLOR);
                    txSetFillColor (VERTEX_COLOR);
                }

                txCircle (now -> x, now -> y, radius);

                if (now -> prev == lastVertex)
                {
                    txSetColor (CHOOSED_VERTEX_COLOR);
                    txSetFillColor (CHOOSED_VERTEX_COLOR);
                }
                else
                {
                    txSetColor (VERTEX_COLOR);
                    txSetFillColor (VERTEX_COLOR);
                }

                txCircle (now -> prev -> x, now -> prev -> y, radius);
            }

            now = now -> next;
        }
    }

    double getAngle(Point *p1, Point *p2, Point *p3) {
        double v1_x = p2->x - p1->x;
        double v1_y = p2->y - p1->y;
        double v2_x = p3->x - p2->x;
        double v2_y = p3->y - p2->y;

        double pseudoscalar = (v1_x * v2_y) - (v1_y * v2_x);
        double angle = acos((v1_x * v2_x + v1_y * v2_y) / sqrt((v1_x * v1_x + v1_y * v1_y) * (v2_x * v2_x + v2_y * v2_y)));
        if (pseudoscalar >= 0)
            return angle;
        else
            return -angle;

    }

    double getDistance(Point *p1, Point *p2) {
        /** @todo: конвертировать из пикселей в нормальные единицы */
        return sqrt((p2->x - p1->x) * (p2->x - p1->x) + (p2->y - p1->y) * (p2->y - p1->y));
    }

    void Export(const char *filename) {
        std::fstream file;
        file.open(filename, std::fstream::out);

        if (!vertexList) {
            file.close();
            return;
        }

        Point *lst = vertexList;
        Point *nextlst = vertexList->next;
        Point *prevlst = vertexList;

        /* Stage 1. out start point coordinates */
        file << lst->x << ' ' << lst->y;

        if (!nextlst) {
            file << std::endl;
            file.close();
            return;
        }

        /* Export angle */
        /* For this, let's create a point to set vector with end in
           first point, which is parallel to Ox axis */
        Point *zero = new Point(-10, lst->y, NULL, NULL);
        double startAngle = getAngle(zero, lst, nextlst);

        file << ' ' << startAngle << std::endl;

        lst = lst->next;
        nextlst = nextlst->next;

        while (nextlst) {
            file << std::endl;
            file << getDistance(prevlst, lst) * 3 << ' ' << getAngle(prevlst, lst, nextlst);

            prevlst = lst;
            lst = nextlst;
            nextlst = nextlst->next;
        }

        file << std::endl << getDistance(prevlst, lst) << std::endl;

        file.close();
    }

    void Import(const char *filename) {
        std::fstream file;
        file.open(filename, std::fstream::in);

        if (!file.is_open())
            return; /* no file present */

        /* read points and fill vertex list */
        /* first, clear vertex list */
        RemoveVertexList();

        double sx, sy, sangle, sdistance;
        double global_angle = 0;

        /* read start point */
        file >> sx >> sy;
        if (!file.good()) { /* we have problems with file - maybe empty? */
            file.close();
            return;
        }
        vertexList = new Point(sx, sy);

        Point *localList = vertexList;

        /* try to read angle - means next point is defined */
        file >> sangle;
        while (file.good()) {
            global_angle += sangle;

            /* if we read angle, we can read the distance */
            file >> sdistance;
            
            sdistance /= 3;

            /* we know a global angle and path */
            /* calculate delta-x and delta-y */
            double delta_x = sdistance * cos(global_angle);
            double delta_y = sdistance * sin(global_angle);

            /* create point */
            localList->next = new Point(localList->x + delta_x, localList->y + delta_y, localList, NULL);
            localList = localList->next;

            /* try to read next angle */
            file >> sangle;
        }

        choosedVertex = localList;
        lastVertex = localList;

        file.close();
    }
};

int main()
    {
    txCreateWindow (1000, 672);
    
    txTextOut(5, 652, "Mouse (vertexes): Left - Add and Move, Right - Delete and Clone; Esc - save and exit; Space - expand lines; Ctrl-S - save in report.txt, Ctrl-O - load report.txt");
    
    txTextCursor (false);

    HDC field = txLoadImage ("field_color.bmp");
    
    if (field)
       txBitBlt (txDC(), 0, 0, 1000, 652, field, 0, 0);

    Manager* manager = new Manager ();

    manager->Import("report.txt");

    while (!GetAsyncKeyState(VK_ESCAPE))
    {
        if (txMouseButtons () & 1)       manager -> LButtonDown ();
        if (! (txMouseButtons () & 1))   manager -> LButtonUp ();
        if (txMouseButtons () >> 1)      manager -> RButtonDown ();
        if (!(txMouseButtons () >> 1))   manager -> RButtonUp ();
        if (GetAsyncKeyState (VK_SPACE)) manager -> radius = ROBOT_RADIUS;
        else manager -> radius = 10;
        if (GetAsyncKeyState (VK_CONTROL)) {
           if (GetAsyncKeyState ('S')) manager -> Export ("report.txt");
           if (GetAsyncKeyState ('O')) manager -> Import ("report.txt");                     
        }

        txBegin ();
        txBitBlt (txDC(), 0, 0, 1000, 667, field, 0, 0);
        manager -> Draw ();
        txEnd ();
        txSleep (10);
    }

    manager->Export("report.txt");

    txDeleteDC(field);
    _txExit = 1;

    return 0;
    }

