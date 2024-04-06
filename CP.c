#include <stdio.h>
#include <stdlib.h>

struct AVLNode
{
    int key;
    int height;
    struct AuctionItem *item;
    struct AVLNode *left;
    struct AVLNode *right;
};

struct AuctionItem
{
    int itemId;
    char itemName[50];
    float currentBid;
};

struct AVLNode *createNode(struct AuctionItem *item)
{
    struct AVLNode *newNode = (struct AVLNode *)malloc(sizeof(struct AVLNode));
    newNode->key = item->itemId;
    newNode->height = 1;
    newNode->item = item;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

void freeAVLTree(struct AVLNode *node)
{
    if (node == NULL)
        return;

    freeAVLTree(node->left);
    freeAVLTree(node->right);
    free(node->item);
    free(node);
}

int height(struct AVLNode *node)
{
    if (node == NULL)
        return 0;
    return node->height;
}

int max(int a, int b)
{
    return (a > b) ? a : b;
}

struct AVLNode *rightRotate(struct AVLNode *y)
{
    struct AVLNode *x = y->left;
    struct AVLNode *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}

struct AVLNode *leftRotate(struct AVLNode *x)
{
    struct AVLNode *y = x->right;
    struct AVLNode *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}

int getBalance(struct AVLNode *node)
{
    if (node == NULL)
        return 0;
    return height(node->left) - height(node->right);
}

struct AVLNode *insertNode(struct AVLNode *node, struct AuctionItem *item)
{
    if (node == NULL)
        return createNode(item);

    if (item->itemId < node->key)
        node->left = insertNode(node->left, item);
    else if (item->itemId > node->key)
        node->right = insertNode(node->right, item);
    else
        return node;

    node->height = 1 + max(height(node->left), height(node->right));

    int balance = getBalance(node);

    if (balance > 1 && item->itemId < node->left->key)
        return rightRotate(node);

    if (balance < -1 && item->itemId > node->right->key)
        return leftRotate(node);

    if (balance > 1 && item->itemId > node->left->key)
    {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    if (balance < -1 && item->itemId < node->right->key)
    {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

struct AVLNode *minValueNode(struct AVLNode *node)
{
    struct AVLNode *current = node;
    while (current->left != NULL)
        current = current->left;
    return current;
}

struct AVLNode *deleteNode(struct AVLNode *root, int key)
{
    if (root == NULL)
        return root;

    if (key < root->key)
        root->left = deleteNode(root->left, key);
    else if (key > root->key)
        root->right = deleteNode(root->right, key);
    else
    {
        if ((root->left == NULL) || (root->right == NULL))
        {
            struct AVLNode *temp = root->left ? root->left : root->right;
            if (temp == NULL)
            {
                temp = root;
                root = NULL;
            }
            else
                *root = *temp;
            free(temp->item);
            free(temp);
        }
        else
        {
            struct AVLNode *temp = minValueNode(root->right);
            root->key = temp->key;
            root->right = deleteNode(root->right, temp->key);
        }
    }

    if (root == NULL)
        return root;

    root->height = 1 + max(height(root->left), height(root->right));

    int balance = getBalance(root);

    if (balance > 1 && getBalance(root->left) >= 0)
        return rightRotate(root);

    if (balance > 1 && getBalance(root->left) < 0)
    {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    if (balance < -1 && getBalance(root->right) <= 0)
        return leftRotate(root);

    if (balance < -1 && getBalance(root->right) > 0)
    {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

struct AuctionItem *searchItem(struct AVLNode *root, int itemId)
{
    if (root == NULL || root->key == itemId)
        return (root == NULL) ? NULL : root->item;

    if (root->key < itemId)
        return searchItem(root->right, itemId);

    return searchItem(root->left, itemId);
}

void displayTree(struct AVLNode *root)
{
    if (root != NULL)
    {
        displayTree(root->left);
        printf("Item ID: %d, Item Name: %s, Current Bid: %.2f\n", root->item->itemId, root->item->itemName, root->item->currentBid);
        displayTree(root->right);
    }
}

void displayMenu()
{
    printf("\nMenu:\n");
    printf("1. Add Item for Auction\n");
    printf("2. Place Bid\n");
    printf("3. Display Auction Details\n");
    printf("4. Exit\n");
}

int main()
{
    struct AVLNode *root = NULL;
    int choice;
    do
    {
        displayMenu();
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
        {
            struct AuctionItem *newItem = (struct AuctionItem *)malloc(sizeof(struct AuctionItem));
            printf("Enter Item ID: ");
            scanf("%d", &newItem->itemId);
            printf("Enter Item Name: ");
            scanf("%s", newItem->itemName);
            printf("Enter Current Bid: ");
            scanf("%f", &newItem->currentBid);

            root = insertNode(root, newItem);
            printf("Item added successfully!\n");
            break;
        }
        case 2:
        {
            int itemId;
            float newBid;
            printf("Enter Item ID to place bid: ");
            scanf("%d", &itemId);
            printf("Enter New Bid: ");
            scanf("%f", &newBid);

            struct AuctionItem *item = searchItem(root, itemId);
            if (item != NULL)
            {
                if (newBid > item->currentBid)
                {
                    item->currentBid = newBid;
                    printf("Bid placed successfully!\n");
                }
                else
                {
                    printf("New bid must be higher than the current bid.\n");
                }
            }
            else
            {
                printf("Item not found.\n");
            }
            break;
        }
        case 3:
        {
            printf("Auction Details:\n");
            displayTree(root);
            break;
        }
        case 4:
            printf("Exiting...\n");
            break;
        default:
            printf("Invalid choice! Please enter a valid option.\n");
        }
    } while (choice != 4);

    freeAVLTree(root);

    return 0;
}