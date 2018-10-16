#include "bptree.h"
#include <vector>
#include <sys/time.h>
#include <random>
#include <algorithm>
using namespace std;

#define dump(x) cerr<<#x<<"="<<x<<endl
void
print_tree_core(NODE *n)
{
	printf("[");
	for (int i = 0; i < n->nkey; i++) {
		if (!n->isLeaf) print_tree_core(n->chi[i]);
		printf("%d", n->key[i]);
		if (i != n->nkey-1 && n->isLeaf) putchar(' ');
	}
	if (!n->isLeaf) print_tree_core(n->chi[n->nkey]);
	printf("]");
}

void
print_tree(NODE *node)
{
	print_tree_core(node);
	printf("\n"); fflush(stdout);
}


//insertの時のようのノード生成
TEMP *
create_new_node(){
	TEMP *node;
	if (!(node = (TEMP *)calloc(1, sizeof(TEMP)))) ERR;
	node->isLeaf=true;
	node->nkey=0;
	return node;
}

//個人的なnew node
NODE *
new_bptree_node()
{
    NODE *new_node;

		if(!(new_node=(NODE *)calloc(1,sizeof(NODE))))ERR;

    new_node->parent = NULL;
    /*new_node->next     = NULL;*/
    new_node->isLeaf = false;
    new_node->nkey = 0;


    return new_node;
}

TEMP *
new_bptree_temp()
{
    TEMP *new_node;

		if(!(new_node=(TEMP *)calloc(1,sizeof(TEMP))))ERR;
    /*new_node->next     = NULL;*/
    new_node->isLeaf = false;
    new_node->nkey = 0;


    return new_node;
}

NODE *
alloc_leaf_dash(NODE *parent){
	NODE *node;
	if (!(node = (NODE *)calloc(1, sizeof(NODE)))) ERR;
	node->isLeaf = true;
	node->parent = parent;
	node->nkey = 0;

	return node;

}

NODE *
alloc_leaf(NODE *parent)
{
	NODE *node;
	if (!(node = (NODE *)calloc(1, sizeof(NODE)))) ERR;
	node->isLeaf = true;
	node->parent = parent;
	node->nkey = 0;

	return node;
}

NODE *
alloc_internal(NODE *parent)
{
	NODE *node;
	if (!(node = (NODE *)calloc(1, sizeof(NODE)))) ERR;
	node->isLeaf = false;
	node->parent = parent;
	node->nkey = 0;

	return node;
}

NODE *
alloc_root(NODE *left, int rs_key, NODE *right)
{
	NODE *node;

	if (!(node = (NODE *)calloc(1, sizeof(NODE)))) ERR;
	node->parent = NULL;
	node->isLeaf = false;
	node->key[0] = rs_key;
	node->chi[0] = left;
	node->chi[1] = right;
	node->nkey = 1;

	left->parent=node;
	right->parent=node;

	//分割ノードを繋げるポインタ
	left->chi[N-1]=right;

	return node;
}

NODE *
find_leaf(NODE *node, int key)
{
	int kid;

	if (node->isLeaf) return node;
	for (kid = 0; kid < node->nkey; kid++) {
		if (key < node->key[kid]) break;
	}

	return find_leaf(node->chi[kid], key);
}

NODE *
insert_in_node(NODE *p,NODE *c,int key,int index){
	int i;
	for(i=p->nkey;i>index;--i){
		p->key[i]=p->key[i-1];
		p->chi[i+1]=p->chi[i];
	}
	p->key[index]=key;

	p->chi[index+1]=c;
	p->nkey+=1;
	c->parent=p;

	cout<<"insert_in_node"<<endl;
	for(int i=0;i<p->nkey;i++){
		cout<<p->key[i]<<endl;
	}

	return p;
}
TEMP *
insert_in_node_temp(TEMP *p,NODE *c,int key,int index){
	int i;
	for(i=p->nkey;i>index;--i){
		p->key[i]=p->key[i-1];
		p->chi[i+1]=p->chi[i];
	}
	p->key[index]=key;

	p->chi[index+1]=c;
	p->nkey+=1;
	c->parent=(NODE*)p;

	return p;
}

