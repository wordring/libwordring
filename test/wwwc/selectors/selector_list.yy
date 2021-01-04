%{
%}

%token <hash_token> HASH_TOKEN
%token <ident_token> IDENT_TOKEN
%token <string_token> STRING_TOKEN
%token <function_token> FUNCTION_TOKEN

%token <std::any> ANY_VALUE




%%

selector_list
    : complex_selector_list
    ;




complex_selector_list
    : complex_selector complex_selector_list_1
    ;

complex_selector_list_1
	:
    | ',' complex_selector
	;




compound_selector_list
    : compound_selector compound_selector_list_1
    ;

compound_selector_list_1
    :
    | ',' compound_selector
    ;




simple_selector_list
    : simple_selector simple_selector_list_1
    ;

simple_selector_list_1
    :
    | ',' simple_selector
    ;




relative_selector_list
    : relative_selector relative_selector_list_1
    ;
    
relative_selector_list_1
    :
    | ',' relative_selector
    ;




/* <complex-selector> = <compound-selector> [ <combinator>? <compound-selector> ]* */
complex_selector
    : compound_selector complex_selector_1
    ;

/* [ <combinator>? <compound-selector> ]* */
complex_selector_1
    :
    | opt_combinator compound_selector
    | complex_selector_1 opt_combinator compound_selector
    ;

/* <combinator>? */
opt_combinator
    :
    | combinator
    ;




/* <relative-selector> = <combinator>? <complex-selector> */
relative_selector
    : opt_combinator complex_selector
    ;




/* <compound-selector> = [ <type-selector>? <subclass-selector>*
                        [ <pseudo-element-selector> <pseudo-class-selector>* ]* ]!
*/
compound_selector
    : compound_selector_1
    | compound_selector_2
    | compound_selector_3
    ;

compound_selector_1
    : type_selector repeat0_subclass_selector repeat0_compound_selector_4
    ;

compound_selector_2
    : repeat0_subclass_selector subclass_selector repeat0_compound_selector_4
    ;

compound_selector_3
    : pseudo_element_selector repeat0_pseudo_class_selector repeat0_compound_selector_4
    ;

/* ( pseudo_element_selector pseudo_class_selector* )* */
repeat0_compound_selector_4
    :
    | pseudo_element_selector repeat0_pseudo_class_selector
    ;

/* <subclass-selector>* */
repeat0_subclass_selector
    :
    | subclass_selector
    | repeat0_subclass_selector subclass_selector
    ;

/* <pseudo-class-selector>* */
repeat0_pseudo_class_selector
    :
    | pseudo_class_selector
    | repeat0_pseudo_class_selector pseudo_class_selector
    ;




/* <simple-selector> = <type-selector> | <subclass-selector> */
simple_selector
    : type_selector
    | subclass_selector
    ;




/* <combinator> = '>' | '+' | '~' | [ '|' '|' ] */
combinator
    : '>'
    | '+'
    | '~'
    | '|' '|'
    ;




/* <type-selector> = <wq-name> | <ns-prefix>? '*' */
type_selector
    : wq_name
    | opt_ns_prefix '*'
    ;

opt_ns_prefix
    :
    | ns_prefix
    ;




/* <ns-prefix> = [ <ident-token> | '*' ]? '|' */
ns_prefix
    : ns_prefix_1 '|'
    ;

/* ( IDENT_TOKEN | '*' )? */
ns_prefix_1
    :
    | IDENT_TOKEN
    | '*'
    ;




/* <wq-name> = <ns-prefix>? <ident-token> */
wq_name
    : opt_ns_prefix IDENT_TOKEN
    ;




/* <subclass-selector> = <id-selector> | <class-selector> |
                      <attribute-selector> | <pseudo-class-selector> */
subclass_selector
    : id_selector
    | class_selector
    | attribute_selector
    | pseudo_class_selector
    ;




/* <id-selector> = <hash-token> */
id_selector
    : HASH_TOKEN
    ;




/* <class-selector> = '.' <ident-token> */
class_selector
    : '.' IDENT_TOKEN
    ;




/* <attribute-selector> = '[' <wq-name> ']' |
                       '[' <wq-name> <attr-matcher> [ <string-token> | <ident-token> ] <attr-modifier>? ']' */
attribute_selector
    : '[' wq_name ']'
    | '[' wq_name attr_matcher attribute_selector_1 opt_attr_modifier ']'
    ;

/* [ <string-token> | <ident-token> ] */
attribute_selector_1
    : STRING_TOKEN
    | IDENT_TOKEN
    ;

/* <attr-modifier>? */
opt_attr_modifier
    :
    | attr_modifier
    ;




/* <attr-matcher> = [ '~' | '|' | '^' | '$' | '*' ]? '=' */
attr_matcher
    : attr_matcher_1 '=';

/*[ '~' | '|' | '^' | '$' | '*' ]?*/
attr_matcher_1
    :
    | '~'
    | '|'
    | '^'
    | '$'
    | '*'
    ;




/* <attr-modifier> = i | s */
attr_modifier
    : 'i'
    | 's'
    ;




/* <pseudo-class-selector> = ':' <ident-token> |
                          ':' <function-token> <any-value> ')' */
pseudo_class_selector
    : ':' IDENT_TOKEN
    | ':' FUNCTION_TOKEN ANY_VALUE ')'
    ;




/* <pseudo-element-selector> = ':' <pseudo-class-selector> */
pseudo_element_selector
    : ':' pseudo_class_selector
    ;



%%


