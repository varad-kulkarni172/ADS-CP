#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

typedef struct AVLNode
{
    char itemId[50];
    char itemName[50];
    char currentBid[50];
    int height;
    struct AVLNode *left;
    struct AVLNode *right;
} AVLNode;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void AddControls(HWND hwnd);
void AddItem(HWND hwnd);
void PlaceBid(HWND hwnd);
void DisplayAuctionDetails(HWND hwnd);
void ExitApplication(HWND hwnd);
void SaveToFile();
void insert(AVLNode **root, char itemId[], char itemName[], char currentBid[]);
AVLNode *newNode(char itemId[], char itemName[], char currentBid[]);
int height(AVLNode *node);
int balanceFactor(AVLNode *node);
AVLNode *rightRotate(AVLNode *y);
AVLNode *leftRotate(AVLNode *x);
void preorderTraversal(AVLNode *root);

HWND hwndMain;
HWND hwndItemId, hwndItemName, hwndCurrentBid;
HWND hwndNewBid;

AVLNode *root = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "MyWindowClass";

    if (!RegisterClass(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error", MB_ICONERROR | MB_OK);
        return 0;
    }

    hwndMain = CreateWindow("MyWindowClass", "Auction Bidding Application", WS_OVERLAPPEDWINDOW, 100, 100, 500, 400, NULL, NULL, hInstance, NULL);

    if (hwndMain == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error", MB_ICONERROR | MB_OK);
        return 0;
    }

    ShowWindow(hwndMain, nCmdShow);
    UpdateWindow(hwndMain);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        AddControls(hwnd);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case 1:
            AddItem(hwnd);
            break;
        case 2:
            PlaceBid(hwnd);
            break;
        case 3:
            DisplayAuctionDetails(hwnd);
            break;
        case 4:
            ExitApplication(hwnd);
            break;
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void AddControls(HWND hwnd)
{

    CreateWindow("STATIC", "BIDDING HUB: AVL-BASED AUCTION ITEM MANAGEMENT\n", WS_VISIBLE | WS_CHILD | SS_CENTER, 0, 0, 500, 40, hwnd, NULL, NULL, NULL);

    CreateWindow("STATIC", "Item ID:", WS_VISIBLE | WS_CHILD, 20, 100, 80, 20, hwnd, NULL, NULL, NULL);
    hwndItemId = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 110, 100, 150, 20, hwnd, NULL, NULL, NULL);

    CreateWindow("STATIC", "Item Name:", WS_VISIBLE | WS_CHILD, 20, 130, 80, 20, hwnd, NULL, NULL, NULL);
    hwndItemName = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 110, 130, 150, 20, hwnd, NULL, NULL, NULL);

    CreateWindow("STATIC", "Base Price:", WS_VISIBLE | WS_CHILD, 20, 160, 100, 20, hwnd, NULL, NULL, NULL);
    hwndCurrentBid = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 110, 160, 150, 20, hwnd, NULL, NULL, NULL);

    CreateWindow("STATIC", "New Bid:", WS_VISIBLE | WS_CHILD, 20, 190, 80, 20, hwnd, NULL, NULL, NULL);
    hwndNewBid = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 110, 190, 150, 20, hwnd, NULL, NULL, NULL);

    CreateWindow("BUTTON", "Add Item", WS_VISIBLE | WS_CHILD, 20, 230, 100, 30, hwnd, (HMENU)1, NULL, NULL);

    CreateWindow("BUTTON", "Place Bid", WS_VISIBLE | WS_CHILD, 140, 230, 100, 30, hwnd, (HMENU)2, NULL, NULL);
}

