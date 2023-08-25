#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "quadtree.h"


//create a variable to keep track of trace, and use another variable so that some functions
//don't print anything, so that we can use them as helper functions

int cnt_space(char* input){
	int count = 0;
    // input sentence
    char ch = *(input+1);
  
    // counting spaces
    while (ch != '\0') {
        if (isspace(ch)) {
            count++;
            //printf("%c\n",(ch + 2));
        }
  
        ch = *input++;
    }
  
    // returning number of spaces
    return (count);
}

int mini(int x, int y){

	return y ^ ((x ^ y) & -(x < y));
}

int init_quad(int w) {
	int x = pow(2,w);
	initMxCifTree();
	initRectTree();
	mxCifTree.size = x;
	mxCifTree.count = 0;
	mxCifTree.mxCifRoot = (struct cNode*) malloc(sizeof(struct cNode));
	return x;
}

void list_rects(struct rectangle* node) {
	if(node == NULL) {
		return;
	}
	list_rects(node -> left);
	printf("%s %d %d %d %d\n",node -> Rect -> Name,node -> Rect -> Center[0],node -> Rect -> Center[1],
		node -> Rect -> Length[0],node -> Rect -> Length[1]);
	list_rects(node -> right);
	return;
}

void list_rect(struct bNode* node) {
	if(node == NULL) {
		return;
	}
	//struct Rectangle *r = node -> Rect;
	list_rect(node -> binSon[0]);
	printf("%s %d %d %d %d\n",node -> Rect -> Name,node -> Rect -> Center[0],node -> Rect -> Center[1],
		node -> Rect -> Length[0],node -> Rect -> Length[1]);
	list_rect(node -> binSon[1]);
	return;
}

void create_rect(char *name,int xcor,int ycor,int hdist,int vdist) {
	struct rectangle* curr = rectTree;
	struct rectangle* prev = NULL;
	int i = 0;
	struct Rectangle *r1 = malloc(sizeof(struct Rectangle));
	r1 -> binSon[0] = NULL;
	r1 -> binSon[1] = NULL;
	strcpy(r1 -> Name,name);
	r1 -> Center[0] = xcor;
	r1 -> Center[1] = ycor;
	r1 -> Length[0] = hdist;
	r1 -> Length[1] = vdist;
	//insert rectangle into rectree
	if(curr == NULL) {
		rectTree = malloc(sizeof(struct rectangle));
		rectTree -> right = NULL;
		rectTree -> left = NULL;
		rectTree -> Rect = r1;
		mxCifTree.count++;
	}
	else {
		while(curr != NULL) {
			if(strcmp(curr -> Rect -> Name,r1 -> Name) == 0) {
				return;
			}
			else if(strcmp(curr -> Rect -> Name,r1 -> Name) > 0) {
				prev = curr;
				curr = curr -> left;
				i = 1;
			}
			else {
				prev = curr;
				curr = curr -> right;
				i = 2;
			}
		}
		if(i == 1) {
			prev -> left = malloc(sizeof(struct rectangle));
			curr = prev -> left;
			curr -> right = NULL;
			curr -> left = NULL;
			curr -> Rect = r1;
			mxCifTree.count++;
		}
		if(i == 2) {
			prev -> right = malloc(sizeof(struct rectangle));
			curr = prev -> right;
			curr -> right = NULL;
			curr -> left = NULL;
			curr -> Rect = r1;
			mxCifTree.count++;
		}
	}

}

char* search_ytree(int goalx,int goaly,int quadwidth,struct bNode *node) {
	struct Rectangle *r1;
	char *ans;
	int wx,ex,ny,sy;//west-x east-x north-y south-y
	if(node == NULL) {
		return NULL;
	}
	r1 = node -> Rect;
	wx = (r1 -> Center[X]) - (r1 -> Length[X]);
	ex = (r1 -> Center[X]) + (r1 -> Length[X]);
	ny = (r1 -> Center[Y]) + (r1 -> Length[Y]);
	sy = (r1 -> Center[Y]) - (r1 -> Length[Y]);
	if(ex >= goalx && wx <= goalx && ny >= goaly && sy <= goaly) {
		ans = (char*) malloc(strlen(r1 -> Name));
		strcpy(ans,r1 -> Name);
		return ans;
	}
	
	if(sy > goaly) {
		return search_ytree(goalx,goaly,quadwidth,node -> binSon[0]);
	}
	else {
		return search_ytree(goalx,goaly,quadwidth,node -> binSon[1]);
	}
	
}
//change so that we return a char*(rectangle name)
char* search_xtree(int goalx,int goaly,int quadwidth,struct bNode *node) {
	struct Rectangle *r1;
	char *ans;
	int wx,ex,ny,sy;//west-x east-x north-y south-y
	if(node == NULL) {
		return NULL;
	}
	r1 = node -> Rect;
	wx = (r1 -> Center[X]) - (r1 -> Length[X]);
	ex = (r1 -> Center[X]) + (r1 -> Length[X]);
	ny = (r1 -> Center[Y]) + (r1 -> Length[Y]);
	sy = (r1 -> Center[Y]) - (r1 -> Length[Y]);
	if(ex >= goalx && wx <= goalx && ny >= goaly && sy <= goaly) { //fix for x-axis
		ans = (char*) malloc(strlen(r1 -> Name));
		strcpy(ans,r1 -> Name);
		return ans;
	}
	

	if(sy > goaly) {
		return search_xtree(goalx,goaly,quadwidth,node -> binSon[0]);
	}
	else {
		return search_xtree(goalx,goaly,quadwidth,node -> binSon[1]);
	}
}


