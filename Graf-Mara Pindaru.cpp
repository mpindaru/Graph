#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <stack>
#define MAXTREES 64
#define MAXN 105
#define INFINITY 0x3f3f3f3f
using namespace std;

ifstream fin("graph.in");
ofstream fout("graph.out");

template <class T> class FibonacciHeap;
template <class T> struct FibonacciNode
{
private:
    FibonacciNode<T> *previousNode;
    FibonacciNode<T> *nextNode;
    FibonacciNode<T> *child;
    FibonacciNode<T> *parent;
    T value;
    int numberChildren;
    bool marked;
public:
    friend class FibonacciHeap<T>;
    FibonacciNode<T> *getPreviousNode()
    {
        return previousNode;
    }
    FibonacciNode<T> *getNextNode()
    {
        return nextNode;
    }
    FibonacciNode<T> *getChild()
    {
        return child;
    }
    FibonacciNode<T> *getParent()
    {
        return parent;
    }
    T getValue()
    {
        return value;
    }
    bool isMarked()
    {
        return marked;
    }
    bool hasChildren()
    {
        return child;
    }
    bool hasParent()
    {
        return parent;
    }
};

template <class T> class FibonacciHeap
{
protected:
    FibonacciNode <T> *heap;
public:
    FibonacciHeap()
    {
        heap=_initializeEmptyNode();
    }
    ~FibonacciHeap()
    {
        if(heap)
            _deleteAllNodes(heap);
    }
    FibonacciNode<T> *insertNode(T value)
    {
        FibonacciNode<T> *newNode=_initializeNewNode(value);
        heap=_mergeHeaps(heap, newNode);
        return newNode;
    }
    void mergeHeaps(FibonacciHeap &other)
    {
        heap=_mergeHeaps(heap, other.heap);
        other.heap=_initializeEmptyNode();
    }
    bool isEmpty()
    {
        return (heap==NULL);
    }
    T getMinimum()
    {
        return heap->value;
    }
    T removeMinimum()
    {
        FibonacciNode<T> *oldHeap=heap;
        heap=_removeMinimum(heap);
        T minimum=oldHeap->value;
        delete oldHeap;
        return minimum;
    }
    void decreaseValue(FibonacciNode<T> *node, T newValue)
    {
        heap=_decreaseValue(heap, node, newValue);
    }
    FibonacciNode<T> *findNodeWithValue(T value)
    {
        return _findNodeWithValue(heap,value);
    }
    void deleteNode(T value)
    {
        FibonacciNode<T> *nodeToDelete=findNodeWithValue(value);
        if(nodeToDelete!=NULL)
        {
            decreaseValue(nodeToDelete,getMinimum()-1);
            removeMinimum();
        }
    }
private:
    FibonacciNode<T> *_initializeEmptyNode()
    {
        return NULL;
    }
    FibonacciNode<T> *_initializeNewNode(T newValue)
    {
        FibonacciNode<T> *newNode=new FibonacciNode<T>;
        newNode->child=NULL;
        newNode->parent=NULL;
        newNode->value=newValue;
        newNode->numberChildren=0;
        newNode->marked=false;
        newNode->nextNode=newNode->previousNode=newNode;
        return newNode;
    }
    void _deleteAllNodes(FibonacciNode<T> *node)
    {
        if(node==NULL)
            return;
        FibonacciNode<T> *currentNode=node;
        do
        {
            FibonacciNode<T> *auxiliaryCurrentNode=currentNode;
            currentNode=currentNode->nextNode;
            _deleteAllNodes(auxiliaryCurrentNode->child);
            delete auxiliaryCurrentNode;
        }
        while(currentNode!=node);
    }
    FibonacciNode<T> *_mergeHeaps(FibonacciNode<T> *firstNode, FibonacciNode<T> *secondNode)
    {
        if(firstNode==NULL)
            return secondNode;
        if(secondNode==NULL)
            return firstNode;
        if(firstNode->value>secondNode->value)
            swap(firstNode, secondNode);
        FibonacciNode<T> *firstNodeNext=firstNode->nextNode;
        FibonacciNode<T> *secondNodePrevious=secondNode->previousNode;
        firstNode->nextNode=secondNode;
        secondNode->previousNode=firstNode;
        firstNodeNext->previousNode=secondNodePrevious;
        secondNodePrevious->nextNode=firstNodeNext;
        return firstNode;
    }
    void _addChild(FibonacciNode<T> *parent, FibonacciNode<T> *child)
    {
        child->nextNode=child->previousNode=child;
        child->parent=parent;
        ++parent->numberChildren;
        parent->child=_mergeHeaps(parent->child,child);
    }
    void _unMarkUnParentAllNodes(FibonacciNode<T> *node)
    {
        if(node==NULL)
            return;
        FibonacciNode<T> *currentNode=node;
        do
        {
            currentNode->marked=false;
            currentNode->parent=NULL;
            currentNode=currentNode->nextNode;
        }
        while(currentNode!=node);
    }
    FibonacciNode<T> *_removeMinimum(FibonacciNode<T> *node)
    {
        _unMarkUnParentAllNodes(node->child);
        if(node->nextNode==node)
            node=node->child;
        else
        {
            node->nextNode->previousNode=node->previousNode;
            node->previousNode->nextNode=node->nextNode;
            node=_mergeHeaps(node->nextNode,node->child);
        }
        if(node==NULL)
            return node;
        FibonacciNode<T> *trees[MAXTREES]= {NULL}; //trees[i] retine un pointer catre un copil al radacinii eliminate anterior
        //care are numarul de copii egal cu i
        while(true)
        {
            if(trees[node->numberChildren]!=NULL)
            {
                FibonacciNode<T> *treeSameDegree=trees[node->numberChildren];
                if(treeSameDegree==node)
                    break;
                trees[node->numberChildren]=NULL;
                if(node->value<treeSameDegree->value) //node devine parintele lui treeSameDegree
                {
                    treeSameDegree->previousNode->nextNode=treeSameDegree->nextNode;
                    treeSameDegree->nextNode->previousNode=treeSameDegree->previousNode;
                    _addChild(node,treeSameDegree);
                }
                else //node devine copilul lui treeSameDegree
                {
                    treeSameDegree->previousNode->nextNode=treeSameDegree->nextNode;
                    treeSameDegree->nextNode->previousNode=treeSameDegree->previousNode;
                    if(node->nextNode==node)
                    {
                        treeSameDegree->previousNode=treeSameDegree->nextNode=treeSameDegree;
                        _addChild(treeSameDegree,node);
                        node=treeSameDegree;
                    }
                    else
                    {
                        node->previousNode->nextNode=treeSameDegree;
                        node->nextNode->previousNode=treeSameDegree;
                        treeSameDegree->nextNode=node->nextNode;
                        treeSameDegree->previousNode=node->previousNode;
                        _addChild(treeSameDegree,node);
                        node=treeSameDegree;
                    }
                }
                continue;
            }
            else
                trees[node->numberChildren]=node;
            node=node->nextNode;
        }
        FibonacciNode<T> *newMinimum=node, *currentNode=node;
        do
        {
            if(currentNode->value<newMinimum->value)
                newMinimum=currentNode;
            currentNode=currentNode->nextNode;
        }
        while(currentNode!=node);
        return newMinimum;
    }
    FibonacciNode<T> *_cutFromHeap(FibonacciNode<T> *heap, FibonacciNode<T> *node)
    {
        if(node->nextNode==node)
            node->parent->child=NULL;
        else
        {
            node->nextNode->previousNode=node->previousNode;
            node->previousNode->nextNode=node->nextNode;
            node->parent->child=node->nextNode;
        }
        node->nextNode=node->previousNode=node;
        node->marked=false;
        return _mergeHeaps(heap,node);
    }
    FibonacciNode<T> *_decreaseValue(FibonacciNode<T> *heap, FibonacciNode<T> *node, T value)
    {
        if(node->value<value)
            return heap;
        node->value=value;
        if(node->value<node->parent->value)
        {
            heap=_cutFromHeap(heap,node);
            FibonacciNode<T> *parent=node->parent;
            node->parent=NULL;
            while(parent!=NULL && parent->marked)
            {
                heap=_cutFromHeap(heap,parent);
                node=parent;
                parent=node->parent;
                node->parent=NULL;
            }
            if(parent!=NULL && parent->parent!=NULL)
                parent->marked=true;
        }
        return heap;
    }
    FibonacciNode<T> *_findNodeWithValue(FibonacciNode<T> *heap, T value)
    {
        if(heap==NULL)
            return NULL;
        FibonacciNode<T> *currentNode=heap;
        do
        {
            if(currentNode->value==value)
                return currentNode;
            FibonacciNode<T> *foundInChildren=_findNodeWithValue(currentNode->child, value);
            if(foundInChildren)
                return foundInChildren;
            currentNode=currentNode->nextNode;
        }
        while(currentNode!=heap);
        return NULL;
    }
};

