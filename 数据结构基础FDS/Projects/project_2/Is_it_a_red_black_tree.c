#include<stdio.h>
#include<stdlib.h>

typedef struct TreeNode * Tree;
struct TreeNode
{
	int data;  //data is the element of tree node.
	Tree Left, Right;  //the two children of tree node.
	int color;  //the color of tree node.
};

Tree CreateTree (int a[], int n);
int checkchild (Tree p);
int countleftcolor (Tree p);
int countrightcolor (Tree p);
int checkcount (Tree p);

int main()
{
	int m;
	scanf("%d", &m);
	printf("total test cases are %d\n", m);  //show the total test cases. Note: test cases should be less than 30.
	int i, j;
	int input[m][30];  //contain every elements.
	int number[m];
	for(i=0; i<m; i++){
		scanf("%d", &number[i]);
		printf("%d numbers in this case\n", number[i]);  //show the amount of numbers in every test case.
		for(j=0; j<number[i]; j++) scanf("%d", &input[i][j]);
		//scanf("\n");
	}
	
	for(i=0; i<m; i++){
		Tree p = CreateTree (input[i], number[i]);
		if(p -> color == 0) printf("No\n");
		else if(checkchild (p) == 1 && checkcount (p) == 1) printf("Yes\n");  
		else printf("No\n");  //output the result.
	}
	
	return 0;
}

Tree CreateTree (int a[], int n)  //create a binary search tree for a specific test case.
{
	Tree Root;
	Root = (Tree)malloc(sizeof(struct TreeNode));  
    if(a[0] < 0){
    	Root -> color = 0;  //color is 0 means node is red.
    	a[0] = -a[0];  //if node is red, we should ensure the element is positive.
    }else Root -> color = 1;  //color is 1 means node is black.
    Root -> data = a[0];  //load the element.
	Root -> Left = Root -> Right = NULL;

	
	int i;
	Tree temp;  //temp will be the pointer of father node of the new node.
	for(i = 1; i < n; i++){
		Tree newone;
		newone = (Tree)malloc(sizeof(struct TreeNode));
		if(a[i] < 0){
			newone -> color = 0;
			a[i] = -a[i];
		}else newone -> color = 1;
		newone -> data = a[i];
		newone -> Left = newone -> Right = NULL;  //construct a new node.
		
	    temp = Root;
		while(1){
			if(newone -> data < temp -> data && temp -> Left == NULL ){
				temp -> Left = newone;  //if the left subtree is empty, the new node will be the left subtree.
				break;
			}else if(newone -> data > temp -> data && temp -> Right == NULL){
				temp -> Right = newone;  //if the right subtree is empty, the new node will be the right subtree.
				break;
			}else if(newone -> data < temp -> data && temp -> Left != NULL) 
			    temp = temp -> Left;  //if the left subtree is not empty, the temp pointer will move to the left subtree.
			 else if(newone -> data > temp -> data && temp -> Right != NULL) 
			    temp = temp -> Right;  //if the right subtree is not empty, the temp pointer will move to the right subtree.
			
		}
	}
	
	return Root;  //return the root pointer.
}

int checkchild (Tree p){
	

	if(p -> Left == NULL && p -> Right != NULL){
		if(p -> color == 0 && p -> Right -> color == 0) return 0;  //if a node is red and its right child is red, return 0.
		else return checkchild (p -> Right);  //recursively check right subtree.
	}
	if(p -> Right == NULL && p -> Left != NULL){  
		if(p -> color == 0 && p -> Left -> color == 0) return 0;  //if a node is red and its left child is red, return 0.
		else return checkchild (p -> Left);  //recursively check left subtree.
	}
	
	if(p -> Left == NULL && p -> Right == NULL) return 1;  //because NULL node is black, so leaf node will return 1.
	
	if(p -> color == 0 && (p -> Left -> color == 0 || p -> Right -> color == 0) && p -> Left != NULL && p -> Right != NULL) return 0;
	else if(checkchild (p -> Left) == 0) return 0;
	else return checkchild (p -> Right);
	
	
}

int countleftcolor (Tree p){
	int lcount = 0;
	if(p -> Left == NULL) lcount = p -> color + 1;  //count the black node from leaf node to the external node.
	else lcount = countleftcolor (p -> Left) + p -> color;  //recursively count left path.
	return lcount;
}

int countrightcolor (Tree p){
	int rcount = 0;
	if(p -> Right == NULL) rcount = p -> color + 1;  //count the black node from leaf node to the external node.
	else rcount = countrightcolor (p -> Right) + p -> color;  //recursively count right path.
	return rcount;
}

int checkcount (Tree p){
	if(countleftcolor (p) != countrightcolor (p)) return 0;  //if the black node between left path is not equal to right path, return 0.
	
	if(p -> Left == NULL && p -> Right == NULL) return 1;  //if the node is leaf node, return 1.
	else if(p -> Left != NULL && p -> Right == NULL) return checkcount (p -> Left);  //recursively check left subtree.
	else if(p -> Left == NULL && p -> Right != NULL) return checkcount (p -> Right);  //recursively check right subtree.
	else if(checkcount (p -> Left) == 0) return 0;
	else return checkcount (p -> Right);
}