char* search_pt(int a,int currx, int curry, int goalx, int goaly,int quadwidth,struct cNode *node,
	int trace) {
	//printf(" %d",a);
	char *b,*c;
	if(node == NULL) {
		//printf("  NULL node  %d\n",a);
		return NULL;
	}
	if(trace == 1) {
		printf(" %d",a);
	}
	
	b = search_xtree(goalx,goaly,quadwidth,node -> binSon[X]);
	if(b != NULL) {
		return b;
	}
	c = search_ytree(goalx,goaly,quadwidth,node -> binSon[Y]);
	if(c != NULL) {
		return c;
	}
	
	if(currx > goalx && curry < goaly) { //Northwest quadrant
		//printf("curr:(%d,%d)  goal:(%d,%d)\n",currx,curry,goalx,goaly);
		return search_pt((a*4)+1,currx - (quadwidth/4),curry + (quadwidth/4),goalx,goaly,quadwidth/2,node ->spcSon[NW],trace);
	}
	else if(currx < goalx && curry < goaly) { //Northeast quadrant
		//printf("curr:(%d,%d)  goal:(%d,%d)\n",currx,curry,goalx,goaly);
		return search_pt((a*4)+2,currx + (quadwidth/4),curry + (quadwidth/4),goalx,goaly,quadwidth/2,node ->spcSon[NE],trace);
	}
	else if(currx > goalx && curry > goaly) { //Southwest quadrant
		//printf("curr:(%d,%d)  goal:(%d,%d)\n",currx,curry,goalx,goaly);
		return search_pt((a*4)+3,currx - (quadwidth/4),curry - (quadwidth/4),goalx,goaly,quadwidth/2,node ->spcSon[SW],trace);
	}
	else if(currx < goalx && curry > goaly) { //Southeast quadrant
		//printf("curr:(%d,%d)  goal:(%d,%d)\n",currx,curry,goalx,goaly);
		return search_pt((a*4)+4,currx + (quadwidth/4),curry - (quadwidth/4),goalx,goaly,quadwidth/2,node ->spcSon[SE],trace);
	}
	else {
		//printf("]: no rectangle found\n");
		return NULL;
	}
}



struct Rectangle* rtree_find(struct rectangle *node,char* name) {
	int a;
	if(node == NULL) {
		return NULL;
	}
	a = strcmp(node -> Rect -> Name,name);
	if(a == 0) {
		return node -> Rect;
	}
	else if(a > 0) {
		return rtree_find(node -> left,name);
	}
	else {
		return rtree_find(node -> right,name);
	}

}

//searches current quadrant if rectangle at node intersects r1
char* quad_search(struct Rectangle *r1,struct bNode *node,char *ans) {
	int l;
	struct Rectangle *r2;
	int cwx,cex,cny,csy,nwx,nex,nny,nsy;

	if(node == NULL) {
		return ans;
	}

	r2 = node -> Rect;

	//Rectangle r1 coordinates
	cwx = (r1 -> Center[X]) - (r1 -> Length[X]);
	cex = (r1 -> Center[X]) + (r1 -> Length[X]);
	cny = (r1 -> Center[Y]) + (r1 -> Length[Y]);
	csy = (r1 -> Center[Y]) - (r1 -> Length[Y]);
	//Rectangle r2 coordinates
	nwx = (r2 -> Center[X]) - (r2 -> Length[X]);
	nex = (r2 -> Center[X]) + (r2 -> Length[X]);
	nny = (r2 -> Center[Y]) + (r2 -> Length[Y]);
	nsy = (r2 -> Center[Y]) - (r2 -> Length[Y]);

	/*Check if Current rectangle in node interests r1*/
	if(strcmp(r1 -> Name,r2 -> Name) != 0 && nwx < cex && nex > cwx && nny > csy && nsy < cny) {
		l = strlen(ans);
		ans = (char*) realloc(ans,l+strlen(node -> Rect -> Name)+1);
		strcat(ans," ");
		strcat(ans,node -> Rect -> Name);
	}
	ans = quad_search(r1,node -> binSon[0],ans);  //left node traversal
	ans = quad_search(r1,node -> binSon[1],ans); //right node traversal
	return ans;
}

char* touch_search(struct Rectangle *r1,struct bNode *node,char *ans) {
	int l;
	struct Rectangle *r2;
	int cwx,cex,cny,csy,nwx,nex,nny,nsy;

	if(node == NULL) {
		return ans;
	}

	r2 = node -> Rect;

	//Rectangle r1 coordinates
	cwx = (r1 -> Center[X]) - (r1 -> Length[X]);
	cex = (r1 -> Center[X]) + (r1 -> Length[X]);
	cny = (r1 -> Center[Y]) + (r1 -> Length[Y]);
	csy = (r1 -> Center[Y]) - (r1 -> Length[Y]);
	//Rectangle r2 coordinates
	nwx = (r2 -> Center[X]) - (r2 -> Length[X]);
	nex = (r2 -> Center[X]) + (r2 -> Length[X]);
	nny = (r2 -> Center[Y]) + (r2 -> Length[Y]);
	nsy = (r2 -> Center[Y]) - (r2 -> Length[Y]);

	if( strcmp(r1 -> Name,r2 -> Name) != 0 && 
		(((cex == nwx || cwx == nex) && (cny >= nsy && csy <= nny) ) || 
		((cny == nsy || csy == nny) && (cex >= nwx && cwx <= nex) ))) {
		l = strlen(ans);
		ans = (char*) realloc(ans,l+strlen(node -> Rect -> Name)+1);
		strcat(ans," ");
		strcat(ans,node -> Rect -> Name);
	}

	ans = touch_search(r1,node -> binSon[0],ans);  //left node traversal
	ans = touch_search(r1,node -> binSon[1],ans); //right node traversal
	return ans;
}

char* within_search(struct Rectangle *r1,struct bNode *node,char *ans) {
	int l;
	struct Rectangle *r2;
	int cwx,cex,cny,csy,nwx,nex,nny,nsy;

	if(node == NULL) {
		return ans;
	}

	r2 = node -> Rect;

	//Rectangle r1 coordinates
	cwx = (r1 -> Center[X]) - (r1 -> Length[X]);
	cex = (r1 -> Center[X]) + (r1 -> Length[X]);
	cny = (r1 -> Center[Y]) + (r1 -> Length[Y]);
	csy = (r1 -> Center[Y]) - (r1 -> Length[Y]);
	//Rectangle r2 coordinates
	nwx = (r2 -> Center[X]) - (r2 -> Length[X]);
	nex = (r2 -> Center[X]) + (r2 -> Length[X]);
	nny = (r2 -> Center[Y]) + (r2 -> Length[Y]);
	nsy = (r2 -> Center[Y]) - (r2 -> Length[Y]);

	/*Check if Current rectangle in node interests r1*/
	//printf("--reached here--\n");
	if(nex <= cex && nwx >= cwx && nny <= cny && nsy >= csy) {
		//printf("--reach--\n");
		l = strlen(ans);
		ans = (char*) realloc(ans,l+strlen(node -> Rect -> Name)+1);
		strcat(ans," ");
		strcat(ans,node -> Rect -> Name);
	}
	ans = within_search(r1,node -> binSon[0],ans);  //left node traversal
	ans = within_search(r1,node -> binSon[1],ans); //right node traversal
	return ans;
}


