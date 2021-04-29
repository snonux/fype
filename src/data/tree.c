/*:*
 *: File: ./src/data/tree.c
 *: A simple interpreter
 *: 
 *: WWW		: https://github.com/snonux/fype
 *: AUTHOR	: http://buetow.org
 *: E-Mail	: fype at dev.buetow.org
 *: 
 *: Copyright (c) 2005 - 2008, Paul Buetow 
 *: All rights reserved.
 *: 
 *: Redistribution and use in source and binary forms, with or without modi-
 *: fication, are permitted provided that the following conditions are met:
 *:  * Redistributions of source code must retain the above copyright
 *:    notice, this list of conditions and the following disclaimer.
 *:  * Redistributions in binary form must reproduce the above copyright
 *:    notice, this list of conditions and the following disclaimer in the
 *:    documentation and/or other materials provided with the distribution.
 *:  * Neither the name of P. B. Labs nor the names of its contributors may 
 *:    be used to endorse or promote products derived from this software 
 *:    without specific prior written permission.
 *: 
 *: THIS SOFTWARE IS PROVIDED BY PAUL C. BUETOW AS IS'' AND ANY EXPRESS OR 
 *: IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 *: WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *: DISCLAIMED. IN NO EVENT SHALL PAUL C. BUETOW BE LIABLE FOR ANY DIRECT, 
 *: INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 *: (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 *:  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 *: HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 *: STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 *: IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 *: POSSIBILITY OF SUCH DAMAGE.
 *:*/

#include "tree.h"

Tree*
tree_new() {
   Tree *p_tree = malloc(sizeof(Tree));

   p_tree->p_treenode_root = NULL;

   return p_tree;
}


void
tree_delete(Tree *p_tree) {
   if (!p_tree)
      return;

   if (p_tree->p_treenode_root)
      treenode_delete(p_tree->p_treenode_root);

   free(p_tree);
}

void _tree_print(TreeNode *p_treenode, int i_indent);

void _indent(int i_indent) {
   for (int i = 0; i < i_indent; ++i)
      if (i % TREE_PRINT_INDENT)
         printf(" ");
      else
         printf("|");
}

void
_tree_print_cb2(void *p_void, void *p_indent) {
   _tree_print(p_void, (int) p_indent);
}

void
_tree_print_cb(void *p_void, void *p_indent) {
   TreeNode *ptn = p_void;
   _indent((int) p_indent);

#ifdef FYPE
   TokenType tt = (TokenType) treenode_get_val(ptn);

   if (IS_NOT_TERMINAL(tt))
      goto no_token_val;

   Token *p_token = treenode_get_val2(ptn);

   if (!p_token)
      goto no_token_val;

   char *c_token_val = token_get_val(p_token);
   TokenType tt_token = token_get_tt(p_token);

   if (!c_token_val)
      c_token_val = "";

   printf(" %s=%s", tt_get_name(tt_token), c_token_val);
   return;

no_token_val:
   printf(" %s", tt_get_name(tt));

#else
   printf(" %d", (int) treenode_get_val(ptn));
#endif
}

void
_tree_print(TreeNode *p_treenode, int i_indent) {
   TokenType tt = (TokenType)treenode_get_val(p_treenode);

#ifdef FYPE
   _Bool b_print_nl = false;
   if (IS_NOT_TERMINAL(tt)) {
      _indent(i_indent);
      printf("%s:", tt_get_name(tt));
      b_print_nl = true;
   }
#else
   _indent(i_indent);
   printf("%s:", tt_get_name(tt));
#endif

   array_iterate2(p_treenode->p_array_childs, _tree_print_cb, (void*) 0);

#ifdef FYPE
   if (b_print_nl)
#endif
      printf("\nTree ");

   array_iterate2(p_treenode->p_array_childs,
                  _tree_print_cb2,
                  (void*) (i_indent + TREE_PRINT_INDENT));
}

void
tree_print(Tree *p_tree) {
   if (!p_tree)
      return;

   printf("\nTree ");
   _tree_print(tree_get_root(p_tree), 0);
   printf("\n");
}

TreeNode*
treenode_new(void *p_val) {
   return treenode_new2(p_val, NULL);
}

TreeNode*
treenode_new2(void *p_val, void *p_val2) {
   TreeNode *p_treenode = malloc(sizeof(TreeNode));

   p_treenode->tnt = IS_LEAF;
   p_treenode->p_array_childs = array_new();
   p_treenode->p_val = p_val;
   p_treenode->p_val2 = p_val2;

   return p_treenode;
}

void
treenode_delete(TreeNode *p_treenode) {
   if (!p_treenode)
      return;

   int i_size = array_get_size(p_treenode->p_array_childs);

   for (int i = 0; i < i_size; ++i)
      treenode_delete(array_get(p_treenode->p_array_childs, i));

   array_delete(p_treenode->p_array_childs);

   free(p_treenode);
}

void
treenode_insert_left(TreeNode *p_treenode, TreeNode *p_treenode2) {
   array_unshift(p_treenode->p_array_childs, p_treenode2);
}

void
treenode_insert_right(TreeNode *p_treenode, TreeNode *p_treenode2) {
   array_push(p_treenode->p_array_childs, p_treenode2);
}

TreeIteratorState*
treeiteratorstate_new(TreeNode *ptn) {
   TreeIteratorState *p_state = malloc(sizeof(TreeIteratorState));

   p_state->ptn = ptn;
   p_state->i_pos = 0;

   return p_state;
}

void
treeiteratorstate_delete(TreeIteratorState *p_state) {
   free(p_state);
}

TreeIterator*
treeiterator_new(Tree *p_tree) {
   TreeIterator *p_iter = malloc(sizeof(TreeIterator));

   p_iter->p_stack = stack_new();
   p_iter->p_state = treeiteratorstate_new(tree_get_root(p_tree));

   return p_iter;
}

void
treeiterator_delete(TreeIterator *p_iter) {
   while (!stack_empty(p_iter->p_stack))
      treeiteratorstate_delete(stack_pop(p_iter->p_stack));

   stack_delete(p_iter->p_stack);

   if (p_iter->p_state)
      treeiteratorstate_delete(p_iter->p_state);
}

_Bool
treeiterator_has_next(TreeIterator *p_iter) {
   return p_iter->p_state != NULL;
}

TreeNode*
treeiterator_next(TreeIterator *p_iter) {
   if (!treeiterator_has_next(p_iter))
      return NULL;

   TreeNode *ptn = p_iter->p_state->ptn;

   Array *p_array_childs = treenode_get_childs(ptn);
   int i_num_childs = array_get_size(p_array_childs);

   if (p_iter->p_state->i_pos >= i_num_childs) {
      treeiteratorstate_delete(p_iter->p_state);

      p_iter->p_state =
         stack_empty(p_iter->p_stack) ? NULL : stack_pop(p_iter->p_stack);

      return ptn;
   }

   TreeNode *ptn_next = array_get(p_array_childs, p_iter->p_state->i_pos++);
   stack_push(p_iter->p_stack, p_iter->p_state);
   p_iter->p_state = treeiteratorstate_new(ptn_next);

   return ptn;
}

