#ifndef TREE_H
#define TREE_H

#include "../defines.h"
#include "array.h"
#include "stack.h"

#ifdef FYPE
#include "../core/token.h"
#endif

#define TREE_PRINT_INDENT 3

#define tree_get_root(t) t->p_treenode_root
#define tree_set_root(t,tn) t->p_treenode_root = tn
#define treenode_get_num_childs(tn) array_get_size(tn->p_array_childs)
#define treenode_get_tnt(tn) tn->tnt
#define treenode_get_val(tn) tn->p_val
#define treenode_get_val2(tn) tn->p_val2
#define treenode_set_tnt(tn,t) tn->tnt = t
#define treenode_set_val(tn,v) tn->p_val = v
#define treenode_set_val2(tn,v) tn->p_val2 = v
#define treenode_insert_child treenode_insert_right
#define treenode_get_childs(tn) tn->p_array_childs
#define treenode_get_child(tn,i) array_get(tn->p_array_childs,i)
#define treenode_get_first_child(tn) array_get_first(tn->p_array_childs)
#define treenode_get_last_child(tn) array_get_last(tn->p_array_childs)

typedef enum {
   IS_NOTLEAF,
   IS_LEAF,
} TreeNodeType;

typedef struct {
   TreeNodeType tnt;
   Array *p_array_childs;
   void *p_val;
   void *p_val2;
} TreeNode;

typedef struct {
   TreeNode *p_treenode_root;
} Tree;

typedef struct {
   TreeNode *ptn;
   int i_pos;
} TreeIteratorState;

typedef struct {
   Stack *p_stack;
   TreeIteratorState *p_state;
} TreeIterator;

Tree* tree_new();
void tree_delete(Tree *p_tree);
void tree_print(Tree *p_tree);
TreeNode* treenode_new(void *p_val);
TreeNode* treenode_new2(void *p_val, void *p_val2);
void treenode_delete(TreeNode *p_treenode);
void treenode_insert_left(TreeNode *p_treenode, TreeNode *p_treenode2);
void treenode_insert_right(TreeNode *p_treenode, TreeNode *p_treenode2);

TreeIteratorState* treeiteratorstate_new(TreeNode *ptn);
void treeiteratorstate_delete(TreeIteratorState *p_state);

TreeIterator* treeiterator_new(Tree *p_tree);
void treeiterator_delete(TreeIterator *p_iter);
_Bool treeiterator_has_next(TreeIterator *p_iter);
TreeNode* treeiterator_next(TreeIterator *p_iter);

#endif