//(char *ans,struct Rectangle *r1,)
//q_num = label of current quadtree
//output of rectangle_search in sample output is wrong, as C should come before A
char* rect_search(char *ans,struct Rectangle *r1,struct cNode *node,
	int q_num,int originx,int originy,int width,int trace,int touch) {
	//find rectangle in rectTree in function that calls this function
	int wx,ex,ny,sy,w;
	if(trace == 1) {
		printf(" %d",q_num);
	}
	//printf(" %d",q_num);
	

	if(node == NULL) {
		return ans;
	}
	w = width/4;
	//get wx,ex,ny,sy values
	wx = (r1 -> Center[X]) - (r1 -> Length[X]);
	ex = (r1 -> Center[X]) + (r1 -> Length[X]);
	ny = (r1 -> Center[Y]) + (r1 -> Length[Y]);
	sy = (r1 -> Center[Y]) - (r1 -> Length[Y]);

	//search y-tree and x-tree
	if(touch != 1 && touch != 5) {
		ans = quad_search(r1,node -> binSon[Y],ans);
		ans = quad_search(r1,node -> binSon[X],ans);
	}
	if(touch != 0 && touch != 5){
		//make new touch_search function
		ans = touch_search(r1,node -> binSon[Y],ans);
		ans = touch_search(r1,node -> binSon[X],ans);
	}
	if(touch == 5) {
		ans = within_search(r1,node -> binSon[Y],ans);
		ans = within_search(r1,node -> binSon[X],ans);
	}
	


	if(ny > originy) {
		if(wx < originx) {
			//recursive call to Quad1
			ans = rect_search(ans,r1,node -> spcSon[0],(q_num*4)+1,originx-w,originy+w,width/2,trace,touch);
		}
		if(ex > originx) {
			//recursive call to Quad2
			ans = rect_search(ans,r1,node -> spcSon[1],(q_num*4)+2,originx+w,originy+w,width/2,trace,touch);
		}	
	}
	if(sy < originy) {
		if(wx < originx) {
			//recursive call to Quad3
			ans = rect_search(ans,r1,node -> spcSon[2],(q_num*4)+3,originx-w,originy-w,width/2,trace,touch);
		}
		if(ex > originx) {
			//recursive call to Quad4
			ans = rect_search(ans,r1,node -> spcSon[3],(q_num*4)+4,originx+w,originy-w,width/2,trace,touch);
		}
	}
	return ans;
}

char* vert_search(struct Rectangle *r1,struct bNode *node,char *ans,int *min) {
	struct Rectangle *r2;
	int cwx,cex,cny,csy,nwx,nex,nny,nsy;

	if(node == NULL) {
		return ans;
	}

	r2 = node -> Rect;

	//Rectangle r1 coordinates
	cwx = 0;
	cex = mxCifTree.size;
	cny = (r1 -> Center[Y]) + (r1 -> Length[Y]);
	csy = (r1 -> Center[Y]) - (r1 -> Length[Y]);
	//Rectangle r2 coordinates
	nwx = (r2 -> Center[X]) - (r2 -> Length[X]);
	nex = (r2 -> Center[X]) + (r2 -> Length[X]);
	nny = (r2 -> Center[Y]) + (r2 -> Length[Y]);
	nsy = (r2 -> Center[Y]) - (r2 -> Length[Y]);

	//Its ok if it touches rectangle 2
	//not ok if it intersects rectangle 2
	if(strcmp(r1 -> Name,r2 -> Name) != 0) {
		if(nwx < cex && nex > cwx && nny > csy && nsy < cny) {
			//blank because it intersects
		}
		else {
			//change here for y values
			if(abs(csy - nny) < *min) {
				*min = abs(csy - nny);
				ans = r2 -> Name;
			}
			else if(abs(nsy - cny) < *min) {
				*min = abs(nsy - cny);
				ans = r2 -> Name;
			}
		}
	}
	ans = vert_search(r1,node -> binSon[0],ans,min);  //left node traversal
	ans = vert_search(r1,node -> binSon[1],ans,min); //right node traversal
	return ans;
}

char* hor_search(struct Rectangle *r1,struct bNode *node,char *ans,int *min) {
	struct Rectangle *r2;
	int cwx,cex,cny,csy,nwx,nex,nny,nsy;

	if(node == NULL) {
		return ans;
	}

	r2 = node -> Rect;

	//Rectangle r1 coordinates
	cwx = (r1 -> Center[X]) - (r1 -> Length[X]);
	cex = (r1 -> Center[X]) + (r1 -> Length[X]);
	cny = mxCifTree.size;
	csy = 0;
	//Rectangle r2 coordinates
	nwx = (r2 -> Center[X]) - (r2 -> Length[X]);
	nex = (r2 -> Center[X]) + (r2 -> Length[X]);
	nny = (r2 -> Center[Y]) + (r2 -> Length[Y]);
	nsy = (r2 -> Center[Y]) - (r2 -> Length[Y]);

	//Its ok if it touches rectangle 2
	//not ok if it intersects rectangle 2
	if(strcmp(r1 -> Name,r2 -> Name) != 0) {
		if(nwx < cex && nex > cwx && nny > csy && nsy < cny) {
			//blank because it intersects
			//printf("reached here\n");
		}
		else {
			//printf("reached here\n");
			if(abs(cwx - nex) < *min) {
				//printf("reached here\n");
				*min = abs(cwx - nex);
				ans = r2 -> Name;
			}
			else if(abs(nwx - cex) < *min) {
				//printf("reached here\n");
				*min = abs(nwx - cex);
				ans = r2 -> Name;
			}
		}
	}
	ans = hor_search(r1,node -> binSon[0],ans,min);  //left node traversal
	ans = hor_search(r1,node -> binSon[1],ans,min); //right node traversal
	return ans;
}

