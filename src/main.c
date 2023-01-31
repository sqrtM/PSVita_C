#include <vitasdk.h>
#include "include/vitasdk.h"
#include <stdio.h>
#include <stdlib.h>

#include "../common/debugScreen.h"

#define printf psvDebugScreenPrintf

struct node
{
	int key;
	struct node *left, *right;
};

struct node *newNode(int item)
{
	struct node *temp = (struct node *)malloc(sizeof(struct node));
	temp->key = item;
	temp->left = temp->right = NULL;
	return temp;
}

void inorder(struct node *root)
{
	if (root != NULL)
	{
		// Traverse left
		inorder(root->left);

		// Traverse root
		printf("%d -> ", root->key);

		// Traverse right
		inorder(root->right);
	}
}

struct node *insert(struct node *node, int key)
{
	// Return a new node if the tree is empty
	if (node == NULL)
		return newNode(key);

	// Traverse to the right place and insert the node
	if (key < node->key)
		node->left = insert(node->left, key);
	else
		node->right = insert(node->right, key);

	return node;
}

struct node *minValueNode(struct node *node)
{
	struct node *current = node;

	// Find the leftmost leaf
	while (current && current->left != NULL)
		current = current->left;

	return current;
}

struct node *deleteNode(struct node *root, int key)
{
	// Return if the tree is empty
	if (root == NULL)
		return root;

	// Find the node to be deleted
	if (key < root->key)
		root->left = deleteNode(root->left, key);
	else if (key > root->key)
		root->right = deleteNode(root->right, key);

	else
	{
		if (root->left == NULL)
		{
			struct node *temp = root->right;
			free(root);
			return temp;
		}
		else if (root->right == NULL)
		{
			struct node *temp = root->left;
			free(root);
			return temp;
		}

		// If the node has two children
		struct node *temp = minValueNode(root->right);

		// Place the inorder successor in position of the node to be deleted
		root->key = temp->key;

		// Delete the inorder successor
		root->right = deleteNode(root->right, temp->key);
	}
	return root;
}
int main(int argc, char *argv[])
{

	psvDebugScreenInit();
	psvDebugScreenSetBgColor(25135);

	struct node *root = NULL;
	root = insert(root, 8);
	root = insert(root, 3);
	root = insert(root, 4550);
	root = insert(root, 6);
	root = insert(root, 7);
	root = insert(root, 10);
	root = insert(root, 99595);
	root = insert(root, 4);

	printf("Inorder traversal: ");
	inorder(root);

	sceKernelDelayThread(3 * 1000000); // Wait for 3 seconds
	sceKernelExitProcess(0);
	return 0;
}
