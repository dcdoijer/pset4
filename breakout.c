// breakout.c
//
// Computer Science 50
// Problem Set 4
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include "gevents.h"
#include "gobjects.h"
#include "gwindow.h"

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

// height and width of paddle
#define P_WIDTH 100
#define P_HEIGHT 10

// width and height of the bricks
#define B_WIDTH 30
#define B_HEIGHT 10

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);
    
// number of lives initially
int lives = LIVES;

// number of points initially
int points = 0;
   
int main(void)
{      
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);
    
    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;
    
    double drand48(void);
    double vert = 2.0;
    double hori = 2 * drand48();

    // keep playing until game over
    waitForClick();
    while (lives > 0 && bricks > 0)
    {

        move(ball, hori, vert);
        GObject object = detectCollision(window, ball);
        
        // if the ball hits the top invert direction
        // if ball hits bottom, deduct life and restart main
        if (getY(ball) + getHeight(ball) >= HEIGHT)
        {
            lives--;
            waitForClick();
            setLocation(ball, 190, 290);
        }
        else if (getY(ball) <= 0)
        {
            vert = -vert;
        }
        
        // if the ball hits the paddle, invert vertical direction    
        if (object == paddle)
        {
            vert = -vert;
        }

        // if the ball hits a brick, remove brick and
        // invert vertical direction
        if (object != NULL)
        {
            if (strcmp(getType(object), "GRect") == 0 && object != paddle)
            {
                removeGWindow(window, object);
                bricks--;
                points++;
                vert = -vert;
                updateScoreboard(window, label, points);
            }
        }

                // if the ball hits the sides, invert direction
        if (getX(ball) + getWidth(ball) >= WIDTH)
        {
            hori = -hori;
        }   
        else if (getX(ball) <= 0)
        {
            hori = -hori;
        }

        pause(10);

        GEvent move_paddle = getNextEvent(MOUSE_EVENT);

        if (move_paddle != NULL)
        {
            if (getEventType(move_paddle) == MOUSE_MOVED)
            {
                double x = getX(move_paddle) - getWidth(paddle) / 2;
                double y = HEIGHT * .90;
                setLocation(paddle, x, y);
            }
        }
        
    }

    // wait for click before exiting
    waitForClick();
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    // initialize first brick position
    int brick_x = 4;
    int brick_y = 50;
    char* colors[5] = {"RED", "GREEN", "BLUE", "ORANGE", "YELLOW"};

    for (int rows = 0; rows < ROWS; rows++)
    {
        for (int cols = 0; cols < COLS; cols++)
        {
            GRect bricks = newGRect(brick_x, brick_y, B_WIDTH, B_HEIGHT);
            setFilled(bricks, true);
            setColor(bricks, colors[rows]);
            addGWindow(window, bricks);
            brick_x += 40;
        }
        brick_y += 15;
        brick_x = 4;
    }
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    GOval ball = newGOval(190, 290, 20, 20);
    setColor(ball, "BLACK");
    setFilled(ball, true);
    addGWindow(window, ball);
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    GRect paddle = newGRect(((WIDTH * .5) - 50), (HEIGHT * .90), P_WIDTH, P_HEIGHT);
    setFilled(paddle, true);
    setColor(paddle, "BLACK");
    addGWindow(window, paddle);
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    char* points = "0";
    GLabel label = newGLabel(points);
    setFont(label, "SansSerif-36");
    setColor(label, "GRAY");
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
    addGWindow(window, label);
    return label;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