//search vertical/horizantal neighbors
char* vh_search(char *ans,struct Rectangle *r1,struct cNode *node,int originx,
	int originy,int width,int vh,int*min) {
	int w;

	if(node == NULL) {
		return ans;
	}
	w = width/4;

	if(vh == 0) {
		ans = hor_search(r1,node -> binSon[Y],ans,min);
		ans = hor_search(r1,node -> binSon[X],ans,min);
	}
	
	if(vh == 1){
		ans = vert_search(r1,node -> binSon[Y],ans,min);
		ans = vert_search(r1,node -> binSon[X],ans,min);
	}
	

	ans = vh_search(ans,r1,node -> spcSon[0],originx-w,originy+w,width/2,vh,min);
	ans = vh_search(ans,r1,node -> spcSon[1],originx+w,originy+w,width/2,vh,min);
	ans = vh_search(ans,r1,node -> spcSon[2],originx-w,originy-w,width/2,vh,min);
	ans = vh_search(ans,r1,node -> spcSon[3],originx+w,originy-w,width/2,vh,min);
	return ans;
}

char* n_search(int cx,int cy,struct bNode *node,char *ans,double *min) {
	struct Rectangle *r2;
	int wx,ex,ny,sy,cornerx,cornery,temp1,temp2;
	double dist;
	if(node == NULL) {
		return ans;
	}

	r2 = node -> Rect;

	//Rectangle r2 coordinates
	wx = (r2 -> Center[X]) - (r2 -> Length[X]);
	ex = (r2 -> Center[X]) + (r2 -> Length[X]);
	ny = (r2 -> Center[Y]) + (r2 -> Length[Y]);
	sy = (r2 -> Center[Y]) - (r2 -> Length[Y]);

	//Its ok if it touches rectangle 2
	//not ok if it intersects rectangle 2
	if(wx <= cx && ex >= cx && ny >= cy && sy <= cy) {
		*min = 0;
		ans = r2 -> Name;
	}
	else if(sy < cy && ny > cy) {
		if(abs(cx-ex) > abs(cx-wx)) {
			*min = abs(cx-wx);
		}
		else {
			*min = abs(cx-ex);
		}
		ans = r2 -> Name;
	}
	else if(wx < cx && ex > cx) {
		if(abs(cy-sy) > abs(ny-cy)) {
			*min = abs(ny-cy);
		}
		else {
			*min = abs(cy-sy);
		}
		ans = r2 -> Name;
	}
	else {

		cornerx = mini(abs(ex-cx),abs(cx-wx));//distance to closest x corner
		cornery = mini(abs(cy-sy),abs(ny-cy));//distance to closest y corner
		dist = (pow(cornerx,2)) + (pow(cornery,2)); //pythagreon theorem
		dist = sqrt(dist);

		if(dist < *min) {
			*min = dist;
			ans = r2 -> Name;
		}
	}

	ans = n_search(cx,cy,node -> binSon[0],ans,min);  //left node traversal
	ans = n_search(cx,cy,node -> binSon[1],ans,min); //right node traversal
	return ans;
}

char* near_search(char *ans,int cx,int cy,struct cNode *node,int originx,
	int originy,int width,double *min) {
	int w;

	if(node == NULL) {
		return ans;
	}
	w = width/4;

	
	ans = n_search(cx,cy,node -> binSon[Y],ans,min);
	ans = n_search(cx,cy,node -> binSon[X],ans,min);
	

	ans = near_search(ans,cx,cy,node -> spcSon[0],originx-w,originy+w,width/2,min);
	ans = near_search(ans,cx,cy,node -> spcSon[1],originx+w,originy+w,width/2,min);
	ans = near_search(ans,cx,cy,node -> spcSon[2],originx-w,originy-w,width/2,min);
	ans = near_search(ans,cx,cy,node -> spcSon[3],originx+w,originy-w,width/2,min);
	return ans;
}



void insert_tree(struct bNode *node,struct Rectangle *r1,int xy) {
	//c = x/y-cordinate of current node. r = x/y-cordinate of r1
	int c,r;
	struct Rectangle *r2 = node -> Rect;
	if(r2 == NULL) {
		node -> Rect = r1;
		return;
	}
	/*
	if(strcmp(r2 -> Name,r1 -> Name) == 0) {
		return;
	}*/
	if(xy == 0) {
		//c and r will be the x-coordinates
		r = (r1 -> Center[X]) + (r1 -> Length[X]);
		c = (r2 -> Center[X]) + (r2 -> Length[X]);
	}
	else {
		//c and r will be the y-coordinates
		r = (r1 -> Center[Y]) + (r1 -> Length[Y]);
		c = (r2 -> Center[Y]) + (r2 -> Length[Y]);
	}

	if(c > r) {
		if(node -> binSon[0] == NULL) {
			node -> binSon[0] = (struct bNode*) malloc(sizeof(struct bNode));
			node -> binSon[0] -> Rect = NULL;
			node -> binSon[0] -> binSon[0] = NULL;
			node -> binSon[0] -> binSon[1] = NULL;
		}
		insert_tree(node -> binSon[0],r1,xy);
	}
	else {
		if(node -> binSon[1] == NULL) {
			node -> binSon[1] = (struct bNode*) malloc(sizeof(struct bNode));
			node -> binSon[1] -> Rect = NULL;
			node -> binSon[1] -> binSon[0] = NULL;
			node -> binSon[1] -> binSon[1] = NULL;
		}
		insert_tree(node -> binSon[1],r1,xy);
	}

}