NODE *
insert_in_parent(NODE *node, int key, NODE *node_dash, DATA *data){
	if (node==Root){
		return Root=alloc_root(node,key,node_dash);
	}


	cout<<"node key"<<endl;
	for(int i=0;i<node->nkey;i++){
		cout<<node->key[i]<<endl;
	}
	// cout<<"node_dash key"<<endl;
	// for(int i=0;i<node_dash->nkey;i++){
	// 	cout<<node_dash->key[i]<<endl;
	// }

	NODE *parent=node->parent;

	// cout<<"node parent key"<<endl;
	// for(int i=0;i<parent->nkey;i++){
	// 	cout<<parent->key[i]<<endl;
	// }

	int index=0,i;

	while(parent->key[index]<key && index<parent->nkey)index++;

	if(parent->nkey<N-1){
		cout<<"parent->nkey<N-1 の"<<endl;
		for(int i=0;i<parent->nkey;i++){
			cout<<parent->key[i]<<endl;
		}
		insert_in_node(parent,node_dash,key,index);
		return Root;
	}else{
		//親がいっぱいなので再帰処理で親ブロックを追加
		dump(parent->nkey);
		assert(parent->nkey==N-1);


		//copy p to t
		TEMP *t=new_bptree_temp();
		for(i=0;i<N-1;i++){
			t->key[i]=parent->key[i];
			t->chi[i]=parent->chi[i];
			t->nkey++;
		}
		assert(parent->nkey==t->nkey);
		t->chi[i]=parent->chi[i];


		//insert
		insert_in_node_temp(t,node_dash,key,index);
		assert((parent->nkey+1==t->nkey));

		int mid_pos=(N+1)/2;

		//erase
		for(i=0;i<N;i++){
			parent->key[i]=0;
			parent->chi[i]=NULL;
		}
		parent->chi[i]=NULL;
		parent->nkey=0;

		//create p'
		NODE *pp = new_bptree_node();

		//copy t into p  left
		for(i=0;i<mid_pos;i++){
			parent->key[i]=t->key[i];
			parent->chi[i]=t->chi[i];
			print_tree(t->chi[i]);
			t->chi[i]->parent=parent;


			dump(t->key[i]);

			parent->nkey++;
		}
		assert(parent->nkey==mid_pos);
		parent->chi[i]=t->chi[i];
		t->chi[i]->parent=parent;

		print_tree(t->chi[i]);



		//let k'' = t[mid_pos]
		int key_temp=t->key[mid_pos];
		cout<<"key_tempの値"<<endl;
		cout<<key_temp<<endl;
		//copy t into p' right
		for(i=mid_pos+1;i<t->nkey;i++){
			pp->chi[i-(mid_pos+1)]=t->chi[i];
			pp->key[i-(mid_pos+1)]=t->key[i];
			t->chi[i]->parent=pp;

			print_tree(t->chi[i]);
			dump(t->key[i]);
			// dump(t->key[i]);
			pp->nkey++;
		}
		assert((pp->nkey+mid_pos+1)==t->nkey);
		assert(i==4);
		pp->chi[i-(mid_pos+1)]=t->chi[i];
		print_tree(t->chi[i]);
		t->chi[i]->parent=pp;



		cout<<"parent key"<<endl;
		for(int i=0;i<parent->nkey;i++){
			cout<<parent->key[i]<<endl;
		}

		cout<<"pp key"<<endl;
		for(int i=0;i<pp->nkey;i++){
			cout<<pp->key[i]<<endl;
		}

		//親のポインタを指定

		// cerr<<"key_tempとkeyのひかく "<<key_temp<<" "<<key<<endl;
		// if(key_temp<key){
		// 	node->parent=pp;
		// 	node_dash->parent=pp;
		// }else{
		// 	node->parent=parent;
		// 	node_dash->parent=parent;
		// }

		cout<<"parentのポインタ"<<endl;
		for(int i=0;i<parent->nkey;i++){
			cout<<parent->key[i]<<endl;
		}
		cout<<"ppのポインタ"<<endl;
		for(int i=0;i<pp->nkey;i++){
			cout<<pp->key[i]<<endl;
		}

		return insert_in_parent(parent,key_temp,pp,data);

	}
}
//temp用の要素挿入関数
TEMP *
insert_in_leaf_temp(TEMP *leaf,int key,DATA *data){
	int i;

	// std::cout<<"key="<<key<<endl;

	if(key<leaf->key[0]){
		for(int i=leaf->nkey;i>0;i--){
			leaf->chi[i]=leaf->chi[i-1];
			leaf->key[i]=leaf->key[i-1];
		}
		leaf->key[0]=key;
		leaf->chi[0]=(NODE *)data;
	}
	else{
		for(i = leaf->nkey-1;i>=0;--i){
			if(key > (int)leaf->key[i]){
				// leaf->key[i+2]=leaf->key[i];
				// leaf->chi[i+2]=leaf->chi[i];
				break;
			}else{
				leaf->key[i+1]=leaf->key[i];
				leaf->chi[i+1]=leaf->chi[i];
			}
		}

		leaf->key[i+1]=key;
		leaf->chi[i+1]=(NODE * )data;
	}
	leaf->nkey++;

	return leaf;

}

