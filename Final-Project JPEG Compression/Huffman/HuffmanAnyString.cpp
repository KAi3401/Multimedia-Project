#include <bits/stdc++.h>
using namespace std;

struct Node {
    char id;
    int freq;
    Node *left, *right;
    Node():id('\0'), freq(0), left(NULL), right(NULL){};
    Node(char d, int f):id(d), freq(f), left(NULL), right(NULL){};
};

struct cmp {
    bool operator()(Node* l, Node* r) {
        return (l->freq > r->freq);
    }
};

void print(map<char, string> &ans, Node* root, string str = "") {
    if (!root) return;
    if (root->id != '\0') ans[root->id] = str;
    print(ans, root->left, str + "0");
    print(ans, root->right, str + "1");
}

void HuffmanCodes(string s) {
    map<char, int> freq;
    for(char c : s) freq[c]++;

    priority_queue<Node*, vector<Node*>, cmp> pq;
    for(auto pair : freq) {
        pq.push(new Node(pair.first, pair.second));
    }

    while (pq.size() != 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        Node* top = new Node('\0', left->freq + right->freq);
        top->left = left;
        top->right = right;
        pq.push(top);
    }

    map<char, string> ans;
    print(ans, pq.top());

    for(auto pair : ans) {
        cout << pair.first << ": " << pair.second << endl;
    }
}

int main() {
    string s; getline(cin, s);
    HuffmanCodes(s);
    return 0;
}