void insert_hlp(struct Rectangle *r1,int originx,int originy,struct cNode *node,int width) {
	int wx,ex,ny,sy,w;

	w = width/4;

	wx = (r1 -> Center[X]) - (r1 -> Length[X]);
	ex = (r1 -> Center[X]) + (r1 -> Length[X]);
	ny = (r1 -> Center[Y]) + (r1 -> Length[Y]);
	sy = (r1 -> Center[Y]) - (r1 -> Length[Y]);


	if(ex >= originx && wx <= originx) {
		//make sure that node -> binSon is not NULL
		if(node -> binSon[Y] == NULL) {
			node -> binSon[Y] = (struct bNode*) malloc(sizeof(struct bNode));
			node -> binSon[Y] -> Rect = r1;
			node -> binSon[Y] -> binSon[0] = NULL;
			node -> binSon[Y] -> binSon[1] = NULL;
		}
		else {
			insert_tree(node -> binSon[Y],r1,Y);
		}
		//put in y-axis binary tree
	}
	else if(ny >= originy && sy <= originy) {
		if(node -> binSon[X] == NULL) {
			node -> binSon[X] = (struct bNode*) malloc(sizeof(struct bNode));
			node -> binSon[X] -> Rect = r1;
			node -> binSon[X] -> binSon[0] = NULL;
			node -> binSon[X] -> binSon[1] = NULL;
		}
		else {
			insert_tree(node -> binSon[X],r1,X);
		}
		//put in x-axis binary tree
	}
	else {
		//if it doesn't intersect a current line go to appropriate quadtree
		//if the quadtree is NULL, use malloc to create it
		if(ex < originx && sy > originy) {
			//QUAD 1
			if(node -> spcSon[NW] == NULL) {
				node -> spcSon[NW] = (struct cNode*) malloc(sizeof(struct cNode));
				node -> spcSon[NW] -> spcSon[NW] = NULL;
				node -> spcSon[NW] -> spcSon[NE] = NULL;
				node -> spcSon[NW] -> spcSon[SW] = NULL;
				node -> spcSon[NW] -> spcSon[SE] = NULL;
				node -> spcSon[NW] -> binSon[X] = NULL;
				node -> spcSon[NW] -> binSon[Y] = NULL;
			}
			insert_hlp(r1,originx-w,originy+w,node -> spcSon[NW],width/2);
		}
		else if(ex > originx && sy > originy) {
			//QUAD 2
			if(node -> spcSon[NE] == NULL) {
				node -> spcSon[NE] = (struct cNode*) malloc(sizeof(struct cNode));
				node -> spcSon[NE] -> spcSon[NW] = NULL;
				node -> spcSon[NE] -> spcSon[NE] = NULL;
				node -> spcSon[NE] -> spcSon[SW] = NULL;
				node -> spcSon[NE] -> spcSon[SE] = NULL;
				node -> spcSon[NE] -> binSon[X] = NULL;
				node -> spcSon[NE] -> binSon[Y] = NULL;
			}
			insert_hlp(r1,originx+w,originy+w,node -> spcSon[NE],width/2);
		}
		else if(ex < originx && sy < originy) {
			//QUAD 3
			if(node -> spcSon[SW] == NULL) {
				node -> spcSon[SW] = (struct cNode*) malloc(sizeof(struct cNode));
				node -> spcSon[SW] -> spcSon[NW] = NULL;
				node -> spcSon[SW] -> spcSon[NE] = NULL;
				node -> spcSon[SW] -> spcSon[SW] = NULL;
				node -> spcSon[SW] -> spcSon[SE] = NULL;
				node -> spcSon[SW] -> binSon[X] = NULL;
				node -> spcSon[SW] -> binSon[Y] = NULL;
			}
			insert_hlp(r1,originx-w,originy-w,node -> spcSon[SW],width/2);
		}
		else {
			//QUAD 4
			if(node -> spcSon[SE] == NULL) {
				node -> spcSon[SE] = (struct cNode*) malloc(sizeof(struct cNode));
				node -> spcSon[SE] -> spcSon[NW] = NULL;
				node -> spcSon[SE] -> spcSon[NE] = NULL;
				node -> spcSon[SE] -> spcSon[SW] = NULL;
				node -> spcSon[SE] -> spcSon[SE] = NULL;
				node -> spcSon[SE] -> binSon[X] = NULL;
				node -> spcSon[SE] -> binSon[Y] = NULL;
			}
			insert_hlp(r1,originx+w,originy-w,node -> spcSon[SE],width/2);
		}

	}
}

struct bNode* minValueNode(struct bNode* node)
{
    struct bNode* current = node;
 
    /* loop down to find the leftmost leaf */
    while (current && current->binSon[0] != NULL)
        current = current->binSon[0];
 
    return current;
}

//xy determines whether they are x or y coordinates
struct bNode* deleteNode(struct bNode* curr, struct Rectangle* r1,int xy){
	struct Rectangle *r2;
	int c,r;
    // base case
    if (curr == NULL)
        return curr;
 	r2 = curr -> Rect;

 	//r is the r1 coordinate and c is the r2 coordinate
 	if(xy == 0) {
 		//c and r are x coordinates
 		r = (r1 -> Center[X]) + (r1 -> Length[X]);
		c = (r2 -> Center[X]) + (r2 -> Length[X]);
 	}
 	else {
 		//c and r are y coordinates
 		r = (r1 -> Center[Y]) + (r1 -> Length[Y]);
		c = (r2 -> Center[Y]) + (r2 -> Length[Y]);
 	}
    //If the Rect to be deleted has a coordinate less than current node,
    //go to left subtree
    if (c > r)
        curr->binSon[0] = deleteNode(curr->binSon[0],r1,xy);
 
    // If the key to be deleted
    // is greater than the root's
    // key, then it lies in right subtree
    else if (c < r)
        curr->binSon[1] = deleteNode(curr->binSon[1],r1,xy);
 
    // if key is same as root's key,
    // then This is the node
    // to be deleted
    else {
        // node with only one child or no child
        if (curr->binSon[0] == NULL) {
            struct bNode* temp = curr->binSon[1];
            curr -> Rect = NULL;
            free(curr);
            return temp;
        }
        else if (curr->binSon[1] == NULL) {
            struct bNode* temp = curr->binSon[0];
            curr -> Rect = NULL;
            free(curr);
            return temp;
        }
 
        // node with two children:
        // Get the inorder successor
        // (smallest in the right subtree)
        struct bNode* temp = minValueNode(curr->binSon[1]);
 
        // Copy the inorder
        // successor's content to this node
        curr->Rect = temp->Rect;
 
        // Delete the inorder successor
        curr->binSon[1] = deleteNode(curr->binSon[1],r1,xy);
    }
    return curr;
}

