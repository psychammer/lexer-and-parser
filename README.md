# Updated parser.h and parser.c

### parser.h
added:    
    ParseTreeNode *parse_dec_assign();  
    ParseTreeNode *parse_it_assign();   
    ParseTreeNode *parse_array();  
    ParseTreeNode *create_dec_assign_node();  
    ParseTreeNode *create_it_assign_node();  
    ParseTreeNode *create_array_node();  

### parser.c
  added:  
    dec-assign-stmt    at line 172-205  
    it-assign-stmt     at line 208-235  
    array-stmt draft   at line 238-300  
    create_dec_assign_node(),
    create_it_assign_node(),
    create_array_node()       at line 1011-1024
  
