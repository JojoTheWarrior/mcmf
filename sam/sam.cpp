// sam.cpp
typedef long long ll;

struct state {
    int len, link;
    map<char, int> next;
};

const int MAXLEN = 20'000;
state st[MAXLEN*2];
int sz, last;

void sa_init(){
    st[0].len = 0;
    st[0].link = -1;
    sz++;
    last = 0;
}

void sa_extend(char c){
    int cur = sz++;
    st[cur].len = st[last].len + 1;
    int p = last;
    while (p != -1 && !st[p].next.count(c)){
        st[p].next[c] = cur;
        p = st[p].link;
    }
    if (p == -1){
        st[cur].link = 0;
    } else {
        int q = st[p].next[c];
        if (st[p].len + 1 == st[q].len){
            st[cur].link = q;
        } else {
            int clone = sz++;
            st[clone].len = st[p].len + 1;
            st[clone].next = st[q].next;
            st[clone].link = st[q].link;
            while (p != -1 && st[p].next[c] == q){
                st[p].next[c] = clone;
                p = st[p].link;
            }
            st[q].link = st[cur].link = clone;
        }
    }
    last = cur;
}

// get # of different substrings
ll get_diff_substrings(){
    ll tot = 0;
    for (int i = 1; i < sz; i++){
        tot += st[i].len - st[st[i].link].len;
    }
    return tot;
}

/*
lexicographically k-th substring
Given a string S, find K-th lexicographically smallest substring.
The lexicographically k-th substring corresponds to the k-th path in the suffix automaton.
Therefore after counting the number of paths from each state:

d[v] = # of paths starting from state v
d[v] = 1 + sum over c of d[v.next[c]]

we can easily search for the k-th path from the root.

smallest cyclic shift, construct SAM for S+S, find lexicographically least path of length(S) (greedily take characters).

number of occurences
given pattern P, # times P appears as substring of T
construct SAM for T
for each state, if it was not created via cloning (and is not the initial state), initialize with cnt = 1.
then go through states in decreasing order of len, and add cnt[v] to the suffix link's:
cnt[link(v)] += cnt[v]

first occurence position
in sa_extend, when making new state cur set firstpos[cur] = len[cur] - 1
when cloning, set firstpos[clone] = firstpos[q]

all occurence positions
store a list of all suffix references leading to it. answer is all firstpos for all states we can find on a DFS/BFS starting from state t
*/

// longest common substring
string lcs (string S, string T) {
    sa_init();
    for (int i = 0; i < S.size(); i++)
        sa_extend(S[i]);

    int v = 0, l = 0, best = 0, bestpos = 0;
    for (int i = 0; i < T.size(); i++) {
        while (v && !st[v].next.count(T[i])) {
            v = st[v].link ;
            l = st[v].len;
        }
        if (st[v].next.count(T[i])) {
            v = st [v].next[T[i]];
            l++;
        }
        if (l > best) {
            best = l;
            bestpos = i;
        }
    }
    return T.substr(bestpos - best + 1, best);
} 

/*
largest common substring of multiple strings
T = S1 + D1 + S2 + D2 + ... + Sk + Dk
if a substring is in Si, there's a way to go from that substring's state to a containing Di but none of the others.
let reach[v][i] = "can you reach Di from state v without touching any others"
process in decreasing length of states.
*/