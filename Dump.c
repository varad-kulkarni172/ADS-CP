#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure definitions
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
    float previousBid;
};

// Function prototypes
struct AVLNode *createNode(struct AuctionItem *item);
void freeAVLTree(struct AVLNode *node);
int height(struct AVLNode *node);
int max(int a, int b);
struct AVLNode *rightRotate(struct AVLNode *y);
struct AVLNode *leftRotate(struct AVLNode *x);
int getBalance(struct AVLNode *node);
struct AVLNode *insertNode(struct AVLNode *node, struct AuctionItem *item);
struct AVLNode *minValueNode(struct AVLNode *node);
struct AVLNode *deleteNode(struct AVLNode *root, int key);
struct AuctionItem *searchItem(struct AVLNode *root, int itemId);
void displayTree(struct AVLNode *root);
void displayMenu();
void saveToFile(struct AVLNode *root, const char *filename);
struct AVLNode *loadFromFile(const char *filename);

// Main function
int main()
{
    struct AVLNode *root = NULL;
    int choice;
    const char *filename = "auction_data.txt";

    // Load data from file
    root = loadFromFile(filename);

    do
    {
        displayMenu();
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }

        switch (choice)
        {
        case 1:
        {
            struct AuctionItem *newItem = (struct AuctionItem *)malloc(sizeof(struct AuctionItem));
            if (newItem == NULL)
            {
                printf("Memory allocation failed.\n");
                exit(EXIT_FAILURE);
            }
            printf("Enter Item ID: ");
            if (scanf("%d", &newItem->itemId) != 1)
            {
                printf("Invalid input. Please enter a number.\n");
                free(newItem);
                while (getchar() != '\n'); // Clear input buffer
                continue;
            }
            printf("Enter Item Name: ");
            getchar(); // Consume newline character left in input buffer
            fgets(newItem->itemName, sizeof(newItem->itemName), stdin);
            newItem->itemName[strcspn(newItem->itemName, "\n")] = '\0'; // Remove newline character if present
            printf("Enter Current Bid: ");
            if (scanf("%f", &newItem->currentBid) != 1)
            {
                printf("Invalid input. Please enter a number.\n");
                free(newItem);
                while (getchar() != '\n'); // Clear input buffer
                continue;
            }

            // Initialize previous bid to 0
            newItem->previousBid = 0.0;

            root = insertNode(root, newItem);
            printf("Item added successfully!\n");
            break;
        }
        case 2:
        {
            int itemId;
            float newBid;
            printf("Enter Item ID to place bid: ");
            if (scanf("%d", &itemId) != 1)
            {
                printf("Invalid input. Please enter a number.\n");
                while (getchar() != '\n'); // Clear input buffer
                continue;
            }
            printf("Enter New Bid: ");
            if (scanf("%f", &newBid) != 1)
            {
                printf("Invalid input. Please enter a number.\n");
                while (getchar() != '\n'); // Clear input buffer
                continue;
            }

            struct AuctionItem *item = searchItem(root, itemId);
            if (item != NULL)
            {
                if (newBid > item->currentBid)
                {
                    item->previousBid = item->currentBid; // Save previous bid
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

    // Save data to file before exiting
    saveToFile(root, filename);

    // Free memory
    freeAVLTree(root);

    return 0;
}

// Function implementations

struct AVLNode *createNode(struct AuctionItem *item)
{
    struct AVLNode *newNode = (struct AVLNode *)malloc(sizeof(struct AVLNode));
    if (newNode == NULL)
    {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
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
        printf("Item ID: %d, Item Name: %s, Current Bid: %.2f, Previous Bid: %.2f\n", root->item->itemId, root->item->itemName, root->item->currentBid, root->item->previousBid);
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

void saveToFile(struct AVLNode *root, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Error opening file for writing.\n");
        return;
    }
    saveNodesToFile(root, file); // Recursive function to save nodes to file
    fclose(file);
}

void saveNodesToFile(struct AVLNode *node, FILE *file)
{
    if (node == NULL)
        return;

    // Save current node's data to file
    fprintf(file, "%d,%s,%.2f,%.2f\n", node->item->itemId, node->item->itemName, node->item->currentBid, node->item->previousBid);

    // Recursively save left and right subtrees
    saveNodesToFile(node->left, file);
    saveNodesToFile(node->right, file);
}

struct AVLNode *loadFromFile(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("File not found or unable to open.\n");
        return NULL;
    }

    struct AVLNode *root = NULL;
    int itemId;
    char itemName[50];
    float currentBid, previousBid;

    // Read data from file and insert into AVL tree
    while (fscanf(file, "%d,%[^,],%f,%f\n", &itemId, itemName, &currentBid, &previousBid) == 4)
    {
        struct AuctionItem *newItem = (struct AuctionItem *)malloc(sizeof(struct AuctionItem));
        if (newItem == NULL)
        {
            printf("Memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }
        newItem->itemId = itemId;
        strcpy(newItem->itemName, itemName);
        newItem->currentBid = currentBid;
        newItem->previousBid = previousBid;

        root = insertNode(root, newItem);
    }

    fclose(file);
    return root;
}
