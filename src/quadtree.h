#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STRING_LEN 256
#define MAX_NAME_LEN 6

#define NDIR_1D 2 //#directions in 1d space
#define NDIR_2D 4 //#directions in 2d space

typedef enum {X, Y} axis;
typedef enum {NW, NE, SW, SE} quadrant; //Use this in ordering traversal
typedef enum {LEFT, RIGHT, BOTH} direction;

typedef char tname[MAX_NAME_LEN + 1];

struct Rectangle {
	tname Name; //Name of rectangle
	struct Rectangle *binSon[NDIR_1D]; //Left and Right sons
	int Center[NDIR_1D]; //Centroid ...
	int Length[NDIR_1D]; //... distance to the borders of rect
	int Label; //Used for LABEL() operation
};

struct bNode {
	struct bNode *binSon[NDIR_1D]; //Left and Right sons
	struct Rectangle *Rect; //Pointer to the rectangle whose area contains the axis subdivision point
};

struct cNode {
	struct cNode *spcSon[NDIR_2D];//Four principal quad directions (Quadtree representation)
	struct bNode *binSon[NDIR_1D];//Pointers to rectangle sets for each of the axis 
                                  //(binary tree for each quadtree)
};

struct mxCif {
	struct cNode *mxCifRoot; //Root cNode
	struct Rectangle World; //world extent
	int id; //Quadtree ID
	int count;//number of rectangles
	int size;//world size
};

struct rectangle {
	struct rectangle *left;
	struct rectangle *right;
	struct Rectangle *Rect;
};
/*These are variables holding roots of corresponding trees*/
struct mxCif mxCifTree;
struct rectangle *rectTree;

void initMxCifTree(void) {
	mxCifTree.mxCifRoot = NULL;
	strcpy(mxCifTree.World.Name,"MX-CIF");
	mxCifTree.World.binSon[LEFT] = NULL;
	mxCifTree.World.binSon[RIGHT] = NULL;
	//world size will be assigned later

}

void initRectTree(void) {rectTree = NULL;}