int delete_free(struct cNode* node) {
	if(node -> binSon[X] == NULL && node -> binSon[Y] == NULL && node -> spcSon[NW] == NULL
			&& node -> spcSon[NE] == NULL && node -> spcSon[SW] == NULL 
			&& node -> spcSon[SE] == NULL) {
			node = NULL;
			free(node);
			return 1;
		}
		return 0;
}

//returns name of rectangle deleted
//need to delete quadtree nodes that are now empty as a result of deletion

char* delete_rect(struct Rectangle *r1,struct cNode* node,int originx,int originy,int width) {

	int wx,ex,ny,sy,w;
	char *a,*b,*c;

	if(node == NULL) {
		return NULL;
	}

	w = width/4;

	wx = (r1 -> Center[X]) - (r1 -> Length[X]);
	ex = (r1 -> Center[X]) + (r1 -> Length[X]);
	ny = (r1 -> Center[Y]) + (r1 -> Length[Y]);
	sy = (r1 -> Center[Y]) - (r1 -> Length[Y]);

	b = search_xtree(r1 -> Center[X],r1 -> Center[Y],width,node -> binSon[X]);

	if(b != NULL) {
		//delete rectangle in x-tree 
		node -> binSon[X] = deleteNode(node -> binSon[X],r1,0);
		//check if node NULL. Then check if y-tree root node is NULL. If they are, check if the
		//cNode has all quadrants as NULL. if it does, then we have to delete the sub-quadtree
		//delete_free(node);
		//printf("reached here - inside b != NULL\n");
		a = r1 -> Name;
		return a;
	}

	c = search_ytree(r1 -> Center[X],r1 -> Center[Y],width,node -> binSon[Y]);

	if(c != NULL) {
		//delete rectangle in y-tree
		node -> binSon[Y] = deleteNode(node -> binSon[Y],r1,1);
		//delete_free(node);
		//printf("reached here - inside c != NULL\n");
		a = r1 -> Name;
		return a;
	}
	//printf("reached here - after y check.   Quadwidth: %d\n",width);

	if(ny > originy) {
		if(wx < originx) {
			//recursive call to Quad1. change to delete_rect
			//printf("reached == NW at (%d,%d)\n",originx,originy);
			a = delete_rect(r1,node -> spcSon[NW],originx-w,originy+w,width/2);
			
			if(delete_free(node-> spcSon[NW]) == 1) {
				node-> spcSon[NW]= NULL;
			}
		}
		else if(ex > originx) {
			//recursive call to Quad2. change to delete_rect
			//printf("reached == NE at (%d,%d)\n",originx,originy);
			a = delete_rect(r1,node -> spcSon[NE],originx+w,originy+w,width/2);
			if(delete_free(node-> spcSon[NE]) == 1) {
				node-> spcSon[NE]= NULL;
			}
		}	
	}
	else if(sy < originy) {
		if(wx < originx) {
			//recursive call to Quad3. change to delete_rect
			//printf("reached == SW at (%d,%d)\n",originx,originy);
			a = delete_rect(r1,node -> spcSon[SW],originx-w,originy-w,width/2);
			if(delete_free(node-> spcSon[SW]) == 1) {
				node-> spcSon[SW]= NULL;
			}
		}
		else if(ex > originx) {
			//recursive call to Quad4. change to delete_rect
			a = delete_rect(r1,node -> spcSon[SE],originx+w,originy-w,width/2);
			if(delete_free(node-> spcSon[SE]) == 1) {
				node-> spcSon[SE]= NULL;
			}
		}
	}
	return a;
}