NODE *
insert_in_leaf(NODE *leaf, int key, DATA *data)
{
	int i;
	if (key < leaf->key[0]) {
		for (i = leaf->nkey; i > 0; i--) {
			leaf->chi[i] = leaf->chi[i-1] ;
			leaf->key[i] = leaf->key[i-1] ;
		}
		leaf->key[0] = key;
		leaf->chi[0] = (NODE *)data;
	}
	else {
		// Quiz
    // Step 2. Insert the new key

		for(i = leaf->nkey-1;i>=0;--i){
			if(key > (int)leaf->key[i]){
				break;
			}else{
				leaf->key[i+1]=leaf->key[i];
				leaf->chi[i+1]=leaf->chi[i];
			}
		}

		leaf->key[i+1]=key;
		leaf->chi[i+1]=(NODE * )data;


	}
  // Step 1. Increment the number of keys
	leaf->nkey++;

	return leaf;
}


void
insert(int key, DATA *data)
{
	NODE *leaf;

	if (Root == NULL) {
		// cout<<"呼び出し"<<endl;
		leaf = alloc_leaf(NULL);
		Root = leaf;
	}
	else {
		leaf = find_leaf(Root, key);
	}

	if (leaf->nkey < (N-1)) {
		insert_in_leaf(leaf, key, data);
	}
	else {
    // Split (quiz at 10/09)
		TEMP *Temp=create_new_node();
		// std::cout<<"tempのキー数"<<Temp->nkey<<endl;
		for(int i=0;i<N-1;i++){
			Temp->key[i]=leaf->key[i];
			Temp->chi[i]=leaf->chi[i];
			Temp->nkey++;
		}
		//std::cout<<"key="<<key<<endl;
		//insert
		insert_in_leaf_temp(Temp,key,data);


		NODE *leaf_dash=alloc_leaf_dash(NULL);

		leaf_dash->chi[N-1]=leaf->chi[N-1];

		leaf->chi[N-1]=leaf_dash;

		for(int i=0;i<N-1;i++){
			leaf->chi[i]=NULL;
			leaf->key[i]=0;
		}
		leaf->nkey=0;

		int mid_pos=(N-1)/2;
		for(int i=0;i<=mid_pos;i++){
			leaf->chi[i]=Temp->chi[i];
			leaf->key[i]=Temp->key[i];
			leaf->nkey++;
		}
		for(int i=mid_pos+1;i<N;i++){
			leaf_dash->key[i-(mid_pos+1)]=Temp->key[i];
			leaf_dash->chi[i-(mid_pos+1)]=Temp->chi[i];
			leaf_dash->nkey++;
		}

		int key_dash=leaf_dash->key[0];

		//dleaf->parent =



		insert_in_parent(leaf,key_dash,leaf_dash,data);


	}
}

void
init_root(void)
{
	Root = NULL;
}

void
search_core(const int key)
{
  NODE *n = find_leaf(Root, key);
	for (int i = 0; i < n->nkey+1; i++) {
		if (n->key[i] == key) return;
	}
  cout << "Key not found: " << key << endl;
	ERR;
}

int
interactive()
{
  int key;

  std::cout << "Key: ";
  std::cin >> key;

  return key;
}

int
main(int argc, char *argv[])
{
	init_root();

  // while (true) {
	// 	insert(interactive(), NULL);
  //   print_tree(Root);
  // }

	for(int i=1;i<13;i++){
		insert(i,NULL);
		print_tree(Root);

	}

	print_tree(Root);
	init_root();

	for(int i=12;i>0;i--){
		insert(i,NULL);
		print_tree(Root);

	}
	print_tree(Root);
	init_root();

	vector<int> vec;
	for(int i=1;i<13;i++)vec.push_back(i);
	mt19937_64 get_rand_mt;
	shuffle(vec.begin(),vec.end(),get_rand_mt);

	for(auto itr:vec){
		cout<<"挿入キー";
		 dump(itr);
		insert(itr,NULL);
		print_tree(Root);
	}
	print_tree(Root);



	return 0;
}