struct Node
{
    int neighbouringNode;
    double cost;
    bool operator < (const Node &other) const
    {
        return (cost<other.cost);
    }
    bool operator > (const Node &other) const
    {
        return (cost>other.cost);
    }
    Node operator - (const int number)
    {
        return {neighbouringNode,cost-number};
    }
    bool operator == (const Node &other) const
    {
        return (cost==other.cost);
    }
};

struct edge
{
    int firstNode, secondNode;
    double cost;
};
class Graph
{
private:
    int numberNodes, numberEdges;
    vector<vector<Node> > AdjacencyList;
    vector<vector<int> > transposeAdjacencyList;
    vector<vector<int> > stronglyConnectedComponents;
    int numberStronglyConnectedComponents;
    bool wasVisited[MAXN]= {false};
    stack<int> S;
public:
    Graph();
    Graph(int newNumberNodes, int newNumberEdges);
    void addEdge(int startNode, int secondNode, double cost);
    void printGraph();
    void depthFirstSearch(int startNode);
    void transposeDepthFirstSearch(int startNode, int numberStronglyConnectedComponent);
    void Kosaraju();
    void printStronglyConnectedComponents();
    vector<double> Dijkstra(int startNode);
};
Graph::Graph()
{
    numberEdges=numberNodes=0;
    vector<Node> AdjacencyListRow;
    vector<int> transposeAdjacencyListRow;
    for(int i=0; i<MAXN; ++i)
        AdjacencyList.push_back(AdjacencyListRow), transposeAdjacencyList.push_back(transposeAdjacencyListRow);
}
Graph::Graph(int newNumberNodes, int newNumberEdges)
{
    numberNodes=newNumberNodes, numberEdges=newNumberEdges;
    vector<Node> AdjacencyListRow;
    vector<int> transposeAdjacencyListRow;
    for(int i=0; i<=numberNodes; ++i)
        AdjacencyList.push_back(AdjacencyListRow), transposeAdjacencyList.push_back(transposeAdjacencyListRow);
}
void Graph::printGraph()
{
    cout<<numberNodes<<" "<<numberEdges<<"\n";
    for(int FirstNode=1; FirstNode<=numberNodes; FirstNode++)
        for(auto &Neighbour:AdjacencyList[FirstNode])
            cout<<FirstNode<<" "<<Neighbour.neighbouringNode<<" "<<Neighbour.cost<<"\n";
}
void Graph::addEdge(int startNode, int secondNode, double cost)
{
    for(auto &Neighbour:AdjacencyList[startNode])
        if(Neighbour.neighbouringNode==secondNode)
            return;
    AdjacencyList[startNode].push_back({secondNode,cost});
    transposeAdjacencyList[secondNode].push_back(startNode);
}
vector<double> Graph::Dijkstra(int startNode)
{
    vector<double> minimumDistance;
    minimumDistance.push_back(0);
    for(int i=1; i<=numberNodes; i++)
        minimumDistance.push_back(INFINITY);
    minimumDistance[startNode]=0;
    FibonacciHeap<Node> F;
    F.insertNode({startNode,0});
    while(!F.isEmpty())
    {
        int minimumNode=F.getMinimum().neighbouringNode;
        F.removeMinimum();
        for(auto x:AdjacencyList[minimumNode])
        {
            double possibleCost=minimumDistance[minimumNode]+x.cost;
            if(possibleCost<minimumDistance[x.neighbouringNode])
            {
                if(minimumDistance[x.neighbouringNode]!=INFINITY)
                    F.deleteNode({x.neighbouringNode, minimumDistance[x.neighbouringNode]});
                minimumDistance[x.neighbouringNode]=possibleCost;
                F.insertNode({x.neighbouringNode,minimumDistance[x.neighbouringNode]});
            }
        }
    }
    return minimumDistance;
}
void Graph::depthFirstSearch(int startNode)
{
    wasVisited[startNode]=true;
    for(auto &x:AdjacencyList[startNode])
        if(!wasVisited[x.neighbouringNode])
            depthFirstSearch(x.neighbouringNode);
    S.push(startNode);
}
void Graph::transposeDepthFirstSearch(int startNode, int numberStronglyConnectedComponent)
{
    wasVisited[startNode]=true;
    stronglyConnectedComponents[numberStronglyConnectedComponent].push_back(startNode);
    for(auto &x:transposeAdjacencyList[startNode])
        if(!wasVisited[x])
            transposeDepthFirstSearch(x, numberStronglyConnectedComponent);
}
void Graph::Kosaraju()
{
    numberStronglyConnectedComponents=0;
    vector<int> row;
    for(int i=0; i<=numberNodes; ++i)
        stronglyConnectedComponents.push_back(row);
    for(int i=1; i<=numberNodes; ++i)
        if(!wasVisited[i])
            depthFirstSearch(i);
    for(int i=1; i<=numberNodes; ++i)
        wasVisited[i]=false;
    while(!S.empty())
    {
        int currentNode=S.top();
        S.pop();
        if(!wasVisited[currentNode])
        {
            transposeDepthFirstSearch(currentNode,numberStronglyConnectedComponents);
            numberStronglyConnectedComponents++;
        }
    }
}
void Graph::printStronglyConnectedComponents()
{
    cout<<numberStronglyConnectedComponents<<"\n";
    for(int i=0; i<numberStronglyConnectedComponents; ++i)
    {
        sort(stronglyConnectedComponents[i].begin(),stronglyConnectedComponents[i].end());
        for(auto &x:stronglyConnectedComponents[i])
            cout<<x<<" ";
        cout<<"\n";
    }
}
void testClass()
{
    int numberNodes, numberEdges;
    fin>>numberNodes>>numberEdges;
    Graph G(numberNodes, numberEdges);
    int firstNode, secondNode;
    double cost;
    for(int i=0; i<numberEdges; ++i)
        fin>>firstNode>>secondNode>>cost, G.addEdge(firstNode,secondNode,cost);
    G.printGraph();
    int startNode;
    fin>>startNode;
    vector<double> minimumDistance=G.Dijkstra(startNode);
    for(auto it=minimumDistance.begin()+1; it!=minimumDistance.end(); ++it)
        cout<<*it<<" ";
    cout<<"\n";
    G.Kosaraju();
    G.printStronglyConnectedComponents();
}
int main()
{
    testClass();
    return 0;
}