//return 0 when false,-1 when trace off, 1 when trace on
int dec(int trace) {
    //Use memory allocation
    char str[100],s3[10];
    char *s1,*s2,*s4,*s5;
    char *e;
    int index;
    int a,b,c,d,f,n,wx,ex,ny,sy;
    int *z;
    double *zz;
    struct Rectangle *r,*r1;


    if(fgets(str,100,stdin) == NULL) {
    	return 0; 
    }
    s1 = (char*) malloc(strlen(str));
    strcpy(s1,str);

    s4 = (char*) malloc(strlen(" "));
    strcpy(s4," ");

    r1 = (struct Rectangle*) malloc(sizeof(struct Rectangle));

    z = (int*) malloc(sizeof(int));
    zz = (double*) malloc(sizeof(double));

    e = strchr(str, '(');
    
    if(e == NULL){
    	if(strlen(str) > 1) {
    		if(strcmp(str,"TRACE ON\n") != 0 && strcmp(str,"TRACE OFF\n") != 0) {
    			free(s1);
        		return trace;
    		}	
    	}
        else {
        	free(s1);
        	return 0;
        }
    }
    else {
    	index = (int)(e - str);
    	s2 = s1 + index;
    }
    
    
    if(strncmp(s1,"INIT_QUADTREE",index) == 0) {
        n = sscanf(s2, "(%d)\n", &a);
        b = init_quad(a);
        printf("INIT_QUADTREE(%d): initialized a quadtree of width %d\n",a,b);
    }
    else if(strncmp(s1,"DISPLAY",index) == 0) {

        printf("2\n");
    }
    else if(strncmp(s1,"LIST_RECTANGLES",index) == 0) {
    	printf("LIST_RECTANGLES(): listing %d rectangles:\n",mxCifTree.count);
    	//list_rect(mxCifTree.mxCifRoot -> spcSon[2] -> binSon[0]);
    	list_rects(rectTree);
    }
    else if(strncmp(s1,"CREATE_RECTANGLE",index) == 0) {
    	n = sscanf(s2, "(%[^,],%d,%d,%d,%d)\n", s3,&a,&b,&c,&d);
    	create_rect(s3,a,b,c,d);
        printf("CREATE_RECTANGLE(%s,%d,%d,%d,%d): created rectangle %s\n",s3,a,b,c,d,s3);
    }

    else if(strncmp(s1,"SEARCH_POINT",index) == 0) {
    	n = sscanf(s2, "(%d,%d)\n", &a,&b);
        printf("SEARCH_POINT(%d,%d)",a,b);
        if(trace == 1) {
        	printf("[");
        }
        s5 = search_pt(0,mxCifTree.size/2,mxCifTree.size/2,a,b,mxCifTree.size,mxCifTree.mxCifRoot,trace);
        if(trace == 1) {
        	printf("]");
        }
        if(s5 == NULL) {
        	printf(": no rectangle found\n");
        }
        else {
        	printf(": found rectangle %s\n",s5);
        }
        
    }
    else if(strncmp(s1,"RECTANGLE_SEARCH",index) == 0) {
    	//free(s4);
        n = sscanf(s2, "(%[^)]\n", s3);
        printf("RECTANGLE_SEARCH(%s)",s3);
        if(trace == 1) {
        	printf("[");
        }
        b = mxCifTree.size;
        r = rtree_find(rectTree,s3);
        s4 = (char*) malloc(strlen(" "));
        //printf("-%s-\n",s4);
        s4 = rect_search(s4,r,mxCifTree.mxCifRoot,0,b/2,b/2,b,trace,0); //change trace var. later
        if(trace == 1) {
        	printf("]");
        }
        printf(": ");
        a = strlen(s4);
        if(a > 1) {
        	printf("%s intersects rectangles:%s\n",s3,s4);
        }
        else {
        	printf("%s does not intersect an existing rectangle\n",s3);
        }
        
    }
    else if(strncmp(s1,"INSERT",index) == 0) {
        n = sscanf(s2, "(%[^)]\n", s3);
        printf("INSERT(%s): ",s3);
        b = mxCifTree.size;
        r = rtree_find(rectTree,s3);
        r1 -> binSon[0] = NULL;
		r1 -> binSon[1] = NULL;
		strcpy(r1 -> Name,"temp");
		r1 -> Center[0] = (r -> Center[0]);
		r1 -> Center[1] = (r -> Center[1]);
		r1 -> Length[0] = (r -> Length[0]);
		r1 -> Length[1] = (r -> Length[1]);

        s4 = (char*) malloc(strlen(" "));
        s4 = rect_search(s4,r1,mxCifTree.mxCifRoot,0,b/2,b/2,b,-1,0); //change trace var. later
        a = strlen(s4);
        if(a > 1) {
        	printf("failed: intersects with %s\n",s4);
        }
        else {
        	/*
        	if(mxCifTree.mxCifRoot == NULL) {
        		mxCifTree.mxCifRoot = (struct cNode*) malloc(sizeof(struct cNode));
        	}*/
        	insert_hlp(r,b/2,b/2,mxCifTree.mxCifRoot,b);
        	printf("inserted rectangle %s\n",r -> Name);
        }   
    }
    else if(strcmp(s1,"TRACE ON\n") == 0) {
    	//printf("reached here\n");
    	trace = 1;
    }
    else if(strcmp(s1,"TRACE OFF\n") == 0) {
    	trace = -1;
    }
    else if(strncmp(s1,"DELETE_RECTANGLE",index) == 0) {
        n = sscanf(s2, "(%[^)]\n", s3);
        printf("DELETE_RECTANGLE(%s): ",s3);
        b = mxCifTree.size;
        r = rtree_find(rectTree,s3);
        s5 = delete_rect(r,mxCifTree.mxCifRoot,b/2,b/2,b);
        if(strcmp(s5,r->Name) == 0) {
        	printf("deleted rectangle %s\n",s5);
        }
        else {
        	printf("failed to delete rectangle %s",r->Name);
        }
        //printf("8A %s\n",s3);
    }
    else if(strncmp(s1,"DELETE_POINT",index) == 0) {
    	n = sscanf(s2, "(%d,%d)\n", &a,&b);
    	c = mxCifTree.size;
    	s5 = search_pt(0,c/2,c/2,a,b,c,mxCifTree.mxCifRoot,-1);
    	if(s5 == NULL) {
    		printf("DELETE_POINT(%d,%d): No rectangle found\n",a,b);
    	}
    	else {
    		r = rtree_find(rectTree,s5);
    		delete_rect(r,mxCifTree.mxCifRoot,c/2,c/2,c);
    		printf("DELETE_POINT(%d,%d): deleted rectangle %s\n",a,b,s5);
    	}
    	
    }
    else if(strncmp(s1,"MOVE",index) == 0) {
    	n = sscanf(s2, "(%[^,],%d,%d)\n", s3,&a,&b);
    	c = mxCifTree.size;
    	//a = change in x-center  b = change in y-center
    	r = rtree_find(rectTree,s3);
    	//create new rectangle that has new coordinates
    	
		r1 -> binSon[0] = NULL;
		r1 -> binSon[1] = NULL;
		strcpy(r1 -> Name,"temp");
		r1 -> Center[0] = (r -> Center[0]) + a;
		r1 -> Center[1] = (r -> Center[1]) + b;
		r1 -> Length[0] = (r -> Length[0]);
		r1 -> Length[1] = (r -> Length[1]);
		//use rectangle search to see if there are any rectangles that intersect
    	//besides the one we want to move
		s4 = rect_search(s4,r1,mxCifTree.mxCifRoot,0,c/2,c/2,c,-1,0);
		s4++;

		if(strlen(s4) < 1) {

			s5 = delete_rect(r,mxCifTree.mxCifRoot,c/2,c/2,c);
			r -> Center[0] = r1 -> Center[0];
			r -> Center[1] = r1 -> Center[1];
			//printf("before insert help\n");
			insert_hlp(r,c/2,c/2,mxCifTree.mxCifRoot,c);
			//printf("after insert help\n");
			printf("MOVE(%s,%d,%d): rectangle %s moved successfully\n",
				r -> Name,a,b,r -> Name);
		}
		else {
			printf("MOVE(%s,%d,%d): move failed\n",s3,a,b);
		}
		s4--;
		
    	
    	//delete rectangle we want to move, change its coordinates, then insert it
        //printf("9 %s %d %d\n",s3,a,b);
    }
    else if(strncmp(s1,"TOUCH",index) == 0) {
        n = sscanf(s2, "(%[^)]\n", s3);
        //printf("10 %s\n",s3);
        printf("TOUCH(%s)",s3);
        if(trace == 1) {
        	printf("[");
        }
        b = mxCifTree.size;
        r = rtree_find(rectTree,s3);
        s4 = (char*) malloc(strlen(" "));
        
        //printf("-%s-\n",s4);
        s4 = rect_search(s4,r,mxCifTree.mxCifRoot,0,b/2,b/2,b,trace,1); //change trace var. later
        if(trace == 1) {
        	printf("]");
        }
        printf(": ");
        a = strlen(s4);
        d = cnt_space(s4);
        if(a <= 1) {
        	printf("no rectangle found\n");
        	//printf("found %d rectangle:%s\n",isspace(s4),s4);
        }
        else if(d > 1) {
        	printf("found %d rectangles:%s\n",d,s4);
        }
        else {
        	printf("found 1 rectangle:%s\n",s4);
        }
    }
    else if(strncmp(s1,"WITHIN",index) == 0) {
    	n = sscanf(s2, "(%[^,],%d)\n", s3,&a);
    	printf("WITHIN(%s)",s3);
        if(trace == 1) {
        	printf("[");
        }
        b = mxCifTree.size;
        r = rtree_find(rectTree,s3);
        s4 = (char*) malloc(strlen(" "));
        r1 -> binSon[0] = NULL;
		r1 -> binSon[1] = NULL;
		strcpy(r1 -> Name,r -> Name);
		r1 -> Center[0] = (r -> Center[0]);
		r1 -> Center[1] = (r -> Center[1]);
		r1 -> Length[0] = (r -> Length[0]) + a;
		r1 -> Length[1] = (r -> Length[1]) + a;
		wx = (r1 -> Center[X]) - (r1 -> Length[X]);
		ex = (r1 -> Center[X]) + (r1 -> Length[X]);
        //printf("-%s-\n",s4);
        s4 = rect_search(s4,r1,mxCifTree.mxCifRoot,0,b/2,b/2,b,trace,2); //change trace var. later
        if(trace == 1) {
        	printf("]");
        }
        printf(": ");
        a = strlen(s4);
        d = cnt_space(s4);
        if(a <= 1) {
        	printf("no rectangle found\n");
        	//printf("found %d rectangle:%s\n",isspace(s4),s4);
        }
        else if(d > 1) {
        	printf("found %d rectangles:%s\n",d,s4);
        }
        else {
        	printf("found 1 rectangle:%s\n",s4);
        }
        //printf("11 %s %d\n",s3,a);
    }
    else if(strncmp(s1,"HORIZ_NEIGHBOR",index) == 0) {
        n = sscanf(s2, "(%[^)]\n", s3);
        printf("HORIZ_NEIGHBOR(%s)",s3);

        //a = 1;
        b = mxCifTree.size;
        r = rtree_find(rectTree,s3);
        *z = 32;
        s5 = vh_search(s5,r,mxCifTree.mxCifRoot,b/2,b/2,b,0,z);
        //printf(": %d\n",*z);
        if(s5 != NULL) {
        	printf(": found rectangle %s\n",s5);
        }
        else {
        	printf(": no rectangle found\n");
        }
        //
        //rect_search(s4,r1,mxCifTree.mxCifRoot,0,b/2,b/2,b,trace,2);
        
        //printf("12A %s\n",s3);
    }
    else if(strncmp(s1,"VERT_NEIGHBOR",index) == 0) {
        n = sscanf(s2, "(%[^)]\n", s3);
        printf("VERT_NEIGHBOR(%s)",s3);
        //first check for any touching rectangles
        //then check which rectangles intersect the extension
        //a = 1;
        b = mxCifTree.size;
        r = rtree_find(rectTree,s3);
        *z = 32;
        s5 = vh_search(s5,r,mxCifTree.mxCifRoot,b/2,b/2,b,1,z);
        //printf(": %d\n",*z);
        if(s5 != NULL) {
        	printf(": found rectangle %s\n",s5);
        }
        else {
        	printf(": no rectangle found\n");
        }
        
        //printf("12B %s\n",s3);
    }
    else if(strncmp(s1,"NEAREST_RECTANGLE",index) == 0) {
    	n = sscanf(s2, "(%d,%d)\n", &a,&b);
    	printf("NEAREST_RECTANGLE(%d,%d)",a,b);
    	d = mxCifTree.size;
        *zz = 32;
        s5 = near_search(s5,a,b,mxCifTree.mxCifRoot,d/2,d/2,d,zz);

        if(s5 != NULL) {
        	printf(": found rectangle %s\n",s5);
        }
        else {
        	printf(": no rectangle found\n");
        }
        //printf("13 %d %d\n",a,b);
    }
    else if(strncmp(s1,"WINDOW",index) == 0) {
    	n = sscanf(s2, "(%d,%d,%d,%d)\n", &a,&b,&c,&d);
    	printf("NEAREST_RECTANGLE(%d,%d,%d,%d)",a,b,c,d);
    	f = mxCifTree.size;
        r1 -> binSon[0] = NULL;
		r1 -> binSon[1] = NULL;
		strcpy(r1 -> Name,"temp");
		r1 -> Center[0] = a + (c/2);
		r1 -> Center[1] = b + (d/2);
		r1 -> Length[0] = c/2;
		r1 -> Length[1] = d/2;
        s4 = (char*) malloc(strlen(" "));

        s4 = rect_search(s4,r1,mxCifTree.mxCifRoot,0,f/2,f/2,f,0,5); 

        printf(": ");
        a = strlen(s4);
        d = cnt_space(s4);
        if(a <= 1) {
        	printf("no rectangle found\n");
        }
        else if(d > 1) {
        	printf("found %d rectangles:%s\n",d,s4);
        }
        else {
        	printf("found 1 rectangle:%s\n",s4);
        }

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

    free(r1);
    free(s1);
    free(s4);
    free(z);
    return trace;
}

int main() {
	int x = 1;

    while(x!= 0) {
        //0 represents an error or exit
        x = dec(x);
    }
	return 0;
}

