#include <bits/stdc++.h>
using namespace std;
struct Node {
    int id;
    int freq;
    Node *left, *right;
    Node():id(-1), freq(0), right(NULL), left(NULL){};
    Node(int d, int f):id(d), freq(f), right(NULL), left(NULL){};
};

struct cmp{
    bool operator()(Node* l, Node* r){
        return (l->freq > r->freq);
    }
};

void print(string ans[], Node* root, string str = ""){
    if (!root)    return;
    if (root->id != -1)    ans[root->id] = str;
    print(ans, root->left, str + "0");
    print(ans, root->right, str + "1");
}

void HuffmanCodes(string ans[], int n){
    priority_queue<Node*, vector<Node*>, cmp> pq;
    for(int i = 0, f; i < n; i++){
        cin >> f;
        pq.push(new Node(i, f));
    }
    while (pq.size() != 1){
        Node* l = pq.top();    pq.pop();
        Node* r = pq.top();    pq.pop();
        Node* top = new Node(-1, l->freq + r->freq);
        top->left = l;
        top->right = r;
        pq.push(top);
    }
    print(ans, pq.top());
}
int main(){
    int n;    cin >> n;
    string ans[105] = {};
    HuffmanCodes(ans, n);
    for(int i = 0; i < n; i++){
    	cout << ans[i];
    	if(i!=n-1) cout<<" ";
	}         
    return 0;
}