AVLNode *newNode(char itemId[], char itemName[], char currentBid[])
{
    AVLNode *node = (AVLNode *)malloc(sizeof(AVLNode));
    strcpy(node->itemId, itemId);
    strcpy(node->itemName, itemName);
    strcpy(node->currentBid, currentBid);
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

int height(AVLNode *node)
{
    if (node == NULL)
        return 0;
    return node->height;
}

int balanceFactor(AVLNode *node)
{
    if (node == NULL)
        return 0;
    return height(node->left) - height(node->right);
}

AVLNode *rightRotate(AVLNode *y)
{
    AVLNode *x = y->left;
    AVLNode *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = 1 + (height(y->left) > height(y->right) ? height(y->left) : height(y->right));
    x->height = 1 + (height(x->left) > height(x->right) ? height(x->left) : height(x->right));

    return x;
}

AVLNode *leftRotate(AVLNode *x)
{
    AVLNode *y = x->right;
    AVLNode *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = 1 + (height(x->left) > height(x->right) ? height(x->left) : height(x->right));
    y->height = 1 + (height(y->left) > height(y->right) ? height(y->left) : height(y->right));

    return y;
}

void insert(AVLNode **root, char itemId[], char itemName[], char currentBid[])
{
    if (*root == NULL)
    {
        *root = newNode(itemId, itemName, currentBid);
        return;
    }

    int compare = strcmp(itemId, (*root)->itemId);
    if (compare == 0)
    {
        MessageBox(NULL, "Item with the same ID already exists!", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    if (compare < 0)
        insert(&(*root)->left, itemId, itemName, currentBid);
    else
        insert(&(*root)->right, itemId, itemName, currentBid);

    (*root)->height = 1 + (height((*root)->left) > height((*root)->right) ? height((*root)->left) : height((*root)->right));

    int balance = balanceFactor(*root);

    if (balance > 1 && strcmp(itemId, (*root)->left->itemId) < 0)
        *root = rightRotate(*root);

    if (balance < -1 && strcmp(itemId, (*root)->right->itemId) > 0)
        *root = leftRotate(*root);

    if (balance > 1 && strcmp(itemId, (*root)->left->itemId) > 0)
    {
        (*root)->left = leftRotate((*root)->left);
        *root = rightRotate(*root);
    }

    if (balance < -1 && strcmp(itemId, (*root)->right->itemId) < 0)
    {
        (*root)->right = rightRotate((*root)->right);
        *root = leftRotate(*root);
    }
}

AVLNode *search(AVLNode *root, char itemId[])
{
    if (root == NULL || strcmp(root->itemId, itemId) == 0)
        return root;

    if (strcmp(root->itemId, itemId) < 0)
        return search(root->right, itemId);
    else
        return search(root->left, itemId);
}

int serialNumber = 1;

void AddItem(HWND hwnd)
{

    char itemId[50], itemName[50], currentBid[50];
    GetWindowText(hwndItemId, itemId, sizeof(itemId));
    GetWindowText(hwndItemName, itemName, sizeof(itemName));
    GetWindowText(hwndCurrentBid, currentBid, sizeof(currentBid));

    AVLNode *existingItem = search(root, itemId);
    if (existingItem != NULL)
    {
        MessageBox(hwnd, "Item with the same ID already exists!", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    insert(&root, itemId, itemName, currentBid);

    FILE *file = fopen("auction_data.txt", "a");
    if (file != NULL)
    {
        fprintf(file, "==================================BIDDING HUB: AVL-BASED AUCTION ITEM MANAGEMENT========================================");
        fprintf(file, "Serial Number: %d\nItem Id: %s\nItem Name: %s\nBase Price: %s\n\n", serialNumber, itemId, itemName, currentBid);
        fclose(file);
        MessageBox(hwnd, "Item added successfully!", "Success", MB_OK | MB_ICONINFORMATION);
    }
    else
    {
        MessageBox(hwnd, "Error opening file for writing.", "Error", MB_OK | MB_ICONERROR);
    }

    serialNumber++;
}

void updateBid(AVLNode *root, char itemId[], char currentBid[], char newBid[])
{
    if (root == NULL)
        return;

    int compare = strcmp(itemId, root->itemId);
    if (compare == 0)
    {

        int currentBidValue = atoi(root->currentBid);
        int newBidValue = atoi(newBid);

        if (newBidValue > currentBidValue)
        {

            sprintf(root->currentBid, "%d", newBidValue);

            MessageBox(NULL, "Bid placed successfully!", "Success", MB_OK | MB_ICONINFORMATION);
        }
        else
        {

            MessageBox(NULL, "New bid must be greater than current bid.", "Error", MB_OK | MB_ICONERROR);
        }
    }
    else if (compare < 0)
    {
        updateBid(root->left, itemId, currentBid, newBid);
    }
    else
    {
        updateBid(root->right, itemId, currentBid, newBid);
    }
}

void PlaceBid(HWND hwnd)
{

    char itemId[50], currentBid[50], newBid[50];
    GetWindowText(hwndItemId, itemId, sizeof(itemId));
    GetWindowText(hwndCurrentBid, currentBid, sizeof(currentBid));
    GetWindowText(hwndNewBid, newBid, sizeof(newBid));

    int currentBidValue = atoi(currentBid);
    int newBidValue = atoi(newBid);

    if (newBidValue > currentBidValue)
    {

        updateBid(root, itemId, currentBid, newBid);

        SetWindowText(hwndCurrentBid, newBid);

        FILE *file = fopen("auction_data.txt", "a");
        if (file != NULL)
        {

            fprintf(file, "Item Id: %s\nCurrent Price: %s\nUpdated Price: %s\n\n", itemId, currentBid, newBid);
            fclose(file);
            MessageBox(hwnd, "Bid placed successfully!", "Success", MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            MessageBox(hwnd, "Error opening file for writing.", "Error", MB_OK | MB_ICONERROR);
        }
    }
    else
    {

        MessageBox(hwnd, "New bid must be greater than current bid.", "Error", MB_OK | MB_ICONERROR);
    }
}

void DisplayAuctionDetails(HWND hwnd)
{

    MessageBox(hwnd, "Display Auction Details functionality will be implemented here.", "Display Auction Details", MB_OK | MB_ICONINFORMATION);
}

void ExitApplication(HWND hwnd)
{

    MessageBox(hwnd, "Exiting Application...", "Exit", MB_OK | MB_ICONINFORMATION);
    PostQuitMessage(0);
}