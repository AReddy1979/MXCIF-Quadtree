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
}

struct bNode {
	struct bNode *binSon[NDIR_1D]; //Left and Right sons
	struct Rectangle *Rect; //Pointer to the rectangle whose area contains the axis subdivision point
}

struct cNode {
	struct cNode *spcSon[NDIR_2D];//Four principal quad directions
	struct bNode *binSon[NDIR_2D];//Pointers to rectangle sets for each of the axis
}

struct mxCif {
	struct cNode *mxCifRoot; //Root cNode
	struct Rectangle World; //world extent
	int id; //Quadtree ID
}
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

void initRectTree(void) {rectTree = NULL}

int dec() {
    //Use memory allocation
    char str[100],s3[10];
    char *s1,*s2;
    char *e;
    int index;
    int a,b,c,d,n;

    if(fgets(str,100,stdin) == NULL) {
    	return 0;
    }
    s1 = (char*) malloc(strlen(str));
    strcpy(s1,str);

    e = strchr(str, '(');
    if(e == NULL){
    	if(strlen(str) > 1) {
    		free(s1);
        	return 1;
    	}
        free(s1);
        return 0;
    }
    index = (int)(e - str);
    s2 = s1 + index;
    
    if(strncmp(s1,"INIT_QUADTREE",index) == 0) {
        n = sscanf(s2, "(%d)\n", &a);
        printf("1 %d\n",a);
    }
    else if(strncmp(s1,"DISPLAY",index) == 0) {
        printf("2\n");
    }
    else if(strncmp(s1,"LIST_RECTANGLES",index) == 0) {
        printf("3\n");
    }
    else if(strncmp(s1,"CREATE_RECTANGLE",index) == 0) {
    	n = sscanf(s2, "(%[^,],%d,%d,%d,%d)\n", s3,&a,&b,&c,&d);
        printf("4 %s %d %d %d %d\n",s3,a,b,c,d);
    }
    else if(strncmp(s1,"SEARCH_POINT",index) == 0) {
    	n = sscanf(s2, "(%d,%d)\n", &a,&b);
        printf("5 %d %d\n",a,b);
    }
    else if(strncmp(s1,"RECTANGLE_SEARCH",index) == 0) {
        n = sscanf(s2, "(%[^)]\n", s3);
        printf("6 %s\n",s3);
    }
    else if(strncmp(s1,"INSERT",index) == 0) {
        n = sscanf(s2, "(%[^)]\n", s3);
        printf("7 %s\n",s3); //needs work
    }
    else if(strncmp(s1,"DELETE_RECTANGLE",index) == 0) {
        n = sscanf(s2, "(%[^)]\n", s3);
        printf("8A %s\n",s3);
    }
    else if(strncmp(s1,"DELETE_POINT",index) == 0) {
    	n = sscanf(s2, "(%d,%d)\n", &a,&b);
        printf("8B %d %d\n",a,b);
    }
    else if(strncmp(s1,"MOVE",index) == 0) {
    	n = sscanf(s2, "(%[^,],%d,%d)\n", s3,&a,&b);
        printf("9 %s %d %d\n",s3,a,b);
    }
    else if(strncmp(s1,"TOUCH",index) == 0) {
        n = sscanf(s2, "(%[^)]\n", s3);
        printf("10 %s\n",s3);
    }
    else if(strncmp(s1,"WITHIN",index) == 0) {
    	n = sscanf(s2, "(%[^,],%d)\n", s3,&a);
        printf("11 %s %d\n",s3,a);
    }
    else if(strncmp(s1,"HORIZ_NEIGHBOR",index) == 0) {
        n = sscanf(s2, "(%[^)]\n", s3);
        printf("12A %s\n",s3);
    }
    else if(strncmp(s1,"VERT_NEIGHBOR",index) == 0) {
        n = sscanf(s2, "(%[^)]\n", s3);
        printf("12B %s\n",s3);
    }
    else if(strncmp(s1,"NEAREST_RECTANGLE",index) == 0) {
    	n = sscanf(s2, "(%d,%d)\n", &a,&b);
        printf("13 %d %d\n",a,b);
    }
    else if(strncmp(s1,"WINDOW",index) == 0) {
    	n = sscanf(s2, "(%d,%d,%d,%d)\n", &a,&b,&c,&d);
        printf("14 %d %d %d %d\n",a,b,c,d);
    }
    else if(strncmp(s1,"NEAREST_NEIGHBOR",index) == 0) {
        n = sscanf(s2, "(%[^)]\n", s3);
        printf("15 %s\n",s3);
    }
    else if(strncmp(s1,"LEXICALLY_GREATER_NEAREST_NEIGHBOR",index) == 0) {
        n = sscanf(s2, "(%[^)]\n", s3);
        printf("16 %s\n",s3);
    }
    else if(strncmp(s1,"LABEL",index) == 0) {
        printf("17\n");
    }
    else if(strncmp(s1,"SPATIAL_JOIN",index) == 0) {
        printf("18\n");
    }



    free(s1);
    return 1;
}

int main() {
	int x = -1;

    while(x!= 0) {
        //printf("Enter input:\n");
        x = dec();
    }
	return 0;
}

